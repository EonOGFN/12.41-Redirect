#pragma once

#include <utility>
#include <unordered_map>

class UObjectBase;

namespace signatures {
	constexpr const char* GetViewPoint = "4C 8B DC 49 89 5B 20 55 56 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 41 0F";
}

namespace offsets {
	constexpr unsigned long long FOV = 0x18;
	constexpr unsigned long long PlayerController = 0x30;
	constexpr unsigned long long Pawn = 0x0298; //AcknowledgedPawn
	constexpr unsigned long long CurrentWeapon = 0x0588; //AcknowledgedPawn
	constexpr unsigned long long WeaponData = 0x0358;
}

template <typename T>
struct PointerHash {
	static_assert(std::is_pointer<T>::value, "T must be a pointer type");

	std::size_t operator()(T const& s) const noexcept
	{
		return (std::size_t)(s); //the pointer itself is the hash
	}
};

namespace custom_fov {
	bool enable_custom_fov(void);

	UObjectBase* get_current_weapon(void* localplayer);

	void __fastcall GetViewPointHook(void* _this, void* out, unsigned char StereoPass);

	inline void(__fastcall* GetViewPointOriginal)(void*, void*, unsigned char) = nullptr;

	inline std::unordered_map<void*, bool, PointerHash<void*>> FortWeapons;
}