#include "MJGameLogic.h"

///��Ϸ���õ�
CMJGameLgic::CMJGameLgic()
{
}
//����
CMJGameLgic::~CMJGameLgic()
{
}

//���ù���
void CMJGameLgic::setmode(BYTE beight, bool bqidui)
{
	m_beight = beight;
	m_bqidui = bqidui;
}

//����Ƿ�������,�������ӵĸ���
int CMJGameLgic::GetKingcardList(BYTE handcard[], BYTE cardcount, BYTE byResult[])
{
	int king_count = 0;
	for (unsigned int i = 0; i < m_king_count; i++)
	{
		int iTempcount = get_card_count(handcard, cardcount, m_king_card[i]);
		if (iTempcount > 0)
		{
			for (int j = 0; j < iTempcount; j++)
			{
				byResult[king_count + j] = m_king_card[i];
			}
			king_count += iTempcount;
		}
	}
	return king_count;
}
//���������Ӽ���ϵ
bool CMJGameLgic::CheckArraySubSet(BYTE cardlist[], BYTE count, BYTE checklist[], BYTE checkcount)
{
	if (count < checkcount)
	{
		return false;
	}
	bool bUseIndex[HAND_CARD_COUNT];
	memset(bUseIndex, 0, sizeof(bUseIndex));

	int ihavecount = 0;
	for (int i = 0; i < checkcount; i++)
	{
		for (int j = 0; j < count; j++)
		{
			if (bUseIndex[j])
			{
				continue;
			}
			if (checklist[i] == cardlist[j])
			{
				ihavecount++;
				bUseIndex[j] = true;
				break;
			}
		}
		if (ihavecount == checkcount)
		{
			return true;
		}
	}
	return  (ihavecount == checkcount);
}

/*����Ƿ��������
@param: hand_card  �������
@param: hand_card_count  ���������
@param: check_card  �������
return  ���� ��������
*/
BYTE CMJGameLgic::CheckCanPeng(const BYTE hand_card[], const BYTE hand_card_count, BYTE check_card, bool bUseking)
{
	return __super::check_can_peng(hand_card, hand_card_count, check_card);
}

/*����Ƿ���Գ���
@param: hand_card  �������
@param: hand_card_count  ���������
@param: check_card  �������
@param: chi_type  �������� 0x00:���ܳ�  0x01:��ͷ 0x10������ 0x11����β
return  ���� ��������
*/
BYTE CMJGameLgic::check_can_chi(const BYTE hand_card[], unsigned int hand_card_count, BYTE check_card, BYTE& chi_type, block_card_info blockinfo[])
{
	return __super::check_can_chi(hand_card, hand_card_count, check_card, chi_type, blockinfo);
}
/*����Ƿ��������
@param: hand_card  �������
@param: hand_card_count  ���������
@param: check_card  �������
return  ���� ��������
*/
bool CMJGameLgic::CheckOtherGang(const BYTE hand_card[], const BYTE hand_card_count, BYTE check_card, bool bUseking)
{
	if (!check_cardvalue(check_card))
	{
		return false;
	}
	if (is_king(check_card)) //��Ӳ��ܳ���
	{
		return false;
	}
	if (hand_card_count > HAND_CARD_COUNT || hand_card_count < 3)
	{
		return false;
	}
	BYTE  temp_hand_value[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_value, hand_card, hand_card_count);
	BYTE  temp_card_count = hand_card_count;

	//���Ƽ��
	int check_card_count = get_card_count(temp_hand_value, temp_card_count, check_card);
	//if (check_card_count < 1)
	//{
	//	return false;
	//}
	int iKingCardcont = 0;
	if (bUseking)
	{
		iKingCardcont = is_have_king_value(temp_hand_value, temp_card_count);
	}
	if (check_card_count + iKingCardcont >= 3)
	{
		return true;
	}
	return false;
}
/*����Ƿ���԰��ܣ�����
@param: hand_card  �������
@param: hand_card_count  ���������
@param: bloci_datat  ��������
@param: check_card  �������
*/
BYTE CMJGameLgic::CheckSelfGang(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE check_card, BYTE block_card[], bool bUseKing)
{
	if (!check_cardvalue(check_card))
	{
		return 0;
	}
	return 0;
}
/*����Ƿ���԰��ܣ�����
@param: hand_card  �������
@param: hand_card_count  ���������
@param: bloci_datat  ��������
@param: check_card  �������
*/
BYTE CMJGameLgic::CheckGangAfterblock(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE block_card[], bool bUseKing)
{
	//return CheckSelfGang(hand_card, hand_card_count, user_block, MJ_CARD_ARROW_HZ, block_card, bUseKing);
	return 0;
}
/*����Ƿ��������--���ƺ�
@param: hand_card  �������
@param : hand_card_count  ���������
@param : ting_card  �����б�
@param : bking  �Ƿ�������
@param : king_card  ����
return  result
*/
BYTE CMJGameLgic::CheckCanTing(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard userBlock, TingData& ting_info, BYTE byFatforbidcard, BYTE byLimitHuFan)
{
	BYTE gang_type = BLOCK_TYPE_GAVE;
	if (hand_card_count > 0 && hand_card_count <= HAND_CARD_COUNT)
	{
		BYTE temp_out_card[HAND_CARD_COUNT] = { 0x00 };
		bool	bTileFalg[CARD_INDEX_NUM];
		memset(bTileFalg, 0, sizeof(bTileFalg));
		unsigned int  temp_ting_count = 0;

		for (unsigned int i = 0; i < hand_card_count; i++)
		{
			if (!check_cardvalue(hand_card[i]))
			{
				continue;
			}
			if (bTileFalg[hand_card[i]])
			{
				continue;
			}
			if(hand_card[i] == byFatforbidcard)
			{
				continue;
			}

			if (get_card_count(temp_out_card, HAND_CARD_COUNT, hand_card[i]) == 0)
			{
				BYTE temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
				memcpy(temp_hand_card, hand_card, hand_card_count);
				//���һ���ƣ��ж��Ƿ��������
				temp_out_card[temp_ting_count] = hand_card[i];
				if (del_card_value(temp_hand_card, hand_card_count, hand_card[i], 1))
				{
					bTileFalg[hand_card[i]] = true;

					BYTE win_card[NORMAL_CARD_NUM] = { 0x00 };
					BYTE bytingcount = 0;
					if (IsTingEX(temp_hand_card, hand_card_count - 1, userBlock, win_card, bytingcount,byLimitHuFan))
					{
						gang_type = BLOCK_TYPE_TING;

						ting_info.ting_card[temp_ting_count][0] = hand_card[i];
						memcpy(&ting_info.ting_card[temp_ting_count][1], win_card, sizeof(win_card));
						temp_ting_count++;

					}
				}
			}
		}
	}
	return  gang_type;
}
/*��������Ƿ�����-���ƺ�
@param : hand_card  ����
@param : hand_card_count ������
@param : win_card	���Ժ�����
@param : first_card �Ƿ��һ��
return result
*/
bool CMJGameLgic::IsTingEX(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard userBlock, BYTE win_card[], BYTE& bytingCount, BYTE byLimitHuFan)
{
	bytingCount = 0;
	if (hand_card_count % 3 == 1)
	{
		bool bTing = false;
		for (unsigned int i = 0; i < NORMAL_CARD_NUM; i++)
		{
			if (!check_cardvalue(m_normal_card[i]))
			{
				continue;
			}
			if (CheckCanWin(hand_card, hand_card_count, userBlock, m_normal_card[i], true,byLimitHuFan))
			{
				bTing = true;
				win_card[bytingCount++] = m_normal_card[i];
			}
		}
		return bTing;
	}
	return false;
}

