#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <crossline.h>

#include "integrator.h"
#include "args.h"
#include "plot.h"
#include "object.h"

#include "cmds.c"

extern object_t *function_default_context;
int main(int argc, char *argv[])
{
	static char buffer[1024];
	char *pos = buffer;
	int len = 0;
	void(*command_func)(char **);

	if(argc != 2)
	{
		fprintf(stderr, "%s [function file name]\n", argv[0]);
		return -1;
	}

	printf("\n%s", help_message);

	dll_file_name = argv[1];
	is_source_compiled = function_source_compile(argv[1]);

	if(function_file_load() == 0)
	{
		fprintf(stderr, "Problem in loading file %s\n", argv[1]);
		return -2;
	}

	object_t tmp = object_create(100, sizeof(double));
	function_default_context = &tmp;

	command_init();

	command_register(list);
	command_register(reload);
	command_register(call);
	command_register(plot);
	command_register(integrate);
	command_register(pointer);
	command_register(help);
	command_register(quit);

	crossline_history_load ("history.txt");

	while(!should_quit_program)
	{
		if(crossline_readline(">> ", buffer, sizeof buffer) == NULL) break;

		// Resetting
		pos = buffer;

		tok_t tok = tokenize_next(&pos);

		tok.start[tok.len] = 0;
		command_func = command_get(tok.start);
		tok.start[tok.len] = ' ';

		if(command_func)
			command_func(&pos);
		else
			fprintf(stderr, "Command is Unrecognized.\n");
	}

	fprintf(stderr, "Goodbye.\n");
	commands_dinit();

	object_destroy(function_default_context);
}
