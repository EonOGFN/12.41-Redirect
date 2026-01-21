#include <redirector.h>

#include <memcury.h>
#include <UnrealContainers.hpp>
#include <regex>
#include <defines.h>
#include <lazy_importer.hpp>
#include <minhook/MinHook.h>
#include <hwid.h>
#include <ensure_init.h>

std::wstring redirector::replace_host(const std::wstring& url)
{
	std::wregex url_regex(L"^(https?)://([^/]+)(/.*)$");
	std::wstring result = std::regex_replace(url, url_regex, Globals::ServerAdress + L"$3");
	return result;
}

bool redirector::should_redirect(std::wstring& url)
{
	for (auto& redirected_url : REDIRECTED_URLS)
	{
		if (url.contains(redirected_url))
			return true;
	}

	return false;
}

bool redirector::place_hooks()
{
	unsigned long long base_address = *(unsigned long long*)(__readgsqword(0x60) + 0x10);
	void** pProcessRequest = (void**)(base_address + 0x6605AF8);
	DWORD oldProtect;

	if (VirtualProtect(pProcessRequest, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		ProcessRequestOriginal = reinterpret_cast<decltype(ProcessRequestOriginal)>(*pProcessRequest);

		*pProcessRequest = redirector::process_request_hook;

		VirtualProtect(pProcessRequest, sizeof(void*), oldProtect, &oldProtect);

		return true;
	}

	return false;
}

void redirector::CheckAndModifyURL(std::wstring& new_url) {
	std::wstring cmd = GetCommandLineW(); 
	bool hasNetworkStabilization = cmd.find(L"-EnableNetworkStabilization=true") != std::wstring::npos;
	bool hasTextureStreaming = cmd.find(L"-OptimizeTextureStreaming") != std::wstring::npos;

	if (hasNetworkStabilization && hasTextureStreaming) {
		size_t pos = new_url.find(L"/account/api/oauth/token");
		if (pos != std::wstring::npos) {
			ensure_init::set_init();

			if (Globals::ServerAdress.find(L"services.eonfn.dev") != std::wstring::npos) {
				new_url.replace(pos, 24, L"/v2/account/api/login/token");
			}
		}
	}
}

bool redirector::process_request_hook(FCurlHttpRequest* request)
{
	std::call_once(init_flag, [request]()
		{
			void** VTable = *reinterpret_cast<void***>(request);

			SetUrl = reinterpret_cast<decltype(SetUrl)>(VTable[SETURL_VTABLE]);
			GetUrl = reinterpret_cast<decltype(GetUrl)>(VTable[GETURL_VTABLE]);
		});

	UC::FString url;
	GetUrl(request, &url);

	LOG("URL : {}", url.ToString());

	//url : /fortnite/api/cloudstorage/user/:accountId
	if (url.ToWString().contains(xorstr_(L"/fortnite/api/cloudstorage/user/")))
	{
		//we want to get accountId

		std::wstring url_str = url.ToWString();
		std::wstring accountId = url_str.substr(url_str.find_last_of(L'/') + 1);

		if (accountId != xorstr_(L"ClientSettings.Sav")) {
			hwid::current_account_id = accountId;
		}
	}

	std::wstring original_url = url.ToWString();
	std::wstring New_URL = replace_host(url.ToWString());

	if constexpr (LOGGING) {
		std::string ss_url = std::string(New_URL.begin(), New_URL.end());
		LOG("New URL : {}", ss_url);
	}

	redirector::CheckAndModifyURL(New_URL);

	UC::FString result_url = UC::FString(New_URL.c_str());

	if (should_redirect(original_url))
	{
		SetUrl(request, &result_url);
	}
	else {
		LOG("Not redirecting {}", url.ToString().c_str());
	}

//	DWORD oldProtect;
	//LI_FN(VirtualProtect)(SetupRequest, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
	bool result = ProcessRequestOriginal(request);
	//LI_FN(VirtualProtect)(SetupRequest, 1, oldProtect, &oldProtect);
	return result;
}
