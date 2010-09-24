#include <dump.h>
#include <ht/dict.h>
#include <btree/bt.h>
#include <server.h>

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BTREE_INDEX_WIDTH	151

static void
dump_item(char *k, size_t k_sz, char *v, size_t v_sz, void *ctx) { 

	struct dump_info *di = ctx;
	int pos = di->pos;

	printf("dumping item [%s] -> [%s]\n", k, v);

	/* key size */
	memcpy(di->ptr + di->pos, &k_sz, sizeof(uint32_t));
	di->pos += sizeof(uint32_t);

	/* key */
	memcpy(di->ptr + di->pos, k, k_sz);
	di->pos += k_sz;

	/* val size */
	memcpy(di->ptr + di->pos, &v_sz, sizeof(uint32_t));
	di->pos += sizeof(uint32_t);

	/* key */
	memcpy(di->ptr + di->pos, v, v_sz);
	di->pos += v_sz;

	di->index = bt_insert(di->index, k, k_sz, pos);
}

void *
dump_thread_main(void *ptr) {

	struct dump_info *di = ptr;
	long filesize, pagesize;
	int fd;

	printf("Dumping to [%s]\n", di->filename);

	/* compute file size */
	filesize = sizeof(size_t) * 2 * di->d->ht->count
		+ di->d->ht->total_key_len + di->d->ht->total_val_len;
	printf("filesize=%ld\n", filesize);
	pagesize = sysconf(_SC_PAGE_SIZE);
	if(filesize % pagesize) {
		filesize = (filesize + pagesize) & (~(pagesize-1));
	}
	printf("filesize=%ld\n", filesize);

	/* stretch file */
	fd = open(di->filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
	lseek(fd, filesize-1, SEEK_SET);
	write(fd, "", 1);

	/* init index */
	di->index = bt_node_new(BTREE_INDEX_WIDTH);

	/* mmap */
	di->ptr = mmap(NULL, filesize, PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
	di->pos = 0;

	dict_foreach(di->d, dump_item, di);
	munmap(di->ptr, filesize);
	close(fd);

	/* save index to file */
	bt_save(di->index, "/tmp/out.index");
	bt_free(di->index);

//	dict_free(di->d);
//	free(di);
	di->s->state = IDLE;
	return NULL;
}


void
dump_flush(struct server *s, struct dict *d, char *filename) {

	struct dump_info *di = calloc(sizeof(struct dump_info), 1);

	di->d = d;
	di->s = s;
	di->filename = filename;

	pthread_create(&di->thread, NULL, dump_thread_main, di);
}

