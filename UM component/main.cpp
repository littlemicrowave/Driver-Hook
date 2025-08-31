#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <memory>
#include <string_view>
#include <cstdint>
#include <vector>

typedef struct _REQUEST
{
	PVOID buffer;
	DWORD64 addrress;
	SIZE_T size;
	DWORD pid;
	BOOL read;
	BOOL write;
	BOOL getBase;
	PVOID output;
	const char* moduleName;
} REQUEST;

DWORD64 baseAddr = 0;
static DWORD processID = 0;

template<typename ...Arg> 
DWORD64 callHook(const Arg... args)
{
	PVOID hookedFunc = GetProcAddress(LoadLibraryA("win32u.dll"), "NtQueryCompositionSurfaceStatistics");
	auto func = static_cast<DWORD64(_stdcall*)(Arg...)>(hookedFunc);

	return func(args...);
}

struct HandleDisposer
{
	using pointere = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
			CloseHandle(handle);
	}
};

using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;
static DWORD getProcessID(std::string_view process_name)
{
	PPROCESSENTRY32 pEntry;
	const unique_handle snaphot_handle = std::make_unique<CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL)>;

	while(Process32Next())

}