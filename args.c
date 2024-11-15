#include <ctype.h>
#include <stdint.h>
#include "args.h"

tok_t tokenize_next(char **buffer)
{
	tok_t t = {0};

	while(buffer[0][0] && isspace(buffer[0][0]))
		buffer[0]++;

	t.start = buffer[0];

	while(buffer[0][0])
	{
		if(isspace(buffer[0][0]))
			break;
		t.len++;
		buffer[0]++;
	}

	return t;
}

static object_t command_listings;

void command_init()
{
	command_listings = object_create(64, sizeof(void(*)(char **pos)));
}

void ___command_register(const char *key, void(*f)(char **pos))
{
	void(**k)(char **pos) = &f;
	object_add(&command_listings, key, (void *)k);
}

void(*command_get(const char *key))(char **pos)
{
	uintptr_t *p = object_get(&command_listings, key);
	return p ? (void *)*(uintptr_t *)p: 0;
}

void commands_dinit()
{
	object_destroy(&command_listings);
}
