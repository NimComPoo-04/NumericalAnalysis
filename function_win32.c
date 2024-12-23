/*
 * This file has to be treated like its in function.c
 * this file exists for the sole perpose of declutering things
 */

#include <windows.h>

static char function_file_name[] = "./external-functions-shared-object.dll";

static HMODULE function_file_handle = NULL;

int function_file_load()
{
	if(function_file_handle != 0)
		function_file_unload();

	function_file_handle = LoadLibrary(function_file_name);

	if(function_file_handle == 0)
	{
		function_file_unload();
		return 0;
	}
	else
		return 1;
}

void *function_get(const char *func_name)
{
	return GetProcAddress(function_file_handle, func_name);
}

void function_file_unload()
{
	FreeLibrary(function_file_handle);
}

int function_source_compile(char *name)
{
	char tccpath[] = ".\\cmpl\\wintcc\\tcc";
	char tccinclude[] = "-I .\\cmpl\\wintcc\\include -I .\\cmpl\\wintcc\\include\\winapi";
	char tcclib[] = ".\\cmpl\\wintcc\\lib";

	char commandline[1024];

	sprintf(commandline, "%s -Wall -g -rdynamic -shared -o %s -I . %s -L %s %s"
			     " special_integrals.c"
			     " function.c object.c integrator.c",
			tccpath, function_file_name, tccinclude, tcclib, name);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CreateProcess(NULL, commandline, 0, 0, FALSE, 0, 0, 0, &si, &pi);

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD wstatus;
	GetExitCodeProcess(pi.hProcess, &wstatus);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if(wstatus == 0)
	{
		free(function_file_source);
		function_read_source(name);

		return 1;
	}
	else
	{
		fprintf(stderr, "Compilation Error in extended file. exit status = %ld\n", wstatus);

		return 0;
	}

	return 0;
}
