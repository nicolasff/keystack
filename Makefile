OUT=keystack
OBJS=keystack.o btree/bt.o ht/dict.o net/loop.o cmd.o server.o client.o log.o queue.o
CFLAGS=-O0 -ggdb -I. -Wall -Wextra -pedantic
LDFLAGS=-lrt -levent

all: $(OUT) Makefile

$(OUT): $(OBJS) Makefile
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJS)

%.o: %.c %.h Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)

