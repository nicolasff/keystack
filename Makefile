OUT=db
OBJS=test.o btree/bt.o ht/dict.o
CFLAGS=-O0 -ggdb
LDFLAGS=-lrt

all: $(OUT) Makefile

$(OUT): $(OBJS) Makefile
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJS)

%.o: %.c %.h Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(OUT)

