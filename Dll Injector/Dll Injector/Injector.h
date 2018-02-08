#pragma once

#include "Memory.h"

class Injector : public Memory
{
public:
	void inject(const wchar_t* dllPath) const;
};