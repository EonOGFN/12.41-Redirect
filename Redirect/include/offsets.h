#pragma once


namespace offsets {
	constexpr unsigned long long GWorld = 0x8158708;
	constexpr unsigned long long OwningGameInstance = 0x0188;
	constexpr unsigned long long LocalPlayers = 0x0038;
	constexpr unsigned long long PlayerController = 0x30;
	constexpr unsigned long long TargetedBuilding = 0x1278;

	constexpr unsigned long long PerformBuildingEditInteraction = 0x194B0D8;
	constexpr unsigned long long BuildingEditReset = 0x22EFB4F;
	constexpr unsigned long long CompleteBuildingEditInteraction = 0x22F4500;
}

#define GET_AS(addr, offset) *reinterpret_cast<uint64_t*>((uint64_t)addr + offset)