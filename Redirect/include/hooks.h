#pragma once

constexpr unsigned long long RequestExit = 0x2C2C760;
constexpr unsigned long long UnsafeEnvironmentPopup = 0x2089180;

namespace hooks {
	void UnsafeEnvironmentPopupHook(wchar_t** unknown1,
		unsigned __int8 _case,
		__int64 unknown2,
		char unknown3);

	__int64 RequestExitWithStatusHook(bool Force, unsigned char ReturnCode);

	void null_func();

	void place();
}