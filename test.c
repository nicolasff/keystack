#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <btree/bt.h>
#include <ht/dict.h>
#include <net/loop.h>
#include <server.h>

struct outfile {

	void *ptr;
	int pos;
	struct bt_node *tree;
};

void
dump_item(char *k, size_t sz, void *v, void *c) { 

	uint32_t data_sz = strlen((char*)v);
	struct outfile *ctx = c;
	int pos = ctx->pos;

	// printf("dumping item %s\n", k);

	/* key size */
	memcpy(ctx->ptr + ctx->pos, &sz, sizeof(uint32_t));
	ctx->pos += sizeof(uint32_t);

	/* key */
	memcpy(ctx->ptr + ctx->pos, k, sz);
	ctx->pos += sz;

	/* val size */
	memcpy(ctx->ptr + ctx->pos, &data_sz, sizeof(uint32_t));
	ctx->pos += sizeof(uint32_t);

	/* key */
	memcpy(ctx->ptr + ctx->pos, v, data_sz);
	ctx->pos += data_sz;

	ctx->tree = bt_insert(ctx->tree, (int)(long)k, pos);
}

int
main(int argc, char *argv[]) {


	int fd, i, n = 1000, ret;
	struct timespec t0, t1;

	struct server *s;
	struct outfile ctx; /* dump context */
	struct dict *d = dict_new(n);
	size_t filesize = 0;
	long pagesize;

	/* fill db */
	for(i = 0; i < n; i++) {
		char *key, *val;
		key = calloc(20, 1);
		val = calloc(100, 1);

		sprintf(key, "key-%d", i);
		memset(val, 'A', 99);

		dict_set(d, key, strlen(key), val, strlen(val));
		filesize += sizeof(size_t) * 2 + strlen(key) + strlen(val);
	}

	/* compute file size */
	pagesize = sysconf(_SC_PAGE_SIZE);
	if(filesize % pagesize) {
		filesize = (filesize + pagesize) & (~(pagesize-1));
	}

	int socket = net_start("0.0.0.0", 1277);
	printf("listening on socket %d\n", socket);
	s = server_new();
	net_loop(socket, s);

	/* dump DB to disk and save indices */


	clock_gettime(CLOCK_MONOTONIC, &t0);
	/* create file and stretch it */
	fd = open("/tmp/out.dump", O_RDWR | O_CREAT | O_TRUNC, 0600);
	ret = lseek(fd, filesize-1, SEEK_SET);
	write(fd, "", 1);

	/* mmap */
	ctx.ptr = mmap(NULL, filesize, PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
	ctx.tree = bt_node_new(151);
	ctx.pos = 0;

	/* dump to disk */
	dict_foreach(d, dump_item, &ctx);
	ret = munmap(ctx.ptr, filesize);
	close(fd);
	clock_gettime(CLOCK_MONOTONIC, &t1);

	bt_save(ctx.tree, "/tmp/out.index");

	float mili0 = t0.tv_sec * 1000 + t0.tv_nsec / 1000000;
	float mili1 = t1.tv_sec * 1000 + t1.tv_nsec / 1000000;
	printf("Dumped %d keys in %0.2f sec: %0.2f/sec\n", n, (mili1-mili0)/1000.0, 1000*(float)n/(mili1-mili0));

	//bt_dump(ctx.tree);

	return EXIT_SUCCESS;
}

