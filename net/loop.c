#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <event.h>
#include <stdio.h>

#include <net/loop.h>
#include <ht/dict.h>


/**
 * Sets up a non-blocking socket
 */
int
net_start(const char *ip, short port) {

	int reuse = 1;
	struct sockaddr_in addr;
	int fd, ret;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	memset(&(addr.sin_addr), 0, sizeof(addr.sin_addr));
	addr.sin_addr.s_addr = inet_addr(ip);

	/* this sad list of tests could use a Maybe monad... */

	/* create socket */
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == fd) {
		syslog(LOG_ERR, "Socket error: %m\n");
		return -1;
	}

	/* reuse address if we've bound to it before. */
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse,
				sizeof(reuse)) < 0) {
		syslog(LOG_ERR, "setsockopt error: %m\n");
		return -1;
	}

	/* set socket as non-blocking. */
	ret = fcntl(fd, F_SETFD, O_NONBLOCK);
	if (0 != ret) {
		syslog(LOG_ERR, "fcntl error: %m\n");
		return -1;
	}

	/* bind */
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (0 != ret) {
		syslog(LOG_ERR, "Bind error: %m\n");
		return -1;
	}

	/* listen */
	ret = listen(fd, SOMAXCONN);
	if (0 != ret) {
		syslog(LOG_DEBUG, "Listen error: %m\n");
		return -1;
	}

	/* there you go, ready to accept! */
	return fd;
}

static void
on_available_data(int fd, short event, void *ptr) {

	/* TODO: make this non-blocking */
	struct client *c = ptr;
	char magic;

	printf("on_available_data\n");

	read(fd, &magic, 1);
	read(fd, &c->cmd, 1);

	/* read key size */
	read(fd, &c->key_sz, sizeof(uint32_t));
	c->key_sz = ntohl(c->key_sz);

	/* read key */
	c->key = calloc(1+c->key_sz, 1);
	read(fd, c->key, c->key_sz);

}

static void
on_connect(int fd, short event, void *ptr) {
	(void)event;


	struct event_base *base = ptr;
	struct sockaddr_in addr;
	socklen_t addr_sz = sizeof(addr);
	int client_fd;

	printf("on_connect\n");
	client_fd = accept(fd, (struct sockaddr*)&addr, &addr_sz);

	struct client *c = calloc(sizeof(struct client), 1);
	c->fd = client_fd;

	/* wait for new data */
	event_set(&c->ev, c->fd, EV_READ, on_available_data, c);
	event_base_set(base, &c->ev);
	event_add(&c->ev, NULL);
}

void
net_loop(int fd, struct dict *d) {

	struct event_base *base = event_base_new();
	struct event ev;

	event_set(&ev, fd, EV_READ | EV_PERSIST, on_connect, base);
	event_base_set(base, &ev);
	event_add(&ev, NULL);

	event_base_loop(base, 0);
}

