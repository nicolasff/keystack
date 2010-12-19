#include <log.h>
#include <client.h>
#include <queue.h>
#include <cmd.h>
#include <server.h>
#include <ht/dict.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

struct log *
log_open(const char *filename) {

	struct log *l = calloc(1, sizeof(struct log));
	l->fd = open(filename, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR|S_IWUSR);
	l->last_flush = time(NULL);

	return l;
}


void
log_record(struct log *l, struct cmd *c) {

	time_t now;
	uint32_t message_size;
	message_size = htonl(c->raw_size);
	
	/* write message size to log file. */
	write(l->fd, &message_size, sizeof(uint32_t));

	/* write message to log file. */
	write(l->fd, c->raw, c->raw_size);

	now = time(NULL);
	if(now - l->last_flush > 1) { /* flush every second */
		fdatasync(l->fd);
		l->last_flush = now;
	}
}

void
log_add_cb(char *key, size_t key_size, char* val, size_t val_size, void *ctx) {

	int fd;
	uint32_t msg_size, tmp;
	char c;

	/* get fd from context */
	memcpy(&fd, ctx, sizeof(int));

	/* compute message size */
	msg_size = 1 + sizeof(uint32_t) + key_size
		+ sizeof(uint32_t) + val_size;

	tmp = htonl(msg_size);

	write(fd, &tmp, sizeof(uint32_t)); /* write cmd size */

	/* write cmd type */
	c = CMD_SET;
	write(fd, &c, 1);

	/* write key size and key */
	tmp = htonl(key_size);
	write(fd, &tmp, sizeof(uint32_t));
	write(fd, key, key_size);

	/* write val size and val */
	tmp = htonl(val_size);
	write(fd, &tmp, sizeof(uint32_t));
	write(fd, val, val_size);
}

void
log_rewrite(const char *filename, struct server *s) {
	
	struct client c;
	struct cmd *cm;
	int fd = open(filename, O_RDONLY), tmp_fd, ret;
	char tmp_filename[] = "/tmp/keystack-rewrite-file-XXXXXX";

	c.s = s;

	s->log = NULL;

	printf("Importing existing data..."); fflush(stdout);
	/* first, import log */
	while(1) {
		int ret;
		uint32_t sz;
		char *buffer;

		/* read size */
		ret = read(fd, &sz, sizeof(uint32_t));
		if(ret != sizeof(uint32_t)) {
			break;
		}
		sz = ntohl(sz);
		
		/* read message */
		buffer = malloc(sz);
		ret = read(fd, buffer, sz);
		if(ret != (int)sz) {
			break;
		}

		/* process message */
		cm = cmd_parse(buffer, sz);
		cmd_run(s, cm);
		cmd_free(cm);
	}
	close(fd);	
	printf("done.\nRewriting log file... "); fflush(stdout);

	/* second, write to tmp file. */
	tmp_fd = mkstemp(tmp_filename);
	if(tmp_fd == -1) {
		fprintf(stderr, "Failed to create tmp log file.\n");
		return;
	}

	dict_foreach(s->d, log_add_cb, &tmp_fd);
	printf("done (%ld keys).\nSyncing... ", dict_count(s->d)); fflush(stdout);
	fdatasync(tmp_fd);
	close(tmp_fd);

	printf("done.\nReplacing log file... "); fflush(stdout);
	ret = rename(tmp_filename, filename);
	if(ret != 0) {
		fprintf(stderr, "Failed to replace log file.\n");
		return;
	}

	/* third, replace log file with tmp file */
	printf("done.\nStarting to serve clients.\n");
}

