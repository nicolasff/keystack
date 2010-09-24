#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

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
	cmd[4] = 1;
	
	tmp = htonl(key_sz);
	memcpy(cmd + 4 + 1, &tmp, 4);
	memcpy(cmd + 4 + 1 + 4, key, key_sz);

	tmp = htonl(val_sz);
	memcpy(cmd + 4 + 1 + 4 + key_sz, &tmp, 4);
	memcpy(cmd + 4 + 1 + 4 + key_sz + 4, val, val_sz);

	int ret = write(fd, cmd, sz + 4);
	free(cmd);

	read_reply(fd);
}

void
get(int fd, char *key, uint32_t key_sz) {
	uint32_t tmp, sz = 1 + 4 + key_sz;
	
	char *cmd = malloc(sz + 4);
	
	tmp = htonl(sz);
	memcpy(cmd, &tmp, 4);
	cmd[4] = 2;
	
	tmp = htonl(key_sz);
	memcpy(cmd + 4 + 1, &tmp, 4);
	memcpy(cmd + 4 + 1 + 4, key, key_sz);

	write(fd, cmd, sz + 4);
	free(cmd);

	read_reply(fd);
}

int
main() {

	int fd, ret, i=0;
	struct sockaddr_in addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1277);
	memset(&(addr.sin_addr), 0, sizeof(addr.sin_addr));
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));

	while(++i) {
		char c;
		char key[50], val[50];
		sprintf(key, "key-%d", i);
		sprintf(val, "val-%d", i);

		set(fd, key, strlen(key), val, strlen(val));
		get(fd, key, strlen(key));

	//	if(i % 1000 == 0) {
			printf("sent %d commands\n", i);
	//	}
	}

	return 0;
}

