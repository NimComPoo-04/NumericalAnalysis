/*
 * This file has to be treated like its in function.c
 * this file exists for the sole perpose of declutering things
 */

#include <dlfcn.h>
#include <unistd.h>
#include <sys/wait.h>

static char function_file_name[] = "./external-functions-shared-object.so";

static void *function_file_handle;

int function_file_load()
{
	if(function_file_handle != 0)
		function_file_unload();

	function_file_handle = dlopen(function_file_name, RTLD_LAZY | RTLD_GLOBAL);

	if(function_file_handle == 0)
	{
		fprintf(stderr, "Loading Error: %s\n", dlerror());
		function_file_unload();
		return 0;
	}
	else
	{
		return 1;
	}
}

void *function_get(const char *func_name)
{
	return dlsym(function_file_handle, func_name);
}

void function_file_unload()
{
	if(function_file_handle != 0)
		dlclose(function_file_handle);
	function_file_handle = 0;
}

int function_source_compile(char *name)
{
	char tccpath[] = "./cmpl/tinycc/tcc";

	char *tccargs[] = {
		tccpath,
		"-Wall", "-g", "-rdynamic", "-shared",			// common flags
		"-o", function_file_name,				// output file
		"-I", ".", "-I", "./cmpl/tinycc/include",
		"-L", "./cmpl/tinycc",
		name, "function.c", "object.c", "integrator.c",		// important files
		"special_integrals.c",
		"-lm", 0
	};

	if(fork() == 0)
		execv(tccpath, tccargs);
	else
	{
		int wstatus;
		wait(&wstatus);

		if(WEXITSTATUS(wstatus) == 0)
		{
			free(function_file_source);
			function_read_source(name);

			return 1;
		}
		else
		{
			fprintf(stderr, "Compilation Error in extended file. exit status = %d\n", WEXITSTATUS(wstatus));

			return 0;
		}
	}

	return 0;
}
