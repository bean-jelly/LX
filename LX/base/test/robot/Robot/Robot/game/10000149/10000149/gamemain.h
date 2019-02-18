#pragma once

#include <stdio.h>
#include <stdint.h>
#include <cstddef>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32

#ifdef BASICGAME_EXPORTS
#define BASICGAME_API __declspec(dllexport)
#else
#define BASICGAME_API __declspec(dllimport)
#endif

#else
#define BASICGAME_API extern
#define WINAPI
typedef void *HANDLE;
#endif


extern "C"
{
	HANDLE BASICGAME_API WINAPI CreateGameInterface(UINT uVer);
}