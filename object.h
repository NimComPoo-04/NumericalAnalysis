#ifndef _OBJECT_H_
#define _OBJECT_H_

#define VARSIZ 16

struct object_bucket_t
{
	char key[VARSIZ];
	struct object_bucket_t *next;
	char value[];
};

typedef struct object_t
{
	struct object_bucket_t **buckets;
	int element_size;
	int size;
	int length;
} object_t;

object_t object_create(int initial_size, int element_size);
void *object_add(object_t *o, const char *key, char *value);
void *object_get(object_t *o, const char *key);
void object_destroy(object_t *o);

#endif
