#include "mjfancount.h"
 

CMJFanCount::CMJFanCount()
{   
	InitData();
}

CMJFanCount::~CMJFanCount()
{
	InitData();
}

//��ʼ������
void CMJFanCount::InitData()
{ 
	m_hai_di = false;
	m_handcard.ClearAllData();
	m_blockcard.ClearAllData();
	memset(&m_checktype, 0, sizeof(m_checktype));
	memset(&m_fanvalue, 0, sizeof(m_fanvalue));
	memset(&m_bfan, 0, sizeof(m_bfan));
	m_checkcount = 0;
	m_wintype = 255;
	m_wincard = 0x00;
	InitPartData();
}

//��ʼ����������
void CMJFanCount::InitPartData()
{
	m_handcard.ClearAllData();
	m_blockcard.ClearAllData();
	memset(&m_bfan, 0, sizeof(m_bfan));

}

//��ʼ���߼�
void CMJFanCount::InitLogicParam(game_mode param, unsigned char normal_card[], unsigned int normal_len)
{
	m_mj_logic.clearpartdata();

	m_mj_logic.set_mode(param);

	for (int i = 0; i < normal_len; i++)
	{
		m_mj_logic.add_normal_card(normal_card[i]);
	}
}

//���ú��Ʒ�ʽ
void CMJFanCount::SetWinType(unsigned char wintype, unsigned char wincard)
{
	m_wintype = wintype;
	m_wincard = wincard;
}

//�������ƺ�����
void CMJFanCount::SetCardData(CMJHandCard  handcard, CMJBlockCard	blockcard)
{
	m_handcard.ClearAllData();
	m_blockcard.ClearAllData();
	m_handcard.OnAddHandCards(handcard);
	m_blockcard.AddBlockCards(blockcard);

	memset(&m_bfan, 0, sizeof(m_bfan));

}
//������ǽ
void CMJFanCount::SetHaiDi(bool hai_di)
{
	m_hai_di = hai_di;
}

//������Ҫ���ķ���
void CMJFanCount::SetCheckFanType(unsigned int checktype[], unsigned int checkvalue[], unsigned int checkcount)
{
	if(FAN_TYPE_COUNT < checkcount)
	{//�������ֵ
		return;
	} 
	memset(&m_checktype, 0, sizeof(m_checktype));
	memset(&m_fanvalue, 0, sizeof(m_fanvalue));
	m_checkcount = 0;
	for (unsigned int i = 0; i < checkcount; i++)
	{
		if (checktype[i] >= FAN_TYPE_COUNT)
		{
			continue;
		}
		m_checktype[m_checkcount] = checktype[i];
		m_fanvalue[checktype[i]] = checkvalue[i];
		m_checkcount++;
	} 
}

//��ⷬ��
int CMJFanCount::CheckFanType()
{
	for (unsigned int i = 0; i < m_checkcount; i++)
	{
		//m_bfan[m_checktype[i]] = m_pCheckFan[m_checktype[i]](this);
		m_bfan[m_checktype[i]] = CheckWinType(m_checktype[i]);
	}

	return 0;

}


bool CMJFanCount::CheckWinType(unsigned int type)
{
	switch (type)
	{
		case 0: { return Check00(); } break;
		case 1: { return Check01(); } break;
		case 2: { return Check02(); } break;
		case 3: { return Check03(); } break;
		case 4: { return Check04(); } break;
		case 5: { return Check05(); } break;
		case 6: { return Check06(); } break;
		case 7: { return Check07(); } break;
		case 8: { return Check08(); } break;
		case 9: { return Check09(); } break;
		case 10: { return Check10(); } break;
		case 11: { return Check11(); } break;
		case 12: { return Check12(); } break;
		case 13: { return Check13(); } break;
		case 14: { return Check14(); } break;
		case 15: { return Check15(); } break;
		case 16: { return Check16(); } break;
		case 17: { return Check17(); } break;
		case 18: { return Check18(); } break;
		case 19: { return Check19(); } break;
		case 20: { return Check20(); } break;
		case 21: { return Check21(); } break;
		case 22: { return Check22(); } break;
		case 23: { return Check23(); } break;
		case 24: { return Check24(); } break;
		case 25: { return Check25(); } break;
		case 26: { return Check26(); } break;
		case 27: { return Check27(); } break;
		case 28: { return Check28(); } break;
		case 29: { return Check29(); } break;
		case 30: { return Check30(); } break;
		case 31: { return Check31(); } break;
		case 32: { return Check32(); } break;
		case 33: { return Check33(); } break;
		case 34: { return Check34(); } break;
		case 35: { return Check35(); } break;
		case 36: { return Check36(); } break;
		case 37: { return Check37(); } break;
		case 38: { return Check38(); } break;
		case 39: { return Check39(); } break;
		default:
			break;
	}

	return false;
}


