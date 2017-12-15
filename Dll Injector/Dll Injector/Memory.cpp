#include "Memory.h"

void Memory::Attach(const char* const processName, DWORD rights)
{
	PROCESSENTRY32 processEntry;
	HANDLE processHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (processHandle == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Invalid handle snapshot!");

	processEntry.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32First(processHandle, &processEntry))
	{
		CloseHandle(processHandle);
		throw std::runtime_error("Process32First error");
	}

	processName_ = processName;

	do
	{
		if (lstrcmp(processName, processEntry.szExeFile) == 0)
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
		throw std::runtime_error("Invalid Handle value!");

	moduleEntry.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(moduleHandle, &moduleEntry))
	{
		CloseHandle(moduleHandle);
		throw std::runtime_error("Module32First error!");
	}

	if (moduleName == nullptr)
		moduleName = processName_;

	do
	{
		if (lstrcmp(moduleEntry.szModule, moduleName) == 0)
		{
			CloseHandle(moduleHandle);
			return (DWORD)moduleEntry.modBaseAddr;
		}
	} while (Module32Next(moduleHandle, &moduleEntry));

	CloseHandle(moduleHandle);
	throw std::runtime_error("Could not find process!");
}

template<typename T>
T Memory::ReadProcess(const DWORD& addr, const size_t size) const
{
	T data;
	ReadProcessMemory(processHandle_, (LPVOID)addr, &data, size, NULL)
	return data;
}

template<typename T>
bool Memory::Write(const T& data, const DWORD& addr, const size_t size) const
{
	if (!WriteProcessMemory(processHandle_, (LPVOID)addr, &data, size, NULL))
		return false;
	return true;
}

template<typename T>
bool Memory::WriteBuffer(const T* const data, const DWORD& addr, const size_t size) const
{
	if (!WriteProcessMemory(processHandle_, (LPVOID)addr, data, size, NULL))
		return false;
	return true;
}

void DllInject::Inject(const char* const dllPath) const
{
	size_t size = strlen(dllPath) + 1;

	LPVOID loadLibAddr = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
	LPVOID pDllPath = VirtualAllocEx(processHandle_, 0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!pDllPath)
		throw std::runtime_error("Error reserving memory in target process!");

	std::cout << "Dll path: " << pDllPath << std::endl;
	std::cin.get();

	if (!WriteBuffer<char>(dllPath, (DWORD)pDllPath, size))
		throw std::runtime_error("Error writing to process!");

	HANDLE remoteThread = CreateRemoteThread(processHandle_, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibAddr, pDllPath, 0, 0);

	if (remoteThread == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Invalid handle in inject function!");

	WaitForSingleObject(remoteThread, INFINITE);

	if (!VirtualFreeEx(processHandle_, pDllPath, NULL, MEM_RELEASE))
	{
		CloseHandle(remoteThread);
		throw std::runtime_error("Error freeing memory from process!");
	}

	CloseHandle(remoteThread);
}