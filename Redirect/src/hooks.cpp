#include <defines.h>
#include <hooks.h>
#include <raax_patch.h>
#include <intrin.h>
#include <minhook/MinHook.h>
#include <lazy_importer.hpp>


void hooks::UnsafeEnvironmentPopupHook(wchar_t** unknown1, unsigned __int8 _case, __int64 unknown2, char unknown3)
{
	LOG("UnsafeEnvironmentPopupHook");
}

__int64 hooks::RequestExitWithStatusHook(bool Force, unsigned char ReturnCode)
{
	LOG("RequestExitWithStatusHook force : {}, retcode : {}", (int)Force, (int)ReturnCode);

	return 0;
}

void hooks::null_func()
{
	LOG("null_func called");


	MEMORY_BASIC_INFORMATION mbi;
	LI_FN(VirtualQuery)((void*)_ReturnAddress(), (MEMORY_BASIC_INFORMATION*) & mbi, (SIZE_T)sizeof(mbi));

	LOG("null_func called from : 0x{:X}", (unsigned long long)mbi.BaseAddress);

	if (mbi.Protect == 0x40 && mbi.AllocationProtect == 0x40)
	{
		LI_FN(VirtualFree)(mbi.AllocationBase, mbi.RegionSize, mbi.Type); // free the memory
	}
}

void hooks::place()
{
	LOG("Placing hooks");

	unsigned long long base_address = *(unsigned long long*)(__readgsqword(0x60) + 0x10);

	for (auto& offset : raax_patch::RAAX_CRASH_PATCH_OFFSETS)
	{
		unsigned long long address = base_address + offset;

		MH_CreateHook((void*)address, hooks::null_func, nullptr);
	}

	MH_CreateHook((void*)(base_address + UnsafeEnvironmentPopup), hooks::UnsafeEnvironmentPopupHook, nullptr);
	MH_CreateHook((void*)(base_address + RequestExit), hooks::RequestExitWithStatusHook, nullptr);

	MH_EnableHook(MH_ALL_HOOKS);

	LOG("Placed hooks");
}
