#include "Injector.h"

#include <Windows.h>
#include <stdexcept>

#define wsizeof(wString) (lstrlenW(wString) * sizeof(wchar_t)) + 1 

void Injector::inject(const wchar_t* dllPath) const
{
	const size_t size = wsizeof(dllPath);

	LPVOID loadLibAddr = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryW"); // Gets address of loadlibraryw for unicode support when loading dll path
	LPVOID pDllPath = VirtualAllocEx(hProcess_, nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (pDllPath == nullptr)
		throw std::runtime_error("Error reserving process memory!");

	if (!writeBuffer((DWORD)pDllPath, dllPath, size))
	{
		VirtualFreeEx(hProcess_, pDllPath, 0, MEM_RELEASE);
		throw std::runtime_error("Error writing dll path to process");
	}

	HANDLE remoteThread = CreateRemoteThread(hProcess_, nullptr, size, (LPTHREAD_START_ROUTINE)loadLibAddr, pDllPath, NULL, nullptr);
	if (remoteThread == nullptr)
	{
		VirtualFreeEx(hProcess_, pDllPath, 0, MEM_RELEASE);
		throw std::runtime_error("Error creating remote thread!");
	}

	std::cout << "[*] Waiting for dll to finish executing!" << std::endl;

	WaitForSingleObject(remoteThread, INFINITE);
	CloseHandle(remoteThread);

	if (!VirtualFreeEx(hProcess_, pDllPath, 0, MEM_RELEASE))
		throw std::runtime_error("Error freeing memory from process!");
}