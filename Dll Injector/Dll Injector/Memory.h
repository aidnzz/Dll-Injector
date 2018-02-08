#pragma once

#include <Windows.h>
#include <iostream>

class Memory
{
public:
	Memory() // Make class data members usable
		: pId_(0), hProcess_(nullptr), processName_(nullptr)
	{
	}

	~Memory()
	{
		CloseHandle(hProcess_);
	}

	void attach(const wchar_t* processName, DWORD dwAccessRights = PROCESS_ALL_ACCESS);
	DWORD getbaseAddress(const wchar_t* moduleName); // If a nullptr is passed will use process name as module name

	template<typename T>
	bool readProcess(const DWORD addr, T buffer, const size_t size) const; // Destination, Buffer, Bytes of data

	template<typename T>
	bool write(const DWORD addr, T data, const size_t size) const; // Destination, Data, Bytes of data

	template<typename T>
	bool writeBuffer(const DWORD addr, T data, const size_t size) const; // Destination, Data, Bytes of data

protected:
	DWORD pId_;
	HANDLE hProcess_;

private:
	const wchar_t* processName_;
	friend std::wostream& operator<<(std::wostream& stream, const Memory& obj);

};