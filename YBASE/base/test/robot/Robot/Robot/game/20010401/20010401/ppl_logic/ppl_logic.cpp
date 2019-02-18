#include "ppl_logic.h"
#include <random>

//随机算法
static int GetRandValue(int iLeft, int iRight)
{
	if (iLeft > iRight)
	{
		iRight = iLeft;
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(iLeft, iRight);
	return dis(gen);
}

unsigned char CGameLogic::GetHighCard(unsigned char iCard[], int iCardCount)
{
	unsigned char result_value = 0x00;
	if (iCard == NULL)
	{
		return result_value;
	}
	if (iCardCount > ALL_CARD_COUNT)
	{
		return result_value;
	}
	for (int i = 0; i < iCardCount; i++)
	{
		if (result_value == 0x00)
		{
			result_value = iCard[i];
			continue;
		}
		if (GetCardNum(result_value) < GetCardNum(iCard[i]))
		{
			result_value = iCard[i];
		}
		else if (GetCardNum(result_value) == GetCardNum(iCard[i]))
		{
			if (GetCardColor(result_value) < GetCardColor(iCard[i]))
			{
				result_value = iCard[i];
			}
		}
		else
		{
			;
		}
	}

	return result_value;
}

CGameLogic::CGameLogic()
{
	m_have_king = false;
} 

CGameLogic::~CGameLogic()
{ 

}

//混乱扑克
bool CGameLogic::RandCard(unsigned char iCard[], int iCardCount)
{
	if (iCardCount > ALL_CARD_COUNT)
	{
		return false;
	}
	unsigned char iCardList[ALL_CARD_COUNT];
	memset(iCardList, 0, sizeof(iCardList));
	memcpy(iCardList, iCard, iCardCount);
	int pos = 0;
	do
	{
		int i = GetRandValue(0, iCardCount - pos - 1);
		iCard[pos] = iCardList[i];
		pos++;
		iCardList[i] = iCardList[iCardCount - pos];
	} while (pos < iCardCount);

	return true;
}

//取出牌列表
bool CGameLogic::GetCardList(unsigned char iCard[], int iCardCount, unsigned char iOutList[], int iOutLen)
{
	if (iCardCount > ALL_CARD_COUNT)
	{
		return false;
	}
	RandCard(iCard, iCardCount);
	for (int i = 0; i < iOutLen; i++)
	{
		int pos = iCardCount - i - 1;
		iOutList[i] = iCard[pos];
	}

	return true;
}

//添加牌列表
int CGameLogic::AddCardList(unsigned char iCard[], int iCardCount, unsigned char iOutList[], int iOutLen)
{
	int addcount = 0;
	if (iCardCount + iOutLen > ALL_CARD_COUNT)
	{
		return addcount;
	}
	for (int i = 0; i < iOutLen; i++)
	{
		if (iOutList[i] == 0x00)
		{
			continue;
		}
		iCard[iCardCount++] = iOutList[i];
		addcount++;
	}

	return addcount;
}

//随机设置癞子牌
int CGameLogic::SetReplaceCard(bool b_is_have_king)
{
	m_have_king = b_is_have_king;
	if (b_is_have_king)
	{
		return ALL_CARD_COUNT;
	}
	return ALL_CARD_COUNT - 2;
}

//获取扑克数字
unsigned char CGameLogic::GetCardNum(unsigned char iCard)
{
	unsigned char iCardNum = iCard & UG_VALUE_MASK;
	if (iCardNum == 0x01)
	{
		iCardNum = 14;
	}
	else if (iCardNum == 0x0E || iCardNum == 0x0F)
	{
		iCardNum = 15;
	}
	return iCardNum;
}

//扑克中是否存在指定扑克点数大小的牌
bool CGameLogic::IsHaveCardNum(unsigned char iCard[], unsigned char iCardCount, unsigned char iCardValue)
{
	for (int i = 0; i < iCardCount; i++)
	{
		if (GetCardNum(iCard[i]) == iCardValue)
		{
			return true;
		}
	}
	return false;
}

//检查牌列表类型
/*
[in]iCard: 牌列表
[in]iCardCount: 牌数量
[in]iPublicCard: 公共牌
[out]TypeValue:牌类型数值
[out]Type：牌类型
[out]High: 牌中的高牌
*/
bool CGameLogic::CheckCardListType(unsigned char iCard[], int iCardCount, int &TypeValue, int &Type, unsigned char &HighCard)
{
	bool res = false;
	if (iCard == NULL)
	{
		return res;
	}
	if (iCardCount > MAX_COUNT)
	{
		return res;
	}
	unsigned char temp_list[MAX_COUNT] = { 0 };
	memcpy(temp_list, iCard, sizeof(temp_list));
	int temp_len = iCardCount;
	int temp_type = GetCardType(temp_list, temp_len);

	switch (temp_type)
	{
	case TYPE_THREE:
	{
		//判断牌型三条
		res = true;
		TypeValue = 15;
		HighCard = GetHighCard(temp_list, temp_len);
		Type = TYPE_THREE;
		break;
	}
	case TYPE_STR_FLUSH:
	{
		//判断牌型同花顺
		res = true;
		TypeValue = 10;
		HighCard = GetHighCard(temp_list, temp_len);
		Type = TYPE_STR_FLUSH;
		break;
	}
	case TYPE_FLUSH:
	{
		//判断牌型同花
		res = true;
		TypeValue = 6;
		HighCard = GetHighCard(temp_list, temp_len);
		Type = TYPE_FLUSH;
		break;
	}
	case TYPE_STR:
	{
		//判断牌型顺子
		res = true;
		TypeValue = 4;
		HighCard = GetHighCard(temp_list, temp_len);
		Type = TYPE_STR;
		break;
	}
	case TYPE_PAIR:
	{
		//判断牌型对子
		res = true;
		TypeValue = 2;
		HighCard = GetHighCard(temp_list, temp_len);
		Type = TYPE_PAIR;
		break;
	}
	case TYPE_SPEIAL:
	{
		//判断牌型杂花
		res = true;
		TypeValue = 0;
		HighCard = GetHighCard(temp_list, temp_len);
		Type = TYPE_SPEIAL;
		break;
	}
	case TYPE_HIGH_CARD:
	{
		//判断牌型高牌
		res = true;
		TypeValue = 0;
		HighCard = GetHighCard(temp_list, temp_len);
		Type = TYPE_HIGH_CARD;
		break;
	}
	default:
		break;
	}
	return res;
}

bool CGameLogic::IsReplaceCard(unsigned char byCard)
{
	if (byCard == 0 || m_have_king == false)
	{
		return false;
	}

	if (GetCardNum(byCard) == 15 || GetCardNum(byCard) == 16)
	{
		return true;
	}
	return false;
}

//检查是否三条
bool CGameLogic::CheckThree(unsigned char iCard[], int iCardCount)
{
	if (iCard == NULL)
	{
		return false;
	}
	if (iCardCount != MAX_COUNT)
	{
		return false;
	}
	SortCard(iCard, iCardCount);
	int	king_num = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (IsReplaceCard(iCard[i]))
			king_num++;
	}
	if (king_num == 2)
	{
		return true;
	}
	else if (king_num == 1)
	{
		if (GetCardNum(iCard[1]) == GetCardNum(iCard[2]))
		{
			return true;
		}
	}
	else
	{
		if (GetCardNum(iCard[0]) == GetCardNum(iCard[1]) && GetCardNum(iCard[1]) == GetCardNum(iCard[2]))
		{
			return true;
		}
	}
	return false;
}

