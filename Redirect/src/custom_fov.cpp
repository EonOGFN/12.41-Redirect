#include <custom_fov.h>
#include <settings.h>
#include <memcury.h>
#include <minhook/MinHook.h>
#include <defines.h>

#include <ue4.h>

bool custom_fov::enable_custom_fov(void)
{
	void* GetViewPoint = Memcury::Scanner::FindPattern(signatures::GetViewPoint).GetAs<void*>();

	MH_STATUS status = MH_CreateHook(GetViewPoint, custom_fov::GetViewPointHook, (LPVOID*)&custom_fov::GetViewPointOriginal);

	if (status != MH_OK)
	{
		LOG("Failed to create hook for GetViewPoint {}", (int)status);
		return false;
	}

	status = MH_EnableHook(GetViewPoint);

	if (status != MH_OK)
	{
		LOG("Failed to enable hook for GetViewPoint {}", (int)status);
		return false;
	}

	return true;
}

void __fastcall custom_fov::GetViewPointHook(void* _this, void* out, unsigned char StereoPass)
{
	custom_fov::GetViewPointOriginal(_this, out, StereoPass);

	if (!settings::b_use_custom_fov)
		return;

	float* fov = reinterpret_cast<float*>(reinterpret_cast<unsigned long long>(out) + offsets::FOV);
	
	if (settings::b_use_custom_fov)
		*fov = settings::fov;
}