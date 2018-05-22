#include <iostream>
#include <strsafe.h>

#include "injector.h"

static const char banner[] = {
R"(	 
 _____       _           _    ___       
|_   _|     (_)         | |  / _ \      
  | |  _ __  _  ___  ___| |_| | | |_ __ 
  | | | '_ \| |/ _ \/ __| __| | | | '__|
 _| |_| | | | |  __/ (__| |_| |_| | |   
|_____|_| |_| |\___|\___|\__|\___/|_|   
           _/ |                         
          |__/                          


)"
};

struct InjectInfo
{
	const wchar_t* szProcessName;
	const wchar_t* szDllPath;
};

static bool inject(const InjectInfo& info)
{
	try
	{
		Injector injector;
		
		injector.attach(info.szProcessName);
		injector.inject(info.szDllPath);
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
		std::wcerr << "Usage: " << argv[0] << " [DLL Path] [Process]" << '\n';
		std::wcerr << "       " << argv[0] << " [DLL Path]" << '\n';
		
		return 1;
	}
	
	wchar_t szProcessName[MAX_PATH];

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	if (hStdOut == INVALID_HANDLE_VALUE) 
	{
		MessageBoxA(nullptr, "Error: could not get console handle!", nullptr, MB_ICONERROR);
		return 1;
	}

	SetConsoleTitleA("Injector by: incognito04");
	
	WriteConsoleA(hStdOut, banner, strlen(banner), nullptr, nullptr);

	if (argc == 3)
		StringCbCopyW(szProcessName, MAX_PATH, argv[2]);
	else
	{
		std::cout << "Process> ";
		std::wcin.getline(szProcessName, MAX_PATH);
	}

	const InjectInfo info = { szProcessName, argv[1] };
	const bool bStatus = inject(info);
	
	std::cout << "\n[-] DLL injection " << (bStatus ? "completed sucessfully" : "failed") << '\n';

	return 0;
}
