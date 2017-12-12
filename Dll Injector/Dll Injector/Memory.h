#pragma once

#include <Windows.h>
#include <stdexcept>
#include <iostream>
#include <TlHelp32.h>

class Memory
{
public:
	~Memory()
	{
		if (processHandle_ == 0)
			CloseHandle(processHandle_);
	}

	void Attach(const char* const processName, DWORD rights = PROCESS_ALL_ACCESS);
	DWORD GetModuleBaseAddr(const char* moduleName) const;

protected:
	DWORD pId_;
	HANDLE processHandle_;
	const char* processName_;

private:
	friend std::ostream& operator<<(std::ostream& stream, Memory& obj)
	{
		stream << "The process name: " << obj.processName_ << " The process identifier: " << obj.pId_;
		return stream;
	}

};

class DllInjector : public Memory
{
public:
	void Inject(const char* const dllFile);
};