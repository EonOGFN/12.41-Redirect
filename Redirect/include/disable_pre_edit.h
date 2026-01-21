#pragma once

class UObjectBase;

namespace preedit {
	bool disable_reset_confirm(void);

	bool disable_preedits(void);

	inline __int64(__fastcall* perform_building_edit_interaction)(void* This) = nullptr;
	inline __int64(__fastcall* building_edit_reset)(void* This) = nullptr;
	inline char(__fastcall* complete_building_edit_interaction)(void* This) = nullptr;

	__int64 __fastcall perform_building_edit_interaction_hook(void* This);

	__int64 __fastcall building_edit_reset_hook(void* This);

	UObjectBase* get_targeted_building();
}