#pragma once
#include "undefined.h"
//function protos for driver hooking
//get address of specific kernel module
PVOID GetSystemModuleBase(const char* module_name);
//get address of function from specific kernel module
PVOID GetSystemModuleExport(const char* module_name, PCSTR routine_name);
//The rest
bool WriteMemory(PVOID address, PVOID buffer, size_t size);
bool WriteToReadOnly(PVOID address, PVOID buffer, size_t size);
void debug_print(PCSTR text);