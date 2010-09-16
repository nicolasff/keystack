### Description

`ht` is a very simple Hash Table library written in C.

### Features
* String hashing only.
* Optional redefinition of the key hash function (“[djb2](http://www.cse.yorku.ca/~oz/hash.html)” by default).
* Store any data (`void*`).
* Non-blocking resize.
* Optional key alloc/free functions. If none are specified, the key is **not** copied.

Multi-threading support will not be added.
