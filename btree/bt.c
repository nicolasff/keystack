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

static int
safe_strcmp(char *s0, size_t sz0, char *s1, size_t sz1) {

	int ret;

	if(sz0 < sz1) {
		ret = -1;
	} else if(sz0 > sz1) {
		ret = 1;
	} else if(s1) {
		ret = memcmp(s0, s1, sz0);
	} else {
		ret = -1;
	}

	return ret;
}

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
bt_lookup(struct bt_node *b, char *k, size_t sz) {
	
	int i;

	for(i = 0; i <= b->n; ++i) {
		int cmp = 0;
		if(i != b->n) {
			cmp = safe_strcmp(k, sz, b->entries[i].key, b->entries[i].key_size);
		}
		if(i != b->n && cmp == 0) { /* found */
			return b->entries + i;
		}
		if((i == b->n || cmp < 0) && b->children[i]) { /* there is more */
			return bt_lookup(b->children[i], k, sz);
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
		y->entries[j+t].key = NULL;
		y->entries[j+t].key_size = 0;
		if(!y->leaf) {
			z->children[j] = y->children[j + t];
		}
	}

	if(!y->leaf) { /* not leaf */
		z->children[j] = y->children[j + t];
	}

	/* shift x's children to the right */
	y->n = t-1;
	for(j = x->n + 1; j > i + 1; j--) {
		x->children[j] = x->children[j-1];
	}
	x->children[i+1] = z;

	/* shift x's entries to the right */
	for(j = x->n; j > i; j--) {
		x->entries[j] = x->entries[j-1];
	}
	x->entries[i] = y->entries[t-1];
	x->n++;
}

void
bt_insert_nonfull(struct bt_node *x, char *k, size_t sz, int v) {

	int i = x->n - 1;

	if(x->leaf) { // leaf
		while(i >= 0 && safe_strcmp(k, sz,
				x->entries[i].key, x->entries[i].key_size) < 0) {
			x->entries[i+1] = x->entries[i];
			i--;
		}
		x->entries[i+1].key = k;
		x->entries[i+1].key_size = sz;
		x->entries[i+1].value = v;
		x->n++;
	} else {
		while(i >= 0 && safe_strcmp(k, sz,
				x->entries[i].key, x->entries[i].key_size) < 0) {
			i--;
		}
		i++;

		if(x->children[i]->n == x->width) {
			bt_split_child(x, i, x->children[i]);
			if(safe_strcmp(k, sz,
				x->entries[i].key, x->entries[i].key_size) > 0) {
				i++;
			}
		}
		bt_insert_nonfull(x->children[i], k, sz, v);
	}
}

struct bt_node *
bt_insert(struct bt_node *r, char *k, size_t sz, int v) {
	if(r->n == r->width) {
		struct bt_node *s = bt_node_new(r->width);
		s->children[0] = r;
		s->leaf = 0;
		bt_split_child(s, 0, r);
		bt_insert_nonfull(s, k, sz, v);
		return s;
	} else {
		bt_insert_nonfull(r, k, sz, v);
		return r;
	}
}
		

#if 0
static void*
bt_write_block(void *p, struct bt_node *b, uint32_t id, uint32_t *maxid) {

	int i;

	/* write block id */
	uint32_t block_id = htonl(id);
	memcpy(p, &block_id, sizeof(uint32_t));
	p += sizeof(uint32_t);

	/* write block size */
	uint8_t block_sz = b->n;
	memcpy(p, &block_sz, 1);
	p++;
	
	/* write block entries */
	uint32_t first_child = *maxid, child = *maxid;
	for(i = 0; i < b->width; i++) {
		uint32_t k_sz, v;
		if(i < b->n) {
			k_sz = htonl(b->entries[i].key_size);
			v = htonl(b->entries[i].value);
		} else {
			k_sz = v = htonl(0);
		}

		/* write key size */
		memcpy(p, &k_sz, sizeof(uint32_t));
		p += sizeof(uint32_t);

		/* write key */
		if(i < b->n && b->entries[i].key_size != 0) {
			memcpy(p, b->entries[i].key, b->entries[i].key_size);
			p += b->entries[i].key_size;
		}

		/* write value */
		memcpy(p, &v, sizeof(uint32_t));
		p += sizeof(uint32_t);
	}

	/* write block links */
	for(i = 0; i <= b->width; i++) {
		uint32_t c;
		if(i <= b-> n && b->children[i]) {
			c = htonl(child++);
			(*maxid)++;
		} else {
			c = htonl(0);
		}
		memcpy(p, &c, sizeof(uint32_t));
		p += sizeof(uint32_t);
	}

	if(b->children[0] == NULL) { /* no children */
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
#endif

/**
 * Counts the number of nodes
 */
static long
bt_compute_size(struct bt_node *b, uint32_t *count) {

	int i;
	(*count)++;	/* count current node */
	long sz =  1 /* n */
		+ b->width * sizeof(uint32_t) * 2 /* key and value sizes */
		+ (b->width + 1) * sizeof(uint32_t); /* children offsets */

	for(i = 0; i <= b->n; i++) {
		if(i < b->n && b->entries[i].key_size != 0) {
			sz += b->entries[i].key_size;
		}
		if(b->children[i]) {
			sz += bt_compute_size(b->children[i], count);
		}
	}
	return sz;
}

int
bt_save(struct bt_node *b, const char *filename) {

	int fd, ret;
	uint32_t count = 0, w = b->width;
	uint32_t delta = sizeof(uint32_t) * 3, root_sz, root_offset;
	long filesize, pagesize;
	void *ptr;

	unlink(filename);
	fd = open(filename, O_RDWR | O_CREAT, 0660);
	if(!fd) {
		return -1;
	}

	/* count nodes */
	filesize = bt_compute_size(b, &count) + 2 * sizeof(uint32_t);

	/* compute file size */
	pagesize = sysconf(_SC_PAGE_SIZE);
	if(filesize % pagesize) {
		filesize = (filesize + pagesize) & (~(pagesize-1));
	}
	/* stretch file */
	lseek(fd, filesize-1, SEEK_SET);
	ret = write(fd, "", 1);

	/* mmap, write, munmap */
	ptr = mmap(NULL, filesize, PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, 0);
	bt_save_to_mmap(b, ptr, &delta, &root_sz);
	root_offset = delta - root_sz;

	count = htonl(count); /* save number of nodes */
	memcpy(ptr, &count, sizeof(uint32_t));

	w = htonl(w); /* save width of nodes. */
	memcpy(ptr + sizeof(uint32_t), &w, sizeof(uint32_t));

	root_offset = htonl(root_offset); /* save offset of root node. */
	memcpy(ptr + 2 * sizeof(uint32_t), &root_offset, sizeof(uint32_t));

	munmap(ptr, filesize);
	close(fd);
	chmod(filename, 0660);
	return 0;
}

struct bt_node *
bt_load(const char *filename) {

	int fd, i, j, ret;
	struct bt_node *nodes, *b;
	long count;
	struct stat buf;
	long filesize = 0;
	void *ptr, *p;
	uint32_t w;

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

	memcpy(&count, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	count = ntohl(count);

	memcpy(&w, ptr, sizeof(uint32_t));
	ptr += sizeof(uint32_t);
	w = ntohl(w);

	printf("loading %ld nodes of width %d\n", count, (int)w);
	nodes = calloc((size_t)count, sizeof(struct bt_node));

	for(i = 0; i < count; ++i) {

		uint32_t id, k_sz, v, c;
		uint8_t n;
		memcpy(&id, ptr, sizeof(uint32_t));
		ptr += sizeof(uint32_t);
		id = ntohl(id);
		b = nodes + (id-1);

		memcpy(&n, ptr, 1);
		ptr++;
		b->n = n;
		b->width = w;

		b->entries = calloc((size_t)w, sizeof(struct bt_entry));
		/* read k, v */
		for(j = 0; j < (int)w; ++j) {
			memcpy(&k_sz, ptr, sizeof(uint32_t));
			ptr += sizeof(uint32_t);
			k_sz = ntohl(k_sz);

			if(k_sz) {
				b->entries[j].key = malloc(k_sz);
				memcpy(b->entries[j].key, ptr, k_sz);
				ptr += k_sz;
			}

			memcpy(&v, ptr, sizeof(uint32_t));
			ptr += sizeof(uint32_t);
			v = ntohl(v);

			if(j >= (int)n) {
				continue;
			}

			b->entries[j].key_size = k_sz;
			b->entries[j].value = v;

		}

		b->children = calloc((size_t)(w+1), sizeof(struct bt_node*));
		/* read children */
		for(j = 0; j <= (int)w; ++j) {
			memcpy(&c, ptr, sizeof(uint32_t));
			ptr += sizeof(uint32_t);

			c = ntohl(c);
			if(c != 0) {
				b->children[j] = &nodes[c-1];
			}
		}
	}

	munmap(p, filesize);
	close(fd);

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
			printf("(%zd) ", b->entries[i].key_size);
			fflush(stdout);
			int ret = write(1, b->entries[i].key, b->entries[i].key_size);
			(void)ret;
			printf(" [v=%d]", b->entries[i].value);
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


/* btree save */

static uint32_t
bt_dump_block(struct bt_node *b, char *p, uint32_t *offsets) {

	char *p_start = p;

	/* write block size */
	uint8_t block_sz = b->n;
	memcpy(p, &block_sz, 1);
	p++;
	
	/* write block entries */
	int i;
	for(i = 0; i < b->width; i++) {
		uint32_t k_sz, v;
		if(i < b->n) {
			k_sz = htonl(b->entries[i].key_size);
			v = htonl(b->entries[i].value);
		} else {
			k_sz = v = htonl(0);
		}

		/* write key size */
		memcpy(p, &k_sz, sizeof(uint32_t));
		p += sizeof(uint32_t);

		/* write key */
		if(i < b->n && b->entries[i].key_size != 0) {
			memcpy(p, b->entries[i].key, b->entries[i].key_size);
			p += b->entries[i].key_size;
		}

		/* write value */
		memcpy(p, &v, sizeof(uint32_t));
		p += sizeof(uint32_t);
	}

	/* write block links */
	for(i = 0; i <= b->width; i++) {
		uint32_t c;
		if(i <= b-> n && b->children[i]) {
			c = htonl(offsets[i]);
		} else {
			c = htonl(0);
		}
		memcpy(p, &c, sizeof(uint32_t));
		p += sizeof(uint32_t);
	}

	return p - p_start;
}

uint32_t
bt_save_to_mmap(struct bt_node *b, char *p, uint32_t *off, uint32_t *self_size) {

	/* write children first, and record their offsets */

	uint32_t *offsets = calloc(sizeof(uint32_t), 1+b->n);
	int i;
	for(i = 0; i <= b->n; i++) {
		if(b->children[i]) {
			offsets[i] = bt_save_to_mmap(b->children[i], p, off, self_size);
		} else {
			offsets[i] = 0;
		}
	}

	*self_size = bt_dump_block(b, p + *off, offsets);
	free(offsets);
	
	printf("writing %p (id=%ld) at offset %d\n", b, b->id, (int)*off);
	(*off) += *self_size;

	return *off;
}