//��ȡ�����л�ɫ�����������������У�
int CMJGameLgic::GetUserHuaKindCount(const BYTE hand_card[], const unsigned int hand_card_count, CMJBlockCard user_block)
{
	BYTE byCardList[20] = { 0 };
	BYTE byCardCount = hand_card_count;
	memcpy(byCardList, hand_card, byCardCount);
	for (int i = 0; i < (int)user_block.GetBlockLength(); i++)
	{
		BlockData blockdata;
		user_block.GetBlock(i, blockdata);
		byCardList[byCardCount++] = blockdata.blocked_card;
	}

	int iHuaKindNum[4] = { 0 };
	for (int i = 0; i < byCardCount; i++)
	{
		int iHuaKind = get_card_color(byCardList[i]);
		if (iHuaKind >= 0 && iHuaKind < 3) //��Ҫ����
		{
			iHuaKindNum[iHuaKind] ++;
		}
	}

	int iHuaKindCount = 0;
	for (int i = 0; i < 3; i++)
	{
		if (iHuaKindNum[i] > 0)
		{
			iHuaKindCount++;
		}
	}
	return iHuaKindCount;
}
//�Ƿ�ɺ�
bool CMJGameLgic::CheckCanWin(const BYTE hand_card[], const BYTE hand_card_count, CMJBlockCard user_block, BYTE check_card, bool add,BYTE byLimitHuFan)
{
	if (hand_card_count < 1)
	{
		return false;
	}
	BYTE  byCardList[HAND_CARD_COUNT] = { 0 };
	BYTE  bycardcount = hand_card_count;
	memcpy(byCardList, hand_card, bycardcount);
	if (add)
	{
		byCardList[bycardcount++] = check_card;
	}
	int iCount = is_have_king_value(byCardList, bycardcount);

	bool bCanWin = false;
	if (iCount == bycardcount)
	{
		bCanWin = true;
	}
	else
	{
		bCanWin = (check_can_win(hand_card, hand_card_count, check_card,false, add, false) == BLOCK_TYPE_HU);
	}		
	return bCanWin;
}
//�Ƿ���ȫ˳
bool CMJGameLgic::checkIsAllSeq(const BYTE hand_card[], const unsigned int hand_card_count, unsigned int kingcount, bool bHaveKing)
{
	if (hand_card_count <= 0 || (hand_card_count + kingcount) % 3 != 2)
	{
		return false;
	}
	bool	check_jiang = true;
	bool	bwin = false;
	//��һ�μ�������Ƿ��н��ƣ��ڶ��β���������Ӵ���
	for (int s = 0; s < 2; s++)
	{
		bool check_card[CARD_INDEX_NUM];
		memset(check_card, 0, sizeof(check_card));

		if (check_jiang == false && kingcount == 0)
		{
			//����û��������û�����ӣ����ܺ���
			return false;
		}
		//��������
		for (unsigned int i = 0; i < hand_card_count; i++)
		{
			if (check_card[hand_card[i]])
			{
				continue;
			}
			//��¼��������
			check_card[hand_card[i]] = true;

			//ÿ���������ƺ�������
			int temp_count = kingcount;

			//�ֽ��ƺ�ʣ�����
			int remain_index[CARD_INDEX_NUM] = { 0 };
			change_card_index(hand_card, hand_card_count, remain_index);

			if(bHaveKing) //����ӵ�����£������Ʋ��ܵ�������
			{
				if (hand_card[i] == MJ_CARD_WIND_EAST 
					|| hand_card[i] == MJ_CARD_ARROW_HZ
					|| hand_card[i] == MJ_CARD_ARROW_FC
					|| hand_card[i] == MJ_CARD_ARROW_BB)
				{
					continue;
				}
			}
			//��ȡ����
			if (check_jiang)
			{
				if (remain_index[hand_card[i]] < 2)
				{
					continue;
				}
				remain_index[hand_card[i]] -= 2;
			}
			else
			{
				remain_index[hand_card[i]] -= 1;
				temp_count -= 1;
			}

			if (is_zi_card(hand_card[i]) && remain_index[hand_card[i]] > 0)
			{
				break;
			}
			bool bSucess = true;

			for (unsigned int j = 0; j < hand_card_count; j++)
			{
				BYTE  temp_card = hand_card[j];

				if (remain_index[temp_card] == 0)
				{
					continue;
				}
				if (is_zi_card(temp_card))
				{
					bSucess = false;
					break;
				}
				//	�жϺ����Ƿ��������Ķ���  
				int temp_card_value = get_card_value(temp_card);
				int iTempCount = remain_index[temp_card];

				if (temp_card_value < 8)
				{
					if (remain_index[temp_card + 1] >= iTempCount)
					{
						remain_index[temp_card] = 0;
						remain_index[temp_card + 1] -= iTempCount;
					}
					else
					{
						temp_count -= (iTempCount - remain_index[temp_card + 1]);
						remain_index[temp_card] = 0;
						remain_index[temp_card + 1] = 0;
					}

					if (remain_index[temp_card + 2] >= iTempCount)
					{
						remain_index[temp_card] = 0;
						remain_index[temp_card + 2] -= iTempCount;
					}
					else
					{
						temp_count -= (iTempCount - remain_index[temp_card + 2]);
						remain_index[temp_card] = 0;
						remain_index[temp_card + 2] = 0;
					}
				}
				else if (temp_card_value == 8)
				{
					if (remain_index[temp_card + 1] >= iTempCount)
					{
						remain_index[temp_card] = 0;
						remain_index[temp_card + 1] -= iTempCount;
					}
					else
					{
						temp_count -= (iTempCount - remain_index[temp_card + 1]);
						remain_index[temp_card] = 0;
						remain_index[temp_card + 1] = 0;
					}
					temp_count -= iTempCount;				
				}
				else if(temp_card_value == 9)
				{
					remain_index[temp_card] = 0;
					temp_count -= (iTempCount*2);
				}
				else
				{
					break;
				}
				if (temp_count < 0)
				{
					break;
				}
			}			
			if (temp_count >= 0 && bSucess)
			{
				return true;
			}
		}
		check_jiang = false;
	}
	return false;
}
//����Ƿ���ȫ˳����
bool CMJGameLgic::CheckIsAllSeqType(BYTE byCardList[], BYTE byCardCount, CMJBlockCard blocks)
{
	for (unsigned int i = 0; i < blocks.GetBlockLength(); i++)
	{
		BlockData	blockinfo;
		blocks.GetBlock(i, blockinfo);
		if (blockinfo.block_type != BLOCK_TYPE_CHI)
		{
			return false;
		}
	}
	BYTE byTempCardList[HAND_CARD_COUNT] = { 0 };
	BYTE byTempCount = byCardCount;
	memcpy(byTempCardList, byCardList, byCardCount);

	int iKingCount = is_have_king_value(byTempCardList, byTempCount);
	if (iKingCount == byTempCount)
	{
		return true;
	}
	int len = 0;
	for (int i = 0; i < HAND_CARD_COUNT; i++)
	{
		if (is_king(byTempCardList[i]) == false)
		{
			byTempCardList[len++] = byTempCardList[i];
		}
	}
	byTempCount -= iKingCount;
	sort_card(byTempCardList, byTempCount);

	if (checkIsAllSeq(byTempCardList, byTempCount, iKingCount,false))
	{
		return true;
	}
	return false;
}
//�ж��Ƿ���������
bool CMJGameLgic::CheckPPH(CMJHandCard handcards, CMJBlockCard blocks)
{
	//�ж���������
	CMJBlockCard tempBlockCard;
	tempBlockCard.AddBlockCards(blocks);
	for (unsigned int i = 0; i < tempBlockCard.GetBlockLength(); i++)
	{
		BlockData	blockinfo;
		tempBlockCard.GetBlock(i, blockinfo);
		if (blockinfo.block_type == BLOCK_TYPE_CHI)
		{
			return false;
		}
	}
	BYTE  byTempCardlist[HAND_CARD_COUNT] = { 0 };
	BYTE byTempCount = handcards.GetAllHandCard(byTempCardlist);

	BYTE byCardList[HAND_CARD_COUNT] = { 0 };
	BYTE byCardcount = 0;
	int  iKingCount = 0;	
	for (int i = 0; i < byTempCount; i++)
	{
		if (is_king(byTempCardlist[i]))
		{
			iKingCount++;
		}
		else
		{
			byCardList[byCardcount++] = byTempCardlist[i];
		}
	}	
	sort_card(byCardList, byCardcount);

	bool bCardUserIndex[CARD_INDEX_NUM] = { false };
	//�ж���������
	bool	bJang = false;
	for (int i = 0; i < byCardcount; i++)
	{
		if (bCardUserIndex[byCardList[i]]) continue;

		int iGetCount = get_card_count(byCardList, byCardcount, byCardList[i]);
		if (iGetCount == 3)
		{
			bCardUserIndex[byCardList[i]] = true;
			continue;
		}
		if (iGetCount == 2)
		{
			if (!bJang)
			{
				bJang = true;
			}
			else
			{
				if (iKingCount > 0)
				{
					iKingCount--;
				}
				else
				{
					return false;
				}
			}
		}
		if (iGetCount == 1 || iGetCount == 4)
		{
			if (!bJang)
			{
				if (iKingCount > 0)
				{
					iKingCount--;
					bJang = true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				if (iKingCount > 1)
				{
					iKingCount -= 2;
				}
				else
				{
					return false;
				}
			}
		}
		bCardUserIndex[byCardList[i]] = true;
	}
	return true;
}
//����Ƿ��к���
int CMJGameLgic::is_have_hz(unsigned char handcard[], unsigned char cardcount)
{
	int king_count = 0;
	king_count = this->get_card_count(handcard, cardcount, MJ_CARD_ARROW_HZ);
	return king_count;
}
bool CMJGameLgic::check_can_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, bool jiang)
{

	if (hand_card_count == 0)
	{
		return true;
	}
	unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_card, hand_card, hand_card_count);
	unsigned int temp_hand_card_count = hand_card_count;


	//��ȡ���Ӹ���
	unsigned int king_count = 0;
	if (m_allow_laizi)
	{
		king_count = is_have_hz(temp_hand_card, temp_hand_card_count);

		if (king_count >= m_king_count_limit)
		{//�ĺ���ֱ�Ӻ���
			return true;
		}
	}
	sort_card(temp_hand_card, temp_hand_card_count);
	bool check_reslut = false;

	unsigned char temp_no_king_card[HAND_CARD_COUNT] = { 0x00 };

	if (m_king_count == 0)
	{
		memcpy(temp_no_king_card, temp_hand_card, temp_hand_card_count);
	}
	else
	{
		int len = 0;
		for (int i = 0; i < HAND_CARD_COUNT; i++)
		{
			if (is_king(temp_hand_card[i]) == false)
			{
				temp_no_king_card[len++] = temp_hand_card[i];
			}
		}
	}
	unsigned int temp_no_king_card_count = temp_hand_card_count - king_count;
	sort_card(temp_no_king_card, temp_no_king_card_count);
	if (temp_no_king_card_count <= 0)
	{
		return true;
	}
	check_reslut = is_sequence(temp_no_king_card, temp_no_king_card_count, king_count, jiang);

	return check_reslut;

}
unsigned char CMJGameLgic::check_can_win(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card, bool first_card, bool add, bool bking)
{

	unsigned char win_type = BLOCK_TYPE_GAVE;
	if (hand_card_count > HAND_CARD_COUNT || hand_card_count == 0)
	{
		return win_type;
	}
	if (check_cardvalue(check_card) == false)
	{
		return  win_type;
	}
	unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_card, hand_card, hand_card_count);
	unsigned int temp_hand_card_count = hand_card_count;

	//���������ӵ�������
	if (add)
	{
		add_card(temp_hand_card, temp_hand_card_count, &check_card, 1);
	}

	//��ȡ���Ӹ���
	unsigned int king_count = 0;
	if (m_allow_laizi)
	{
		int hz_count = is_have_hz(temp_hand_card, temp_hand_card_count);
		//if (hz_count >= m_king_count_limit && 1 == m_beight)
		//{//�ĺ���ֱ�Ӻ���
		//	return BLOCK_TYPE_HU;
		//}
		king_count = hz_count;
		/*king_count = is_have_king_value(temp_hand_card, temp_hand_card_count);
		if (is_king(check_card) && bking)
		{
			king_count--;
		}*/
	}

	sort_card(temp_hand_card, temp_hand_card_count);
	bool check_reslut = false;

	unsigned char temp_no_king_card[HAND_CARD_COUNT] = { 0x00 };

	if (m_king_count == 0)
	{
		memcpy(temp_no_king_card, temp_hand_card, temp_hand_card_count);
	}
	else
	{
		int len = 0;
		for (int i = 0; i < HAND_CARD_COUNT; i++)
		{
			if (is_king(temp_hand_card[i]) == false)
			{
				temp_no_king_card[len++] = temp_hand_card[i];
			}
			/*for (int j = 0; j < m_king_count; j++)
			{
			if (temp_hand_card[i] != m_king_card[j])
			{
			break;
			}
			}*/

		}
	}
	unsigned int temp_no_king_card_count = temp_hand_card_count - king_count;
	sort_card(temp_no_king_card, temp_no_king_card_count);
	if (temp_no_king_card_count <= 0)
	{
		if (king_count > 0)
		{
			return BLOCK_TYPE_HU;  // �ܺ�, ���ƿ��ܶ���������
		}
		return win_type;
	}


	check_reslut = checkwinnew(temp_no_king_card, temp_no_king_card_count, king_count);

	if (check_reslut)
	{
		win_type = BLOCK_TYPE_HU;
	}

	return win_type;
}

