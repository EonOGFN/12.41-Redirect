#pragma once

#include <string>
#include <intrin.h>
#include <defines.h>

namespace UE4 {
	unsigned long long GetObjects();
	unsigned long long GetAppendStringAddr();
}

struct FName {
	__int32 ComparisonIndex;
	__int32 Number;

	FName() : ComparisonIndex(0), Number(0) { }

	FName(__int32 comparisonIndex, __int32 number) : ComparisonIndex(comparisonIndex), Number(number) { }

	FName(const wchar_t* Name) {
		uintptr_t base_address = *(unsigned long long*)(__readgsqword(0x60) + 0x10);
		uintptr_t func = base_address + 0x02C18800;

		reinterpret_cast<void(*)(FName*, const wchar_t*, bool)>(func)(this, Name, true);

		LOG("FName : 0x{:X} : {}\n", (unsigned long long)Name, ComparisonIndex);
	}


	std::string ToString();
};

class UObjectBase {
public:

	void** VTable;

	/** Flags used to track and report various object states. This needs to be 8 byte aligned on 32-bit
	platforms to reduce memory waste */
	__int32					ObjectFlags;

	/** Index into GObjectArray...very private. */
	__int32							InternalIndex;

	/** Class the object belongs to. */
	struct UClass* ClassPrivate;

	/** Name of this object */
	FName							NamePrivate;

	/** Object this object resides in. */
	class UObjectBase* OuterPrivate;

	bool IsA(UClass* cmp);

	std::string GetName();
	std::string GetFullName();

	static struct UClass* FindClass(const std::string& name);

	static UObjectBase* FindObject(const std::string& name);
};

struct UClass : public UObjectBase {
	inline UClass* get_super() { return *reinterpret_cast<UClass**>((uint64_t)this + 0x40); };
};

struct FUObjectItem
{
	UObjectBase* Object;
	__int32 Flags;
	__int32 ClusterRootIndex;
	__int32 SerialNumber;
};

class FChunkedFixedUObjectArray
{
public:
	FUObjectItem** Objects; // Array of pointers
	FUObjectItem* PreAllocatedObjects;
	__int32 MaxElements;
	__int32 NumElements;
	__int32 MaxChunks;
	__int32 NumChunks;

	__int32 Num() {
		return NumElements;
	}

	UObjectBase* GetObjectPtr(__int32 Index);
};


namespace engine {
	namespace signatures {
		constexpr const char* ENGINE_SIG = "41 B8 01 00 00 00 ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 85 C0";
	}

	unsigned long long GetEngine();
}

namespace ue4 {
	unsigned long long get_player_controller();
}