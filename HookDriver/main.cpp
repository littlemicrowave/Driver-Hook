#include "hook.h"
//ed nt!Kd_IHVDRIVER_Mask 8
//leave DriverEntryAsap

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING reg_path)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(reg_path);

	return hook::CallKernelFunction(&hook::hookHandler);
}