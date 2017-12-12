#include "Memory.h"

void Memory::Attach(const char* const processName, DWORD rights)
{
	PROCESSENTRY32 processEntry;
	HANDLE processHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (processHandle == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Invalid handle in Memory::Attach");

	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(processHandle, &processEntry))
	{
		CloseHandle(processHandle);
		throw std::runtime_error("Error enumerating through processes!");
	}

	processName_ = processName;

	do
	{
		if (strcmp(processName, processEntry.szExeFile) == 0)
		{
			pId_ = processEntry.th32ProcessID;
			processHandle_ = OpenProcess(rights, NULL, pId_);
			CloseHandle(processHandle);
			return;
		}
	} while (Process32Next(processHandle, &processEntry));

	CloseHandle(processHandle);
	throw std::runtime_error("Could not find process!");
}

DWORD Memory::GetModuleBaseAddr(const char* moduleName) const
{
	MODULEENTRY32 moduleEntry;
	HANDLE moduleHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pId_);

	if (moduleHandle == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Invalid handle in Memory::GetModuleBaseAddr");

	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(moduleHandle, &moduleEntry))
	{
		CloseHandle(moduleHandle);
		throw std::runtime_error("Error enumerating through modules!");
	}

	if (moduleName == nullptr)
		moduleName = processName_;

	do
	{
		if (strcmp(moduleName, moduleEntry.szExePath))
		{
			CloseHandle(moduleHandle);
			return (DWORD)moduleEntry.modBaseAddr;
		}
	} while (Module32Next(moduleHandle, &moduleEntry));

	CloseHandle(moduleHandle);
	throw std::runtime_error("Could not find module!");
}

void DllInjector::Inject(const char* const dllFile)
{
	LPVOID pDllPath = VirtualAllocEx(processHandle_, NULL, strlen(dllFile) + 1, MEM_COMMIT, PAGE_READWRITE);
	
	if (!pDllPath)
		throw std::runtime_error("Error reserving memory in target process");

	if (!WriteProcessMemory(processHandle_, pDllPath, (LPVOID)dllFile, strlen(dllFile) + 1, NULL))
		throw std::runtime_error("Error writing dll file path to process");

	HANDLE remoteThread = CreateRemoteThread(processHandle_, NULL, NULL, 
			(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA")
			, pDllPath, NULL, NULL);

	if (remoteThread == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Invalid handle in Inject function!");

	std::cout << "  [*] Waiting for dll to finish executing!" << std::endl;
	WaitForSingleObject(remoteThread, INFINITE);

	std::cout << "  [+] Dll finished excecuting!" << std::endl;
	if (!VirtualFree(pDllPath, NULL, MEM_RELEASE))
	{
		CloseHandle(pDllPath);
		throw std::runtime_error("Error freeing memory from process!");
	}

	CloseHandle(processHandle_);
}