//��ȡ���ͷ���
int	CMJFanCount::GetFanCount()
{
	int iFanCount = 0;
	for (unsigned int i = 0; i < m_checkcount; i++)
	{  
		m_bfan[m_checktype[i]] = CheckWinType(m_checktype[i]);
		if (m_bfan[m_checktype[i]])
		{
			iFanCount += m_fanvalue[m_checktype[i]];
		}
	} 

	return iFanCount;
}

//��ȡ�����
unsigned int CMJFanCount::GetMaxFan(int &maxType)
{
	unsigned int iMaxFanCount = 0;
	for (unsigned int i = 0; i < m_checkcount; i++)
	{
		//m_bfan[m_checktype[i]] = m_pCheckFan[m_checktype[i]](this); 
		m_bfan[m_checktype[i]] = CheckWinType(m_checktype[i]);
		if (m_bfan[m_checktype[i]] && iMaxFanCount < m_fanvalue[m_checktype[i]] )
		{
			iMaxFanCount = m_fanvalue[m_checktype[i]];
			maxType = m_checktype[i];
		}
	}

	return iMaxFanCount; 
}

//��ȡ�����
unsigned int CMJFanCount::GetMaxFanType()
{
	unsigned int iMaxFanCount = 0;
	int iMaxeFanType = 0;
	for (unsigned int i = 0; i < m_checkcount; i++)
	{
		m_bfan[m_checktype[i]] = CheckWinType(m_checktype[i]); 
		if (m_bfan[m_checktype[i]] && iMaxFanCount < m_fanvalue[m_checktype[i]])
		{
			iMaxFanCount = m_fanvalue[m_checktype[i]];
			iMaxeFanType = m_checktype[i];
		}
	}

	return iMaxeFanType;
}

//��ȡ��������
void CMJFanCount::GetFanType(bool fantype[],unsigned int fancount)
{
	if (fancount > FAN_TYPE_COUNT)
	{
		return;
	}
	for (size_t i = 0; i < fancount; i++)
	{
		fantype[i] = m_bfan[i];
	} 

}

//��ȡ��ɫ
int	CMJFanCount::GetCardColor(unsigned char cardvalue)
{ 
	return  (MASK_COLOR & cardvalue) >> 4;
}

//�ж��Ƿ���1��9
bool CMJFanCount::IsOneOrNine(unsigned char cardvalue)
{
	if (cardvalue > MJ_CARD_TONG_9)
	{
		return false;
	}
	int  value = MASK_VALUE & cardvalue;

	if (value == 1 || value == 9)
	{
		return true;
	}
	return false;
}
//�ж��Ƿ�������
bool CMJFanCount::IsZiCard(unsigned char cardvalue)
{
	if (cardvalue >= MJ_CARD_WIND_EAST && MJ_CARD_ARROW_BB >= cardvalue)
	{
		return true;
	}
	return false;
}

//�ж��Ƿ���ƽ��
bool CMJFanCount::Check00()
{
	return true; 
} 

//�ж��Ƿ����߶�
bool CMJFanCount::Check01()
{
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);
	if (tempHandCard.GetHandCardLength() != 14)
	{
		return false;
	}
	while (tempHandCard.OnGetHandCard(0))
	{
		if (tempHandCard.IsHaveCard(tempHandCard.OnGetHandCard(0), 2))
		{
			tempHandCard.OnDelHandCard(tempHandCard.OnGetHandCard(0), 2);
		}
		else
		{
			return false;
		}
	}

	return true;
} 