//是否是顺子123
bool CGameLogic::CheckStraight123(unsigned char iCard[], unsigned char iCardCount)
{
	if (iCard == NULL)
	{
		return false;
	}
	if (iCardCount != MAX_COUNT)
	{
		return false;
	}
	SortCard(iCard, iCardCount);
	int	king_num = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (IsReplaceCard(iCard[i]))
			king_num++;
	}
	if (king_num == 2)
	{
		if (GetCardNum(iCard[2]) == 14 || GetCardNum(iCard[2]) == 2 || GetCardNum(iCard[2]) == 3)
		{
			return true;
		}
	}
	else if (king_num == 1)
	{
		if ((IsHaveCardNum(iCard, iCardCount, 2) && IsHaveCardNum(iCard, iCardCount, 3)) ||
			(IsHaveCardNum(iCard, iCardCount, 14) && IsHaveCardNum(iCard, iCardCount, 2))||
			(IsHaveCardNum(iCard, iCardCount, 14) && IsHaveCardNum(iCard, iCardCount, 3)))
		{
			return true;
		}
	}
	else
	{
		if (IsHaveCardNum(iCard, iCardCount, 14) && 
			IsHaveCardNum(iCard, iCardCount, 0x02) && 
			IsHaveCardNum(iCard, iCardCount, 0x03))
		{
			return true;
		}
	}
	
	return false;
}

