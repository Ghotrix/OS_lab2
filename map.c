#include <malloc.h>
#include <string.h>
#include "map.h"

struct map_t *map_create()
{
	struct map_t *m;
	m = (struct map_t *) malloc(sizeof(struct map_t));
	m->key = NULL;
	m->key_size = 0;
	m->value = NULL;
	m->value_size = 0;
	m->nxt = NULL;

	return m;
}

void map_set(struct map_t *m, void *key, size_t key_size, void *value, size_t value_size)
{
	struct map_t *map;

	if(m->key == NULL) {
		m->key=(void *)malloc(key_size);
		memcpy(m->key, key, key_size);
		m->value=(void *)malloc(value_size);
		memcpy(m->value, value, value_size);
		m->nxt = NULL;

		return;
	}

	for(map = m; ; map = map->nxt) {
		if(!memcmp(key, map->key, key_size)) {
			if(map->value != NULL) {
				free(map->value);
				map->value = (void *)malloc(value_size);
				memcpy(map->value, value, value_size);

				return;
			}
		}

		if(map->nxt == NULL) {
			map->nxt = (struct map_t *)malloc(sizeof(struct map_t));
			map = map->nxt;
			map->key = (void *)malloc(key_size);
			memcpy(map->key, key, key_size);
			map->value = (void *)malloc(value_size);
			memcpy(map->value, value, value_size);
			map->nxt = NULL;

			return;
		}
	}
}

void *map_get_value(struct map_t *m, void *key, size_t key_size)
{
	struct map_t *map;

	for(map = m; map != NULL; map = map->nxt) {
		if (map->key == NULL)
			return NULL;

		if(!memcmp(key, map->key, key_size)) {
			return map->value;
		}
	}

	return NULL;
}

void map_destruct(struct map_t *m)
{
	struct map_t *map, *prev_map = NULL;

	for (map = m; map != NULL; map = map->nxt) {
		if (prev_map != NULL)
			free(prev_map);
		
		prev_map = map;

		if (map->key != NULL) {
			free(map->key);
			free(map->value);
		}
	}
}

size_t map_get_size(struct map_t *m)
{
	size_t size = 0;
	for (struct map_t *map = m; map != NULL; map = map->nxt) {
		size++;
	}

	return size;
}
