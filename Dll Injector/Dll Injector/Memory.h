#pragma once

#include <Windows.h>
#include <iostream>
#include <stdexcept>
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

	template<typename T = int>
	T ReadProcess(const DWORD& addr, const size_t size) const;

	template<typename T = int>
	bool Write(const T& data, const DWORD& addr, const size_t size) const;

	template<typename T = char>
	bool WriteBuffer(const T* const data, const DWORD& addr, const size_t size) const;

protected:
	DWORD pId_;
	HANDLE processHandle_;
	const char* processName_;

private:
	friend std::ostream& operator<<(std::ostream& stream, Memory &obj)
	{
		stream << "Process name: " << obj.processName_ << " process identifier: " << std::hex << obj.pId_;
		return stream;
	}

};

class DllInject : public Memory
{
public:
	void Inject(const char* const dllPath) const;
};

