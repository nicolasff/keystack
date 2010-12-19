#include <log.h>
#include <client.h>
#include <queue.h>
#include <cmd.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

struct log *
log_open(const char *filename) {

	struct log *l = calloc(1, sizeof(struct log));
	l->fd = open(filename, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR|S_IWUSR);
	l->last_flush = time(NULL);
	l->q = queue_new();

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
log_close(struct log *l) {
	/* FIXME */
	close(l->fd);
}

void
log_import(const char *filename, struct server *s) {
	
	struct client c;
	struct cmd *cm;
	int fd = open(filename, O_RDONLY);

	c.s = s;

	while(1) {
		int ret;
		uint16_t sz;
		char *buffer;

		/* read size */
		ret = read(fd, &sz, sizeof(uint16_t));
		if(ret != sizeof(uint16_t)) {
			break;
		}
		sz = ntohl(sz);
		
		/* read message */
		buffer = malloc(sz);
		ret = read(fd, buffer, sz);
		if(ret != sizeof(uint16_t)) {
			break;
		}

		/* process message */
		cm = cmd_parse(buffer, sz);
		cmd_run(s, cm);
	}
	close(fd);	
}

