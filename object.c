#include <stdlib.h>
#include <string.h>

#include "object.h"

object_t object_create(int initial_size, int element_size)
{
	object_t o = {0};

	o.buckets = calloc(sizeof(struct object_bucket_t) + element_size, initial_size);
	o.element_size = element_size;
	o.size = initial_size;
	o.length = 0;

	return o;
}

static int get_hash(object_t *o, const char *key)
{
	int hash = 0;
	int p = 1;

	for(int i = 0; key[i] != 0 && i < VARSIZ; i++)
	{
		hash = (hash + key[i] * p) % o->size;
		p = (p * 31) % o->size;
	}

	return hash;
}

//TODO: Add a resizing of the hash thing when size becomes inefficient

void *object_add(object_t *o, const char *key, char *value)
{
	int hash = get_hash(o, key);

	struct object_bucket_t *k = calloc(sizeof(struct object_bucket_t) + o->element_size, 1);

	memcpy(k->key, key, VARSIZ);

	if(value)
		memcpy(k->value, value, o->element_size);

	k->next = o->buckets[hash];

	o->buckets[hash] = k;

	o->length++;

	return k->value;
}

void *object_get(object_t *o, const char *key)
{
	int hash = get_hash(o, key);

	struct object_bucket_t *k = o->buckets[hash];
	while(k && strncmp(k->key, key, VARSIZ) != 0)
		k = k->next;

	return k ? k->value : NULL;
}

void object_destroy(object_t *o)
{
	for(int i = 0; i < o->size; i++)
	{
		struct object_bucket_t *k = o->buckets[i];
		while(k)
		{
			struct object_bucket_t *next = k->next;
			free(k);
			k = next;
		}
	}

	free(o->buckets);
}
