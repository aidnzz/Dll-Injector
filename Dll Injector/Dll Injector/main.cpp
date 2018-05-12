#include <string>
#include <iostream>

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
    Dllinject.exe <DLL path> <process name>
    Dllinject.exe <DLL path> 	
)"
};

static bool initialize(const wchar_t* processName, const wchar_t* dllPath)
{
	
	SetConsoleTitleA("Dll injector by incognito04");

	try
	{
		Injector injector;
		
		injector.attach(processName);
		injector.inject(dllPath);
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
	std::wstring processName;

	if (argc == 1 || argc > 3)
	{
		std::cerr << usage << '\n';
		return 1;
	}
	else if (argc == 3)
	{
		std::cout << banner << '\n';
		processName = argv[2];
	}
	else
	{
		std::cout << banner << '\n';

		std::cout << "Process name: ";
		std::wcin >> processName;
	}

	const bool status = initialize(processName.c_str(), argv[1]);
	std::cout << "DLL injection " << (status ? "completed sucessfully" : "failed") << '\n';

	return 0;
}
