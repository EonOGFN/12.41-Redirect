#include <error.h>

#include <Windows.h>
#include <lazy_importer.hpp>
#include <memcury.h>
#include <defines.h>

void error::display_error(const char* message)
{
	const char* text = xorstr_("EON | ERROR");
	LI_FN(MessageBoxA).safe()((HWND)0, message, text, MB_ICONERROR);
}