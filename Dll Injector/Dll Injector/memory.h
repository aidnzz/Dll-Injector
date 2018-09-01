#pragma once

#include <Windows.h>
#include <ostream> // wostream

struct Module
{
    uintptr_t baseAddr;
    DWORD dwSize;
};


class Memory
{
public:
    Memory() = default;

    ~Memory()
    {
        if (m_hProcess != nullptr || m_hProcess != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_hProcess);
            m_hProcess = nullptr;
        }
    }

    void attach(const wchar_t* szProcessName, DWORD dwAccessRights = PROCESS_ALL_ACCESS);
    Module getModuleInfo(const wchar_t* szModuleName);
    
    inline bool writeBuffer(uintptr_t addr, LPCVOID buffer, uint32_t nSize) const
    {
        return WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(addr), buffer, nSize, nullptr);
    }

    
    template<typename T>
    inline bool write(uintptr_t addr, T data, uint32_t nSize = sizeof(T)) const
    {
        return WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(addr), &data, nSize, nullptr);
    }

    template<typename T> 
    T read(uintptr_t addr) const
    {
        T data;
        ReadProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(addr), &data, sizeof(data), nullptr);
        return data;
    }

protected:
    DWORD m_pId;
    HANDLE m_hProcess;

private:
    const wchar_t* m_szProcessName;
    friend std::wostream& operator<<(std::wostream&, const Memory&);

};
