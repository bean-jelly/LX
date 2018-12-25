#include "gamecount.h"

CGameCount::CGameCount()
{  
}

CGameCount::~CGameCount()
{   
}

void  CGameCount::InitLogic(game_mode param, unsigned char normal_card[], unsigned int normal_len)
{ 
	__super::InitLogicParam(param, normal_card, normal_len);
}
/*
//Ò»ÌõÁú
bool CGameCount::Check15()
{
	if (m_blockcard.GetBlockLength()>0)
	{
	return false;
	}
	for (int i = 0; i < 3; i++)
	{
	bool  check_win = true;
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);

	for (int j = 0; j < 9; j++)
	{
	unsigned char  temp_card = 16 * i + j + 1;
	if (tempHandCard.IsHaveCard(temp_card))
	{
	tempHandCard.OnDelHandCard(temp_card);
	}
	else
	{
	check_win = false;
	break;
	}
	}
	if (check_win)
	{
	tempHandCard.CardSort();
	unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
	unsigned int temp_hand_count = tempHandCard.GetAllHandCard(temp_hand_card);

	if (m_mj_logic.check_can_sequence(temp_hand_card, temp_hand_count, true))
	{
	return true;
	}
	}
	}

	return false;
}

*/ 