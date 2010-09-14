OUT=test
OBJS=test.o bt.o
CFLAGS=-O0 -ggdb -Wall -Wextra -pthread # -Werror -pedantic
LDFLAGS=-lrt -pthread

all: $(OUT) Makefile

$(OUT): $(OBJS) Makefile
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJS)

%.o: %.c %.h Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)

