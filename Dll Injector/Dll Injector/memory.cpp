#include "memory.h"

#include <TlHelp32.h>

void Memory::attach(const wchar_t* szProcessName, DWORD dwAccessRights)
{
	PROCESSENTRY32W processEntry;
	HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hProcess == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Error: invalid process entry handle for snapshots!");

	processEntry.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(hProcess, &processEntry))
	{
		CloseHandle(hProcess);
		throw std::runtime_error("Error: could not enumerate through processes!");
	}

	m_szProcessName = szProcessName; // set process name

	do
	{
		if (lstrcmpW(szProcessName, processEntry.szExeFile) == 0)
		{
			CloseHandle(hProcess);
			m_hProcess = OpenProcess(dwAccessRights, false, processEntry.th32ProcessID);

			if (m_hProcess == nullptr)
				throw std::runtime_error("Error: could not open process!");

			m_pId = processEntry.th32ProcessID;
			return;
		}
	} while (Process32NextW(hProcess, &processEntry));

	CloseHandle(hProcess);
	throw std::runtime_error("Error: could not find process!");
}

Module Memory::getModuleInfo(const wchar_t* szModuleName)
{
	MODULEENTRY32W moduleEntry;
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pId);

	if (hModule == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Error: invalid Handle value!");

	moduleEntry.dwSize = sizeof(moduleEntry);

	if (!Module32FirstW(hModule, &moduleEntry))
	{
		CloseHandle(hModule);
		throw std::runtime_error("Error: could not enumerate modules through process!");
	}

	if (szModuleName == nullptr)
		szModuleName = m_szProcessName;

	do
	{
		if (lstrcmpW(moduleEntry.szModule, szModuleName) == 0)
		{
			CloseHandle(hModule);
			return { reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr), moduleEntry.dwSize };
		}
	} while (Module32NextW(hModule, &moduleEntry));

	CloseHandle(hModule);
	throw std::runtime_error("Error: could not find module!");
}

std::wostream& operator<<(std::wostream& stream, const Memory& other)
{
	return stream << "Process name: " << other.m_szProcessName << " process identifier: " << other.m_pId;
}