//是否是顺子QKA
bool CGameLogic::CheckStraightQKA(unsigned char iCard[], unsigned char iCardCount)
{
	if (iCard == NULL)
	{
		return false;
	}
	if (iCardCount != MAX_COUNT)
	{
		return false;
	}
	SortCard(iCard, iCardCount);
	int	king_num = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (IsReplaceCard(iCard[i]))
			king_num++;
	}
	if (king_num == 2)
	{
		if (GetCardNum(iCard[2]) == 14 || GetCardNum(iCard[2]) == 12 || GetCardNum(iCard[2]) == 13)
		{
			return true;
		}
	}
	else if (king_num == 1)
	{
		if ((IsHaveCardNum(iCard, iCardCount, 12) && IsHaveCardNum(iCard, iCardCount, 13)) ||
			(IsHaveCardNum(iCard, iCardCount, 14) && IsHaveCardNum(iCard, iCardCount, 13)) ||
			(IsHaveCardNum(iCard, iCardCount, 14) && IsHaveCardNum(iCard, iCardCount, 12)))
		{
			return true;
		}
	}
	else
	{
		if (IsHaveCardNum(iCard, iCardCount, 14) &&
			IsHaveCardNum(iCard, iCardCount, 12) &&
			IsHaveCardNum(iCard, iCardCount, 13))
		{
			return true;
		}
	}

	return false;
}

//检查是否顺子
bool CGameLogic::CheckStraight(unsigned char iCard[], int iCardCount)
{
	if (iCard == NULL)
	{
		return false;
	}
	if (iCardCount != MAX_COUNT)
	{
		return false;
	}
	if (CheckStraight123(iCard, iCardCount))
	{
		return true;
	}
	unsigned char tempCard[3] = { 0 };
	SortCard(iCard, iCardCount);
	memcpy(tempCard, iCard, sizeof(tempCard));
	int	king_num = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (IsReplaceCard(iCard[i]))
			king_num++;
	}
	if (king_num == 2)
	{
		return true;
	}
	else if (king_num == 1)
	{
		if (GetCardNum(tempCard[1]) == GetCardNum(tempCard[2]) + 1)
		{
			return true;
		}
		if (GetCardNum(tempCard[1]) == GetCardNum(tempCard[2]) + 2)
		{
			return true;
		}
	}
	else
	{
		int cardnum0 = GetCardNum(tempCard[0]);
		int cardnum1 = GetCardNum(tempCard[1]);
		int cardnum2 = GetCardNum(tempCard[2]);
		if ((cardnum1 == cardnum0 - 1) && (cardnum2 == cardnum1 - 1))
		{
			return true;
		}
	}

	return false;
}

