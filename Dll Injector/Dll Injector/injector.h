#pragma once

#include "memory.h"

struct Injector : public Memory
{
	void inject(const wchar_t* dllPath) const;
};