bool CMJGameLgic::checkwinnew(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount)
{

	if ((hand_card_count + kingcount) % 3 != 2)
	{
		return false;
	}
	if (hand_card_count == 0)
	{
		if (kingcount % 3 == 2)
		{
			return true;
		}
		return false;
	}
	//�ж��߶�
	if (check_qidui(hand_card, hand_card_count, kingcount) && m_bqidui)
	{
		return true;
	}

	////�ж��Ƿ���131
	//if (check_131(hand_card, hand_card_count, kingcount))
	//{
	//	return true;
	//}

	////�ж�ʮ���� 
	//if (check_qbk(hand_card, hand_card_count, kingcount))
	//{
	//	return true;
	//}

	bool	check_jiang = true;
	bool	bwin = false;
	//��һ�μ�������Ƿ��н��ƣ��ڶ��β���������Ӵ���
	for (int s = 0; s < 2; s++)
	{
		bool check_card[CARD_INDEX_NUM];
		memset(check_card, 0, sizeof(check_card));

		if (check_jiang == false && kingcount == 0)
		{//����û��������û�����ӣ����ܺ���
			return false;
		}
		//��������
		for (unsigned int i = 0; i < hand_card_count; i++)
		{
			if (check_card[hand_card[i]])
			{
				continue;
			}
			//��¼��������
			check_card[hand_card[i]] = true;

			//ÿ���������ƺ�������
			int temp_count = kingcount;

			//�ֽ��ƺ�ʣ�����
			int remain_index[CARD_INDEX_NUM] = { 0 };
			change_card_index(hand_card, hand_card_count, remain_index);

			//��ȡ����
			if (check_jiang)
			{
				if (remain_index[hand_card[i]] < 2)
				{
					continue;
				}
				if (m_allow_mix_jiang)
				{
					remain_index[hand_card[i]] -= 2;
				}
				else
				{
					if (is_258(hand_card[i]))
					{
						remain_index[hand_card[i]] -= 2;
					}
					else
					{
						continue;
					}
				}
			}
			else
			{//ȥһ������������
				if (m_allow_mix_jiang)
				{
					remain_index[hand_card[i]] -= 1;
					temp_count -= 1;
				}
				else
				{
					if (is_258(hand_card[i]))
					{
						remain_index[hand_card[i]] -= 1;
						temp_count -= 1;
					}
					else
					{
						continue;
					}

				}
			}
			//ȥ�����з��ƺ����ƵĿ���
			for (int iwind = 0; iwind < 4; iwind++)
			{
				remain_index[MJ_CARD_WIND_EAST + iwind] = remain_index[MJ_CARD_WIND_EAST + iwind] % 3;
			}
			for (int iarrow = 0; iarrow <3; iarrow++)
			{
				remain_index[MJ_CARD_ARROW_HZ + iarrow] = remain_index[MJ_CARD_ARROW_HZ + iarrow] % 3;
			}
			for (unsigned int j = 0; j < hand_card_count; j++)
			{
				unsigned char  temp_card = hand_card[j];

				if (remain_index[temp_card] == 0)
				{
					continue;
				}

				if (temp_card  < MJ_CARD_WIND_EAST)
				{
					int same_flower_index[9] = { 0 };
					unsigned char first_card = 0x00;
					if (temp_card >= MJ_CARD_WAN_1 && temp_card <= MJ_CARD_WAN_9)
					{
						first_card = MJ_CARD_WAN_1;
					}
					else if (temp_card >= MJ_CARD_TIAO_1 && temp_card <= MJ_CARD_TIAO_9)
					{
						first_card = MJ_CARD_TIAO_1;
					}
					else if (temp_card >= MJ_CARD_TONG_1 && temp_card <= MJ_CARD_TONG_9)
					{
						first_card = MJ_CARD_TONG_1;
					}
					if (first_card != 0x00)
					{
						for (int num = 0; num < 9; num++)
						{
							same_flower_index[num] = remain_index[first_card + num];

							remain_index[first_card + num] = 0;
						}

						temp_count -= check_index_combox(same_flower_index);
					}
					if (temp_count < 0)
					{
						break;
					}
				}
				else if (temp_card >= MJ_CARD_WIND_EAST &&  temp_card <= MJ_CARD_WIND_NORTH)
				{
					unsigned int win_total_count = 0;
					unsigned int min_count = remain_index[temp_card] % 3;
					unsigned int max_count = remain_index[temp_card] % 3;
					unsigned int wind_kind = 0;
					unsigned int wind_count[4] = { 0 };
					for (int iwind = 0; iwind < 4; iwind++)
					{
						int card_count = remain_index[MJ_CARD_WIND_EAST + iwind] % 3;
						if (card_count > 0)
						{
							wind_kind++;
							wind_count[iwind] = card_count;
							win_total_count += wind_count[iwind];
							if (min_count > wind_count[iwind])
							{
								min_count = wind_count[iwind];
							}
							if (max_count < wind_count[iwind])
							{
								max_count = wind_count[iwind];
							}
						}
					}

					//����
					unsigned int wind_count_index[5] = { 0 };
					for (int index = 0; index < 4; index++)
					{
						unsigned int win_card_count = wind_count[index];
						if (win_card_count>0)
						{
							wind_count_index[win_card_count]++;
						}
					}

					////��������С����
					unsigned int card_sort_index[4] = { 0 };
					unsigned int win_len = 0;
					for (int index = 0; index < 5; index++)
					{
						for (unsigned int m = 0; m < wind_count_index[index]; m++)
						{
							card_sort_index[win_len++] = index;
						}
					}

					for (int iwind = 0; iwind < 4; iwind++)
					{
						if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
						{
							remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
						}
					}

					if (wind_kind == 1)
					{
						if (min_count % 3 != 0)
						{
							temp_count -= (3 - (min_count % 3));
						}
					}
					else if (wind_kind == 2)
					{
						if (m_allow_wind_chi)
						{
							if (max_count == min_count)
							{
								temp_count -= (max_count % 3);
							}
							else
							{
								if (min_count == 1 && max_count == 2)
								{
									temp_count -= 3;
								}
								else
								{
									if (win_total_count % 3 != 0)
									{
										temp_count -= (3 - (win_total_count % 3));
									}
								}
							}
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						}
					}
					else if (wind_kind == 3)
					{
						if (m_allow_wind_chi)
						{
							if (card_sort_index[0] == 1 && card_sort_index[1] == 2 && card_sort_index[2] == 3)
							{
								temp_count -= 3;
							}
							else if (card_sort_index[0] == 2 && card_sort_index[1] == 3 && card_sort_index[2] == 4)
							{
								temp_count -= 3;
							}
							else
							{
								if (win_total_count % 3 != 0)
								{
									temp_count -= (3 - (win_total_count % 3));
								}
							}
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						}
					}
					else if (wind_kind == 4)
					{
						if (m_allow_wind_chi)
						{
							if (win_total_count % 3 != 0)
							{
								temp_count -= (3 - (win_total_count % 3));
							}
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						}
					}
				}
				else if (temp_card >= MJ_CARD_ARROW_HZ &&  temp_card <= MJ_CARD_ARROW_BB)
				{//��
					unsigned int arrow_total_count = 0;
					unsigned int min_count = remain_index[temp_card] % 3;
					unsigned int max_count = remain_index[temp_card] % 3;
					unsigned int arrow_kind = 0;
					unsigned int arrow_count[3] = { 0 };
					for (int iarrow = 0; iarrow < 3; iarrow++)
					{
						int card_count = remain_index[MJ_CARD_ARROW_HZ + iarrow] % 3;
						if (card_count > 0)
						{
							arrow_kind++;
							arrow_count[iarrow] = card_count;
							arrow_total_count += arrow_count[iarrow];
							if (min_count > arrow_count[iarrow])
							{
								min_count = arrow_count[iarrow];
							}
							if (max_count < arrow_count[iarrow])
							{
								max_count = arrow_count[iarrow];
							}
						}
					}

					//����
					unsigned int arrow_count_index[5] = { 0 };
					for (int index = 0; index < 3; index++)
					{
						unsigned int arrow_card_count = arrow_count[index];
						if (arrow_card_count>0 && arrow_card_count <= 4)
						{
							arrow_count_index[arrow_card_count]++;
						}
					}

					////��������С����
					unsigned int card_sort_index[4] = { 0 };
					unsigned int win_len = 0;
					for (int index = 0; index < 5; index++)
					{
						for (unsigned int m = 0; m < arrow_count_index[index]; m++)
						{
							card_sort_index[win_len++] = index;
						}
					}

					for (int iarrow = 0; iarrow < 3; iarrow++)
					{
						remain_index[MJ_CARD_ARROW_HZ + iarrow] = 0;
					}

					if (m_allow_arrow_chi)
					{
						if (arrow_kind == 1)
						{//�����ɿ��� 
							remain_index[temp_card] = 0;
							if (min_count % 3 != 0)
							{
								temp_count -= (3 - min_count % 3);
							}
						}
						else if (arrow_kind == 2)
						{
							if (max_count == min_count)
							{
								temp_count -= (max_count % 3);
							}
							else
							{
								if (min_count == 1 && max_count == 2)
								{
									temp_count -= 3;
								}
								else
								{
									if (arrow_total_count % 3 != 0)
									{
										temp_count -= (3 - arrow_total_count % 3);
									}
								}
							}

						}
						else if (arrow_kind == 3)
						{
							if (min_count == max_count)
							{

							}
							else
							{
								if (card_sort_index[0] == 1 && card_sort_index[1] == 2 && card_sort_index[2] == 3)
								{
									temp_count -= 3;
								}
								else
								{
									if (arrow_total_count % 3 != 0)
									{
										temp_count -= (3 - (arrow_total_count % 3));
									}
								}
							}
						}
					}
					else
					{
						for (int temp_i = 0; temp_i < 3; temp_i++)
						{
							if (arrow_count[temp_i] % 3 != 0)
							{
								temp_count -= (3 - arrow_count[temp_i] % 3);
							}
						}
					}

				}
				else
				{
					/*

					if (remain_index[temp_card] == 2)
					{
					if (temp_card >= MJ_CARD_WIND_EAST &&  temp_card <= MJ_CARD_WIND_NORTH)
					{
					if (m_allow_wind_chi)
					{//����Է�
					int wind_count = 0;
					for (int iwind = 0; iwind < 4; iwind++)
					{
					if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
					{
					wind_count++;
					}
					}

					if (wind_count == 1 || wind_count == 2)
					{
					remain_index[temp_card] = 0;
					temp_count -= 1;
					}
					else if (wind_count == 3)
					{
					int del_count = 0;
					for (int iwind = 0; iwind < 4; iwind++)
					{
					if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
					{
					del_count += remain_index[MJ_CARD_WIND_EAST + iwind];
					remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
					}
					}
					temp_count -= (6 - del_count);
					}
					else if (wind_count == 4)
					{
					int win_total_count = 0;
					for (int iwind = 0; iwind < 4; iwind++)
					{
					win_total_count += remain_index[MJ_CARD_WIND_EAST + iwind];
					remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
					}
					if (win_total_count % 3 != 0)
					{
					temp_count -= (3 - (win_total_count % 3));
					}
					}
					}
					else
					{
					//��ɿ���
					remain_index[temp_card] = 0;
					temp_count -= 1;
					}

					}
					else if (temp_card >= MJ_CARD_ARROW_HZ &&  temp_card <= MJ_CARD_ARROW_BB)
					{//��
					if (m_allow_arrow_chi)
					{//�������
					int arrow_count = 0;
					for (int iwind = 0; iwind < 3; iwind++)
					{
					if (remain_index[MJ_CARD_ARROW_HZ + iwind] > 0)
					{
					arrow_count++;
					}
					}

					if (arrow_count == 1 || arrow_count == 2)
					{
					remain_index[temp_card] = 0;
					temp_count -= 1;
					}
					else if (arrow_count == 3)
					{
					int del_count = 0;
					for (int iwind = 0; iwind < 3; iwind++)
					{
					if (remain_index[MJ_CARD_ARROW_HZ + iwind] > 0)
					{
					del_count += remain_index[MJ_CARD_ARROW_HZ + iwind];
					remain_index[MJ_CARD_ARROW_HZ + iwind] = 0;
					}
					}
					temp_count -= (6 - del_count);
					}

					}
					else
					{
					//��ɿ���
					remain_index[temp_card] = 0;
					temp_count -= 1;
					}
					}
					}
					if (temp_count < 0)
					{
					break;
					}
					if (remain_index[temp_card] == 1)
					{
					//�ж��Ƿ��Ƿ��ƣ���������
					if (temp_card >= MJ_CARD_WIND_EAST &&  temp_card <= MJ_CARD_WIND_NORTH)
					{
					if (m_allow_wind_chi)
					{
					int wind_count = 0;
					for (int iwind = 0; iwind < 4; iwind++)
					{
					if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
					{
					wind_count++;
					}

					}

					if (wind_count == 1)
					{
					remain_index[temp_card] = 0;
					temp_count -= 2;
					}
					else if (wind_count == 2)
					{
					remain_index[temp_card] = 0;
					temp_count -= 1;
					for (int iwind = 0; iwind < 4; iwind++)
					{
					if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0 && (MJ_CARD_WIND_EAST + iwind) != temp_card)
					{
					remain_index[MJ_CARD_WIND_EAST + iwind] -= 1;
					break;
					}
					}
					}
					else if (wind_count == 3)
					{
					remain_index[temp_card] = 0;
					for (int iwind = 0; iwind < 4; iwind++)
					{
					if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0 && (MJ_CARD_WIND_EAST + iwind) != temp_card)
					{
					remain_index[MJ_CARD_WIND_EAST + iwind] -= 1;
					}
					}
					}
					else if (wind_count == 4)
					{
					int win_total_count = 0;
					for (int iwind = 0; iwind < 4; iwind++)
					{
					win_total_count += remain_index[MJ_CARD_WIND_EAST + iwind];
					remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
					}
					if (win_total_count % 3 != 0)
					{
					temp_count -= (3 - (win_total_count % 3));
					}

					}

					}
					else
					{
					remain_index[temp_card] = 0;
					temp_count -= 2;

					}
					}
					else if (temp_card >= MJ_CARD_ARROW_HZ &&  temp_card <= MJ_CARD_ARROW_BB)
					{
					if (m_allow_arrow_chi)
					{
					int arrow_count = 0;
					for (int iwind = 0; iwind < 3; iwind++)
					{
					if (remain_index[MJ_CARD_ARROW_HZ + iwind] > 0)
					{
					arrow_count++;
					}

					}

					if (arrow_count == 1)
					{
					remain_index[temp_card] = 0;
					temp_count -= 2;
					}
					else if (arrow_count == 2)
					{
					remain_index[temp_card] = 0;
					temp_count -= 1;
					for (int iwind = 0; iwind < 3; iwind++)
					{
					if (remain_index[MJ_CARD_ARROW_HZ + iwind] > 0 && (MJ_CARD_ARROW_HZ + iwind) != temp_card)
					{
					remain_index[MJ_CARD_ARROW_HZ + iwind] -= 1;
					break;
					}
					}
					}
					else if (arrow_count == 3)
					{
					remain_index[MJ_CARD_ARROW_HZ] -= 1;
					remain_index[MJ_CARD_ARROW_HZ + 1] -= 1;
					remain_index[MJ_CARD_ARROW_HZ + 2] -= 1;
					}
					}
					else
					{
					remain_index[temp_card] = 0;
					temp_count -= 2;
					}
					}

					if (temp_count < 0)
					{
					break;
					}
					}
					*/

				}

				if (temp_count < 0)
				{
					break;
				}
			}

			if (temp_count >= 0)
			{
				return true;
			}
		}
		check_jiang = false;
	}

	return false;
}

