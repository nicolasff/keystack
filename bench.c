#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <event.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

struct client {

	int fd;

	int msg_sz;
	char *buffer;
	size_t got;
	size_t remain;
};

void
read_reply(int fd) {
	
	char c;
	char *str;
	uint32_t sz;

	read(fd, &c, 1);
	switch(c) {
		case 0:
			read(fd, &c, 1);
			break;

		case 1:
			read(fd, &sz, sizeof(uint32_t));
			sz = ntohl(sz);
			str = malloc(sz);
			read(fd, str, sz);
			free(str);
			break;
	}

}

void
set(int fd, char *key, uint32_t key_sz, char *val, uint32_t val_sz) {
	uint32_t tmp, sz = 1 + 4 + key_sz + 4 + val_sz;
	
	char *cmd = malloc(sz + 4);
	
	tmp = htonl(sz);
	memcpy(cmd, &tmp, 4);
	cmd[4] = 2;
	
	tmp = htonl(key_sz);
	memcpy(cmd + 4 + 1, &tmp, 4);
	memcpy(cmd + 4 + 1 + 4, key, key_sz);

	tmp = htonl(val_sz);
	memcpy(cmd + 4 + 1 + 4 + key_sz, &tmp, 4);
	memcpy(cmd + 4 + 1 + 4 + key_sz + 4, val, val_sz);

	int ret = write(fd, cmd, sz + 4);
	free(cmd);
}

void
get(int fd, char *key, uint32_t key_sz) {
	uint32_t tmp, sz = 1 + 4 + key_sz;
	
	char *cmd = malloc(sz + 4);
	
	tmp = htonl(sz);
	memcpy(cmd, &tmp, 4);
	cmd[4] = 1;
	
	tmp = htonl(key_sz);
	memcpy(cmd + 4 + 1, &tmp, 4);
	memcpy(cmd + 4 + 1 + 4, key, key_sz);

	write(fd, cmd, sz + 4);
	free(cmd);
}

void
on_possible_read(int fd, short event, void *ptr) {

	struct client *c = ptr;
	int ret;

	ret = read(fd, c->buffer + c->got, c->remain);
	if(ret) {
		/*printf("just read %d bytes\n", ret);*/
		c->got += ret;
		c->remain -= ret;
	}

	if(c->msg_sz == -1 && c->remain == 0) { /* just got headers */
		uint32_t sz;
		memcpy(&sz, c->buffer, 4);
		sz = ntohl(sz);
		free(c->buffer);
		/*printf("just got headers, expecting %d bytes\n", sz);*/
		c->got = 0;
		c->msg_sz = (int)sz;
		c->remain = sz;
		c->buffer = malloc(sz);
		
	} else if(c->remain == 0) { /* got a complete answer */
		/*printf("got a complete message\n");*/
		free(c->buffer);
		c->buffer = malloc(4);
		c->got = 0;
		c->remain = 4;
		c->msg_sz = -1;
	}
}

void
on_possible_write(int fd, short event, void *ptr) {

	int *i = ptr;
	char c;
	char key[50], val[50];

	(*i)++;
	sprintf(key, "key-%d", *i);
	sprintf(val, "val-%d", *i);

	set(fd, key, strlen(key), val, strlen(val));
	get(fd, key, strlen(key));

	if((*i) % 1000 == 0) {
		printf("sent %d commands\n", *i);
	}
}

int
main() {

	int fd, ret, n = 0;
	struct sockaddr_in addr;
	struct client c;
	struct event ev_r, ev_w;
	struct event_base *base = event_base_new();

	c.msg_sz = -1;
	c.buffer = malloc(4);
	c.got = 0;
	c.remain = 4;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1277);
	memset(&(addr.sin_addr), 0, sizeof(addr.sin_addr));
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));

	event_set(&ev_r, fd, EV_READ | EV_PERSIST, on_possible_read, &c);
	event_base_set(base, &ev_r);
	event_add(&ev_r, NULL);

	event_set(&ev_w, fd, EV_WRITE | EV_PERSIST, on_possible_write, &n);
	event_base_set(base, &ev_w);
	event_add(&ev_w, NULL);

	event_base_loop(base, 0);

	return 0;
}

