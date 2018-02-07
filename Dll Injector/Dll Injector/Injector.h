#pragma once

#include "Memory.h"

class Injector : public Memory
{
public:
	void inject(wchar_t* dllPath) const;
};