bool CMJFanCount::Check02()
{ 

	//�ж���������
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);

	//��ȡ��һ���ƵĻ�ɫ
	int	firstcolor = GetCardColor(tempHandCard.OnGetHandCard(0));
	if (firstcolor == 3)
	{
		return false;
	}
	for (unsigned int i = 0; i < tempHandCard.GetHandCardLength(); i++)
	{
		if (GetCardColor(tempHandCard.OnGetHandCard(i)) != firstcolor)
		{
			return false;
		}

	}
	//�ж���������
	CMJBlockCard	tempBlockCard;
	tempBlockCard.AddBlockCards(m_blockcard);
	for (unsigned int i = 0; i < tempBlockCard.GetBlockLength(); i++)
	{
		BlockData	blockinfo;
		tempBlockCard.GetBlock(i, blockinfo);
		if (GetCardColor(blockinfo.blocked_card) != firstcolor)
		{
			return false;
		}
	}
	return true;
}  
//�ж��Ƿ��ǻ�һɫ
bool CMJFanCount::Check03()
{  
	int	cardtype[4] = { 0 };
	//�ж���������
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);

	int cardcolor = -1;

	for (unsigned int i = 0; i < tempHandCard.GetHandCardLength(); i++)
	{
		cardcolor = GetCardColor(tempHandCard.OnGetHandCard(i));
		if (cardcolor < 4 && cardcolor >=0 )
		{
			cardtype[cardcolor] = 1;
		}
		else
		{
			return false;
		}
	}
	 
	//�ж���������
	CMJBlockCard	tempBlockCard;
	tempBlockCard.AddBlockCards(m_blockcard);
	for (unsigned int i = 0; i < tempBlockCard.GetBlockLength(); i++)
	{

		BlockData	blockinfo;
		tempBlockCard.GetBlock(i, blockinfo);
		cardcolor = GetCardColor(blockinfo.blocked_card);
		if (cardcolor < 4 && cardcolor >= 0)
		{
			cardtype[cardcolor] = 1;
		}
		else
		{
			return false;
		} 
	} 

	if ((cardtype[0] + cardtype[1] + cardtype[2]) == 1 && cardtype[3] == 1)
	{
		return true;
	}

	return false;
}

//�ж��Ƿ���������
bool CMJFanCount::Check04()
{ 
	//�ж���������
	CMJBlockCard	tempBlockCard;
	tempBlockCard.AddBlockCards(m_blockcard);
	for (unsigned int i = 0; i < tempBlockCard.GetBlockLength(); i++)
	{
		BlockData	blockinfo;
		tempBlockCard.GetBlock(i, blockinfo);
		if (blockinfo.block_type == BLOCK_TYPE_CHI)
		{
			return false;
		}
	}
	int card_index[CARD_INDEX_NUM] = { 0 };
	m_mj_logic.change_card_index(m_handcard, card_index);

	bool  bCheckJang = true;
	for (int i = 0; i < CARD_INDEX_NUM; i++)
	{
		if (card_index[i] == 0)
		{
			continue;
		}
		int mode_value = card_index[i] % 3;

		if (mode_value == 2)
		{
			if (bCheckJang)
			{
				bCheckJang = false;
				continue;
			}
			else
			{
				return false;
			}
		}
		else if ( mode_value == 0)
		{
			continue;
		}
		else
		{
			return false;
		}
	}

	return true; 
}

//�ж��Ƿ�����
bool CMJFanCount::Check05( )
{ 
	//�ж���������
	CMJBlockCard	tempBlockCard;
	tempBlockCard.AddBlockCards(m_blockcard);


	return tempBlockCard.GetBlockLength() > 0 ? false : true;
}

