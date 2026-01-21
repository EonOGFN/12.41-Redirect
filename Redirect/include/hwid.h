#pragma once
#include <optional>
#include <string>
#include <Windows.h>

namespace hwid {
	bool read_or_create_regedit_key(HKEY hKey, const char* subKey, const char* valueName, char* out_data, size_t cbData);

	std::optional<bool> check(const std::wstring& account_id);

	bool is_account_id_valid();

	inline std::wstring current_account_id;
}