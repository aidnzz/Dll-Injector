#include "injector.h"
#include <iostream>

#define LOADLIBRARYW  "LoadLibraryW"
#define KERNEL32      "Kernel32.dll"

void Injector::inject(const wchar_t* dllPath) const
{
    WCHAR szFullPath[261]; 
    UINT nSize = sizeof(szFullPath); // Get size of buffer in bytes

    ZeroMemory(szFullPath, nSize);
    GetFullPathNameW(dllPath, 260, szFullPath, nullptr);

    HMODULE hModule = GetModuleHandleA(KERNEL32);
    LPVOID addr = GetProcAddress(hModule, LOADLIBRARYW); 

    LPVOID lpParam = VirtualAllocEx(m_hProcess, nullptr, nSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // Allocate Memory for buffer

    if (lpParam == nullptr)
        throw std::runtime_error("Error: could not reserving memory in process!");

    std::cout << "\n[+] Allocated " << nSize << " bytes of memory in process" << '\n';

    if (!writeBuffer(reinterpret_cast<uintptr_t>(lpParam), szFullPath, nSize))
    {
        VirtualFreeEx(m_hProcess, addr, 0, MEM_RELEASE);
        throw std::runtime_error("Error: could not write dll path to process");
    }

    std::cout << "[+] Wrote DLL path to " << std::hex << reinterpret_cast<uintptr_t>(addr) << " in process memory" << '\n';

    HANDLE hThread = CreateRemoteThread(m_hProcess, nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(addr), lpParam, 0, nullptr);

    if (hThread == nullptr)
    {
        VirtualFreeEx(m_hProcess, addr, 0, MEM_RELEASE);
        throw std::runtime_error("Error: could not create remote thread!");
    }

    std::cout << "[+] Executing DLL in target process! " << '\n';
    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(m_hProcess, addr, 0, MEM_RELEASE);
    CloseHandle(hThread);
}
