#include "Injector.h"

#include <iostream>

void Injector::inject(wchar_t* dllPath) const
{
	size_t size = (lstrlenW(dllPath) * 2) + 1; // Unicode string so double the bytes and the addition byte for the null byte

	LPVOID loadLibAddr = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryW"); // LoadLibraryW for loading unicode strings
	LPVOID pDllPath = VirtualAllocEx(hProcess_, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // Allocates memory on heap in remote process with VirtualAllocEx

	if (!pDllPath)
		throw std::runtime_error("Error reserving memory in target process!");

	if (!writeBuffer<wchar_t*>(dllPath, (DWORD)pDllPath, size)) // Write process name in allocated memory
	{
		VirtualFreeEx(hProcess_, pDllPath, 0, MEM_RELEASE); // Free reserved heap memory
		throw std::runtime_error("Error writing to process!");
	}

	HANDLE remoteThread = CreateRemoteThread(hProcess_, nullptr, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, pDllPath, 0, nullptr);

	if (remoteThread == nullptr)
	{
		VirtualFreeEx(hProcess_, pDllPath, 0, MEM_RELEASE);
		throw std::runtime_error("Invalid handle in inject function!");
	}

	std::cout << "[*] Excecuting dll in target process!" << std::endl;

	WaitForSingleObject(remoteThread, INFINITE); // Wait for remote thread to finish executing

	if (!VirtualFreeEx(hProcess_, pDllPath, 0, MEM_RELEASE))
	{
		CloseHandle(remoteThread);
		throw std::runtime_error("Error freeing memory from process!");
	}

	CloseHandle(remoteThread);
}