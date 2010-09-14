#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "bt.h"

struct bt_node *
bt_node_new(int width) {

	struct bt_node *b = calloc(sizeof(struct bt_node), 1);
	b->width = width;

	b->leaf = 1;
	b->entries = calloc(sizeof(struct bt_entry), width);
	b->children = calloc(sizeof(struct bt_node), width+1);

	return b;
}

void
bt_free(struct bt_node *b) {

	int i;
	for(i = 0; i <= b->n; i++) {
		if(b->children[i]) {
			bt_free(b->children[i]);
		}
	}
	free(b->children);
	free(b->entries);
	free(b);
}



struct bt_entry *
bt_lookup(struct bt_node *b, int k) {
	
	int i;

	for(i = 0; i <= b->n; ++i) {
		if(i != b->n && k == b->entries[i].key) { /* found */
			return b->entries + i;
		}
		if((i == b->n || k < b->entries[i].key) && b->children[i]) { /* there is more */
			return bt_lookup(b->children[i], k);
		}
	}
	return NULL;
}

void
bt_split_child(struct bt_node *x, int i, struct bt_node *y) {
	struct bt_node *z = bt_node_new(x->width);
	int t = (x->width + 1) / 2;
	int j;
	z->n = t-1;

	for(j = 0; j < t-1; j++) {
		z->entries[j] = y->entries[j+t];
		y->entries[j+t].key = 0;
		if(!y->leaf) {
			z->children[j] = y->children[j + t];
		}
	}

	if(!y->leaf) { /* not leaf */
		z->children[j] = y->children[j + t];
	}

	y->n = t-1;
	for(j = x->n + 1; j > i + 1; j--) {
		x->children[j] = x->children[j-1];
	}
	x->children[i+1] = z;

	for(j = x->n; j > i; j--) {
		x->entries[j] = x->entries[j-1];
	}
	x->entries[i] = y->entries[t-1];
	x->n++;
}

void
bt_insert_nonfull(struct bt_node *x, int k, int v) {

	int i = x->n - 1;

	if(x->leaf) { // leaf
		while(i >= 0 && k < x->entries[i].key) {
			x->entries[i+1] = x->entries[i];
			i--;
		}
		x->entries[i+1].key = k;
		x->entries[i+1].value = v;
		x->n++;
	} else {
		while(i >= 0 && k < x->entries[i].key) {
			i--;
		}
		i++;

		if(x->children[i]->n == x->width) {
			bt_split_child(x, i, x->children[i]);
			if(k > x->entries[i].key) {
				i++;
			}
		}
		bt_insert_nonfull(x->children[i], k, v);
	}
}

struct bt_node *
bt_insert(struct bt_node *r, int k, int v) {
	if(r->n == r->width) {
		struct bt_node *s = bt_node_new(r->width);
		s->children[0] = r;
		s->leaf = 0;
		bt_split_child(s, 0, r);
		bt_insert_nonfull(s, k, v);
		return s;
	} else {
		bt_insert_nonfull(r, k, v);
		return r;
	}
}
		

static void*
bt_write_block(void *p, struct bt_node *b, long id, long *maxid) {


	int i;

	/* write block id */
	long block_id = htonl(id);
	memcpy(p, &block_id, sizeof(long));
	p += sizeof(long);

	/* write block size */
	long block_sz = htonl(b->n);
	memcpy(p, &block_sz, sizeof(long));
	p += sizeof(long);
	
	/* write block entries */
	long first_child = *maxid, child = *maxid;
	for(i = 0; i < b->width; i++) {
		long k, v;
		if(i < b->n) {
			k = htonl(b->entries[i].key);
			v = htonl(b->entries[i].value);
		} else {
			k = v = htonl(0);
		}

		memcpy(p, &k, sizeof(long));
		p += sizeof(long);
		memcpy(p, &v, sizeof(long));
		p += sizeof(long);
	}

	/* write block links */
	for(i = 0; i <= b->width; i++) {
		long c;
		if(i < b-> n && b->children[i]) {
			c = htonl(child++);
			(*maxid)++;
		} else {
			c = htonl(0);
		}
		memcpy(p, &c, sizeof(long));
		p += sizeof(long);
	}

	if(b->children[0] == NULL) { /* no children */
		*maxid = id + 1;
		return p;
	}

	for(i = 0; i <= b->width; i++) { /* write each child with a new id */
		if(i <= b-> n && b->children[i]) {
			p = bt_write_block(p, b->children[i], first_child, maxid);
			first_child++;
		}
	}
	return p;
}

/**
 * Counts the number of nodes
 */