/*����Ƿ��������--���ƺ�
@param: hand_card  �������
@param : hand_card_count  ���������
@param : ting_card  �����б�
@param : bking  �Ƿ�������
@param : king_card  ����
return  result
*/
unsigned char CMJGameLgic::check_can_ting(const unsigned char hand_card[], const unsigned int hand_card_count, TingData& ting_info, bool first_card)
{

	unsigned char gang_type = BLOCK_TYPE_GAVE;
	if (hand_card_count > 0 && hand_card_count <= HAND_CARD_COUNT)
	{
		bool	bTileFalg[CARD_INDEX_NUM];
		memset(bTileFalg, 0, sizeof(bTileFalg));
		unsigned int  temp_ting_count = 0;
		for (unsigned int i = 0; i < hand_card_count; i++)
		{
			if (!check_cardvalue(hand_card[i]))
			{
				return gang_type;
			}
			if (bTileFalg[hand_card[i]])
			{
				continue;
			}
			unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
			memcpy(temp_hand_card, hand_card, hand_card_count);
			if (del_card_value(temp_hand_card, hand_card_count, hand_card[i], 1))
			{
				bTileFalg[hand_card[i]] = true;

				unsigned char win_card[NORMAL_CARD_NUM] = { 0x00 };
				if (is_ting(temp_hand_card, hand_card_count - 1, win_card, first_card))
				{
					gang_type = BLOCK_TYPE_TING;
					ting_info.ting_card[temp_ting_count][0] = hand_card[i];
					memcpy(&ting_info.ting_card[temp_ting_count][1], win_card, sizeof(win_card));
					temp_ting_count++;
				}
			}
		}
	}
	return  gang_type;
}

//����Ƿ�����
bool CMJGameLgic::is_ting(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char win_card[], bool first_card)
{
	if (hand_card_count % 3 == 1)
	{
		bool bTing = false;
		unsigned int ting_count = 0;
		for (unsigned int i = 0; i < NORMAL_CARD_NUM; i++)
		{
			unsigned char check_reslut = 0x00;
			check_reslut = check_can_win(hand_card, hand_card_count, m_normal_card[i], first_card, true, false);
			if (check_reslut == BLOCK_TYPE_HU)
			{
				bTing = true;
				win_card[ting_count++] = m_normal_card[i];
			}
		}

		return bTing;
	}
	return false;

}

