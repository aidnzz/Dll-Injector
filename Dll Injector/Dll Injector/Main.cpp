#include <string>
#include <iostream>

#include "Injector.h"

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

static void initialize(const wchar_t* processName, wchar_t* dllPath)
{
	Injector injector;
	SetConsoleTitleA("Dll injector by incognito04");

	try
	{
		injector.attach(processName);
		std::wcout << "\n[+] Attached to " << processName << " successfully!" << std::endl;

		injector.inject(dllPath);
	}
	catch (const std::runtime_error& e)
	{
		MessageBoxA(nullptr, e.what(), nullptr, MB_ICONERROR);
		std::exit(1);
	}
}

int wmain(int argc, wchar_t* argv[]) // For unicode support
{
	std::wstring processName;

	if (argc == 1 || argc > 3)
	{
		std::cerr << usage << std::endl;
		return 1;
	}
	else if (argc == 3)
	{
		std::cout << banner << std::endl;
		processName = argv[2];
	}
	else
	{
		std::cout << banner << std::endl;

		std::cout << "Process name: ";
		std::wcin >> processName;
	}

	initialize(processName.c_str(), argv[1]);
	std::cout << "[+] DLL injection completed" << std::endl;

	return 0;
}
