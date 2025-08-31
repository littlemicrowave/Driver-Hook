#pragma once
#include "memory.h"
namespace hook{
	bool CallKernelFunction(PVOID FunctionAddress);
	NTSTATUS hookHandler(PVOID calledParam);
}