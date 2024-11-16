#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "integrator.h"
#include "args.h"

void get_pointer(char **pos);
void list_functions(char **pos);
void call_function(char **pos);
void call_limits(char **pos);
void integrate(char **pos);
void reload(char **pos);
void quit(char **pos);

// FIXME: make these globals locals and add a (void *) anything argument to args

static int should_quit_program = 0;
static char *dll_file_name = 0;
static int is_source_compiled = 1;

static object_t global_object;

const char *help_message =
"get_pointer     function_name                         -- gets the location of the pointer\n"
"list_functions                                        -- lists all the functions\n"
"call_function   function value ...                    -- allows you to call the function with arguments\n"
"integrate                                             -- starts up a integration wizard to aid integration\n"
"reload                                                -- reloads the binary incase changes have been made\n"
"quit                                                  -- quits out of the program\n"
"\n"
"NOTE: For now there is no concept of scope, so even function argument variable names need to be unique. The integrator relies on these variables for its operations\n"
;

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
	
	global_object = object_create(64, sizeof(double));

	command_init();

	command_register(get_pointer);
	command_register(list_functions);
	command_register(reload);
	command_register(call_function);
	command_register(integrate);
	command_register(quit);

	while(!should_quit_program)
	{
		printf("\n> ");

		fgets(buffer, sizeof(buffer), stdin);
		len = strlen(buffer);
		buffer[len - 1] = 0;

		printf("\n");

		if(feof(stdin)) break;

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
	object_destroy(&global_object);
	function_file_unload();
}

void reload(char **pos)
{
	(void)pos;

	fprintf(stderr, "Reloading function source file.\n");

	function_file_unload();
	if((is_source_compiled = function_source_compile(dll_file_name)))
		function_file_load();


	fprintf(stderr, "Finished Reloading was%ssuccessful.\n", is_source_compiled ? " ": " not ");
}

void get_pointer(char **pos)
{
	if(!is_source_compiled)
	{
		fprintf(stderr, "Source is not compiled, unable to process request.\n");
		return;
	}

	tok_t tok = tokenize_next(pos);

	tok.start[tok.len] = 0;
	printf("%s : %p\n", tok.start, function_get(tok.start));
	tok.start[tok.len] = ' ';
}

void list_functions(char **pos)
{
	if(!is_source_compiled)
	{
		fprintf(stderr, "Source is not compiled, unable to process request.\n");
		return;
	}

	(void)pos;
	function_list_symbols();
}

void call_function(char **pos)
{
	if(!is_source_compiled)
	{
		fprintf(stderr, "Source is not compiled, unable to process request.\n");
		return;
	}

	tok_t which = tokenize_next(pos);

	which.start[which.len] = 0;
	function_type_t func = function_get(which.start);
	which.start[which.len] = ' ';

	if(func == NULL)
	{
		fprintf(stderr, "Function not defined.\n");
		return;
	}

	vec_t vin = {0};
	while(which.len != 0)
	{
		which = tokenize_next(pos);
		which.start[which.len] = 0;
		vin.it[vin.dim++] = strtod(which.start, NULL);
		which.start[which.len] = ' ';
	}

	vec_t v = func(vin);

	printf("result = ");
	for(int i = 0; i < v.dim; i++)
		printf("%lf ", v.it[i]);
	puts("");
}


void quit(char **pos)
{
	(void)pos;
	should_quit_program = 1;
}

static inline integrator_function_t read_function(char *other)
{
	integrator_function_t in = {0};

	tok_t tok = tokenize_next(&other);
	tok.start[tok.len] = 0;
	in.function = function_get(tok.start);
	tok.start[tok.len] = ' ';

	do {
		tok = tokenize_next(&other);
		if(tok.len <= 0)
			break;
		tok.start[tok.len] = 0;
		strcpy(in.names[in.count++], tok.start);
		tok.start[tok.len] = ' ';
	} while(tok.len > 0 && in.count < MAXDIM);

	return in;
}

void integrate(char **pos)
{
	(void)pos;
	printf("Integral Wizard : Specify integrals from outer-most to inner-most\n\n");

	char buffer[1024];
	int len = 0;

	printf("How many Integrals = ");
	fgets(buffer, sizeof buffer, stdin);
	len = strlen(buffer);
	buffer[len - 1] = 0;

	int count = (int)strtol(buffer, NULL, 10);

	puts("");

	object_t obj = object_create(100, sizeof(double));

	integrator_t *in = calloc(count, sizeof(integrator_t));

	for(int i = 0; i < count; i++)
	{
		printf("Integral Number %d\n\n", i+1);

		in[i].method = SIMPSON_1_3;
		in[i].context = &obj;
		in[i].table_dump = integrator_table_dump;

		printf("Integration Variable                = ");
		fgets(buffer, sizeof buffer, stdin);
		len = strlen(buffer);
		buffer[len - 1] = 0;

		strncpy(in[i].variable, buffer, len);

		printf("Starting limit Function (with args) = ");
		fgets(buffer, sizeof buffer, stdin);
		len = strlen(buffer);
		buffer[len - 1] = 0;

		in[i].start = read_function(buffer);
		if(!in[i].start.function)
		{
			fprintf(stderr, "Function for integration not defined.\n");
			goto GONE;
		}

		printf("Ending limit Function (with args)   = ");
		fgets(buffer, sizeof buffer, stdin);
		len = strlen(buffer);
		buffer[len - 1] = 0;

		in[i].end = read_function(buffer);
		if(!in[i].end.function)
		{
			fprintf(stderr, "Function for integration not defined.\n");
			goto GONE;
		}

		printf("Number Of Samples                   = ");
		fgets(buffer, sizeof buffer, stdin);
		len = strlen(buffer);
		buffer[len - 1] = 0;

		in[i].samples = (int)strtol(buffer, 0, 10);

		printf("Maximum Iterations                  = ");
		fgets(buffer, sizeof buffer, stdin);
		len = strlen(buffer);
		buffer[len - 1] = 0;

		in[i].iteration = (int)strtol(buffer, 0, 10);
		if(in[i].iteration <= 0) in[i].iteration = 1000;

		in[i].next = (i == count - 1 ? NULL : in + i + 1);
	}

	printf("Integrated Function (with args)     = ");
	fgets(buffer, sizeof buffer, stdin);
	len = strlen(buffer);
	buffer[len - 1] = 0;

	integrator_function_t func = read_function(buffer);

	if(!func.function)
	{
		fprintf(stderr, "Function for integration not defined.\n");
		goto GONE;
	}

	puts("");
	printf("%-16s  %-16s  %-16s  %-16s  %-16s  %-16s\n", "step", "variable", "h", "start", "end", "result");
	vec_t v = integrator_operate(in, &func);

	printf("\nfinal result = ");
	for(int i = 0; i < v.dim; i++)
		printf("%lf ", v.it[i]);
	puts("");

GONE:
	free(in);
	object_destroy(&obj);
}
