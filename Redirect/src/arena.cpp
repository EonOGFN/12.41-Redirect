#include <arena.h>
#include <defines.h>

void playlist::apply_playlist_extension(void* playlist, FUIExtension extension)
{
	UC::TArray<FUIExtension>* arr = reinterpret_cast<UC::TArray<FUIExtension>*>((unsigned long long)playlist + UIExtensionsOffset);
	LOG("Data : {:X}", (unsigned long long)arr->GetData());
	LOG("Max: {}", arr->Max());
	LOG("Num: {}", arr->Num());

	UC::TAllocatedArray<FUIExtension>* allocated_array = new UC::TAllocatedArray<FUIExtension>(2);
	if (!allocated_array->Add(extension))
	{
		LOG("Failed to add extension to playlist");
		return;
	}

	arr->replace(allocated_array);

	LOG("Added extension to playlist");
	LOG("Data : {:X}", (unsigned long long)arr->GetData());
	LOG("Max: {}", arr->Max());
	LOG("Num: {}", arr->Num());
}

void playlist::init()
{
	FUIExtension extension;
	extension.Slot = EUIExtensionSlot::Primary;
	FName name = FName(xorstr_(L"/Game/UI/Competitive/Arena/ArenaScoringHUD.ArenaScoringHUD_C"));
	
	LOG("ArenaScoringHUD.ArenaScoringHUD_C: {}", name.ComparisonIndex);
	
	extension.WidgetClass.ObjectID.AssetPathName = name;

	void* first_playlist = UObjectBase::FindObject(xorstr_("FortPlaylistAthena Playlist_ShowdownAlt_Solo.Playlist_ShowdownAlt_Solo"));
	LOG("Playlist_ShowdownAlt_Solo: 0x{:X}", (unsigned long long)first_playlist);

	apply_playlist_extension(first_playlist, extension);
}
