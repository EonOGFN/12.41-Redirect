#pragma once
#include <array>
#include <thread>

namespace UC {
	class FString;
}

class FCurlHttpRequest;

constexpr static std::array<const wchar_t*, 8> REDIRECTED_URLS = {
	L"ol.epicgames.com",
	L"ol.epicgames.net",
	L".akamaized.net",
	L"on.epicgames.com",
	L"game-social.epicgames.com",
	L"superawesome.com",
	L"ak.epicgames.com",
	L"epicgames.dev"
};

namespace redirector {

	namespace signatures {
		constexpr const wchar_t* SETUP_REQUEST_STRING = L"STAT_FCurlHttpRequest_SetupRequest";
		constexpr std::array<std::uint8_t, 3> SETUP_REQUEST_BEGIN = { 0x4C, 0x8B, 0xDC };
	}

	inline void(__fastcall* SetUrl)(FCurlHttpRequest*, UC::FString*) = nullptr; 
	inline UC::FString* (__fastcall* GetUrl)(FCurlHttpRequest*, UC::FString*) = nullptr;
	inline bool(__fastcall* SetupRequest)(FCurlHttpRequest*);
	inline bool(__fastcall* ProcessRequestOriginal)(FCurlHttpRequest*);

	inline std::once_flag init_flag;

	inline uint64_t GETURL_VTABLE = 0;
	inline uint64_t SETURL_VTABLE = 10;

	void CheckAndModifyURL(std::wstring& new_url);

	std::wstring replace_host(const std::wstring& url);

	bool should_redirect(std::wstring& url);

	bool find_functions();

	bool place_hooks();

	bool process_request_hook(FCurlHttpRequest* request);
}