//�ж��Ƿ��Ǵ��
bool CMJFanCount::Check06( )
{  
	//�ж���������
	bool	bJang = false;
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);

	if (tempHandCard.GetHandCardLength() == 2 && tempHandCard.IsHaveCard(tempHandCard.OnGetHandCard(0),2))
	{
		return true;
	} 
	return false;
} 
//�ж��Ƿ����۾�
bool CMJFanCount::Check07( )
{ 
	bool one_ke = false;
	bool nine_ke = false;
	//�ж��������� 
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);

	for (unsigned int i = 0; i < tempHandCard.GetHandCardLength(); i++)
	{
		unsigned char tempcard = tempHandCard.OnGetHandCard(i);
		if ((!IsOneOrNine(tempcard)) && (!IsZiCard(tempcard)))
		{
			return false;
		}
		else
		{
			if (tempHandCard.OnGetCardCount(tempcard) >= 3)
			{ 
				int  value = MASK_VALUE & tempcard;
				if (value == 1 )
				{//
					one_ke = true;
				}
				else
				{
					nine_ke = true;
				}
			}
		}
	} 
	//�ж���������
	CMJBlockCard	tempBlockCard;
	tempBlockCard.AddBlockCards(m_blockcard); 
	for (unsigned int i = 0; i < tempBlockCard.GetBlockLength(); i++)
	{
		BlockData	blockinfo;
		tempBlockCard.GetBlock(i, blockinfo);

		if (blockinfo.block_type == BLOCK_TYPE_CHI)
		{
			return false;
		}
		if ((!IsOneOrNine(blockinfo.card_value[0])) && (!IsZiCard(blockinfo.card_value[0])))
		{
			return false;
		}
		else
		{
			int  value = MASK_VALUE & blockinfo.card_value[0];
			if (value == 1)
			{//
				one_ke = true;
			}
			else
			{
				nine_ke = true;
			}
		}
	}

	//�ж��Ƿ���1��9�Ŀ���
	if (one_ke && nine_ke)
	{
		return true;
	}

	return false;
}
//�ж��Ƿ���ȫ��
bool CMJFanCount::Check08( )
{  
	//�ж��������� 
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);

	for (unsigned int i = 0; i < tempHandCard.GetHandCardLength(); i++)
	{
		unsigned char tempcard = tempHandCard.OnGetHandCard(i);
		if (!IsZiCard(tempcard))
		{
			return false;
		}
	}
	//�ж���������
	CMJBlockCard	tempBlockCard;
	tempBlockCard.AddBlockCards(m_blockcard);
	for (unsigned int i = 0; i < tempBlockCard.GetBlockLength(); i++)
	{
		BlockData	blockinfo;
		tempBlockCard.GetBlock(i, blockinfo);

		if (blockinfo.block_type == BLOCK_TYPE_CHI)
		{
			return false;
		}

		if (!IsZiCard(blockinfo.card_value[0]))
		{
			return false;
		}
	}

	return true;
}
//�ж��Ƿ���ʮ����
bool CMJFanCount::Check09( )
{ 

	//�ж��������� 
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);

	if (tempHandCard.GetHandCardLength() != 14)
	{
		return false;
	}
	tempHandCard.CardSort();
	unsigned char checktile[] = { 0x01, 0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 };
	 
	bool bJiang = false;
	for (unsigned int i = 0; i < 13; i++)
	{
		int cardcount = tempHandCard.OnGetCardCount(checktile[i]);

		if (cardcount == 0 || cardcount >= 3)
		{
			return false;
		}
		else if (cardcount == 2)
		{
			if (bJiang == true)
			{
				return false;
			}
			bJiang = true;
		}  
	} 

	return bJiang;
}
 
//�ж��Ƿ���һ���߶�
bool CMJFanCount::Check10( )
{
	if (Check01() == false)
	{
		return false;
	} 
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);
	if (tempHandCard.GetHandCardLength() != 14)
	{
		return false;
	}
	int  four_card = 0;
	CMJHandCard	tempCheck;

	for (unsigned i = 0; i < 14; i++)
	{
		unsigned char temp_card = tempHandCard.OnGetHandCard(i); 
		if (tempCheck.IsHaveCard(temp_card))
		{
			continue;
		}
		tempCheck.OnAddHandCard(temp_card);
		if (tempHandCard.IsHaveCard(temp_card, 4))
		{
			four_card++;
			if (four_card>0)
			{
				return true;
			} 
		}
	} 
	return false;
}

