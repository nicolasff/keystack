#include <stdio.h>

#include <btree/bt.h>
#include <ht/dict.h>
#include <net/loop.h>
#include <server.h>

int
main(int argc, char *argv[]) {

	struct server *s;
	int fd;
	
	(void)argc;
	(void)argv;

	fd = net_start("0.0.0.0", 1277);

	printf("listening on socket %d\n", fd);
	s = server_new();
	net_loop(fd, s);

	return EXIT_SUCCESS;
}

