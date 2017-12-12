#include <iostream>
#include <string>

#include "Memory.h"

static const char asciiBanner[]
{
R"(	 
     ___  ____  _        _         __          
    / _ \/ / / (_)__    (_)__ ____/ /____  ____
   / // / / / / / _ \  / / -_) __/ __/ _ \/ __/
  /____/_/_/ /_/_//_/_/ /\__/\__/\__/\___/_/    By ~ Incognito04
                   |___/                       
)"
};

bool startUp(DllInjector& inject, const char* const processName)
{
	try
	{
		inject.Attach(processName);
	}
	catch (const std::runtime_error& e)
	{
		MessageBoxA(NULL, e.what(), NULL, MB_ICONERROR);
		return false;
	}

	std::cout << "\n  [+] Attached to " << processName << " successfully!" << std::endl;

	if (!SetConsoleTitleA("Dll injector"))
	{
		unsigned int msgId = MessageBoxA(NULL, "Error setting console window title. Continue?", NULL, MB_ICONERROR | MB_YESNO);
		return msgId == IDYES ? true : false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	std::string processName;

	if (argc != 2)
	{
		std::cerr << " Dll Injector:\n\n"
			      << "  Usage:\n\n"
			      << "   Dllinject.exe <dll to inject> [<process name>]\n"
			      << "   Dllinject.exe <dll to inject>" << std::endl;

		std::cerr << "\n Press <Enter> to exit...";
		std::cin.get();

		return 1;
	}
	else if (argc == 3)
		processName = argv[2];
	else
	{
		
		std::cout << "  [Process name]> ";
		std::cin >> processName;
	}

	DllInjector inject;

	if (!startUp(inject, processName.c_str()))
		return 1;

	inject.Inject(argv[1]);

	std::cout << "  [+] Injection completed. Press <Enter> to exit";
	std::cin.get(); std::cin.get();

	return 0;
}