//检查是否同花色
bool CGameLogic::CheckSameColor(unsigned char iCard[], unsigned char iCardCount)
{
	if (iCard == NULL)
	{
		return false;
	}
	if (iCardCount != MAX_COUNT)
	{
		return false;
	}
	SortCard(iCard, iCardCount);
	int	king_num = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (IsReplaceCard(iCard[i]))
			king_num++;
	}
	if (king_num == 2)
	{
		return true;
	}
	else if (king_num == 1)
	{
		if (GetCardColor(iCard[1]) == GetCardColor(iCard[2]))
		{
			return true;
		}
		return false;
	}
	else
	{
		int tmpColor = GetCardColor(iCard[0]);
		for (int i = 0; i < iCardCount; i++)
		{
			if (GetCardColor(iCard[i]) != tmpColor)
			{
				return false;
			}
		}
	}
	return true;
}

//检查是否同花（非顺子）
bool CGameLogic::CheckFlush(unsigned char iCard[], unsigned char iCardCount)
{
	if (CheckStraight(iCard, iCardCount))
	{
		return false;
	}
	if (CheckSameColor(iCard, iCardCount))
	{
		return true;
	}
	return false;
}

//检查是否同花顺
bool CGameLogic::CheckStraightFlush(unsigned char iCard[], unsigned char iCardCount)
{
	if (CheckStraight(iCard, iCardCount) && CheckSameColor(iCard, iCardCount))
	{
		return true;
	}
	return false;
}

//检查是否对子
bool CGameLogic::CheckPair(unsigned char iCard[], int iCardCount)
{
	if (iCard == NULL)
	{
		return false;
	}
	if (iCardCount != MAX_COUNT)
	{
		return false;
	}
	SortCard(iCard, iCardCount);
	int	king_num = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (IsReplaceCard(iCard[i]))
			king_num++;
	}
	if (king_num == 2)
	{
		return true;
	}
	else if (king_num == 1)
	{
		return true;
	}
	else
	{
		if (GetCardNum(iCard[0]) == GetCardNum(iCard[1]))
		{
			return true;
		}
		if (GetCardNum(iCard[0]) == GetCardNum(iCard[2]))
		{
			return true;
		}
		if (GetCardNum(iCard[1]) == GetCardNum(iCard[2]))
		{
			return true;
		}
	}
	return false;
}

//检查是否杂花
bool CGameLogic::CheckTypeSpecial(unsigned char iCard[], int iCardCount)
{
	if (iCard == NULL)
	{
		return false;
	}
	if (iCardCount != MAX_COUNT)
	{
		return false;
	}
	SortCard(iCard, iCardCount);
	int	king_num = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (IsReplaceCard(iCard[i]))
			king_num++;
	}
	if (king_num == 2)
	{
		if (IsHaveCardNum(iCard, iCardCount, 0x02) ||
			IsHaveCardNum(iCard, iCardCount, 0x03) ||
			IsHaveCardNum(iCard, iCardCount, 0x05))
		{
			return true;
		}
	}
	else if (king_num == 1)
	{
		int iCardColor1 = GetCardColor(iCard[1]);
		int iCardColor2 = GetCardColor(iCard[2]);
		if ((IsHaveCardNum(iCard, iCardCount, 0x02) && IsHaveCardNum(iCard, iCardCount, 0x03)) ||
			(IsHaveCardNum(iCard, iCardCount, 0x02) && IsHaveCardNum(iCard, iCardCount, 0x05)) || 
			(IsHaveCardNum(iCard, iCardCount, 0x03) && IsHaveCardNum(iCard, iCardCount, 0x05))
			)
		{
			return true;
		}
	}
	else
	{
		int iCardColor1 = GetCardColor(iCard[0]);
		int iCardColor2 = GetCardColor(iCard[1]);
		int iCardColor3 = GetCardColor(iCard[2]);

		if (iCardColor1 == iCardColor2 && iCardColor2 == iCardColor3)
		{
			return false;
		}

		if (IsHaveCardNum(iCard, iCardCount, 0x02) &&
			IsHaveCardNum(iCard, iCardCount, 0x03) &&
			IsHaveCardNum(iCard, iCardCount, 0x05))
		{
			return true;
		}
	}
	
	return false;
}

