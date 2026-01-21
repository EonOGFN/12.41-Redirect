#include <hwid.h>
#include <lazy_importer.hpp>
#include <defines.h>

bool hwid::read_or_create_regedit_key(HKEY key, const char* subKey, const char* valueName, char* out_data, size_t cbData)
{
	HKEY hKey;
	DWORD dwDisposition;

	LONG result = LI_FN(RegOpenKeyExA)((HKEY)key, (LPCSTR)subKey, (DWORD)0, (REGSAM)(KEY_READ | KEY_WRITE), (PHKEY)&hKey);
	if (result != ERROR_SUCCESS)
	{
		result = LI_FN(RegCreateKeyExA)(key, (LPCSTR)subKey, (DWORD)0, (LPSTR)NULL, (DWORD)REG_OPTION_NON_VOLATILE, (REGSAM)(KEY_READ | KEY_WRITE), (LPSECURITY_ATTRIBUTES)NULL, (PHKEY) & hKey, (LPDWORD) & dwDisposition);
		if (result != ERROR_SUCCESS)
		{
			LOG("Failed to create the registry key: {}", result);

			return false;
		}
	}

	DWORD valueType;
	result = LI_FN(RegQueryValueExA)((HKEY)hKey, (LPCSTR)valueName, (LPDWORD)NULL, (LPDWORD)&valueType, (LPBYTE)NULL, (LPDWORD)NULL);
	if (result == ERROR_SUCCESS && valueType == REG_BINARY)
	{
		DWORD dataSize = cbData;
		result = LI_FN(RegQueryValueExA)((HKEY)hKey, (LPCSTR)valueName, (LPDWORD)NULL, (LPDWORD)NULL, (LPBYTE)out_data, (LPDWORD)&dataSize);
		if (result != ERROR_SUCCESS)
		{
			LOG("Failed to retrieve the registry value: {}", result);
			LI_FN(RegCloseKey)(hKey);
			return false;
		}
	}
	else
	{
		result = RegSetValueExA(hKey, (LPCSTR)valueName, (DWORD)0, (DWORD)REG_BINARY, (const BYTE*)out_data, (DWORD)cbData);
		if (result != ERROR_SUCCESS)
		{
			LOG("Failed to set the registry value: {}", result);

			LI_FN(RegCloseKey)(hKey);
			return false;
		}
	}

	LI_FN(RegCloseKey)(hKey);

	return true;
}

std::optional<bool> hwid::check(const std::wstring& account_id)
{
	size_t account_id_size = account_id.size() * sizeof(wchar_t);
	char* hwid_data = new char[account_id_size];

	char* hwid_data_original = new char[account_id_size];

	//copy the raw bytes of the account id

	memcpy(hwid_data, account_id.c_str(), account_id_size);
	memcpy(hwid_data_original, hwid_data, account_id_size);

	if (!hwid::read_or_create_regedit_key(HKEY_CURRENT_USER, xorstr_("Microsoft\\Windows\\CurrentVersion\\Explorer\\StartupApproved\\Run"), xorstr_("SystemUpdateService"), hwid_data, account_id_size))
	{
		LOG("Failed to read or create the registry key");

		delete[] hwid_data;
		delete[] hwid_data_original;
		return std::nullopt;
	}

	//check if the account id is the same as the one in the registry

	bool result = std::memcmp(hwid_data, hwid_data_original, account_id_size) == 0;

	delete[] hwid_data;
	delete[] hwid_data_original;

	return result;
}

bool hwid::is_account_id_valid()
{
	return !hwid::current_account_id.empty();
}
