// FIXME: make these globals locals and add a (void *) anything argument to args

static const char *help_message =
"pointer    function_name              -- gets the location of the pointer\n"
"list                                  -- lists all the functions\n"
"call       function value ...         -- allows you to call the function with arguments\n"
"integrate                             -- starts up a integration wizard to aid integration\n"
"plot	    function                   -- launches a window for visualization graphs\n"
"reload                                -- reloads the binary incase changes have been made\n"
"help                                  -- prints a help message\n"
"quit                                  -- quits out of the program\n\n" ;

static int should_quit_program = 0;
static char *dll_file_name = 0;
static int is_source_compiled = 1;

static void cmd_reload(char **pos)
{
	(void)pos;

	fprintf(stderr, "Reloading function source file.\n");

	function_file_unload();
	if((is_source_compiled = function_source_compile(dll_file_name)))
		function_file_load();


	fprintf(stderr, "Finished Reloading was%ssuccessful.\n", is_source_compiled ? " ": " not ");
}

static void cmd_pointer(char **pos)
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

static void cmd_list(char **pos)
{
	if(!is_source_compiled)
	{
		fprintf(stderr, "Source is not compiled, unable to process request.\n");
		return;
	}

	(void)pos;
	function_list_symbols();
}

static void cmd_call(char **pos)
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


static void cmd_quit(char **pos)
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

static void cmd_integrate(char **pos)
{
	if(!is_source_compiled)
	{
		fprintf(stderr, "Source is not compiled, unable to process request.\n");
		return;
	}

	(void)pos;
	printf("Integral Wizard : Specify integrals from outer-most to inner-most\n\n");

	char buffer[1024];
	int len = 0;

	printf("How many Integrals = ");

	int count = (int)strtol(command_readline_stdin(), NULL, 10);

	object_t obj = object_create(100, sizeof(double));
	integrator_t *in = calloc(count, sizeof(integrator_t));

	for(int i = 0; i < count; i++)
	{
		in[i].method = SIMPSON_1_3;
		in[i].context = &obj;
		in[i].table_dump = integrator_table_dump;
		in[i].next = (i == count - 1 ? NULL : in + i + 1);

		printf("\nIntegral Number %d\n\n", i+1);

		printf("Integration Variable                = ");
		strncpy(in[i].variable, command_readline_stdin(), len);

		printf("Starting limit Function (with args) = ");
		in[i].start = read_function(command_readline_stdin());
		if(!in[i].start.function)
		{
			fprintf(stderr, "Function for integration not defined.\n");
			goto GONE;
		}

		printf("Ending limit Function (with args)   = ");
		in[i].end = read_function(command_readline_stdin());
		if(!in[i].end.function)
		{
			fprintf(stderr, "Function for integration not defined.\n");
			goto GONE;
		}

		printf("Number Of Samples                   = ");
		in[i].samples = (int)strtol(command_readline_stdin(), 0, 10);

		printf("Maximum Iterations                  = ");
		in[i].iteration = (int)strtol(command_readline_stdin(), 0, 10);
		if(in[i].iteration <= 0) in[i].iteration = 1000;
	}

	printf("Integrated Function (with args)     = ");
	integrator_function_t func = read_function(command_readline_stdin());
	if(!func.function)
	{
		fprintf(stderr, "Function for integration not defined.\n");
		goto GONE;
	}

	printf("\n%-16s  %-16s  %-16s  %-16s  %-16s  %-16s\n", "step", "variable", "h", "start", "end", "result");
	vec_t v = integrator_operate(in, &func);
	printf("\nfinal result = ");
	for(int i = 0; i < v.dim; i++)
		printf("%lf ", v.it[i]);
	puts("");

GONE:
	free(in);
	object_destroy(&obj);
}

static void cmd_plot(char **pos)
{
	if(!is_source_compiled)
	{
		fprintf(stderr, "Source is not compiled, unable to process request.\n");
		return;
	}

	tok_t t = tokenize_next(pos);
	t.start[t.len] = 0;
	function_type_t func = function_get(t.start);
	t.start[t.len] = ' ';

	plot(func);
}

static void cmd_help(char **pos)
{
	(void)pos;
	fprintf(stderr, help_message);
}
