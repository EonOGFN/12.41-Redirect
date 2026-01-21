#pragma once
#include <iostream>
#include <string> 
#include <spdlog/spdlog.h>
#include <xorstr.hpp>

#define TRUE 1
#define FALSE 0

class Globals
{
public:
    static const bool bDisableAC = false;
    inline static const std::wstring ServerAdress = L"https://services.eonfn.dev";
};

/*-------- - (Logging) --------- */
#define LOGGING FALSE  

#if LOGGING
#define CREATE_CONSOLE FALSE
#define LOG(fmt, ...) spdlog::info("Eon " __FUNCSIG__ " " fmt, __VA_ARGS__)
#else
#define LOG(fmt, ...)
#define CREATE_CONSOLE FALSE
#endif
#define JM_XORSTR_DISABLE_AVX_INTRINSICS


/*
    IF YOU WANT TO UPDATE TO ANOTHER VERSION :

    - UPDATE THE VTABLES OFFSETS IN redirector.h
    - UPDATE THE SIGNATURES IN disable_pre_edit.h
    - (MAYBE) UPDATE SIGNATURES IN redirector.h
*/
