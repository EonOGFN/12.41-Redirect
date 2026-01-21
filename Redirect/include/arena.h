#pragma once
#include <UnrealContainers.hpp>
#include <ue4.h>

class FWeakObjectPtr
{
public:
	int                                         ObjectIndex;                                       // 0x0000(0x0004)(NOT AUTO-GENERATED PROPERTY)
	int                                         ObjectSerialNumber;                                // 0x0004(0x0004)(NOT AUTO-GENERATED PROPERTY)

};

struct FSoftObjectPath
{
public:
	FName                                   AssetPathName;                                     // 0x0000(0x0008)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UC::FString                                 SubPathString;                                     // 0x0008(0x0010)(ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

class FSoftObjectPtr
{
public:
	FWeakObjectPtr                                WeakPtr;                                           // 0x0000(0x0008)(NOT AUTO-GENERATED PROPERTY)
	int                                         TagAtLastTest;                                     // 0x0008(0x0004)(NOT AUTO-GENERATED PROPERTY)
	FSoftObjectPath                                     ObjectID;                                          // 0x000C(0x0000)(NOT AUTO-GENERATED PROPERTY)
};


enum class EUIExtensionSlot : unsigned char
{
	Primary = 0,
	TopRightCorner = 1,
	GameInfoBox = 2,
	Quickbar = 3,
	UpperCenter = 4,
	CrosshairRight = 5,
	UnderSquadInfo = 6,
	FullScreenMap = 7,
	BelowRespawnWidget = 8,
	BelowCompass = 9,
	UnderTeammateStatus = 10,
	ControllerBindingCallout = 11,
	AboveStormMessageSlot = 12,
	CustomMinigameCallouts = 13,
	UnderLocalPlayerInfo = 14,
	InventoryScreenReplacement = 15,
	Reticle = 16,
	KillfeedSlot = 17,
	PrioritizedContextualSlot = 18,
	MobileHUDBottomRight = 19,
	EUIExtensionSlot_MAX = 20,
};

struct FUIExtension {
	EUIExtensionSlot                  Slot;                                              // 0x0(0x1)(Edit, ZeroConstructor, DisableEditOnInstance, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	unsigned char                                        Pad_50B3[0x7];                                     // Fixing Size After Last Property  [ Dumper-7 ]
	FSoftObjectPtr             WidgetClass;                                       // 0x8(0x28)(Edit, DisableEditOnInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};



namespace playlist {
	constexpr unsigned long long UIExtensionsOffset = 0x06D0;



	void apply_playlist_extension(void* playlist, FUIExtension extension);

	void init();

	static bool enabled_arena = false;
}