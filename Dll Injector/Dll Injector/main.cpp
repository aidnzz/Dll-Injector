#include <iostream>
#include <strsafe.h>

#include "injector.h"

static const char banner[] = {
R"(	 
   ____  _     _     ___        _           _             
  |  _ \| |   | |   |_ _|_ __  (_) ___  ___| |_ ___  _ __ 
  | | | | |   | |    | || '_ \ | |/ _ \/ __| __/ _ \| '__|
  | |_| | |___| |___ | || | | || |  __/ (__| || (_) | |   
  |____/|_____|_____|___|_| |_|/ |\___|\___|\__\___/|_|   
                             |__/                         

)"
};

static const char usage[] = {
R"(
  ============
  Dll Injector
  ============

  Usage:
    Dllinject.exe [DLL path] [process name]
    Dllinject.exe [DLL path] 	
)"
};

static bool inject(const wchar_t* szProcessName, const wchar_t* szDllPath)
{
	try
	{
		Injector injector;
		
		injector.attach(szProcessName);
		injector.inject(szDllPath);
	}
	catch (const std::runtime_error& e)
	{
		MessageBoxA(nullptr, e.what(), nullptr, MB_ICONERROR);
		return false;
	}
	
	return true;
}

int wmain(int argc, wchar_t* argv[]) // For unicode support
{
	if (argc == 1 || argc > 3)
	{
		std::fputs(usage, stderr);
		return 1;
	}
	
	wchar_t szProcessName[MAX_PATH];
	
	if (HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); hStdOut == INVALID_HANDLE_VALUE) // Requires to be compiled with std:c++17
	{
		MessageBoxA(nullptr, "Error: could not get console handle!", nullptr, MB_ICONERROR);
		return 1;
	}

	SetConsoleTitleA("Dll injector: incognito04");

	if (argc == 3)
		StringCbCopyW(szProcessName, MAX_PATH, argv[2]);
	else
	{
		std::cout << "process name> ";
		std::wcin.getline(szProcessName, MAX_PATH);
	}

	bool bStatus = inject(szProcessName, argv[1]);
	std::cout << "DLL injection " << (bStatus ? "completed sucessfully" : "failed") << '\n';

	return 0;
}