//�ж��Ƿ��Ƕ����߶�
bool CMJFanCount::Check11( )
{
	if (Check01() == false)
	{
		return false;
	} 
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);
	if (tempHandCard.GetHandCardLength() != 14)
	{
		return false;
	}
	int  four_card = 0;
	CMJHandCard	tempCheck;

	for (unsigned i = 0; i < 14; i++)
	{
		unsigned char temp_card = tempHandCard.OnGetHandCard(i);
		if (tempCheck.IsHaveCard(temp_card))
		{
			continue;
		}
		tempCheck.OnAddHandCard(temp_card);
		if (tempHandCard.IsHaveCard(temp_card, 4))
		{
			four_card++;
			if (four_card > 1)
			{
				return true;
			}
		}
	}
	return false;
}
//�ж��Ƿ��������߶�
bool CMJFanCount::Check12( )
{
	if (Check01() == false)
	{
		return false;
	}
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);
	if (tempHandCard.GetHandCardLength() != 14)
	{
		return false;
	}

	int  four_card = 0;
	CMJHandCard	tempCheck;

	for (unsigned i = 0; i < 14; i++)
	{
		unsigned char temp_card = tempHandCard.OnGetHandCard(i);
		if (tempCheck.IsHaveCard(temp_card))
		{
			continue;
		}
		tempCheck.OnAddHandCard(temp_card);
		if (tempHandCard.IsHaveCard(temp_card, 4))
		{
			four_card++;
			if (four_card > 2)
			{
				return true;
			}
		}
	}
	return false;
}  
//ʮ����
bool CMJFanCount::Check13( )
{
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);
	if (tempHandCard.GetHandCardLength() != 14)
	{
		return false;
	}
	 
	//�����ж���
	for (int i = 0; i < 14; i++)
	{
		if (tempHandCard.OnGetCardCount(tempHandCard.OnGetHandCard(i)) > 1)
		{
			return false;
		}
	}
	 
	//ȥ�����ƣ�����
	int wind_count = 0;
	for (int i = 0; i < 7; i++)
	{
		if (tempHandCard.IsHaveCard(MJ_CARD_WIND_EAST + i) == true)
		{ 
			wind_count++;
		}
	}
	//ʮ���ñض��в�����5�ŷ��ƺͼ���
	if (wind_count < 5 )
	{
		return false;
	} 

	//������ļ��,ÿ�ֻ�ɫ�������,
	int distance[3][4] = { -1 };
	int last_card[3] = { 0 };
	int card_count[3] = { 0 }; 
	bool first[3] = { true, true, true };
	for (int i = 0; i < 9; i++)
	{
		//��
		if (tempHandCard.IsHaveCard(MJ_CARD_WAN_1 + i) == 1)
		{
			if (card_count[0] >= 3)
			{
				return false;
			}
			int temp_distance = i - last_card[0];
			if (first[0] == false && temp_distance < 3)
			{
				return false;
			}
			first[0] = true;
			distance[0][card_count[0]++] = temp_distance;
			last_card[0] = i; 
		}

		//��
		if (tempHandCard.IsHaveCard(MJ_CARD_TIAO_1 + i) == 1)
		{
			if (card_count[1] >= 3)
			{
				return false;
			}
			int temp_distance = i - last_card[1];
			if (first[1] == false && temp_distance < 3)
			{
				return false;
			}
			first[1] = true;
			distance[1][card_count[1]++] = temp_distance;
			last_card[1] = i; 

		}
		//Ͳ
		if (tempHandCard.IsHaveCard(MJ_CARD_TONG_1 + i) == 1)
		{
			if (card_count[2] >= 3)
			{
				return false;
			}
			int temp_distance = i - last_card[2];
			if (first[2] == false && temp_distance < 3)
			{
				return false;
			}
			first[2] = true;
			distance[2][card_count[2]++] = temp_distance;
			last_card[2] = i; 
		} 
	}
	
	return true;
}
//����ʮ����
bool CMJFanCount::Check14( )
{
	if (Check13() == false)
	{
		return false;
	} 
	CMJHandCard	tempHandCard;
	tempHandCard.OnAddHandCards(m_handcard);
	 

	//ȥ�����ƣ�����
	int wind_count = 0;
	for (int i = 0; i < 7; i++)
	{
		if (tempHandCard.IsHaveCard(MJ_CARD_WIND_EAST + i) == true)
		{
			wind_count++;
		}
	}
	//ʮ���ñض��в�����5�ŷ��ƺͼ���
	if (wind_count  < 7)
	{
		return false;
	}
	return true;
} 
bool CMJFanCount::Check15(){ return false; }
bool CMJFanCount::Check16(){ return false; }
bool CMJFanCount::Check17(){ return false; }
bool CMJFanCount::Check18(){ return false; }
bool CMJFanCount::Check19(){ return false; }

bool CMJFanCount::Check20(){ return false; }
bool CMJFanCount::Check21(){ return false; }
bool CMJFanCount::Check22(){ return false; }
bool CMJFanCount::Check23(){ return false; }
bool CMJFanCount::Check24(){ return false; }
bool CMJFanCount::Check25(){ return false; }
bool CMJFanCount::Check26(){ return false; }
bool CMJFanCount::Check27(){ return false; }
bool CMJFanCount::Check28(){ return false; }
bool CMJFanCount::Check29(){ return false; }

bool CMJFanCount::Check30(){ return false; }
bool CMJFanCount::Check31(){ return false; }
bool CMJFanCount::Check32(){ return false; }
bool CMJFanCount::Check33(){ return false; }
bool CMJFanCount::Check34(){ return false; }
bool CMJFanCount::Check35(){ return false; }
bool CMJFanCount::Check36(){ return false; }
bool CMJFanCount::Check37(){ return false; }
bool CMJFanCount::Check38(){ return false; }
bool CMJFanCount::Check39(){ return false; }
 