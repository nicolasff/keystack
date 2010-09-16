OUT=hash
OBJS=hash.o dict.o
CFLAGS=-O3 -Wall -Wextra -Werror -pedantic
LDFLAGS=-lrt

all: $(OUT) Makefile

$(OUT): $(OBJS) Makefile
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJS)

%.o: %.c %.h Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

# make sure it works in ANSI C89
dict.o: dict.c dict.h Makefile
	$(CC) -c $(CFLAGS) -ansi --std=c89 -o $@ $<

%.o: %.c Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)