//比较两张扑克的大小 0相等 1大于  2小于
unsigned char CGameLogic::CompareCardValue(unsigned char iCard_First, unsigned char iCard_Second, bool bCompareHua)
{
	unsigned char cbResult = 0;
	do
	{
		int cardNum1 = GetCardNum(iCard_First);
		int cardNum2 = GetCardNum(iCard_Second);
		if (cardNum1 > cardNum2)
		{
			cbResult = 1;
			break;
		}
		if (cardNum1 < cardNum2)
		{
			cbResult = 2;
			break;
		}
		if (bCompareHua)
		{
			if (cardNum1 == cardNum2 && cardNum1 == 15)
			{
				cbResult = 0;
				break;
			}
			if (iCard_First > iCard_Second)
			{
				cbResult = 1;
				break;
			}
			if (iCard_First < iCard_Second)
			{
				cbResult = 2;
				break;
			}
		}

	} while (0);
	return cbResult;
}

//获取对子中最大的牌值
void CGameLogic::GetDoubleMaxNums(unsigned char iCard[], unsigned char iCardCount, unsigned char& max, unsigned char& single)
{
	unsigned char temp_CardList[MAX_COUNT] = { 0 };
	memcpy(temp_CardList, iCard, iCardCount);

	SortCard(temp_CardList, iCardCount);
	int	king_num = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (IsReplaceCard(temp_CardList[i]))
			king_num++;
	}

	if (king_num == 2)
	{
		if (GetCardNum(temp_CardList[2]) == 14)
		{
			max = 0x31;
			single = 0x4F;
		}
		else
		{
			max = temp_CardList[2];
			single = 0x4F;
		}
	}
	else if (king_num == 1)
	{
		max = temp_CardList[1];
		single = temp_CardList[2];
	}
	else
	{
		for (int i = 0; i < iCardCount; i++)
		{
			for (int j = 0; j < iCardCount; j++)
			{
				if (i == j) continue;
				if (GetCardNum(temp_CardList[i]) == GetCardNum(temp_CardList[j]))
				{
					if (temp_CardList[i] > temp_CardList[j])
					{
						max = temp_CardList[i];
					}
					else
					{
						max = temp_CardList[j];
					}
					for (int k = 0; k < iCardCount; k++)
					{
						if (k != i && k != j)
						{
							single = temp_CardList[k];
						}
					}
				}
			}
		}
	}
	
	return;
}

