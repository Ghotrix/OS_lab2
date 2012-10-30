#ifndef MAP_H
#define MAP_H

#include <sys/types.h>

struct map_t {
	struct map_t *nxt;
	void *key;
	size_t key_size;
	void *value;
	size_t value_size;
};

struct map_t *map_create();
void map_set(struct map_t *m, void *key, size_t key_size, 
							  void *value, size_t value_size);
void *map_get_value(struct map_t *m, void *key, size_t key_size);
void map_set_value(struct map_t *m, void *key, size_t key_size, void *value, size_t value_size);

#endif
