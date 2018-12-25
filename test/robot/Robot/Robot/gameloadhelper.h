#pragma once
#ifndef __GAMELOADHELPER_H_
#define __GAMELOADHELPER_H_
#include "basiccode/utility/BasicType.h"

typedef HANDLE(WINAPI* pfnCreateGameInterface)(unsigned int);
class basegamelogic;
class gameloadhelper
{
public:
	static gameloadhelper* get_instance();
public:
	int init_load(const char *dll_path);
	basegamelogic* create_logic();
private:
	gameloadhelper();
	virtual ~gameloadhelper();
private:
	HANDLE			m_handle;
	pfnCreateGameInterface				m_pfn_create;
};

#endif	