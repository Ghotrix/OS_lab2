#ifndef MAP_H
#define MAP_H

#include <sys/types.h>

/* Structure to describe elements of associative array */
struct map_t {
	struct map_t *nxt; /* Pointer to the next element of map */
	void *key; /* Key itself */
	size_t key_size; /* Size of key in bytes */
	void *value; /* Value associated with key */
	size_t value_size; /* Size of value in bytes */
};

/* Function that initialize map */
struct map_t *map_create();

/* Here we setting map */
void map_set(struct map_t *m, void *key, size_t key_size, 
							  void *value, size_t value_size);

/* Get value for given key, need to cast it to needed type */
void *map_get_value(struct map_t *m, void *key, size_t key_size);

/* Clearing up*/
void map_destruct(struct map_t *m);

/* Obvious :-) */
size_t map_get_size(struct map_t *m);

#endif
