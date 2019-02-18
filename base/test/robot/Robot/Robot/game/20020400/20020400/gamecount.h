#pragma once 
#ifndef __GAME_COUNT_H_
#define __GAME_LOGIC_H_

#include "mjlogic\mjfancount.h" 
 
class  CGameCount :public CMJFanCount
{
public:
	CGameCount();
	~CGameCount();
public:
	void  InitLogic(game_mode param, unsigned char normal_card[], unsigned int normal_len);

public:
	//virtual bool Check15(); 
};

#endif//__GAME_LOGIC_H_