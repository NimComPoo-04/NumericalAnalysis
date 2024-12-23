#include "function.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *function_file_source = NULL;
object_t *function_default_context = NULL;

void function_read_source(const char *name)
{
	FILE *f = fopen(name, "rb");

	if(!f) return;

	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	function_file_source = calloc(size + 1, 1);
	fread(function_file_source, size + 1, 1, f);

	function_file_source[size] = 0;

	fclose(f);
}

void function_list_symbols()
{
	const char *source = function_file_source;
	const char *where = NULL;
	int len = 0;

	do {
		where = strstr(source, "FUNCTION");

		if(!where) break;

		len = 0;
		while(where[len++] != '{');

		printf("%.*s\n", len - 1, where);

		source = where + 1;

	} while(where);
}

// Platform specific code

#if defined(_WIN32)
#include "function_win32.c"
#elif defined(__unix__)
#include "function_unix.c"
#else
#error "UNSUPORTED PLATFORM"
#endif


// Graveyard of bad ideas

#if 0
struct function_symbol_stack
{
	const char *start;
	int len;
	struct function_symbol_stack *next;

} *function_symbols_head = NULL;

void function_symbols_push(const char *pos, int len)
{
	struct function_symbol_stack *p = calloc(sizeof(struct function_symbol_stack));
	p->start = pos;
	p->len = len;
	p->next = function_symbols_head;
}

void function_symbols_free()
{
	while(function_symbols_head)
	{
		struct function_symbol_stack *next = function_symbols_head->next;
		free(function_symbols_head);
		function_symbols_head = next;
	}
}
#endif
