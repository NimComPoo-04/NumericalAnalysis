#ifndef _ARGS_H_
#define _ARGS_H_

#include "object.h"

typedef struct
{
	char *start;
	int len;
} tok_t;

tok_t tokenize_next(char **buffer);

void command_init();
void ___command_register(const char *key, void(*)(char **pos));
void(*command_get(const char *key))(char **pos) ;
void commands_dinit();

#define command_register(some) ___command_register(#some, some)

#endif

