#include <null_func.h>
#include <Windows.h>
#include <defines.h>
#include <iostream>

#include <lazy_importer.hpp>

bool patcher::null_func(unsigned long long offset)
{
	unsigned long long base_address = *(unsigned long long*)(__readgsqword(0x60) + 0x10);

	unsigned long long address = base_address + offset;

	LOG("Patching address: 0x{:X}", address);

	DWORD oldProtect;
	bool success = LI_FN(VirtualProtect).safe()((void*)address, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
	if (!success)
		return false;

	*(unsigned char*)address = 0xC3;
	LI_FN(VirtualProtect).safe()((void*)address, 1, oldProtect, &oldProtect);

	return true;
}