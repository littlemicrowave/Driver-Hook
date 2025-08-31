#include "hook.h"
//functions are on hfiref0x/NtCall64 github
bool hook::CallKernelFunction(PVOID FunctionAddrress)
{
	if (!FunctionAddrress)
		return false;
	//Here is used NtQueryCompositionSurfaceStatistics function, but can be used any function Nt and NtD function without security cookie
	PVOID function = GetSystemModuleExport("\\SystemRoot\\System32\\drivers\\dxgkrnl.sys", "NtQueryCompositionSurfaceStatistics");
	if (!function)
		return false;
	//we will rewrite 12 bytes (8 bytes of address will be moved to rax and then jump)
	BYTE orig[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	//TODO: CHANGE OPCODES !!!!!!!!!!!!!!!!
	//Our Code 
	//Detected by EAC and Battleye (change signature to anther opcodes to stay safe)
	BYTE code_start[] = { 0x48, 0xB8 }; //mov rax
	BYTE code_end[] = { 0xFF, 0xE0 }; //jmp rax
	//setting up orig with our shellcode
	RtlSecureZeroMemory(orig, sizeof(orig));
	memcpy((PVOID)((DWORD64)orig), code_start, sizeof(code_start));
	DWORD64 hookAddr = (DWORD64)FunctionAddrress;
	memcpy((PVOID)((DWORD64)orig + sizeof(code_start)), &hookAddr, sizeof(hookAddr));
	memcpy((PVOID)((DWORD64)orig + sizeof(code_start) + sizeof(hookAddr)), code_end, sizeof(code_end));
	//hooking kernel function
	WriteToReadOnly(function, orig, sizeof(orig));
	return true;
}

NTSTATUS hook::hookHandler(PVOID CalledParam)
{
	return STATUS_SUCCESS;
}