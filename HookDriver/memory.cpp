#include "memory.h"
//driver stuff for hooking anther driver function
//communication struct
typedef struct _REQUEST
{
	PVOID buffer;
	DWORD64 addrress;
	SIZE_T size;
	DWORD pid;
	BOOL read;
	BOOL write;
	BOOL base;
	PVOID output;
	const char* moduleName;
} REQUEST;


void debug_print(PCSTR text)
{
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
}

PVOID GetSystemModuleBase(const char* module_name)
{
	ULONG bytes = 0;
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, NULL, bytes, &bytes);

	if (!bytes)
		return nullptr;

	//LEON = 0x4C454F4E for tag
	PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePool2(POOL_FLAG_NON_PAGED, bytes, 0x4C454F4E);
	status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);

	if (status != STATUS_SUCCESS)
		return nullptr;

	PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
	PVOID module_base = 0; 
	PVOID module_size = 0;

	for (unsigned int i = 0; i < modules->NumberOfModules; i++)
	{
		debug_print((char*)module[i].FullPathName); debug_print("\n");
		if (strcmp((char*)module[i].FullPathName, module_name) == 0)
		{
			module_base = module[i].ImageBase;
			module_size = (PVOID)module[i].ImageSize;
			break;
		}
	}
	if (modules)
		ExFreePoolWithTag(modules, 0x4C454F4E);
	if (module_base == 0)
		return nullptr;
	debug_print("[+] modulebase is not null\n");
	return module_base;
}

PVOID GetSystemModuleExport(const char* module_name, PCSTR routine_name)
{
	PVOID lpModule = GetSystemModuleBase(module_name);
	if (!lpModule)
		return nullptr;
	return RtlFindExportedRoutineByName(lpModule, routine_name);
}

bool WriteMemory(PVOID address, PVOID buffer, size_t size)
{
	//Kernel function for writing memory
	if (!RtlCopyMemory(address, buffer, size))
	{
		return false;
	}
	else
		return true;
}

bool WriteToReadOnly(PVOID address, PVOID buffer, size_t size)
{
	//some kernel stuff I don't really understand for writng read only memory
	PMDL Mdl = IoAllocateMdl(address, size, FALSE, FALSE, NULL);
	if (!Mdl)
		return false;
	MmProbeAndLockPages(Mdl, KernelMode, IoReadAccess);
	PVOID Mapping = MmMapLockedPagesSpecifyCache(Mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
	MmProtectMdlSystemAddress(Mdl, PAGE_READWRITE);
	WriteMemory(Mapping, buffer, size);
	MmUnmapLockedPages(Mapping, Mdl);
	MmUnlockPages(Mdl);
	IoFreeMdl(Mdl);
	return true;
}