static int
bt_count(struct bt_node *b) {

	int count = 1, i;
	for(i = 0; i <= b->n; i++) {
		if(b->children[i]) {
			count += bt_count(b->children[i]);
		}
	}
	return count;
}

static int
bt_node_size(struct bt_node *b) {
	return sizeof(long) 			/* id */
		+ sizeof(long)			/* n */
		+ 2 * b->width * sizeof(long)	/* keys, values */
		+ (1+b->width) * sizeof(long);	/* children */
}

int
bt_save(struct bt_node *b, const char *filename) {

	int fd, ret;
	long count, maxid = 1, w = b->width;
	long filesize, pagesize;
	int delta = sizeof(long) * 2;
	void *ptr;

	unlink(filename);
	fd = open(filename, O_RDWR | O_CREAT, 0660);
	if(!fd) {
		return -1;
	}

	/* count nodes */
	count = bt_count(b);

	/* compute file size */
	filesize = bt_node_size(b) * count;
	pagesize = sysconf(_SC_PAGE_SIZE);
	if(filesize % pagesize) {
		filesize = (filesize + pagesize) & (~(pagesize-1));
	}
	/* stretch file */
	lseek(fd, filesize-1, SEEK_SET);
	ret = write(fd, "", 1);

	/* mmap, write, munmap */
	ptr = mmap(NULL, filesize, PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
	bt_write_block(ptr + delta, b, 0, &maxid);

	count = htonl(count); /* save number of nodes */
	memcpy(ptr, &count, sizeof(long));

	w = htonl(w); /* save width of nodes. */
	memcpy(ptr + sizeof(long), &w, sizeof(long));

	munmap(ptr, filesize);
	close(fd);
	chmod(filename, 0660);
	return 0;
}

struct bt_node *
bt_load(const char *filename) {

	int fd, i, j, ret;
	struct bt_node *nodes, *b;
	long count, w;
	struct stat buf;
	long filesize = 0;
	void *ptr, *p;

	ret = stat(filename, &buf);
	if(ret != 0) {
		return NULL;
	}
	filesize = buf.st_size;
	
	fd = open(filename, O_RDONLY);
	if(!fd) {
		return NULL;
	}

	p = ptr = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);

	memcpy(&count, ptr, sizeof(long));
	ptr += sizeof(long);
	count = ntohl(count);


	memcpy(&w, ptr, sizeof(long));
	ptr += sizeof(long);
	w = ntohl(w);

	printf("loading %ld nodes\n", count);
	nodes = calloc((size_t)count, sizeof(struct bt_node));

	for(i = 0; i < count; ++i) {
		b = nodes + i;

		long id, n, k, v, c;
		memcpy(&id, ptr, sizeof(long));
		ptr += sizeof(long);
		id = ntohl(id);

		memcpy(&n, ptr, sizeof(long));
		ptr += sizeof(long);
		n = ntohl(n);
		b->n = n;
		b->width = w;

		b->entries = calloc((size_t)w, sizeof(struct bt_entry));
		/* read k, v */
		for(j = 0; j < w; ++j) {
			memcpy(&k, ptr, sizeof(long));
			ptr += sizeof(long);
			memcpy(&v, ptr, sizeof(long));
			ptr += sizeof(long);

			if(j >= n) {
				continue;
			}
			b->entries[j].key = ntohl(k);
			b->entries[j].value = ntohl(v);
		}

		nodes[i].children = calloc((size_t)(w+1), sizeof(struct bt_node*));
		/* read children */
		for(j = 0; j <= w; ++j) {
			memcpy(&c, ptr, sizeof(long));
			ptr += sizeof(long);

			c = ntohl(c);
			if(c != 0) {
				b->children[j] = nodes + c;
			}
		}
	}

	munmap(p, filesize);
	close(fd);

	bt_dump(nodes);
	return nodes;
}

void
bt_dump_(struct bt_node *b, int indent) {

	int i;
	for(i = 0; i < indent; i++) printf("\t");
	printf("%p: ", b);
	if(!b) {
		printf("\n");
		return;
	}

	for(i = 0; i < b->n; ++i) {
		if(i == 0) {
			printf("[");
		} else {
			printf(",");
		}

		if(b->entries[i].key) {
			//printf("%2d", b->entries[i].key);
			printf("%d", b->entries[i].key);
		}
	}
	printf("]\n");

	for(i = 0; i < b->n+1; ++i) {
		if(b->children[i]) {
			bt_dump_(b->children[i], indent+1);
		}
	}
	if(b->children[0] != NULL) {
		printf("\n");
	}

}

void
bt_dump(struct bt_node *b) {

	bt_dump_(b, 0);
}

