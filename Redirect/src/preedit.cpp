#include <disable_pre_edit.h>

#include <string>
#include <Windows.h>
#include <minhook/MinHook.h>

#include <UnrealContainers.hpp>
#include <ue4.h>
#include <defines.h>
#include <offsets.h>

#include <lazy_importer.hpp>
#include <settings.h>
#include <memcury.h>

#pragma comment(lib, "lib/minhook/minhook.lib")

bool preedit::disable_reset_confirm(void)
{
	LOG("Disabling Reset Confirm");
	unsigned long long base_address = *(unsigned long long*)(__readgsqword(0x60) + 0x10);
	unsigned long long BuildingEditReset = Memcury::Scanner::FindStringRef(L"EditModeInputComponent0")
		.ScanFor({ 0x48, 0x8d, 0x05 }, true, 2)
		.RelativeOffset(3)
		.Get();


	preedit::complete_building_edit_interaction = reinterpret_cast<decltype(preedit::complete_building_edit_interaction)>(base_address + offsets::CompleteBuildingEditInteraction);

	MH_STATUS create_hook_status = MH_CreateHook((void*)BuildingEditReset, building_edit_reset_hook, (LPVOID*)&preedit::building_edit_reset);
	if (create_hook_status != MH_OK)
	{
		LOG("Failed to create hook: {}", MH_StatusToString(create_hook_status));
		return false;
	}

	MH_STATUS enable_hook_status = MH_EnableHook((void*)BuildingEditReset);
	if (enable_hook_status != MH_OK)
	{
		LOG("Failed to enable hook: {}", MH_StatusToString(enable_hook_status));
		return false;
	}

	return true;
}

bool preedit::disable_preedits(void)
{
	unsigned long long base_address = *(unsigned long long*)(__readgsqword(0x60) + 0x10);
	unsigned long long address = base_address + offsets::PerformBuildingEditInteraction;

	MH_STATUS create_hook_status = MH_CreateHook((void*)address, perform_building_edit_interaction_hook, (LPVOID*)&preedit::perform_building_edit_interaction);
	if (create_hook_status != MH_OK)
	{
		LOG("Failed to create hook: {}", MH_StatusToString(create_hook_status));
		return false;
	}

	MH_STATUS enable_hook_status = MH_EnableHook((void*)address);
	if (enable_hook_status != MH_OK)
	{
		LOG("Failed to enable hook: {}", MH_StatusToString(enable_hook_status));
		return false;
	}

	return true;
}

__int64 __fastcall preedit::perform_building_edit_interaction_hook(void* This)
{
	
		static UClass* BuildingPlayerPrimitivePreviewClass = UObjectBase::FindClass("Class FortniteGame.BuildingPlayerPrimitivePreview");

		if (settings::b_use_disable_preedit) {
			auto targeted_building = get_targeted_building();
			if (targeted_building && targeted_building->IsA(BuildingPlayerPrimitivePreviewClass))
			{
				return 0;
			}
		}



	return preedit::perform_building_edit_interaction(This);
}

__int64 __fastcall preedit::building_edit_reset_hook(void* This)
{
	__int64 result = preedit::building_edit_reset(This);

	if (settings::b_use_reset_on_release) {
		preedit::complete_building_edit_interaction(This);
	}

	return result;
}

UObjectBase* preedit::get_targeted_building()
{

	auto player_controller = ue4::get_player_controller();
	if (!player_controller)
	{
		return nullptr;
	}

	auto targeted_building = (UObjectBase*)GET_AS(player_controller, offsets::TargetedBuilding);
	return targeted_building;
}