//比较大小 0相等 1大于 2小于
unsigned char CGameLogic::CompareCardData(unsigned char iCard_First[], unsigned char iCard_Second[], unsigned char type1, unsigned char type2, bool flag_ex, unsigned char iCardCount)
{
	unsigned char res = 0;

	unsigned char Card1_Shape = type1;
	unsigned char Card2_Shape = type2;

	if (flag_ex)
	{
		if (Card2_Shape == TYPE_SPEIAL)
		{
			Card2_Shape = TYPE_MAX;
		}
		if (Card1_Shape == TYPE_SPEIAL)
		{
			Card1_Shape = TYPE_MAX;
		}
	}
	else
	{
		if (Card1_Shape == TYPE_SPEIAL)
		{
			Card1_Shape = TYPE_HIGH_CARD;
		}
		if (Card2_Shape == TYPE_SPEIAL)
		{
			Card2_Shape = TYPE_HIGH_CARD; 
		}
	}

	if (Card1_Shape != Card2_Shape)
	{
		if (Card1_Shape > Card2_Shape)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		unsigned char byResult1[MAX_COUNT]  = { 0 };
		unsigned char byResult2[MAX_COUNT] = { 0 };
		memcpy(byResult1, iCard_First, iCardCount);
		memcpy(byResult2, iCard_Second, iCardCount);

		SortCard(byResult1, iCardCount);
		SortCard(byResult2, iCardCount);
		bool bNeedSinge = true;
		if (Card1_Shape == TYPE_THREE)
		{
			for (int pos = 0; pos < iCardCount; pos++)
			{
				res = CompareCardValue(byResult1[pos], byResult2[pos], true);
				if (res != 0)
				{
					return res;
				}
			}
		}
		else if (Card1_Shape == TYPE_HIGH_CARD
			|| Card1_Shape == TYPE_STR
			|| Card1_Shape == TYPE_STR_FLUSH
			|| Card1_Shape == TYPE_FLUSH)
		{
			bNeedSinge = true;
		}
		else if (Card1_Shape == TYPE_PAIR)
		{
			//比对的时候，先比对牌的牌值，牌值一样再对比单牌的牌值与花色
			unsigned char maxValue1 = 0;
			unsigned char singleValue1 = 0;
			unsigned char maxValue2 = 0;
			unsigned char singleValue2 = 0;
			GetDoubleMaxNums(byResult1, iCardCount, maxValue1, singleValue1);
			GetDoubleMaxNums(byResult2, iCardCount, maxValue2, singleValue2);
			res = CompareCardValue(maxValue1, maxValue2, false);
			if (res == 0)
			{
				res = CompareCardValue(singleValue1, singleValue2, true);
			}
			if (res == 0)
			{
				if (GetCardNum(maxValue1) < GetCardNum(singleValue1))
				{
					res = CompareCardValue(singleValue1, singleValue2, true);
				}
				else
				{
					res = CompareCardValue(maxValue1, maxValue2, true);
				}
			}
			return res;
		}
		//比较单牌
		if (bNeedSinge && res == 0)
		{
			if (CheckStraight123(iCard_First, iCardCount) || CheckStraight123(iCard_Second, iCardCount))
			{
				if (CheckStraight123(iCard_First, iCardCount))
				{
					if (CheckStraightQKA(iCard_Second, iCardCount))
					{
						return 2;
					}
					if (!CheckStraight123(iCard_Second, iCardCount))
					{
						return 1;
					}
				}
				if (CheckStraight123(iCard_Second, iCardCount))
				{
					if (CheckStraightQKA(iCard_First, iCardCount))
					{
						return 1;
					}
					if (!CheckStraight123(iCard_First, iCardCount))
					{
						return 2;
					}
				}
				SortCard(byResult1, iCardCount);
				SortCard(byResult2, iCardCount);
			}
			for (int i = 0; i < iCardCount; i++)
			{
				res = CompareCardValue(byResult1[i], byResult2[i], true);
				if (res > 0)
				{
					return res;
				}
			}
			if (res == 0)
			{
				res = CompareCardValue(byResult1[0], byResult2[0], true);
			}
		}
	}
	return res;
}

//获取牌型
unsigned char CGameLogic::GetCardType(unsigned char iCard[], unsigned char iCardCount)
{
	unsigned char CardList[MAX_COUNT] = { 0 };
	memcpy(CardList, iCard, MAX_COUNT);
	SortCard(iCard, iCardCount);
	if (CheckThree(iCard, iCardCount))
	{
		return TYPE_THREE;
	}
	if (CheckStraightFlush(iCard, iCardCount))
	{
		return TYPE_STR_FLUSH;
	}
	if (CheckFlush(iCard, iCardCount))
	{
		return TYPE_FLUSH;
	}
	if (CheckStraight(iCard, iCardCount))
	{
		return TYPE_STR;
	}
	if (CheckPair(iCard, iCardCount))
	{
		return TYPE_PAIR;
	}
	if (CheckTypeSpecial(iCard, iCardCount))
	{
		return TYPE_SPEIAL;
	}
	return TYPE_HIGH_CARD;
}

