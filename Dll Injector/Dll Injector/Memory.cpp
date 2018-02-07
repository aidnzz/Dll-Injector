#include "Memory.h"

#include <stdexcept>
#include <TlHelp32.h>

void Memory::attach(const wchar_t* processName, DWORD dwAccessRights)
{
	PROCESSENTRY32W processEntry;
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcess == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Invalid handle snapshot!");

	processEntry.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(hProcess, &processEntry))
	{
		CloseHandle(hProcess);
		throw std::runtime_error("Error enumerating through processes");
	}

	do
	{
		if (lstrcmpW(processName, processEntry.szExeFile) == 0) // Case sensitive string search
		{
			hProcess_ = OpenProcess(dwAccessRights, false, processEntry.th32ProcessID);
			CloseHandle(hProcess);
			
			if (hProcess_ == nullptr)
				throw std::runtime_error("Error opening process!");

			pId_ = processEntry.th32ProcessID;
			return;
		}
	} while (Process32NextW(hProcess, &processEntry));

	CloseHandle(hProcess);
	throw std::runtime_error("Could not find process!");
}

DWORD Memory::getModuleBaseAddr(const wchar_t* moduleName) const
{
	MODULEENTRY32W moduleEntry;
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pId_);

	if (hModule == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Invalid Handle value!");

	moduleEntry.dwSize = sizeof(MODULEENTRY32W);

	if (!Module32FirstW(hModule, &moduleEntry))
	{
		CloseHandle(hModule);
		throw std::runtime_error("Module32First error!");
	}

	do
	{
		if (lstrcmpW(moduleEntry.szModule, moduleName) == 0) // Case sensitive string search
		{
			CloseHandle(hModule);
			return (DWORD)moduleEntry.modBaseAddr;
		}
	} while (Module32NextW(hModule, &moduleEntry));

	CloseHandle(hModule);
	throw std::runtime_error("Could not find process!");
}