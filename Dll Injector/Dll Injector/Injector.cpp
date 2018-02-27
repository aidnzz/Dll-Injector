#include "Injector.h"

#include <Windows.h>
#include <stdexcept>

#define wsizeof(wString) (lstrlenW(wString) * sizeof(wchar_t)) + 1 

void Injector::inject(const wchar_t* dllPath) const
{
	wchar_t buffer[MAX_PATH + 1] = {  };

	GetFullPathNameW(dllPath, MAX_PATH, buffer, nullptr);
	const size_t nSize = wsizeof(buffer);

	LPVOID loadLibAddr = GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryW"); // Gets address of loadlibraryw for unicode support when loading dll path
	LPVOID lpDllPath = VirtualAllocEx(hProcess_, nullptr, nSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (lpDllPath == nullptr)
		throw std::runtime_error("Error reserving process memory!");

	if (!writeBuffer((DWORD)lpDllPath, buffer, nSize))
	{
		VirtualFreeEx(hProcess_, lpDllPath, 0, MEM_RELEASE);
		throw std::runtime_error("Error writing dll path to process");
	}

	HANDLE hRemoteThread = CreateRemoteThread(hProcess_, nullptr, nSize, (LPTHREAD_START_ROUTINE)loadLibAddr, lpDllPath, NULL, nullptr);
	if (hRemoteThread == nullptr)
	{
		VirtualFreeEx(hProcess_, lpDllPath, 0, MEM_RELEASE);
		throw std::runtime_error("Error creating remote thread!");
	}

	CloseHandle(hRemoteThread);

	if (!VirtualFreeEx(hProcess_,
		lpDllPath,
		0,
		MEM_RELEASE))
	{
		throw std::runtime_error("Error freeing memory from process!");
	}
}