//排列扑克
bool CGameLogic::SortCard(unsigned char iCard[], unsigned char iCardCount, bool bSortA)
{
	bool bSorted = true;
	unsigned char bTempList[ALL_CARD_COUNT] = { 0 };
	int iLast = iCardCount - 1;
	unsigned char iTemp = 0x00;
	//获取位置数值
	for (int i = 0; i < iCardCount; i++)
	{
		bTempList[i] = GetCardNum(iCard[i]);
	}
	//排序操作(按从大到小排序)
	do
	{
		bSorted = true;
		for (int i = 0; i < iLast; i++)
		{
			if (bTempList[i] < bTempList[i + 1])
			{
				//冒泡排序
				iTemp = iCard[i];
				iCard[i] = iCard[i + 1];
				iCard[i + 1] = iTemp;

				iTemp = bTempList[i];
				bTempList[i] = bTempList[i + 1];
				bTempList[i + 1] = iTemp;

				bSorted = false;
			}
		}
		iLast--;
	} while (!bSorted);

	for (int i = 0; i < iCardCount - 1; i++)
	{
		if ((GetCardNum(iCard[i]) == GetCardNum(iCard[i + 1])) && (iCard[i] < iCard[i + 1]))
		{
			iTemp = iCard[i];
			iCard[i] = iCard[i + 1];
			iCard[i + 1] = iTemp;
		}
	}
	if (bSortA)
	{
		Reverse(iCard, iCardCount);
	}
	return true;
}

//翻转列表
bool CGameLogic::Reverse(unsigned char iCard[], unsigned char iCardCount)
{
	if (iCard == 0 || iCardCount > ALL_CARD_COUNT || iCardCount < 0)
	{
		return false;
	}
	unsigned char bTempList[ALL_CARD_COUNT];
	memset(bTempList, 0, ALL_CARD_COUNT);
	int index = 0;
	for (int i = iCardCount - 1; i >= 0; i--)
	{
		bTempList[index++] = iCard[i];
	}
	memcpy(iCard, bTempList, iCardCount);
	return true;
}

//删除单个扑克
int CGameLogic::RemoveCard(unsigned char iRemoveCard, unsigned char iCardList[], int iCardCount)
{
	if (iCardCount < 1)
	{
		return 0;
	}
	int iDeleteCount = 0; //把要删除的牌置零
	int iRecount = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (iCardList[i] == iRemoveCard)
		{
			iCardList[i] = 0x00;
			iDeleteCount++;
		}
	}
	iRecount = RemoveNummCard(iCardList, iCardCount); //删除做了标记的牌
	if (iDeleteCount != iRecount)
	{
		return 0;
	}
	return iDeleteCount;
}

//删除扑克
int CGameLogic::RemoveCard(unsigned char  iRemoveCard[],		//要删除的牌面
							int			  iRemoveCount,			//要删除的牌总数
							unsigned char iCardList[],			//要处理的数组
							int			  iCardCount)			//处理数组的上限
{
	//检验数据
	if (iRemoveCount > iCardCount) return 0;

	int iRecount = 0;
	int iDeleteCount = 0; //把要删除的牌置零

	for (int i = 0; i < iRemoveCount; i++)
	{
		for (int j = 0; j < iCardCount; j++)
		{
			if (iRemoveCard[i] == iCardList[j])
			{
				iDeleteCount++;
				iCardList[j] = 0x00;
				break;
			}
		}
	}
	iRecount = RemoveNummCard(iCardList, iCardCount); //删除做了标记的牌

	if (iDeleteCount != iRecount)
	{
		return 0;
	}

	return iDeleteCount;
}

//清除 0 位扑克
int CGameLogic::RemoveNummCard(unsigned char iCardList[], int iCardCount)
{
	if (iCardCount < 1)
	{
		return 0;
	}
	unsigned char byTempCardList[ALL_CARD_COUNT];
	memset(byTempCardList, 0, sizeof(byTempCardList));
	int iUseCardCount = 0;
	int iRemoveCount = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (iCardList[i] != 0x00)
		{
			byTempCardList[iUseCardCount++] = iCardList[i];
		}
		else
		{
			iRemoveCount++;
		}
	}
	memset(iCardList, 0, iCardCount);
	memcpy(iCardList, byTempCardList, iUseCardCount);

	return iRemoveCount;
}