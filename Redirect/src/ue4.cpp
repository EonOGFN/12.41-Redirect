#include <ue4.h>
#include <memcury.h>

#include <UnrealContainers.hpp>
#include <offsets.h>

UObjectBase* FChunkedFixedUObjectArray::GetObjectPtr(__int32 Index)
{
	constexpr unsigned long long NumElementsPerChunk = 64 * 1024;
	const __int32 ChunkIndex = Index / NumElementsPerChunk;
	const __int32 WithinChunkIndex = Index % NumElementsPerChunk;
	FUObjectItem* Chunk = Objects[ChunkIndex];
	if (!Chunk) {
		return nullptr;
	}

	FUObjectItem* Object = Chunk + WithinChunkIndex;
	return Object->Object;


}

bool UObjectBase::IsA(UClass* cmp)
{
	for (auto super = this->ClassPrivate; super; super = static_cast<UClass*>(super->get_super()))
	{
		/*		if (!check_pointer((u64)super))
			continue;*/

		if (super == cmp)
		{
			return true;
		}
	}

	return false;
}

std::string UObjectBase::GetName()
{
	std::string name(NamePrivate.ToString());
	if (NamePrivate.Number > 0)
	{
		name += '_' + std::to_string(NamePrivate.Number);
	}

	auto pos = name.rfind('/');
	if (pos == std::string::npos)
	{
		return name;
	}

	return name.substr(pos + 1);
}

std::string UObjectBase::GetFullName()
{
	std::string name;


	if (ClassPrivate != nullptr)
	{
		std::string temp;
		for (auto p = OuterPrivate; p; p = p->OuterPrivate)
		{
			temp = p->GetName() + "." + temp;
		}

		name = ClassPrivate->GetName();
		name += " ";
		name += temp;
		name += GetName();
	}

	return name;
}

struct UClass* UObjectBase::FindClass(const std::string& name)
{
	FChunkedFixedUObjectArray* ObjObjects = (FChunkedFixedUObjectArray*)UE4::GetObjects();
	for (int i = 0; i < ObjObjects->Num(); i++) {
		auto object = ObjObjects->GetObjectPtr(i);

		if (!object)
			continue;

		if (object->GetFullName() == name) {
			return (UClass*)object;
		}
	}

	return nullptr;
}

UObjectBase* UObjectBase::FindObject(const std::string& name)
{
	FChunkedFixedUObjectArray* ObjObjects = (FChunkedFixedUObjectArray*)UE4::GetObjects();
	for (int i = 0; i < ObjObjects->Num(); i++) {
		auto object = ObjObjects->GetObjectPtr(i);

		if (!object)
			continue;

		if (object->GetFullName() == name) {
			return (UObjectBase*)object;
		}
	}

	return nullptr;
}

unsigned long long UE4::GetObjects()
{
	static unsigned long long GObjects = Memcury::Scanner::FindPattern("48 8D 14 40 48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB 03").AbsoluteOffset(4).RelativeOffset(3).Get();
	return GObjects;
}

unsigned long long UE4::GetAppendStringAddr()
{
	static unsigned long long AppendString = Memcury::Scanner::FindStringRef("ForwardShadingQuality_").ScanFor({ 0xE8 }, true, 1).RelativeOffset(1).Get();
	return AppendString;
}

std::string FName::ToString()
{
	UC::FAllocatedString TempString(2048);

	if (UE4::GetAppendStringAddr()) {
		reinterpret_cast<void(*)(void*, UC::FString&)>(UE4::GetAppendStringAddr())(this, TempString);

		std::string result = TempString.ToString();

		//fnamecache.insert(std::make_pair(this->ComparisonIndex, result));

		return result;
	}

	return std::string();
}

unsigned long long engine::GetEngine()
{
	static unsigned long long* GEngine = Memcury::Scanner::FindPattern(signatures::ENGINE_SIG).AbsoluteOffset(9).RelativeOffset(3).GetAs<unsigned long long*>();
	return *GEngine;
}

unsigned long long ue4::get_player_controller()
{
	static unsigned long long base_address = *(unsigned long long*)(__readgsqword(0x60) + 0x10);
	uint64_t world = GET_AS(base_address, offsets::GWorld);
	if (!world)
		return 0;

	uint64_t game_instance = GET_AS(world, offsets::OwningGameInstance);
	if (!game_instance)
		return 0;

	UC::TArray<void*>* local_players = reinterpret_cast<UC::TArray<void*>*>(game_instance + offsets::LocalPlayers);
	auto local_player = local_players->operator[](0);
	if (!local_player)
		return 0;

	auto player_controller = GET_AS(local_player, offsets::PlayerController);
	if (!player_controller)
		return 0;

	return player_controller;
}
