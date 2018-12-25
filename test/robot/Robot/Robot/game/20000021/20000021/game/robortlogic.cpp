
#include "RobortLogic.h"
#include <string>
#include <random>
#include <time.h>

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



//转换所有牌值
int ChangeCardToIndex(const unsigned char card_value[], unsigned int card_count, unsigned int card_index[])
{
	if (card_count == 0)
	{
		return 0;
	}
	int change_count = 0;
	int temp_value = -1;
	for (unsigned int i = 0; i < card_count; i++)
	{
		if (0x00 != card_value[i])
		{
			temp_value = card_value[i];
			card_index[temp_value]++;
			change_count++;
		}
	}
	return change_count;
}

int mGetCardNum(BYTE iCard)
{
	int iCardNum = iCard&UG_VALUE_MASK;
	if (iCardNum == 1)
	{
		iCardNum = 14;
	}
	else if (iCardNum == 2)
	{
		iCardNum = 15;
	}

	return iCardNum;
}
//获取扑克花色
BYTE mGetCardHuaKind(BYTE iCard, BOOL bTrueHua)
{
	int iHuaKind = (iCard&UG_HUA_MASK);
	if (!bTrueHua)
	{
		return iHuaKind = UG_NT_CARD;
	}
	return iHuaKind;
}

bool IsCardValid(BYTE byCard)
{
	if ((byCard > 0x4F) || (byCard == 0x00))
	{
		return false;
	}
	int iCardNum = mGetCardNum(byCard);
	int iHuaKind = (mGetCardHuaKind(byCard, TRUE) >> 4);
	if (iCardNum > 17 || iHuaKind > 4)
	{
		return false;
	}
	return true;
}

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
int mGetCardBulk(BYTE iCard, bool bExtVal)
{
	if ((iCard == 0x4E) || (iCard == 0x4F))
	{
		return bExtVal ? (iCard - 14) : (iCard - 62); //大小鬼64+14-62=16	只返回大小猫的值
	}

	int iCardNum = mGetCardNum(iCard);
	int iHuaKind = mGetCardHuaKind(iCard, TRUE);

	return ((bExtVal) ? ((iHuaKind >> 4) + (iCardNum * 4)) : (iCardNum));
}

static const BYTE m_CardArray[CARD_MAX_COUNT] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, //方块 A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, //梅花 A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, //红桃 A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, //黑桃 A - K
	0x4E, 0x4F																	  //小鬼，大鬼
};
//构造函数
CGameLogic::CGameLogic(void)
{
	m_bSortCardStyle = 0;			//0 牌按大小排序;1 按牌型排序
	m_bKingCanReplace = false;
	m_bFeedCard = true;
	gameconfig.InitConfig();
	ClearTempAnalyseInfo();
	InitAllData();
}
//析造函数
CGameLogic::~CGameLogic()
{
}
//初始化相关变量
void CGameLogic::InitAllData()
{
	m_bSortCardStyle = 0;			//0 牌按大小排序;1 按牌型排序
	m_bKingCanReplace = false;
	gameconfig.InitConfig();
	ClearTempAnalyseInfo();
	m_bFeedCard = true;
}

//[设置相关]
void CGameLogic::SetGameRule(game_config config)
{
	gameconfig = config;
}
int CGameLogic::GetCardNum(BYTE iCard)
{
	int iCardNum = iCard&UG_VALUE_MASK;
	if (iCardNum == 1)
	{
		iCardNum = 14;
	}
	else if (iCardNum == 2)
	{
		iCardNum = 15;
	}

	return iCardNum;
}
//获取扑克花色
BYTE CGameLogic::GetCardHuaKind(BYTE iCard, BOOL bTrueHua)
{
	int iHuaKind = (iCard&UG_HUA_MASK);
	if (!bTrueHua)
	{
		return iHuaKind = UG_NT_CARD;
	}
	return iHuaKind;
}

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
int CGameLogic::GetCardBulk(BYTE iCard, bool bExtVal)
{
	if ((iCard == 0x4E) || (iCard == 0x4F))
	{
		return bExtVal ? (iCard - 14) : (iCard - 62); //大小鬼64+14-62=16	只返回大小猫的值
	}

	int iCardNum = GetCardNum(iCard);
	int iHuaKind = GetCardHuaKind(iCard, TRUE);

	return ((bExtVal) ? ((iHuaKind >> 4) + (iCardNum * 4)) : (iCardNum));
}
//检测牌值是否合法
bool CGameLogic::IsValidataCard(BYTE byCard)
{
	if ((byCard > 0x4F) || (byCard == 0x00))
	{
		return false;
	}
	int iCardNum = GetCardNum(byCard);
	int iHuaKind = (GetCardHuaKind(byCard, TRUE) >> 4);
	if (iCardNum > 17 || iHuaKind > 4)
	{
		return false;
	}
	return true;
}
//按牌面数字从大到小排列扑克
BOOL CGameLogic::SortCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount, BOOL bSysSort)
{
	BOOL bSorted = TRUE, bTempUp;
	int iTemp, iLast = 0, iStationVol[45];
	memset(iStationVol, 0, sizeof(iStationVol));
	if (iCardCount > 45)
	{
		iCardCount = 45;
	}
	iLast = iCardCount - 1;
	//获取位置数值
	for (int i = 0; i < iCardCount; i++)
	{
		iStationVol[i] = GetCardBulk(iCardList[i], true);
	}
	//排序操作(按从大到小排序)
	do
	{
		bSorted = TRUE;
		for (int i = 0; i < iLast; i++)
		{
			if (iStationVol[i] < iStationVol[i + 1])
			{
				//交换位置				//==冒泡排序
				iTemp = iCardList[i];
				iCardList[i] = iCardList[i + 1];
				iCardList[i + 1] = iTemp;

				iTemp = iStationVol[i];
				iStationVol[i] = iStationVol[i + 1];
				iStationVol[i + 1] = iTemp;

				if (bUp != NULL)
				{
					bTempUp = bUp[i];
					bUp[i] = bUp[i + 1];
					bUp[i + 1] = bTempUp;
				}
				bSorted = FALSE;
			}
		}
		iLast--;
	} while (!bSorted);

	//系统序列不考虑花色牌型问题
	if (bSysSort)
	{
		ReverseCard(iCardList, bUp, iCardCount);
		return TRUE;
	}
	if (GetSortCardStyle() == 1) //按牌型排序
		SortCardByStyle(iCardList, iCardCount);

	return TRUE;
}

BOOL CGameLogic::ReverseCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount)
{
	BYTE iTemp;
	for (int i = 0; i < iCardCount / 2; i++)
	{
		iTemp = iCardList[i];
		iCardList[i] = iCardList[iCardCount - 1 - i];
		iCardList[iCardCount - 1 - i] = iTemp;
	}
	return TRUE;
}
//按牌型排序
BOOL CGameLogic::SortCardByStyle(BYTE iCardList[], BYTE iCardCount)
{
	//如果排序设置是要求按大小排序
	if (m_bSortCardStyle == 0)
	{
		SortCard(iCardList, NULL, iCardCount);
		return TRUE;
	}
	//下面的代码==按牌形排大小
	int iStationVol[45];
	memset(iStationVol, 0, sizeof(iStationVol));
	for (int i = 0; i < iCardCount; i++)
	{
		iStationVol[i] = GetCardBulk(iCardList[i], false);
	}
	int Start = 0;
	int j, step;
	BYTE CardTemp[8];					//用来保存要移位的牌形
	int CardTempVal[8];					//用来保存移位的牌面值
	for (int i = 8; i > 1; i--)				//在数组中找一个连续i张相同的值
	{
		for (j = Start; j < iCardCount; j++)
		{
			CardTemp[0] = iCardList[j];			//保存当前i个数组相同的值
			CardTempVal[0] = iStationVol[j];
			for (step = 1; step < i&&j + step < iCardCount;)			//找一个连续i个值相等的数组(并保存于临时数组中)
			{
				if (iStationVol[j] == iStationVol[j + step])
				{
					CardTemp[step] = iCardList[j + step];			//用来保存牌形
					CardTempVal[step] = iStationVol[j + step];		//面值
					step++;
				}
				else
				{
					break;
				}
			}
			if (step >= i)	//找到一个连续i个相等的数组串起始位置为j,结束位置为j+setp-1
			{			//将从Start开始到j个数组后移setp个
				if (j != Start) //排除开始就是有序
				{
					for (; j >= Start; j--) //从Start张至j张后移动i张
					{
						iCardList[j + i - 1] = iCardList[j - 1];
						iStationVol[j + i - 1] = iStationVol[j - 1];
					}
					for (int k = 0; k < i; k++)
					{
						iCardList[Start + k] = CardTemp[k];	//从Start开始设置成CardSave
						iStationVol[Start + k] = CardTempVal[k];
					}
				}
				Start = Start + i;
			}
			j = j + step - 1;
		}
	}
	return TRUE;
}

//混乱扑克,服务器使用
BYTE CGameLogic::RandCard(BYTE iCard[], int iCardCount)
{
	BYTE iSend = 0;
	BYTE iStation = 0;
	BYTE iCardList[108] = { 0 };
	memcpy(&iCardList, m_CardArray, sizeof(m_CardArray));

	do
	{
		//iStation = rand() % (iCardCount - iSend);
		iStation = GetRandValue(0, iCardCount - iSend - 1);
		iCard[iSend] = iCardList[iStation];
		iSend++;
		iCardList[iStation] = iCardList[iCardCount - iSend];
	} while (iSend<iCardCount);

	return iCardCount;
}
//删除扑克
int CGameLogic::RemoveCard(BYTE iRemoveCard[],   //要删除的牌面
						   int iRemoveCount,		//要删除的牌总数
						   BYTE iCardList[],		//要处理的数组
						   int iCardCount)		//处理数组的上限
{
	if (iRemoveCount == 0) return 0;
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
				iCardList[j] = 0;
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
int CGameLogic::RemoveNummCard(BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 1)
	{
		return 0;
	}
	BYTE byTempCardList[54];
	memset(byTempCardList, 0, sizeof(byTempCardList));
	int iUseCardCount = 0;
	int iRemoveCount = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (iCardList[i] != 0)
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
//比较单张
BOOL CGameLogic::CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard)
{
	//第一个表示桌面上最大牌, 第二个表示要出的牌
	return GetCardBulk(iFirstCard) < GetCardBulk(iNextCard);
}
//几张牌是否是相同数字
BOOL CGameLogic::IsSameNumCard(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	int temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i], false)]++;
	}

	int irank = 0;
	for (int i = 0; i < 18; i++)
	{
		if (temp[i] != 0)
		{
			irank = i;
			break;
		}
	}
	if (m_bKingCanReplace)
	{
		if (irank < 16)//王带其他牌
		{
			return (temp[irank] + temp[16] + temp[17] == iCardCount);
		}
		if (irank < 17)
		{
			return (temp[16] + temp[17] == iCardCount);
		}
	}
	else
	{
		return (temp[irank] == iCardCount);
	}
	return 0;
}

//是否为同花
BOOL CGameLogic::IsSameHuaKind(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount <= 0) return FALSE;

	BYTE iFirstHua = GetCardHuaKind(iCardList[0], TRUE); //取得第一张牌的花色

	for (int i = 1; i < iCardCount; i++) //后面的都和第一张的花色比
	{
		if (GetCardHuaKind(iCardList[i], TRUE) != iFirstHua)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//查找用户手中炸弹数
BYTE CGameLogic::GetBombCount(BYTE iCardList[], int iCardCount, int iNumCount, bool bExtVal)
{
	int iCount = 0;
	int temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}
	for (int i = 0; i < 16; i++)
	{
		if (temp[i] >= iNumCount)
			iCount++;
	}
	return iCount;
}

//获取指定张数牌个数
BYTE CGameLogic::GetCountBySpecifyNumCount(BYTE iCardList[], int iCardCount, int Num)
{
	BYTE temp[18] = { 0 };
	int count = 0;
	for (int i = 0; i < iCardCount; i++)
		temp[GetCardBulk(iCardList[i])]++;

	for (int i = 0; i < 18; i++)
		if (temp[i] == Num)
			count++;

	return count;
}

//获取指定牌个数
BYTE CGameLogic::GetCountBySpecifyCard(BYTE iCardList[], int iCardCount, BYTE bCard)
{
	int count = 0;
	for (int i = 0; i < iCardCount; i++)
		if (iCardList[i] == bCard)
			count++;

	return count;
}
//获取指定牌面值牌的个数
BYTE CGameLogic::GetCountBySpecifyBulk(BYTE byCardList[], int iCardCount, int byCardBulk)
{
	if (byCardBulk >= 18)
	{
		return 0;
	}
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(byCardList[i])]++;
	}
	return temp[byCardBulk];
}
//获取指定牌张数牌大小
BYTE CGameLogic::GetBulkBySpecifyCardCount(BYTE iCardList[], int iCardCount, int iCount)
{
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
		temp[GetCardBulk(iCardList[i])]++;

	for (int i = 17; i > 0; i--)
		if (temp[i] == iCount)
			return i;

	return 0;
}

//是否为变种顺子
BOOL CGameLogic::IsVariationSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	int iValue = iCardCount / iCount;
	if (iCardCount != iCount *iValue)						 //张数不相配
		return FALSE;

	int iFirstMax = 0, iSecondMax = 0, iThirdMax = 0, iMin = 18;//找出第一大,第二大,第三大的牌,和最小牌
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)						//牌多少
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int i = 0; i < 18; i++)
	{
		if (temp[i] != 0 && temp[i] != iCount)	//非找定顺子
			return false;
	}

	for (int i = 0; i < 18; i++)						//最小牌最大可能到A
	{
		if (temp[i] != 0)
			iMin = i;
	}

	for (int i = 17; i > 0; i--)
	{
		if (temp[i] != 0)
		{
			iFirstMax = i;						//可能是2也可以是A
			for (int j = i - 1; j > 0; j--)
			{
				if (temp[j] != 0)//找到第二大的退出循环(无第三大的)//可能是A也可以非A
				{
					iSecondMax = j;
					for (int k = j - 1; j > 0; j--)
					{
						if (temp[k] != 0)//查第第三大的退出循环	//可是存在也可以不存在
						{
							iThirdMax = k;
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}

	if (iFirstMax < 15)	//不存在2的情况,正常情况下
	{
		return (iFirstMax - iMin + 1 == iValue);
	}

	if (iFirstMax == 15)	//存在2,再看是否存在A
	{
		if (iSecondMax == 14)		//存在A
		{
			if (iThirdMax == 0)		//不存在第三大,也只有A2两种牌
				return true;

			return (iThirdMax - iMin + 1 == iValue - 2);		//存在 A2情况包括处理AA2233
		}
		return (iSecondMax - iMin + 1 == iValue - 1);
	}

	return false;
}

//是否為順子
BOOL CGameLogic::IsSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int i = 0; i < 15; i++)
	{
		if (temp[i] != 0 && temp[i] != iCount)	//非指定顺
			return false;
	}

	int len = iCardCount / iCount;

	for (int i = 0; i < 15; i++)
	{
		if (temp[i] != 0)//有值
		{
			for (int j = i; j < i + len; j++)
			{
				if (temp[j] != iCount || j >= 15)
					return false;
			}
			return true;

		}
	}
	return false;
}
//是否有指定的顺子(包含癞子牌的检测)
int CGameLogic::GetSequenceXLen(BYTE iCardList[], int iCardCount, int iCount, int iStartPos)
{
	if (iCount == 0)
	{
		return 0;
	}
	if (iStartPos < 3)
	{
		iStartPos = 3;
	}
	int iReplaceCardCount = 0;
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}
	int iMaxLen = 0;
	do
	{
		int iTempLen = 0;
		int iTempReplaceCount = iReplaceCardCount;
		int i = iStartPos;
		for (; i < 15; i++)
		{
			//某张牌不能大于这个牌的数量
			if (temp[i] > iCount) return 0;
			if (temp[i] < iCount)
			{
				iTempReplaceCount -= (iCount - temp[i]);
			}
			if (iTempReplaceCount < 0)
			{
				if (iTempLen > iMaxLen)
				{
					iMaxLen = iTempLen;
				}
				break;
			}
			iTempLen++;
		}

		if (i == 15)
		{
			if (iTempLen > iMaxLen)
			{
				iMaxLen = iTempLen;
			}
			break;
		}
		else
		{
			iStartPos++;
		}

	} while (iStartPos < 15);

	return iMaxLen;
}

//提取指定牌返回找到牌個數
BYTE  CGameLogic::TackOutBySpecifyCard(BYTE iCardList[], int iCardCount, BYTE bCardBuffer[], int &iResultCardCount, BYTE bCard)
{
	iResultCardCount = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (iCardList[i] == bCard)
		{
			bCardBuffer[iResultCardCount++] = iCardList[i];
		}
	}
	return iResultCardCount;
}


//提取1,2,3 or 4张相同数字的牌
int CGameLogic::TackOutBySepcifyCardNumCount(BYTE iCardList[], int iCardCount,
											 BYTE iDoubleBuffer[], BYTE bCardNum,
											 bool bExtVal)
{
	int iCount = 0, temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int i = 0; i < 18; i++)
	{
		if (temp[i] == bCardNum && bCardNum > 0) //现在要查找的牌型:one?double?three?four_bomb?
		{
			for (int j = 0; j < iCardCount; j++)
			{
				if (i == GetCardBulk(iCardList[j]))
					iDoubleBuffer[iCount++] = iCardList[j];
			}
		}
	}
	return iCount;
}

//提取指定花色牌
int CGameLogic::TackOutByCardKind(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BYTE iCardKind)
{
	int count = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (GetCardHuaKind(iCardList[i]) == iCardKind)
		{
			iDoubleBuffer[count++] = iCardList[i];
		}
	}
	return count;
}

//拆出(将手中牌多的拆成少的)
int CGameLogic::TackOutMuchToFew(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], int &iBufferCardCount, BYTE iCardMuch, BYTE iCardFew)
{
	iBufferCardCount = 0;
	int count = 0;
	BYTE iBuffer[54];
	int iCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, iCardMuch);
	if (iCount <= 0)
		return count;
	for (int i = 0; i < iCount; i += iCardMuch)
	{
		memcpy(&iDoubleBuffer[iBufferCardCount], &iBuffer[i], sizeof(BYTE)*iCardFew);
		iBufferCardCount += iCardFew;
		count++;
	}
	return count;
}

//提取某张指定大小的牌
BOOL CGameLogic::TackOutCardBySpecifyCardNum(BYTE iCardList[], int iCardCount, BYTE iBuffer[], int &iBufferCardCount, BYTE iCard, BOOL bExtVal)
{
	iBufferCardCount = 0;
	BYTE iCardNum = GetCardBulk(iCard); //得到牌面点数
	for (int i = 0; i < iCardCount; i++)
	{
		if (GetCardBulk(iCardList[i]) == iCardNum) //现在要查找的牌点数字
		{
			iBuffer[iBufferCardCount++] = iCardList[i];
		}
	}
	return iBufferCardCount;
}

//查找大于iCard的单牌所在iCardList中的序号
BYTE  CGameLogic::GetSerialByMoreThanSpecifyCard(BYTE iCardList[], int iCardCount,
												 BYTE iCard, BYTE iBaseCardCount,
												 bool bExtValue)
{
	BYTE MaxCard = 0;
	BYTE Serial = 0;
	BYTE MaxCardNum = 255;
	int BaseCardNum = GetCardBulk(iCard);	//当前比较值

	for (BYTE i = 0; i < iCardCount; i += iBaseCardCount)
	{
		int temp = GetCardBulk(iCardList[i]);
		if (temp<MaxCardNum && temp>BaseCardNum)
		{
			MaxCardNum = temp;
			Serial = i; //得到序号
			break;
		}
	}
	return Serial;
}


//查找==iCard的单牌所在iCardList中的序号
int  CGameLogic::GetSerialBySpecifyCard(BYTE iCardList[], int iStart, int iCardCount, BYTE iCard)
{
	for (int i = iStart; i < iCardCount; i++)
	{
		if (iCardList[i] == iCard)
			return i;
	}
	return -1;
}

//变种顺子中最大的
BYTE CGameLogic::GetBulkBySpecifyVariationSequence(BYTE iCardList[], int iCardCount, int iSequence)
{
	int iFirstMax = 0, iSecondMax = 0, iThirdMax = 0;//找出第一大,第二大,第三大的牌,和最小牌
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)						//牌多少
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int i = 17; i > 0; i--)
	{
		if (temp[i] == iSequence)
		{
			iFirstMax = i;						//可能是2也可以是A
			for (int j = i - 1; j > 0; j--)
			{
				if (temp[j] == iSequence)//找到第二大的退出循环(无第三大的)//可能是A也可以非A
				{
					iSecondMax = j;
					for (int k = j - 1; j > 0; j--)
					{
						if (temp[k] == iSequence)//查第第三大的退出循环	//可是存在也可以不存在
						{
							iThirdMax = k;
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}

	if (iFirstMax == 15)	//存在2,再看是否存在A
	{
		if (iSecondMax == 14)		//存在A
		{
			if (iThirdMax == 0)		//不存在第三大,也只有A2两种牌
				return 2;

			return iThirdMax;		//存在 A2情况包括处理AA2233
		}
		return iSecondMax;
	}
	return 0;
}

//获取指定顺子中牌点最小值(iSequence 代表顺子的牌数最多为
BYTE  CGameLogic::GetBulkBySpecifySequence(BYTE iCardList[], int iCardCount, int iSequence)
{
	int temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int k = 0; k < 15; k++)
	{
		if (temp[k] == iSequence)
		{
			return k;
		}
	}
	return 0;
}

//找出一个最小或最大的牌
int  CGameLogic::GetBulkBySepcifyMinOrMax(BYTE iCardList[], int iCardCount, int MinOrMax/*1 or 255*/, bool bExtVal)
{
	int CardNum = GetCardBulk(iCardList[0], false);

	if (MinOrMax == 1) //找最小的
	{
		for (int i = 1; i < iCardCount; i++)
		{
			if (GetCardBulk(iCardList[i], false) < CardNum)
				CardNum = GetCardBulk(iCardList[i], false);
		}
	}
	else if (MinOrMax == 255)
	{
		for (int i = 1; i < iCardCount; i++)
		{
			if (GetCardBulk(iCardList[i], false) > CardNum)
				CardNum = GetCardBulk(iCardList[i], false);
		}
	}

	//返回的是 GetCardBulk() 得到的值
	return CardNum;
}

/////////////////////////////////////////////////////////////////////////
/**
* @info 获取牌型
* @param iCardList[] 牌
* @param iCardCount 牌的数量
* @param bExlVol ?
*/
BYTE CGameLogic::GetCardShape(BYTE iCardList[], int iCardCount)
{
	if (IsOnlyOne(iCardList, iCardCount)) return UG_ONLY_ONE;							//单牌
	if (IsDouble(iCardList, iCardCount)) return UG_DOUBLE;								//对牌
	if (IsThreeX(iCardList, iCardCount, 0)) return UG_THREE;							//三张
	if (IsKingBomb(iCardList, iCardCount)) return UG_KING_BOMB;							//王炸
	if (IsBomb(iCardList, iCardCount)) return UG_BOMB;									//4张以上同点牌，炸弹
	if (IsThreeX(iCardList, iCardCount, 3))	return UG_THREE_DOUBLE;						//三带对
	if (IsThreeX(iCardList, iCardCount, 1)) return UG_THREE_ONE;						//三带一
	if (IsFourX(iCardList, iCardCount, 4)) return UG_FOUR_TWO_DOUBLE;					//四带二对(要求是二对)
	if (IsFourX(iCardList, iCardCount, 2)) return UG_FOUR_TWO;							//四带二(不要求成对)
	if (IsStraight(iCardList, iCardCount)) return UG_STRAIGHT;							//顺子
	if (IsDoubleSequence(iCardList, iCardCount)) return UG_DOUBLE_SEC;					//连对
	if (IsThreeXSequence(iCardList, iCardCount, 0)) return UG_THREE_SEQUENCE;			//连三
	if (IsThreeXSequence(iCardList, iCardCount, 3)) return UG_THREE_DOUBLE_SEC;			//连的三带对
	if (IsThreeXSequence(iCardList, iCardCount, 1)) return UG_THREE_ONE_SEQUENCE;		//连的三带一

	return UG_ERROR_KIND;
}

//对牌
BOOL CGameLogic::IsDouble(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount != 2)
		return FALSE;
	return IsSameNumCard(iCardList, iCardCount, bExtVal);
}

//3 带 0,1or2,or3
BOOL CGameLogic::IsThreeX(BYTE iCardList[], int iCardCount, int iX, bool bExtVal)
{
	if (iCardCount > 5 || iCardCount < 3)
		return FALSE;

	if (GetCountBySpecifyNumCount(iCardList, iCardCount, 3) != 1)//是否存在三张
		return false;
	switch (iX)
	{
		case 0:
			return iCardCount == 3;//IsSameNumCard(iCardList, iCardCount, bExtVal);//不带
		case 1:
			return iCardCount == 4;//带单张
		case 2:
			return iCardCount == 5;//带二张（可以非对子）
		case 3:					//带一对
			return GetCountBySpecifyNumCount(iCardList, iCardCount, 2) == 1;//是否存在对牌
		default:
			break;
	}
	return FALSE;
}

//四带1or2
BOOL CGameLogic::IsFourX(BYTE iCardList[], int iCardCount, int iX)
{
	if (iCardCount > 8 || iCardCount < 4)
		return FALSE;

	if (GetCountBySpecifyNumCount(iCardList, iCardCount, 4) != 1)//是否有四个牌型
		return false;

	switch (iX)
	{
		case 0:
			return iCardCount == 4;//四张
		case 1:
			return iCardCount == 5;//四带1张
		case 2:
			return iCardCount == 6;//四带2(不要求成对)
		case 3:
			return (iCardCount == 6 && 1 == GetCountBySpecifyNumCount(iCardList, iCardCount, 2));//要求成对
		case 4:
			return (iCardCount == 8 && 2 == GetCountBySpecifyNumCount(iCardList, iCardCount, 2));	//四带2对
	}

	return FALSE;
}

//王炸
BOOL CGameLogic::IsKingBomb(BYTE iCardList[], int iCardCount)			//是否为王炸(抓到所的王)
{
	if (iCardCount != KING_COUNT)
		return FALSE;

	for (int i = 0; i < iCardCount; i++)
		if (iCardList[i] != 0x4e && iCardList[i] != 0x4f)
			return FALSE;
	return TRUE;
}

//4+张牌 炸弹
BOOL CGameLogic::IsBomb(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 4)
		return FALSE;

	return IsSameNumCard(iCardList, iCardCount, bExtVal); //是否是相同数字
}

//同花炸弹
BOOL CGameLogic::IsBombSameHua(BYTE iCardList[], int iCardCount)
{
	if (!IsBomb(iCardList, iCardCount)) return false;
	if (!IsSameHuaKind(iCardList, iCardCount)) return false;
	return TRUE;
}

//同花(非同花)
BOOL CGameLogic::IsFlush(BYTE iCardList[], int iCardCount)
{
	return IsSameHuaKind(iCardList, iCardCount);
}

//同花顺 5张同花连续牌
BOOL CGameLogic::IsStraightFlush(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (!IsSameHuaKind(iCardList, iCardCount, bExtVal)) return FALSE; //同花？

	if (!IsStraight(iCardList, iCardCount, bExtVal)) return FALSE; //顺子？
	return TRUE;
}

//是否是变种顺子(A2345)或23456
BOOL CGameLogic::IsVariationStraight(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 5)
		return false;
	return IsVariationSequence(iCardList, iCardCount, 1);
}

//是否是顺子指定张数
BOOL CGameLogic::IsStraight(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 5)
		return false;
	return IsSequence(iCardList, iCardCount, 1);
}

//是否是变种连对AA22或2233等
BOOL CGameLogic::IsVariationDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount % 2 != 0 || iCardCount < 4)
		return false;

	return IsVariationSequence(iCardList, iCardCount, 2);
}

//是否是连对
BOOL CGameLogic::IsDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount % 2 != 0 || iCardCount < 6)
		return false;

	return IsSequence(iCardList, iCardCount, 2);
}

//变种三顺带二顺
BOOL CGameLogic::IsVariationThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 10)		//三顺至少2二顺也至少二
		return false;

	BYTE iBuffer3[54], iBuffer2[54];
	BOOL bValue3 = false, bValue2 = false;	//三顺,二顺是否为顺,
	int TackOutCount3 = 0, TackOutCount2 = 0;

	TackOutCount3 = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer3, 3);//三对
	TackOutCount2 = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer2, 2);//二对

	if (TackOutCount3 <= 0 || TackOutCount2 <= 0 || TackOutCount3 + TackOutCount2 != iCardCount || TackOutCount3 / 3 != TackOutCount2 / 2)
		return FALSE;

	bValue3 = IsVariationSequence(iBuffer3, TackOutCount3, 3);
	bValue2 = (IsVariationSequence(iBuffer2, TackOutCount2, 2) || IsSequence(iBuffer2, TackOutCount2, 2));
	return bValue3&&bValue2;
}

//三顺带二顺
BOOL CGameLogic::IsThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 10)		//三顺至少2二顺也至少二
		return false;

	BYTE iBuffer3[54], iBuffer2[54];
	BOOL bValue3 = false, bValue2 = false;	//三顺,二顺是否为顺,
	int TackOutCount3 = 0, TackOutCount2 = 0;

	TackOutCount3 = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer3, 3);//三对
	TackOutCount2 = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer2, 2);//二对
	if (TackOutCount3 <= 0 || TackOutCount2 <= 0 || TackOutCount3 + TackOutCount2 != iCardCount || TackOutCount3 / 3 != TackOutCount2 / 2)
		return FALSE;
	bValue3 = IsSequence(iBuffer3, TackOutCount3, 3);
	//	TCHAR sz[200];
	//wsprintf(sz,"%d",bValue3);
	//	WriteStr(sz);
	bValue2 = (IsVariationSequence(iBuffer2, TackOutCount2, 2) || IsSequence(iBuffer2, TackOutCount2, 2));
	//	TCHAR sz[200];
	//	wsprintf(sz,"bValue3=%d,bValue2=%d==变种2顺%d,标准二顺%d",bValue3,bValue2,IsVariationSequence(iBuffer2,TackOutCount2,2),IsSequence(iBuffer2,TackOutCount2,2));
	//	WriteStr(sz);
	return bValue3&&bValue2;
}


//变种连三带x
BOOL CGameLogic::IsVariationThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX, bool bExtVal)
{
	if (iCardCount < 6)		//三顺至少2
		return false;

	BYTE iBuffer[54];
	int TackOutCount = 0;
	switch (iSeqX)
	{
		case 0:
			if (iCardCount % 3 != 0)
				return false;
			return IsVariationSequence(iCardList, iCardCount, 3);
			break;
		case 1://带单
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount>0 && TackOutCount / 3 * 4 == iCardCount)
				return IsVariationSequence(iBuffer, TackOutCount, 3);
			break;
		case 2://带二单
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount > 0 && TackOutCount / 3 * 5 == iCardCount)
				return IsVariationSequence(iBuffer, TackOutCount, 3);
		case 3://带一对
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount > 0 && TackOutCount / 3 * 5 == iCardCount
				&&GetCountBySpecifyNumCount(iCardList, iCardCount, 2))
				return IsVariationSequence(iBuffer, TackOutCount, 3);

			break;
	}
	return false;
}

//连的三带 0,1 or 2
BOOL CGameLogic::IsThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX, bool bExtVal)
{
	if (iCardCount < 6)		//三顺至少2
		return false;

	BYTE iBuffer[54];
	int TackOutCount = 0;
	switch (iSeqX)
	{
		case 0:
		{
			if (iCardCount % 3 != 0)
				return false;
			return IsSequence(iCardList, iCardCount, 3);
			break;
		}
		case 1://带单
		{
			int iSeqMaxLen = GetSequenceXLen(iCardList, iCardCount, 3, 3);

			if (iCardCount % 4 != 0)
			{
				return FALSE;
			}
			if (iSeqMaxLen >= (iCardCount / 4))
			{
				return TRUE;
			}
			break;
		}
		case 2://带二单
		{
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount > 0 && TackOutCount / 3 * 5 == iCardCount)
				return IsSequence(iBuffer, TackOutCount, 3);
			break;
		}
		case 3://带对
		{
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount > 0 && TackOutCount / 3 * 5 == iCardCount
				&&GetCountBySpecifyNumCount(iCardList, iCardCount, 2) == TackOutCount / 3)
				return IsSequence(iBuffer, TackOutCount, 3);
			break;
		}
		default:
			break;
	}
	return false;
}

//变种四顺带　
BOOL CGameLogic::IsVariationFourXSequence(BYTE iCardList[], int iCardCount, int iSeqX)
{
	if (iCardCount < 8)		//四顺至少2
		return false;

	BYTE iBuffer[54];
	int TackOutCount = 0;
	switch (iSeqX)
	{
		case 0:
			if (iCardCount % 4 != 0)
				return false;
			return IsVariationSequence(iCardList, iCardCount, 4);
		case 1://带单张
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount>0 && TackOutCount / 4 * 5 == iCardCount)
				return IsVariationSequence(iBuffer, TackOutCount, 4);
			break;
		case 2://带二张(可以非对子）
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount)
				return IsVariationSequence(iBuffer, TackOutCount, 4);
			break;
		case 3://带一对
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount
				&&TackOutCount / 4 == GetBulkBySpecifyCardCount(iCardList, iCardCount, 2))
				return IsVariationSequence(iBuffer, TackOutCount, 4);
			break;
		case 4://(带二对）
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount
				&&TackOutCount / 2 == GetBulkBySpecifyCardCount(iCardList, iCardCount, 2))
				return IsVariationSequence(iBuffer, TackOutCount, 4);
			break;
	}
	return FALSE;
}

//四顺带　
BOOL CGameLogic::IsFourXSequence(BYTE iCardList[], int iCardCount, int iSeqX)
{
	if (iCardCount < 8)		//四顺至少2
		return false;

	BYTE iBuffer[54];
	memset(iBuffer, 0, sizeof(iBuffer));
	int TackOutCount = 0;
	switch (iSeqX)
	{
		case 0:
			if (iCardCount % 4 != 0)
			{
				return false;
			}
			return IsSequence(iCardList, iCardCount, 4);
		case 1://带单张
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 5 == iCardCount)
			{
				return IsSequence(iBuffer, TackOutCount, 4);
			}
			break;
		case 2://带二张(可以非对子）
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount)
			{
				return IsSequence(iBuffer, TackOutCount, 4);
			}
			break;
		case 3://带一对
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount 	&&TackOutCount / 4 == GetBulkBySpecifyCardCount(iCardList, iCardCount, 2))
			{
				return IsSequence(iBuffer, TackOutCount, 4);
			}
			break;
		case 4://(带二对）
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount&&TackOutCount / 2 == GetBulkBySpecifyCardCount(iCardList, iCardCount, 2))
			{
				return IsSequence(iBuffer, TackOutCount, 4);
			}
			break;
	}
	return FALSE;
}

//查找一个比当前大的
BOOL CGameLogic::TackOutCardMoreThanLast(BYTE iHandCard[], int iHandCardCount,
										 BYTE iBaseCard[], int iBaseCardCount,
										 BYTE iResultCard[], int &iResultCardCount,
										 bool bSingleOrder)
{
	BYTE iTempCard[54];
	iResultCardCount = 0;
	int iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型

	switch (iBaseShape)
	{
		case UG_ONLY_ONE: //单张
		{
			//手中只剩炸弹加一手牌，直接跟炸弹
			BYTE t_handcard[20] = { 0 };
			memcpy(t_handcard, iHandCard, iHandCardCount);
			BYTE t_outcard[20] = { 0 };
			int count = CheckOnlyBombAndOneHand(t_handcard, iHandCardCount, t_outcard);
			if (count > 0)
			{
				memcpy(iResultCard, t_outcard, count);
				iResultCardCount = count;
				return TRUE;
			}

			//单牌跟牌--先判断手中的单牌能不能大过
			BYTE tSingleCard[20] = { 0 };
			BYTE tSingleCardCount = 0;
			memcpy(tSingleCard, m_single_info, sizeof(m_single_info));
			tSingleCardCount = m_single_count;

			BYTE tOutCard = 0x00;
			//如果对家只剩一张牌，从最大的单牌开始跟
			if (bSingleOrder)//下家是庄且只剩一张牌
			{
				for (int i = tSingleCardCount - 1; i >= 0; i--)
				{
					if (CompareOnlyOne(iBaseCard[0], tSingleCard[i]))
					{
						tOutCard = tSingleCard[i];
						break;
					}
				}
			}
			else
			{
				for (int i = 0; i < tSingleCardCount; i++)
				{
					if (CompareOnlyOne(iBaseCard[0], tSingleCard[i]) && (GetCardBulk(tSingleCard[i]) < 14))
					{
						tOutCard = tSingleCard[i];
						break;
					}
				}
			}
			if (tOutCard != 0x00)
			{
				iResultCard[0] = tOutCard;
				iResultCardCount = 1;
				if (iHandCardCount == 1) return TRUE;
			}
		}
		case UG_DOUBLE:   //对牌
		case UG_THREE:    //三张
		{
			//手中只剩炸弹加一手牌，直接跟炸弹
			BYTE t_handcard[20] = { 0 };
			memcpy(t_handcard, iHandCard, iHandCardCount);
			BYTE t_outcard[20] = { 0 };
			int count = CheckOnlyBombAndOneHand(t_handcard, iHandCardCount, t_outcard);
			if (count > 0)
			{
				memcpy(iResultCard, t_outcard, count);
				iResultCardCount = count;
				return TRUE;
			}
		}
		case UG_BOMB:	//四张 炸弹
		{
			//查找1,2,3,or4张牌
			BYTE iCount = TackOutBySepcifyCardNumCount(iHandCard, iHandCardCount, iTempCard, iBaseCardCount);

			if (iCount > 0)
			{
				BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, iBaseCard[0], iBaseCardCount, false);
				memcpy(iResultCard, &iTempCard[Step], sizeof(BYTE)*iBaseCardCount);

				if (CompareOnlyOne(iBaseCard[0], iResultCard[0]))
					iResultCardCount = iBaseCardCount;
			}
			break;
		}
		case UG_THREE_ONE: //三带一
		{
			if (TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 1))
				return TRUE;
			break;
		}
		case UG_THREE_TWO: //三带二张
		{
			if (TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 2))
				return TRUE;
			break;
		}
		case UG_THREE_DOUBLE:	//三带一对
		{
			if (TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 3))
				return TRUE;
			break;
		}
		case UG_STRAIGHT: //顺子
		{
			if (TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 1))
			{
				return TRUE; //再找牌点大的顺子
			}
			break;
		}
		case UG_STRAIGHT_FLUSH: //同花顺
		{
			if (TackOutStraightFlush(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
				return TRUE;
			break;
		}
		case UG_DOUBLE_SEC: //连对
		{
			if (TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 2))
			{
				return TRUE; //再找牌点大的顺子
			}
			break;
		}
		case UG_THREE_SEQUENCE: //连三
		{
			if (TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 3))
			{
				return TRUE;
			}
			break;
		}
		case UG_THREE_ONE_SEQUENCE: //三带一的连牌
		{
			if (TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 1))
			{
				return TRUE;
			}
			break;
		}
		case UG_THREE_TWO_SEQUENCE: //三带二的连牌
		{
			if (TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 2))
			{
				return TRUE;
			}
			break;
		}
		case UG_THREE_DOUBLE_SEC://三带对连牌
		{
			if (TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 3))
			{
				return TRUE;
			}
			break;
		}
		case UG_THREE_SEC_DOUBLE_SEC:
		{
			if (TrackOut3Sequence2Sequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
			{
				return TRUE;
			}
			break;
		}
		default:
		{
			iResultCardCount = 0;
			break;
		}
	}
	if (iResultCardCount == 0) //没找到同牌型的大牌,就找大一点的牌型
	{
		switch (iBaseShape)
		{
			case UG_ONLY_ONE: //可以拆对子,拆三条来压单牌或者对子
			case UG_DOUBLE:
			{
				if (TackOutCardByNoSameShape(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount))
					return TRUE;
				break;
			}
			case UG_BOMB:
			{
				//上面没找到相同数量的大炸弹,这里找数量更多的
				if (TackOutBomb(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCardCount + 1))
					return TRUE;
			}
				break;
			default: //如果找不到大的对子单牌就找大的牌型,warning此处不用break;
				break;
		}
	}
	if (iResultCardCount == 0)
	{
		TackOutMoreThanLastShape(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount);
	}
	return TRUE;
}

//直接提取比桌面上大的牌型
BOOL  CGameLogic::TackOutMoreThanLastShape(BYTE iCardList[], int iCardCount,
										   BYTE iResultCard[], int &iResultCardCount,
										   BYTE iBaseCard[], int iBaseCardCount)
{
	iResultCardCount = 0;
	TackOutBomb(iCardList, iCardCount, iResultCard, iResultCardCount); //找炸弹

	return true;
}

//提取单个的三带0, 1 or 2 到底带的是几,由 iBaseCount-3 来决定
BYTE CGameLogic::TackOutThreeX(BYTE iCardList[], int iCardCount,
							   BYTE iBaseCard[], int iBaseCount,
							   BYTE iResultCard[], int &iResultCount, int iValue)
{
	iResultCount = 0;
	if (iCardCount < iBaseCount)
		return FALSE;
	BYTE iTempCard[54];
	int threecard = GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 3);//桌面牌三张的点数
	//3张牌总个数
	BYTE iCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iTempCard, 3);

	if (iCount > 0)//提取大于桌面的三条
	{
		BYTE byCardTemp = 0x00;
		for (int i = 0; i < iBaseCount; ++i)
		{
			if (threecard == GetCardBulk(iBaseCard[i]))
			{
				byCardTemp = iBaseCard[i];
				break;
			}
		}
		if (0x00 == byCardTemp)
		{
			return FALSE;
		}

		BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, byCardTemp, 3, true);//牌面值进去
		//if(Step == 0)
		//	return FALSE;
		memcpy(iResultCard, &iTempCard[Step], sizeof(BYTE) * 3);
		//TCHAR sz[200];
		//wsprintf(sz,"Step=%d,iBaseCount=%d",Step,iBaseCount);
		//WriteStr(sz,7,7);

		//if(CompareOnlyOne(iBaseCard[0], iResultCard[0]))			//由于传过来的step可能为0得进行一次比较处理
		if (threecard >= GetBulkBySpecifyCardCount(iResultCard, 3, 3))
			return FALSE;
		//iResultCount = 3;
		//else
		//	return FALSE;
	}
	else
		return FALSE;
	//将原值移走
	BYTE Tmp[54];
	int iTempCount = iCardCount;
	memcpy(Tmp, iCardList, sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard, 3, Tmp, iTempCount);
	iTempCount -= 3;
	int destCount = iBaseCount - 3;
	//	TCHAR sz[200];
	//	wsprintf(sz,"iValue=%d,destCount=%d",iValue,destCount);
	//	WriteStr(sz,8,8);
	switch (iValue)
	{
		case 1:
		case 2:
		{
			iCount = TackOutBySepcifyCardNumCount(Tmp, iTempCount, iTempCard, 1);
			if (iCount >= destCount)//查找到单牌
			{
				memcpy(&iResultCard[3], iTempCard, sizeof(BYTE)*destCount);
				iResultCount = iBaseCount;
				break;
			}
			//拆对来补单牌
			iCount = TackOutBySepcifyCardNumCount(Tmp, iTempCount, iTempCard, 2);
			if (iCount >= destCount)
			{
				memcpy(&iResultCard[3], iTempCard, sizeof(BYTE)*destCount);
				iResultCount = iBaseCount;
				break;
			}

			//拆三张来补单牌
			iCount = TackOutBySepcifyCardNumCount(Tmp, iTempCount, iTempCard, 3);
			if (iCount < 3)//仅一三张无法拆
				break;
			memcpy(&iResultCard[3], iTempCard, sizeof(BYTE)*destCount);
			iResultCount = iBaseCount;
			break;
		}
		case 3:
		{
			iCount = TackOutBySepcifyCardNumCount(Tmp, iTempCount, iTempCard, 2);
			if (iCount > 0)
			{
				memcpy(&iResultCard[3], iTempCard, sizeof(BYTE)*destCount);
				iResultCount = iBaseCount;
				break;
			}
			//拆三张来补单牌
			iCount = TackOutBySepcifyCardNumCount(Tmp, iTempCount, iTempCard, 3);
			if (iCount < 3)//仅一三张无法拆
				break;
			memcpy(&iResultCard[3], iTempCard, sizeof(BYTE)*destCount);
			iResultCount = iBaseCount;
			break;

		}
		default:
			iResultCount = 0;
			break;
	}
	//		wsprintf(sz,"iResultCount=%d,iBaseCount=%d",iResultCount,iBaseCount);
	//	WriteStr(sz,8,8);
	if (iResultCount == iBaseCount)
		return  TRUE;
	iResultCount = 0;
	return FALSE;
	//return (iResultCount == iBaseCount);
}
//提取蝴蝶
BOOL  CGameLogic::TrackOut3Sequence2Sequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount)
{
	iResultCardCount = 0;
	if (iCardCount < iBaseCount)	//张数不够
		return false;
	BYTE tmpBaseCard[54];//,destCard[54];
	int tmpbaseCardCount = 0, destCardCount = 0;
	//将桌面牌的三条分离出来
	tmpbaseCardCount = TackOutBySepcifyCardNumCount(iBaseCard, iBaseCount, tmpBaseCard, 3);
	if (tmpbaseCardCount < 6)	//至少六张以上
		return FALSE;
	//先提取比桌面大的三顺
	if (!TackOutSequence(iCardList, iCardCount, tmpBaseCard, tmpbaseCardCount, iResultCard, iResultCardCount, 3))
		return FALSE;
	//将手牌复制一份(移除三顺牌)
	BYTE TMP[54];
	int TmpCount = iCardCount;
	memcpy(TMP, iCardList, sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard, iResultCardCount, TMP, TmpCount);
	TmpCount -= iResultCardCount;
	destCardCount = iBaseCount - iResultCardCount;	//补牌数量

	BYTE twoList[54];
	int twoCount;
	//将桌面牌的二顺分离出来
	tmpbaseCardCount = TackOutBySepcifyCardNumCount(iBaseCard, iBaseCount, tmpBaseCard, 2);
	if (!TackOutSequence(TMP, TmpCount, tmpBaseCard, tmpbaseCardCount, twoList, twoCount, 2, true))
		return false;
	//	int TwoSequenceLen = (iBaseCount- tmpbaseCardCount)/2;
	//	tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,3);
	memcpy(&iResultCard[iResultCardCount], twoList, sizeof(BYTE)*twoCount);
	iResultCardCount += twoCount;
	return true;
}
//提取指定三条带顺
BOOL  CGameLogic::TrackOut3XSequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int xValue)
{
	iResultCardCount = 0;
	if (iCardCount < iBaseCount)	//张数不够
		return false;
	BYTE tmpBaseCard[54];//,destCard[54];
	int tmpbaseCardCount = 0, destCardCount = 0;
	//将桌面牌的三条分离出来
	tmpbaseCardCount = TackOutBySepcifyCardNumCount(iBaseCard, iBaseCount, tmpBaseCard, 3);
	if (tmpbaseCardCount < 6)	//至少六张以上
		return FALSE;
	//TCHAR sz[200];
	//wsprintf(sz,"三顺子提取之前%d",iResultCardCount);
	//WriteStr(sz);	
	//先提取比桌面大的三顺
	if (!TackOutSequence(iCardList, iCardCount, tmpBaseCard, tmpbaseCardCount, iResultCard, iResultCardCount, 3))
		return FALSE;
	//TCHAR sz[200];
	//wsprintf(sz,"三顺子提取成功%d",iResultCardCount);
	//WriteStr(sz);
	//将手牌复制一份
	BYTE TMP[54];
	int TmpCount = iCardCount;
	memcpy(TMP, iCardList, sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard, iResultCardCount, TMP, TmpCount);
	TmpCount -= iResultCardCount;
	destCardCount = iBaseCount - iResultCardCount;	//补牌数量

	switch (xValue)
	{
		case 1:
		case 2:
		{
			tmpbaseCardCount = TackOutBySepcifyCardNumCount(TMP, TmpCount, tmpBaseCard, 1);//凑单牌
			if (tmpbaseCardCount >= destCardCount)
			{
				memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*destCardCount);//够单
				iResultCardCount += destCardCount;
			}
			else
			{
				memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*tmpbaseCardCount);
				iResultCardCount += tmpbaseCardCount;
				destCardCount -= tmpbaseCardCount;
				tmpbaseCardCount = TackOutBySepcifyCardNumCount(TMP, TmpCount, tmpBaseCard, 2);//用对牌补
				if (tmpbaseCardCount >= destCardCount)
				{
					memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*destCardCount);
					iResultCardCount += destCardCount;
				}
				else
				{
					memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*tmpbaseCardCount);
					iResultCardCount += tmpbaseCardCount;
					destCardCount -= tmpbaseCardCount;
					tmpbaseCardCount = TackOutBySepcifyCardNumCount(TMP, TmpCount, tmpBaseCard, 3);//用三条补
					//
					if (tmpbaseCardCount >= destCardCount)
					{
						memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*destCardCount);
						iResultCardCount += destCardCount;
					}
				}
			}
			break;
		}
		case 3:
		{
			tmpbaseCardCount = TackOutBySepcifyCardNumCount(TMP, TmpCount, tmpBaseCard, 2);//凑对牌
			if (tmpbaseCardCount >= destCardCount)
			{
				memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*destCardCount);
				iResultCardCount += destCardCount;
			}
			else
			{
				memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*tmpbaseCardCount);
				iResultCardCount += tmpbaseCardCount;
				destCardCount -= tmpbaseCardCount;
				//tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,3);//用三条补对
				TackOutMuchToFew(TMP, TmpCount, tmpBaseCard, tmpbaseCardCount, 3, 2);	//将手中三条拆成对来配
				if (tmpbaseCardCount >= destCardCount)//三条拆对够补
				{
					memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*destCardCount);
					iResultCardCount += destCardCount;
				}
			}
			break;
		}
		default:
			break;
	}
	//wsprintf(sz,"iResultCardCount=%d,iBaseCount=%d",iResultCardCount,iBaseCount);
	//WriteStr(sz);
	if (iResultCardCount == iBaseCount)
		return true;
	iResultCardCount = 0;
	return FALSE;
	return (iResultCardCount == iBaseCount);
	//	return FALSE;
}

//重写提取单张的顺子,连对 or 连三
BOOL CGameLogic::TackOutSequence(BYTE iCardList[], int iCardCount, //手中的牌
								 BYTE iBaseCard[], int iBaseCount,   //桌面上最大的牌, 牌的个数
								 BYTE iResultCard[], int &iResultCount, //找到的牌
								 int xSequence, BOOL bNoComp)							//顺子的个数
{
	iResultCount = 0;
	BYTE iTack[54];
	int iTackCount = iCardCount;
	//复制一份
	memcpy(iTack, iCardList, sizeof(BYTE)*iCardCount);
	BYTE iBuffer[54];
	int iBufferCount = 0;
	int iBaseStart, iDestStart = 0, iDestEnd = 0;
	int iSequenceLen = iBaseCount;
	int temp[18] = { 0 };
	int num = 0;
	//提取所有炸弹(从手中删除所有炸弹)
	TackOutAllBomb(iTack, iTackCount, iBuffer, iBufferCount);
	RemoveCard(iBuffer, iBufferCount, iTack, iTackCount);
	iTackCount -= iBufferCount;
	//进行一次系统序例化处理(按牌形排序，小->大测试
	SortCard(iTack, NULL, iTackCount, TRUE);
	//用缓冲队例保存
	for (int i = 0; i < iTackCount; i++)
	{
		temp[GetCardBulk(iTack[i])]++;
	}

	switch (xSequence)
	{
		//单顺
		case 1:
			iSequenceLen = iBaseCount;
			if (!bNoComp)
				iBaseStart = GetSequenceStartPostion(iBaseCard, iBaseCount, 1);
			else
				iBaseStart = 3;
			for (int i = iBaseStart + 1; i < 15; i++)
			{
				if (temp[i] >= 1)
				{
					if (iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if (iDestEnd == iSequenceLen)
						break;
				}
				else
				{
					iDestStart = 0;
					iDestEnd = 0;
				}
			}
			//		TCHAR sz[200];
			//		wsprintf(sz,"xSequence=%d,iSequenceLen=%d,iDestEnd=%d",xSequence,iSequenceLen,iDestEnd);
			//		WriteStr(sz,9,9);
			if (iDestEnd != iSequenceLen)
				return false;
			//提取队列
			for (int j = 0; j < iTackCount; j++)
			{
				if (GetCardBulk(iTack[j]) == iDestStart)//找到一张牌
				{
					iResultCard[iResultCount++] = iTack[j];
					iDestStart++;
					iDestEnd--;
					//break;
				}
				//已经找全
				if (iDestEnd == 0)
				{
					//					wsprintf(sz,"OK");
					//					WriteStr(sz);
					return true;
				}
			}
			break;
		case 2:
			iSequenceLen = iBaseCount / 2;
			if (!bNoComp)
				iBaseStart = GetSequenceStartPostion(iBaseCard, iBaseCount, 2);
			else
				iBaseStart = 3;
			for (int i = iBaseStart + 1; i < 15; i++)
			{
				if (temp[i] >= 2)
				{
					if (iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if (iDestEnd == iSequenceLen)
						break;
				}
				else
				{
					iDestStart = 0;
					iDestEnd = 0;
				}
			}
			if (iDestEnd != iSequenceLen)
				return false;
			num = 0;
			//提取队列
			for (int j = 0; j < iTackCount; j++)
			{
				if (GetCardBulk(iTack[j]) == iDestStart)
				{
					iResultCard[iResultCount++] = iTack[j];
					num++;
				}

				if (num == 2)//一对已经找到
				{
					num = 0;
					iDestStart++;
					iDestEnd--;
					//已经找全
					if (iDestEnd == 0)
						return true;
					//break;
					//i = 0;
					//continue;
				}
			}
			break;
		case 3:
			iSequenceLen = iBaseCount / 3;
			if (!bNoComp)
				iBaseStart = GetSequenceStartPostion(iBaseCard, iBaseCount, 3);
			else
				iBaseStart = 3;
			for (int i = iBaseStart + 1; i < 15; i++)
			{
				if (temp[i] >= 3)
				{
					if (iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if (iDestEnd == iSequenceLen)
						break;
				}
				else
				{
					iDestStart = 0;
					iDestEnd = 0;
				}
			}
			//TCHAR sz[200];
			///wsprintf(sz,"xSequence=%d,iSequenceLen=%d,iDestEnd=%d",xSequence,iSequenceLen,iDestEnd);
			//WriteStr(sz,9,9);
			if (iDestEnd != iSequenceLen)
				return false;
			num = 0;
			//提取队列
			for (int j = 0; j < iTackCount; j++)
			{
				if (GetCardBulk(iTack[j]) == iDestStart)
				{
					iResultCard[iResultCount++] = iTack[j];
					num++;

					if (num == 3)//找到三张
					{
						num = 0;
						iDestStart++;
						iDestEnd--;
						//已经找全
						if (iDestEnd == 0)
							return true;
						//	wsprintf(sz,"iDestStart OK");
						//	WriteStr(sz);					
						//i=0;
						//continue;
						//break;
					}
				}
			}

			break;
	}
	return FALSE;
}

//提取同花順
BOOL CGameLogic::TackOutStraightFlush(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount)
{
	iResultCardCount = 0;
	if (iCardCount < iBaseCount)
		return false;
	BYTE iBaseMinCard = GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1);//桌面的顺子中最小的牌
	BYTE iTack[54];
	int iTackCount = iCardCount;
	//复制一份
	memcpy(iTack, iCardList, sizeof(BYTE)*iCardCount);
	BYTE iBuffer[54];
	int iBufferCount = 0;
	int iDestStart = 0, iDestEnd = 0;
	int iSequenceLen = iBaseCount;
	int temp[18] = { 0 };
	int num = 0;
	//提取所有炸弹(从手中删除所有炸弹)
	TackOutAllBomb(iTack, iTackCount, iBuffer, iBufferCount);
	RemoveCard(iBuffer, iBufferCount, iTack, iTackCount);
	iTackCount -= iBufferCount;

	SortCard(iTack, NULL, iTackCount, TRUE);

	BYTE iTempKind[54];
	int iTempKindCount = 0;
	//TCHAR sz[200];
	//wsprintf(sz,"iTackCount=%d,iBaseCount=%d",iTackCount,iBaseCount);
	//WriteStr(sz);
	//用缓冲队例保存
	for (int kind = 0; kind <= 48; kind += 16)
	{	//提取方块
		iResultCardCount = 0;
		iTempKindCount = TackOutByCardKind(iTack, iTackCount, iTempKind, kind);
		//wsprintf(sz,"kind=%d,iTempKindCount=%d",kind,iTempKindCount);
		//WriteStr(sz);
		if (iTempKindCount >= iBaseCount)					//大于桌面
		{
			for (int i = 0; i < iTempKindCount; i++)
			{
				temp[GetCardBulk(iTempKind[i])]++;
			}

			for (int i = iBaseMinCard + 1; i < 15; i++)//对队例进行遍历
			{
				if (temp[i] >= 1)		//某花色有牌
				{
					if (iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if (iDestEnd == iSequenceLen)
						break;
				}
				else
				{
					iDestStart = 0;
					iDestEnd = 0;
				}
			}
			//wsprintf(sz,"iDestEnd=%d,iCardCount=%d",iDestEnd,iCardCount);
			//WriteStr(sz);

			if (iDestEnd != iBaseCount)	//某种花色不符合,换另外一种花色
				continue;
			//提取队列
			for (int j = 0; j < iTempKindCount; j++)
			{
				if (GetCardBulk(iTempKind[j]) == iDestStart)
				{
					iResultCard[iResultCardCount++] = iTempKind[j];
					iDestStart++;
					iDestEnd--;
				}
				//已经找全
				if (iDestEnd == 0)
					return true;
			}
		}

	}

	return FALSE;
}

//得到顺子的起始位置
int CGameLogic::GetSequenceStartPostion(BYTE iCardList[], int iCardCount, int xSequence)
{
	BYTE temp[18] = { 0 };
	int Postion = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int i = 0; i < 18; i++)
	{
		if (temp[i] == xSequence)
			return i;
	}
	return Postion;
}

//拆大
BOOL CGameLogic::TackOutCardByNoSameShape(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, BYTE iBaseCard[], int iBaseCardCount)
{
	iResultCardCount = 0;
	BYTE temp[18] = { 0 };
	int t = GetCardBulk(iBaseCard[0], false); //得到桌面上那个牌的值
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i], false)]++;
	}

	//拆(炸牌不拆)
	for (int i = 0; i < 18; i++)
	{
		if (temp[i] < 4 &&               //非炸弹牌
			temp[i] > iBaseCardCount &&  //张数比桌面牌多
			i > t)                      //且数字大
		{
			for (int j = 0; j < iCardCount; j++)
			{
				if (GetCardBulk(iCardList[j], false) == i)
				{
					iResultCard[iResultCardCount++] = iCardList[j];
					if (iResultCardCount == iBaseCardCount)
						return true;
				}
			}
		}
	}
	return false;
}
//是否可以出牌
BOOL CGameLogic::CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, bool bFirstOut)
{
	BYTE iOutCardShape = GetCardShape(iOutCard, iOutCount);

	if (iOutCardShape == UG_ERROR_KIND) //牌型不对
		return FALSE;

	if (bFirstOut)
		return TRUE;
	BYTE iBaseCardShape = GetCardShape(iBaseCard, iBaseCount); //桌面上的牌型

	if (iBaseCardShape > iOutCardShape)						//牌形<
	{
		return FALSE;
	}

	if (iBaseCardShape < iOutCardShape)						//牌形>
	{
		if (UG_BOMB <= iOutCardShape)					//炸弹
		{
			return TRUE;
		}
		//处理不一样的牌形也可以大变种顺子和顺子大小比较
		if (iBaseCount != iOutCount) //张数限制
			return FALSE;

		switch (iBaseCardShape)
		{
			case UG_STRAIGHT:									//同花順大于順子
			{
				if (iOutCardShape == UG_STRAIGHT_FLUSH)
					return true;
			}
			case UG_VAR_STRAIGHT:							//最小变种顺子
			{
				if (iOutCardShape == UG_STRAIGHT)			//变咱顺子中有效最大值小于正常顺子中最大牌
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 1) < GetBulkBySpecifySequence(iOutCard, iOutCount, 1);
				return false;
			}
			case UG_VAR_DOUBLE_SEC://最小变种顺子
			{
				if (iOutCardShape == UG_DOUBLE_SEC)	//变咱顺子中有效最大值小于正常顺子中最大牌
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 2) < GetBulkBySpecifySequence(iOutCard, iOutCount, 2);
				return false;
			}
			case UG_VAR_THREE_SEC:		//变种三顺
			case UG_VAR_THREE_ONE_SEC://变种三顺
			case UG_VAR_THREE_TWO_SEC://变种三带二顺
			case UG_VAR_THR_DOUBLE_SEC_1://变种三带二顺
			case UG_VAR_THR_DOUBLE_SEC_2://变种三顺带二顺
			{
				if (iOutCardShape == iBaseCardShape + 1)
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 3) < GetBulkBySpecifySequence(iOutCard, iOutCount, 3);
				return false;
			}
			case UG_VAR_FOUR_SEC:		//变种四顺
			case UG_VAR_FOUR_ONE_SEC:	//变种四带一顺
			case UG_VAR_FOUR_TWO_SEC:	//变种四带二顺
			case UG_VAR_FOUR_ONE_DOUBLE_SEC://变种四带一对顺
			case UG_VAR_FOUR_TWO_DOUBLE_SEC://变种四带二对顺
			{
				if (iOutCardShape == iBaseCardShape + 1)
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 4) < GetBulkBySpecifySequence(iOutCard, iOutCount, 4);
				return false;
			}
			case UG_THREE_TWO://三帶一對＞三帶二單
			{
				if (iOutCardShape == UG_THREE_DOUBLE)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 3);
				return false;
			}
			case UG_THREE_TWO_SEQUENCE://三帶一對順(或蝴蝶)>三帶二單順
			{
				if (iOutCardShape == UG_THREE_DOUBLE_SEC || iOutCardShape == UG_THREE_SEC_DOUBLE_SEC)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 3);
				return false;
			}
			case UG_FOUR_TWO://四帶一對＞四帶二單
			{
				if (iOutCardShape == UG_FOUR_ONE_DOUBLE)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4);
				return false;
			}
			case UG_FOUR_TWO_SEC://四帶一對順＞四帶二單順
			{
				if (iOutCardShape == UG_FOUR_ONE_DOUBLE_SEC)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4);
				return false;
			}
			case UG_THREE_DOUBLE_SEC:	//蝴蝶大于三順帶對
			{
				if (iOutCardShape == UG_THREE_SEC_DOUBLE_SEC)
					return GetBulkBySpecifySequence(iBaseCard, iBaseCount, 3) < GetBulkBySpecifySequence(iOutCard, iOutCount, 3);
				return false;
			}
		}
		return false;
	}

	switch (iBaseCardShape)			//处理牌形一致
	{
		case UG_ONLY_ONE:  //单张
		case UG_DOUBLE:    //对牌
		case UG_THREE:     //三张
		{
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
		}
		case UG_BOMB: //4+张 炸弹
		{
			if (iBaseCount > iOutCount) //张数大的炸弹大
				return FALSE;

			if (iBaseCount == iOutCount) //张数相同,比点数
				return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
			return TRUE;
		}

		case UG_FLUSH:			//同花(非顺子）比较同花中最大的牌
		{
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 255) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 255);
		}
		case UG_STRAIGHT_FLUSH: //同花顺
		case UG_STRAIGHT:		//顺子
		case UG_DOUBLE_SEC: //连对
		case UG_THREE_SEQUENCE:  //连三  
		case UG_FOUR_SEQUENCE:	//四顺
			if (iOutCount != iBaseCount)
				return FALSE;
			{
				return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
			}

		case UG_THREE_ONE:		//三带一
		case UG_THREE_TWO:		//三带二
		case UG_THREE_DOUBLE:	//三带对
		{
			return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 3);
		}
		case UG_FOUR_ONE:						//四带一
		case UG_FOUR_TWO:						//四带二
		case UG_FOUR_ONE_DOUBLE:				//四带一对
		case UG_FOUR_TWO_DOUBLE:				//四带二对
		{
			return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4);
		}

		case UG_THREE_ONE_SEQUENCE: //2+个三带一
		case UG_THREE_TWO_SEQUENCE: //2+个三带二
		case UG_THREE_DOUBLE_SEC://三带对顺
		case UG_THREE_SEC_DOUBLE_SEC:		//三顺带二顺(蝴蝶)
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 3));
		}
		case UG_FOUR_ONE_SEC:					//四顺
		case UG_FOUR_TWO_SEC:
		case UG_FOUR_ONE_DOUBLE_SEC:
		case UG_FOUR_TWO_DOUBLE_SEC:
		{
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4));
		}
			//变种牌形处理
		case UG_VAR_STRAIGHT://单顺
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 1) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount, 1));

			break;
		}
		case UG_VAR_DOUBLE_SEC://对顺
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 2) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount, 2));
			break;
		}
		case UG_VAR_THREE_SEC://三顺
		case UG_VAR_THREE_ONE_SEC://三带一顺
		case UG_VAR_THREE_TWO_SEC://三带二顺
		case UG_VAR_THR_DOUBLE_SEC_1://三带对顺
		case UG_VAR_THR_DOUBLE_SEC_2://三顺带二顺
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount, 3));
			break;
		}
		case UG_VAR_FOUR_SEC:		//变种四顺
		case UG_VAR_FOUR_ONE_SEC:	//变种四带一顺
		case UG_VAR_FOUR_TWO_SEC:	//变种四带二顺
		case UG_VAR_FOUR_ONE_DOUBLE_SEC://变种四带一对顺
		case UG_VAR_FOUR_TWO_DOUBLE_SEC://变种四带二对顺
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4));

			return false;
		}
	}

	return FALSE;
}

//提取所有炸弹为提反单顺,双顺,三顺做准备
BOOL CGameLogic::TackOutAllBomb(BYTE iCardList[], int iCardCount,
								BYTE iResultCard[], int &iResultCardCount, int iNumCount)
{
	iResultCardCount = 0;
	BYTE bCardBuffer[54];
	BYTE bombcount = GetBombCount(iCardList, iCardCount, iNumCount);
	for (int i = iNumCount; i < 9; i++)
	{
		int count = TackOutBySepcifyCardNumCount(iCardList, iCardCount, bCardBuffer, i);
		if (count > 0)
		{
			memcpy(&iResultCard[iResultCardCount], bCardBuffer, sizeof(BYTE)*count);
			iResultCardCount += count;
			break;
		}
	}
	return true;
}

//提取炸弹
BOOL CGameLogic::TackOutBomb(BYTE iCardList[], int iCardCount,
							 BYTE iResultCard[], int &iResultCardCount, int iNumCount)
{
	iResultCardCount = 0;
	BYTE bCardBuffer[54];
	BYTE bombcount = GetBombCount(iCardList, iCardCount, iNumCount);
	for (int i = iNumCount; i < 9; i++)
	{
		int count = TackOutBySepcifyCardNumCount(iCardList, iCardCount, bCardBuffer, i);
		if (count > 0)
		{
			memcpy(iResultCard, bCardBuffer, sizeof(BYTE)*i);
			iResultCardCount = i;
			break;
		}
	}
	if (iResultCardCount == 0)
		TackOutKingBomb(iCardList, iCardCount, iResultCard, iResultCardCount);
	return true;
}

//提取王炸
BOOL CGameLogic::TackOutKingBomb(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount)
{
	iResultCardCount = 0;

	BYTE bCardBuf[8];
	int kingcount = 0;
	int SingKing = KING_COUNT / 2;
	int count = TackOutBySpecifyCard(iCardList, iCardCount, bCardBuf, kingcount, 0x4e);
	if (count != SingKing)
		return false;

	memcpy(iResultCard, bCardBuf, sizeof(BYTE)*count);

	count = TackOutBySpecifyCard(iCardList, iCardCount, bCardBuf, kingcount, 0x4f);
	if (count != SingKing)
	{
		return false;
	}
	memcpy(&(iResultCard[SingKing]), bCardBuf, sizeof(BYTE)*count);
	return iResultCardCount = KING_COUNT;
}

//检测数组的子集关系
bool CGameLogic::CheckArraySubSet(BYTE cardlist[], BYTE count, BYTE checklist[], BYTE checkcount)
{
	if (count < checkcount)
	{
		return false;
	}

	bool bUseIndex[CARD_MAX_COUNT];
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
//是否是比较牌型
bool CGameLogic::IsMustCallShape(BYTE cardlist[], BYTE count)
{
	if (count < 4)
	{
		return false;
	}

	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(cardlist, count, byReslult, iReslultcount))
	{
		return true;
	}
	if (GetCountBySpecifyBulk(cardlist, count, GetCardBulk(0x02)) == 4)
	{
		return true;
	}
	return false;
}
//获取选择牌型
void  CGameLogic::GetchangeCardList(BYTE byCardList[], BYTE byCardCount, BYTE byReslut[])
{
	if (byCardCount < 3)
	{
		return;
	}

	int iCount = 0;

	BYTE byTempCardList[20] = { 0 };
	int iTempCount = TackOutBySepcifyCardNumCount(byCardList, byCardCount, byTempCardList, 1);
	for (int i = 0; i < iTempCount; i++)
	{
		if (GetCardBulk(byTempCardList[i]) > 13) continue;
		byReslut[iCount++] = byTempCardList[i];
		if (iCount >= 3)
		{
			break;
		}
	}
	if (iCount < 3)
	{
		memset(byTempCardList, 0, sizeof(byTempCardList));
		iTempCount = TackOutBySepcifyCardNumCount(byCardList, byCardCount, byTempCardList, 2);
		for (int i = 0; i < iTempCount; i += 2)
		{
			if (GetCardBulk(byTempCardList[i]) > 14) continue;
			byReslut[iCount++] = byTempCardList[i];
			if (iCount >= 3)
			{
				break;
			}
		}
	}

	if (iCount < 3)
	{
		memset(byTempCardList, 0, sizeof(byTempCardList));
		iTempCount = TackOutBySepcifyCardNumCount(byCardList, byCardCount, byTempCardList, 3);
		for (int i = 0; i < iTempCount; i += 3)
		{
			if (GetCardBulk(byTempCardList[i]) > 14) continue;
			byReslut[iCount++] = byTempCardList[i];
			if (iCount >= 3)
			{
				break;
			}
		}
	}

	if (iCount < 3)
	{
		memset(byTempCardList, 0, sizeof(byTempCardList));
		iTempCount = TackOutBySepcifyCardNumCount(byCardList, byCardCount, byTempCardList, 4);
		for (int i = 0; i < iTempCount; i++)
		{
			byReslut[iCount++] = byTempCardList[i];
			if (iCount >= 3)
			{
				break;
			}
		}
	}

}
///是否是一副牌
bool CGameLogic::IsASetCardlist(BYTE byCardList[], BYTE byCardCount)
{
	if (byCardCount != CARD_MAX_COUNT)
	{
		return false;
	}

	BYTE byallcardList[CARD_MAX_COUNT] = { 0 };
	memcpy(byallcardList, m_CardArray, sizeof(byallcardList));
	if (CheckArraySubSet(byallcardList, CARD_MAX_COUNT, byCardList, byCardCount))
	{
		return true;
	}

	return false;
}

//牌型叫分参考信息
BYTE CGameLogic::InfoForCallScore(BYTE byCardList[], BYTE byCardCount)
{
	BYTE cardfacepoint = 0;
	if (byCardCount < 4)
	{
		return cardfacepoint;
	}

	
	BYTE temp[18] = { 0 };
	for (int i = 0; i < byCardCount; i++)
	{
		temp[GetCardBulk(byCardList[i], false)]++;
	}

	//火箭检测
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(byCardList, byCardCount, byReslult, iReslultcount))
	{
		cardfacepoint += 8;
	}
	else//单大小王检测
	{
		if (temp[16]) cardfacepoint += 3;
		if (temp[17]) cardfacepoint += 4;
	}

	//炸弹检测
	for (int j = 0; j < 15; j++)
	{
		if (temp[j] == 4)
		{
			cardfacepoint += 6;
		}
	}

	//飞机检测
	for (int j = 0; j < 13; j++)
	{
		if (temp[j] == 3 && temp[j+1] == 3)
		{
			temp[j+1] = 0;//清空一个三张，避免重复检测
			cardfacepoint += 2;
		}
	}

	//2的个数
	cardfacepoint += (2 * GetCountBySpecifyBulk(byCardList, byCardCount, 15));

	return cardfacepoint;
}

//分析手牌 NTpos庄家位置 0：本家  1：上家  2：下家
Analysis_Result CGameLogic::AnalyzeHandCard(BYTE byCardList[], BYTE byCardCount, bool bSplitBomb, bool bSaveResult)
{
	ClearTempAnalyseInfo();
	//拷贝数组，备份使用
	BYTE backup_cardlist[20] = { 0 };
	BYTE backup_CardCount = byCardCount;
	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
	
	int total_outhand_count = 0;

	bool have_little_king = false;
	bool have_large_king = false;

	int boom_count = 0;
	int single_count = 0;
	int double_count = 0;
	int triple_count = 0;
	BYTE boom_info[18] = { 0 };
	BYTE single_info[18] = { 0 };
	BYTE double_info[18] = { 0 };
	BYTE triple_info[18] = { 0 };

	BYTE SequenceListStart[4] = { 0 };
	BYTE SequenceCount[4] = { 0 };

	//转换牌值
	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	//去除大小王
	if (temp[16] > 0)
	{
		temp[16] = 0;
		have_little_king = true;
	}
	if (temp[17] > 0)
	{
		temp[17] = 0;
		have_large_king = true;
	}
	
	//去除顺子(包括单顺，双顺，飞机（三顺）),顺子有可能拆炸弹，bSplitBomb参数控制是否拆
	FindSequence(temp, SequenceListStart, SequenceCount, bSplitBomb);

	//去除炸弹
	for (int i = 1; i < 18; i++)
	{
		if (temp[i] == 4)
		{
			boom_info[boom_count++] = i;
			temp[i] = 0;
		}
	}

	//去除三张
	for (int i = 1; i < 18; i++)
	{
		if (temp[i] == 3)
		{
			triple_info[triple_count++] = i;
			temp[i] = 0;
		}
	}

	//去除对子
	for (int i = 1; i < 18; i++)
	{
		if (temp[i] == 2)
		{
			double_info[double_count++] = i;
			temp[i] = 0;
		}
	}

	//获取单牌
	int single = 0;
	for (int i = 1; i < 18; i++)
	{
		if (temp[i] != 0)
		{
			if (triple_count > single_count)
			{
				single_info[single_count++] = i;
				temp[i] = 0;
			}
			else
			{
				single_info[single_count++] = i;
				if (single == 0) single = i;
			}
		}
	}

	//统计手数--------------------------------------------------------------------------------------------------------------------
	if (have_little_king != have_large_king)//单大王，单小王算一手牌，双王最大不计算
	{
		total_outhand_count++;
	}
	//炸弹，三张带牌逻辑
	int temp_1 = boom_count;
	int temp_2 = triple_count;
	int temp_3 = double_count;
	int temp_4 = single_count;

	for (int i = 0; i < 4; i++)
	{
		BYTE type = SequenceCount[i] & 0xF0;
		BYTE count = SequenceCount[i] & 0x0F;

		if (count > 0x00)
		{
			total_outhand_count++;
			if (type == 0x30)
			{//飞机可以带牌
				if (temp_4 >= count)temp_4 -= count;
				else if (temp_3 >= count)temp_3 -= count;
				else if (((temp_4 + temp_3) >= count) && (count == 2))
				{//可以拆对子补翅膀
					temp_3 = temp_3 - (count - temp_4);
					temp_4 = count - temp_4;
				}
			}
		}
	}
	
	while ((temp_1 + temp_2) > 0)
	{
		//炸弹带牌，优先带单张
		if (temp_1 > 0)
		{
			total_outhand_count++;
			temp_1--;
			if (temp_4 >= 2)
			{
				temp_4 -= 2;
				continue;
			}
			else if (temp_3 >= 2)
			{
				temp_3 -= 2;
				continue;
			}
		}
		//三带对子（参数控制），三带一
		if (temp_2 > 0)
		{
			total_outhand_count++;
			temp_2--;
			if (temp_4 >= 1)
			{
				temp_4--;
				continue;
			}
			else if (temp_3 >= 1)
			{
				temp_3--;
				continue;
			}
		}
	}
	total_outhand_count += temp_3;
	total_outhand_count += temp_4;

	if (bSaveResult)
	{
		//赋值
		m_have_little_king = have_little_king;
		m_have_large_king = have_large_king;
		memcpy(m_boom_info, boom_info, sizeof(boom_info));
		m_boom_count = boom_count;
		memcpy(m_triple_info, triple_info, sizeof(triple_info));
		m_triple_count = triple_count;
		memcpy(m_SequenceListStart, SequenceListStart, sizeof(SequenceListStart));
		memcpy(m_SequenceCount, SequenceCount, sizeof(SequenceCount));
		memcpy(m_double_info, double_info, sizeof(double_info));
		m_double_count = double_count;
		memcpy(m_single_info, single_info, sizeof(single_info));
		m_single_count = single_count;//所有单牌数
		m_real_single_count = temp_4;//真正的单牌数
		m_total_outhand_count = total_outhand_count;
	}
	
	Analysis_Result result;

	//赋值
	result.have_little_king = have_little_king;
	result.have_large_king = have_large_king;
	memcpy(result.boom_info, boom_info, sizeof(boom_info));
	result.boom_count = boom_count;
	memcpy(result.triple_info, triple_info, sizeof(triple_info));
	result.triple_count = triple_count;
	memcpy(result.SequenceListStart, SequenceListStart, sizeof(SequenceListStart));
	memcpy(result.SequenceCount, SequenceCount, sizeof(SequenceCount));
	memcpy(result.double_info, double_info, sizeof(double_info));
	result.double_count = double_count;
	memcpy(result.single_info, single_info, sizeof(single_info));
	result.single_count = single_count;//所有单牌数
	result.real_single_count = temp_4;//真正的单牌数
	result.total_outhand_count = total_outhand_count;

	return result;
}

//查找顺子
void CGameLogic::FindSequence(BYTE byTempCardList[], BYTE bySequenceList[], BYTE bySequenceCount[], bool bSplitBomb)
{
	BYTE Sequence[4][20] = { 0x00 };
	BYTE SequenceCount[4] = { 0x00 };

	BYTE temp[18] = { 0 };
	memcpy(temp, byTempCardList, sizeof(temp));

	BYTE boom_info[18] = { 0 };
	BYTE triple_info[18] = { 0 };
	int boom_count = 0;
	int triple_count = 0;

	//去除炸弹
	for (int i = 0; i < 18; i++)
	{
		if (temp[i] == 4 && !bSplitBomb)
		{
			boom_info[boom_count++] = i;
			temp[i] = 0;
		}
	}

	//去除三张
	for (int i = 0; i < 18; i++)
	{
		if (temp[i] == 3)
		{
			triple_info[triple_count++] = i;
			temp[i] = 0;
		}
	}

	//先获取五连单顺
	int num = 0;
	do
	{
		BYTE start_card = 0x00;
		BYTE count = 1;
		for (int i = 1; i < 15; i++)
		{
			if (temp[i] != 0 && start_card == 0x00)
			{
				start_card = i;
				count = 1;
			}
			else if (temp[i] != 0 && temp[i - 1] != 0)
			{
				count++;
			}
			else if (temp[i] == 0 && temp[i - 1] != 0)
			{
				count = 1;
				start_card = 0x00;
			}

			if (count == 5) break;
		}

		if (count == 5)
		{
			SequenceCount[num] = 0x10;//单顺标示
			for (int j = 0; j < 5; j++)
			{
				temp[j + start_card]--;
				Sequence[num][j] = j + start_card;
				SequenceCount[num]++;
			}
		}
		num++;
	} while (num < 4);//最多四个顺子

	//获取剩余牌中是否还有添加到单顺当中的牌
	int seq_count = 0;//顺子个数
	for (int i = 0; i < 4; i++)
	{
		if (Sequence[i][0] == 0x00)
		{
			seq_count = i;
			break;
		}
		int pos = 1;
		BYTE j = (SequenceCount[i] & 0x0F) - 0x01;
		while ((temp[Sequence[i][j] + pos] != 0) && ((Sequence[i][j] + pos) < 15))
		{
			if ((temp[Sequence[i][j] + pos] == 1)//有紧接着的一张单牌直接添加
				|| (temp[Sequence[i][j] + pos] == 2 && temp[Sequence[i][j] + pos + 1] == 1)//紧跟一个对子，并且对子面还有一个单张，此时拆对子连顺子
				|| (temp[Sequence[i][j] + pos] == 4 && temp[Sequence[i][j] + pos + 1] == 1 && temp[Sequence[i][j] + pos + 2] == 1 && temp[Sequence[i][j] + pos + 3] == 1))
			{
				temp[Sequence[i][j] + pos]--;
				Sequence[i][j + pos] = Sequence[i][j] + pos;
				SequenceCount[i]++;
				j = (SequenceCount[i] & 0x0F) - 0x01;
			}
			else break;
		}
	}

	//合并顺子，如果有头尾相连，直接合并
	for (int i = 0; i < 4; i++)
	{
		if (Sequence[i][0] == 0x00)
		{
			seq_count = i;
			break;
		}
		BYTE num = (SequenceCount[i] & 0x0F) - 0x01;
		for (int j = i + 1; j < seq_count; j++)
		{
			if (Sequence[i][num] + 1 == Sequence[j][0])
			{
				memcpy(&Sequence[i][num + 1], &Sequence[j][0], 20);
				SequenceCount[i] = SequenceCount[i] + (SequenceCount[j] & 0x0F);
				for (; (j + 1) < 4; j++)
				{
					memcpy(&Sequence[j][0], &Sequence[j + 1][0], 20);
					SequenceCount[j] = SequenceCount[j + 1];
				}
			}
		}
		//如果起始位置还有余牌，还原第一张为对子
		BYTE j = (SequenceCount[i] & 0x0F);
		while ((temp[Sequence[i][0]] == 1) && (j > 5))
		{
			BYTE decrease_Sequence[20] = { 0x00 };
			temp[Sequence[i][0]]++;
			memcpy(decrease_Sequence, &Sequence[i][1], (sizeof(decrease_Sequence) - sizeof(BYTE)));
			memcpy(&Sequence[i][0], decrease_Sequence, sizeof(decrease_Sequence));
			SequenceCount[i]--;

			j = (SequenceCount[i] & 0x0F);
		}
	}

	//提取三顺
	for (int i = 0; i < (triple_count - 1); i++)
	{
		if (SequenceCount[seq_count] == 0x00)
		{
			//Q以上的都不用做三顺
			if (triple_info[i] + 1 == triple_info[i + 1] && triple_info[i] < 12)
			{
				SequenceCount[seq_count] = 0x31;
				Sequence[seq_count][0] = triple_info[i];//记录三顺
				//清除三条记录
				int iDelStartPos = i;
				while (iDelStartPos < 17)
				{
					triple_info[iDelStartPos] = triple_info[iDelStartPos + 1];
					triple_info[iDelStartPos + 1] = 0;
					iDelStartPos++;
				}
				triple_count--;

				int iPos = 0x0F & SequenceCount[seq_count];
				while ((triple_info[i] == (Sequence[seq_count][iPos - 1] + 1)) && triple_info[i] <= 14)
				{
					Sequence[seq_count][iPos] = triple_info[i];
					SequenceCount[seq_count]++;
					//清除三条记录
					int iiDelStartPos = i;
					while (iiDelStartPos < 17)
					{
						triple_info[iiDelStartPos] = triple_info[iiDelStartPos + 1];
						triple_info[iiDelStartPos + 1] = 0;
						iiDelStartPos++;
					}
					triple_count--;
					iPos = 0x0F & SequenceCount[seq_count];
				}
				i--;
				seq_count++;
			}
		}
		else
		{
			seq_count++;
		}
	}

	//提取双顺,J以上的双顺不用做双顺
	for (int i = 3; i < 11; i++)
	{
		if (SequenceCount[seq_count] == 0x00)
		{
			if (temp[i] == 2 && temp[i + 1] == 2 && temp[i + 2] == 2)
			{
				SequenceCount[seq_count] = 0x21;
				Sequence[seq_count][0] = i;//记录双顺
				temp[i] = 0;
				int j = i + 1;
				int iPos = 0x0F & SequenceCount[seq_count];
				while (temp[j] == 2 && j <= 14)
				{
					Sequence[seq_count][iPos] = j;
					SequenceCount[seq_count]++;
					temp[j] = 0;
					j++;
					iPos = 0x0F & SequenceCount[seq_count];
				}
			}
		}
		else
		{
			seq_count++;
		}
	}

	//剩余的三条添加进来
	for (int i = 0; i < triple_count; i++)
	{
		if (triple_info[i] != 0)
		{
			temp[triple_info[i]] += 3;//添加三条到牌组中
		}
	}

	//将剩余牌与三条（不包含三顺）进行试组合,再次提取顺子
	int time = 0;
	do
	{
		BYTE start_card = 0x00;
		BYTE count = 1;
		for (int i = 1; i < 15; i++)
		{
			if (temp[i] != 0 && start_card == 0x00)
			{
				start_card = i;
				count = 1;
			}
			else if (temp[i] != 0 && temp[i - 1] != 0)
			{
				count++;
			}
			else if (temp[i] == 0 && temp[i - 1] != 0)
			{
				count = 1;
				start_card = 0x00;
			}

			if (count == 5) break;
		}

		if (count == 5)
		{
			SequenceCount[seq_count] = 0x10;//单顺标示
			for (int j = 0; j < 5; j++)
			{
				temp[j + start_card]--;
				Sequence[seq_count][j] = j + start_card;
				SequenceCount[seq_count]++;
			}
			seq_count++;
		}
		time++;
	} while (time < 4);//最多四个顺子

	//将提取出的顺子再和剩余牌组合
	for (int i = seq_count - 1; i >= 0; i--)
	{
		BYTE type = SequenceCount[i] & 0xF0;
		if (type == 0x10)
		{

			int pos = 1;
			//向前寻找
			while ((temp[Sequence[i][0] - pos] != 0) && ((Sequence[i][0] - pos) < 15))
			{
				if ((temp[Sequence[i][0] - pos] == 1)//有紧接着的一张单牌直接添加
					|| (temp[Sequence[i][0] - pos] == 2 && temp[Sequence[i][0] - pos - 1] == 1)//紧跟一个对子，并且对子后面还有一个单张，此时拆对子连顺子
					|| (temp[Sequence[i][0] - pos] == 3 && temp[Sequence[i][0] - pos - 1] == 1))
				{
					BYTE increase_Sequence[20] = { 0x00 };
					temp[Sequence[i][0] - pos]--;
					memcpy(increase_Sequence, &Sequence[i][0], sizeof(increase_Sequence));
					Sequence[i][0] = Sequence[i][0] - pos;
					memcpy(&Sequence[i][1], increase_Sequence, (sizeof(increase_Sequence) - sizeof(BYTE)));
					SequenceCount[i]++;

				}
				else break;
			}
			//向后寻找
			BYTE j = (SequenceCount[i] & 0x0F) - 0x01;
			while ((temp[Sequence[i][j] + pos] != 0) && ((Sequence[i][j] + pos) < 15))
			{
				if ((temp[Sequence[i][j] + pos] == 1)//有紧接着的一张单牌直接添加
					|| (temp[Sequence[i][j] + pos] == 2 && temp[Sequence[i][j] + pos + 1] == 1)//紧跟一个对子，并且对子后面还有一个单张，此时拆对子连顺子
					|| (temp[Sequence[i][j] + pos] == 3 && temp[Sequence[i][j] + pos + 1] == 1))
				{
					temp[Sequence[i][j] + pos]--;
					Sequence[i][j + pos] = Sequence[i][j] + pos;
					SequenceCount[i]++;

					if (temp[Sequence[i][0]] == 1)
					{
						BYTE decrease_Sequence[20] = { 0x00 };
						temp[Sequence[i][0]] ++;
						memcpy(decrease_Sequence, &Sequence[i][1], (sizeof(decrease_Sequence) - sizeof(BYTE)));//如果起始位置还有余牌，还原第一张为对子
						memcpy(&Sequence[i][0], decrease_Sequence, sizeof(decrease_Sequence));
						SequenceCount[i]--;
					}
					j = (SequenceCount[i] & 0x0F) - 0x01;
				}
				else break;
			}
			//如果5单顺或者6单顺中拆开后有三个以上的三条，就拆开
			BYTE k = (SequenceCount[i] & 0x0F);
			int last_couple = 0;
			int last_single = 0;
			for (BYTE m = 0; m < k; m++)
			{
				if (temp[Sequence[i][m]] == 2)
				{
					last_couple++;
				}
				if (temp[Sequence[i][m]] == 1)
				{
					last_single++;
				}
			}
			if ((last_single >= ((k + 1) / 2))//对子数大于连牌张数+1再除以1，拆开顺子(有2个或以上个三条)
				|| ((SequenceCount[i] & 0xF0) == 0x10 && (last_single > k - last_couple * 2 - last_single)))//比较拆开后单的数量
			{
				//顺子还原回去
				while (k > 0)
				{
					k--;
					temp[Sequence[i][k]]++;
				}
				//从顺子列表中剔除
				int n = i;
				while (n < 3)
				{
					memcpy(&Sequence[n][0], &Sequence[n + 1][0], 20 * sizeof(BYTE));
					SequenceCount[n] = SequenceCount[n + 1];
					n++;
				}
				SequenceCount[3] = 0x00;
				memset(&Sequence[3][0], 0, 20 * sizeof(BYTE));
				i--;
			}
		}
		else if (type == 0x20)
		{
			int pos = 1;
			BYTE j = SequenceCount[i] & 0x0F;
			while ((temp[Sequence[i][j] + pos] != 0) && ((Sequence[i][j] + pos) < 15))
			{
				if (temp[Sequence[i][j] + pos] == 2)//有紧接着的对子直接添加
				{
					temp[Sequence[i][j] + pos] -= 2;
					Sequence[i][j + pos] = Sequence[i][j] + pos;
					SequenceCount[i]++;

					if (temp[Sequence[i][0]] == 1)
					{
						BYTE decrease_Sequence[20] = { 0x00 };
						temp[Sequence[i][0]] += 2;
						memcpy(decrease_Sequence, &Sequence[i][1], (sizeof(decrease_Sequence) - sizeof(BYTE)));//如果起始位置还有余牌，还原第一张为三条
						memcpy(&Sequence[i][0], decrease_Sequence, sizeof(decrease_Sequence));
						SequenceCount[i]--;
					}
					j = SequenceCount[i] & 0x0F - 0x01;
				}
				else break;
			}
		}
	}

	//合并顺子，如果有头尾相连，直接合并
	for (int i = 0; i < 4; i++)
	{
		if (Sequence[i][0] == 0x00)
		{
			seq_count = i;
			break;
		}
		//如果起始位置还有余牌，还原第一张为三条
		BYTE j = (SequenceCount[i] & 0x0F);
		while ((temp[Sequence[i][0]] == 2) && (j > 5))
		{
			BYTE decrease_Sequence[20] = { 0x00 };
			temp[Sequence[i][0]]++;
			memcpy(decrease_Sequence, &Sequence[i][1], (sizeof(decrease_Sequence) - sizeof(BYTE)));
			memcpy(&Sequence[i][0], decrease_Sequence, sizeof(decrease_Sequence));
			SequenceCount[i]--;

			j = (SequenceCount[i] & 0x0F);
		}
	}

	//如果有两个顺子张数相同起始相差一,并且小顺子的头一张可以还原为三条,此时将两个顺子还原为双顺加三带一
	for (UINT i = 0; i < 4; i++)
	{
		if (SequenceCount[i] == 0x00)
		{//不是顺子
			break;
		}
		if ((SequenceCount[i] & 0xF0) != 0x10)
		{//不是单顺
			continue;
		}
		for (UINT j = i+1; j < 4; j++)
		{
			if (SequenceCount[j] == 0x00)
			{
				break;
			}
			if (((SequenceCount[i] & 0xF0) != 0x10) || (SequenceCount[j] != SequenceCount[i]))
			{//不是单顺
				continue;
			}
			if (Sequence[j][0] + 1 == Sequence[i][0] && temp[Sequence[j][0]] == 2)
			{
				//删除两个单顺,组成一个双顺
				temp[Sequence[j][0]]++;
				temp[Sequence[i][(SequenceCount[i]&0x0F) -1]]++;

				SequenceCount[i] = 0x20 + (SequenceCount[i] & 0x0F) - 0x01;
				Sequence[i][(SequenceCount[i]&0x0F)] = 0x00;
				for (size_t k = j; k < 3; k++)
				{
					memcpy(Sequence[k], Sequence[k + 1], 20 * sizeof(BYTE));
					SequenceCount[k] = SequenceCount[k + 1];
					memset(Sequence[k + 1], 0, 20 * sizeof(BYTE));
					SequenceCount[k + 1] = 0x00;
				}
			}
			if (Sequence[j][0] == Sequence[i][0] + 1 && temp[Sequence[i][0]] == 2)
			{
				//删除两个单顺,组成一个双顺
				temp[Sequence[i][0]]++;
				temp[Sequence[j][(SequenceCount[j] & 0x0F) - 1]]++;

				SequenceCount[j] = 0x20 + (SequenceCount[j] & 0x0F) - 0x01;
				Sequence[j][(SequenceCount[j]&0x0F)] = 0x00;
				for (size_t k = i; k < 3; k++)
				{
					memcpy(Sequence[k], Sequence[k + 1], 20 * sizeof(BYTE));
					SequenceCount[k] = SequenceCount[k + 1];
					memset(Sequence[k + 1], 0, 20 * sizeof(BYTE));
					SequenceCount[k + 1] = 0x00;
				}
			}
		}
	}

	//提取双顺
	for (int i = 3; i < 14; i++)
	{
		if (temp[i] == 2 && temp[i + 1] == 2 && temp[i + 2] == 2 && i < 11)
		{
			SequenceCount[seq_count] = 0x23;
			Sequence[seq_count][0] = i;//记录双顺
			temp[i] = 0; temp[i + 1] = 0; temp[i + 2] = 0;
			i += 2;
		}
		else if (temp[i] == 2 && SequenceCount[seq_count] > 0 && i < 13)
		{
			SequenceCount[seq_count]++;
			temp[i + 1] = 0;
		}
		else
		{
			if (SequenceCount[seq_count] > 0)seq_count++;
		}
		if (SequenceCount[seq_count] > 0 && i == 13) seq_count++;
	}

	//memcpy(byTempCardList, temp, sizeof(temp));

	//删除掉提取出来的牌
	for (int i = 0; i < 4; i++)
	{
		int count_length = 0x0F & SequenceCount[i];
		int count_width = (0xF0 & SequenceCount[i])>>4;
		while (count_length > 0)
		{
			byTempCardList[Sequence[i][count_length-1]] -= count_width;
			count_length--;
		}
	}

	bySequenceList[0] = Sequence[0][0];
	bySequenceList[1] = Sequence[1][0];
	bySequenceList[2] = Sequence[2][0];
	bySequenceList[3] = Sequence[3][0];
	memcpy(bySequenceCount, SequenceCount, sizeof(SequenceCount));
}

//获取手牌中的对应牌值
BYTE CGameLogic::GetRealCardValue(BYTE byTempCardList[], BYTE &byTempCardCount, BYTE check_value)
{

	BYTE card_value[12] = { 0x00 };
	if (byTempCardCount > 20 || byTempCardList == NULL || !IsValidataCard(check_value))
	{
		return 0x00;
	}

	for (int j = 0; j < byTempCardCount; j++)
	{
		if (GetCardBulk(byTempCardList[j], false) == check_value)
		{
			card_value[0] = byTempCardList[j];
			RemoveCard(card_value, 1, byTempCardList, byTempCardCount);
			byTempCardCount--;
			break;
		}
	}
	return card_value[0];
}
//寻找匹配牌型
bool CGameLogic::GetMatchCards(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int& iResultCardCount, bool bFellowFriend)
{
	BYTE backup_cardlist[20] = { 0 }; //手牌数据
	BYTE backup_CardCount = 0;	      //手牌数目
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;
	int iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型

	switch (iBaseShape)
	{
		case UG_ONLY_ONE: //单张
		{
			BYTE byStartValue = GetCardBulk(iBaseCard[0]);
			GetSingleCard(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, !bFellowFriend);
			//如果只剩一对或者一单,剩下全是顺子就直接跟大小王
			if (iResultCardCount == 0 && (m_double_count + m_single_count == 1))
			{
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				if (m_have_large_king && !m_have_little_king)
				{
					iResultCard[0] = 0x4F;
					iResultCardCount = 1;
				}
				if (!m_have_large_king && m_have_little_king)
				{
					iResultCard[0] = 0x4E;
					iResultCardCount = 1;
				}
			}
			if (m_have_little_king && !m_have_large_king && m_double_count == 1 && m_single_count == 0)
			{
				iResultCard[0] = 0x4E;
				iResultCardCount = 1;
			}
			if (!m_have_little_king && m_have_large_king && m_double_count == 1 && m_single_count == 0)
			{
				iResultCard[0] = 0x4F;
				iResultCardCount = 1;
			}
			break;
		}
		case UG_DOUBLE:   //对牌
		{
			BYTE byStartValue = GetCardBulk(iBaseCard[0]);
			GetDoubleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, !bFellowFriend);
			break;
		}
		case UG_THREE:    //三张
		{
			BYTE byStartValue = GetBulkBySpecifyCardCount(iBaseCard, iBaseCardCount, 3);
			GetTripleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, false);
			break;
		}
		case UG_THREE_ONE: //三带一
		{
			BYTE byStartValue = GetBulkBySpecifyCardCount(iBaseCard, iBaseCardCount, 3);
			GetTripleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, false);
			GetSingleCard(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, 0x00, true);
			if (iResultCardCount == 3)
			{
				if (m_have_large_king && !m_have_little_king)
				{
					iResultCard[iResultCardCount++] = 0x4F;
				}
				if (!m_have_large_king && m_have_little_king)
				{
					iResultCard[iResultCardCount++] = 0x4E;
				}
				else
				{
					memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
					iResultCardCount = 0;
				}
			}
			break;
		}
		case UG_THREE_DOUBLE: //三带二（对子）
		{
			BYTE byStartValue = GetBulkBySpecifyCardCount(iBaseCard, iBaseCardCount, 3);
			GetTripleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, false);
			GetDoubleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, 0x00, true);
			break;
		}
		case UG_STRAIGHT:	//顺子 5张+
		{
			if (!bFellowFriend)
			{//目前只跟对手的
				GetSequence(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount, false);
			}
			break;
		}
		case UG_DOUBLE_SEC: //双顺
		{
			break;
		}
		case UG_BOMB:	//四张 炸弹
		{
			break;
		}
		case UG_FOUR_TWO:	//四张 炸弹
		{
			break;
		}
		case UG_FOUR_TWO_DOUBLE:	//四张 炸弹
		{
			break;
		}
		default:
		{
			iResultCardCount = 0;
			break;
		}
	}

	//检测能否出牌
	if (!CanOutCard(iResultCard, iResultCardCount, iBaseCard, iBaseCardCount))
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
	}
	else return true;

	return false;
}

//判断手中是否只剩炸弹加一手牌
int CGameLogic::CheckOnlyBombAndOneHand(BYTE byCardList[], BYTE byCardCount, BYTE byBoomCardList[])
{
	BYTE backup_cardlist[20] = { 0 };
	BYTE backup_CardCount = byCardCount;
	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));

	bool have_little_king = false;
	bool have_large_king = false;

	int boom_count = 0;
	BYTE boom_info[18] = { 0 };


	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	BYTE t_removecard[20] = { 0 };

	//去除大小王
	if (temp[16]>0)
	{
		temp[16] = 0;
		have_little_king = true;
		memset(t_removecard, 0, sizeof(t_removecard));
		t_removecard[0] = 0x4E;
		RemoveCard(t_removecard, 1, backup_cardlist, backup_CardCount);
	}
	
	if (temp[17]>0)
	{
		temp[17] = 0;
		have_large_king = true;
		memset(t_removecard, 0, sizeof(t_removecard));
		t_removecard[0] = 0x4F;
		RemoveCard(t_removecard, 1, backup_cardlist, backup_CardCount);
	}
	
	//去除炸弹
	for (int i = 0; i < 18; i++)
	{
		if (temp[i] == 4)
		{
			boom_info[boom_count++] = i;
			temp[i] = 0;
			memset(t_removecard, 0, sizeof(t_removecard));
			for (size_t i = 0; i < 4; i++)
			{
				t_removecard[i] = GetRealCardValue(backup_cardlist, backup_CardCount, i);
			}
		}
	}
	
	if (GetCardShape(backup_cardlist, backup_CardCount) == UG_ERROR_KIND)
	{//不是一手牌
		return 0;
	}

	if (have_little_king && have_large_king)
	{
		byBoomCardList[0] = 0x4E;
		byBoomCardList[1] = 0x4F;
		return 2;
	}
	
	if ((!have_little_king && !have_large_king)&& (boom_count >= 1))
	{
		BYTE backup_CardCount = byCardCount;
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		for (size_t i = 0; i < 4; i++)
		{
			byBoomCardList[i] = GetRealCardValue(backup_cardlist, backup_CardCount, boom_info[0]);
		}
		return 4;
	}
	return 0;
}

//清除手数相关数据
void CGameLogic::ClearTempAnalyseInfo()
{
	m_total_outhand_count = 0;
	m_real_single_count = 0;
	m_have_little_king = false;
	m_have_large_king = false;

	m_boom_count = 0;
	m_single_count = 0;
	m_double_count = 0;
	m_triple_count = 0;
	memset(m_boom_info, 0, sizeof(m_boom_info));
	memset(m_single_info, 0, sizeof(m_single_info));
	memset(m_double_info, 0, sizeof(m_double_info));
	memset(m_triple_info, 0, sizeof(m_triple_info));

	memset(m_SequenceListStart, 0, sizeof(m_SequenceListStart));
	memset(m_SequenceCount, 0, sizeof(m_SequenceCount));//0x15表示单顺五连，0x24表示双顺4连，0x23表示飞机2连
	 
	return;
}
//跟牌逻辑（角色分类）
bool CGameLogic::FellowCard(Card_Info card_info, BYTE iResultCard[], int& iResultCardCount, COperateRecord out_record)
{	
	CRecordTool Uplast_card;
	CRecordTool Downlast_card;
	Uplast_card.SetLastCard(card_info.UpUserCard, card_info.UpUserCardCount);
	Downlast_card.SetLastCard(card_info.DownUserCard, card_info.DownUserCardCount);

	bool bBasePosIsNext = false;
	if (card_info.DeskPos == 2)bBasePosIsNext = true;
	if (card_info.NTposition == 0)//本家庄家
	{
		LandlordFellow(card_info.UserCard, card_info.UserCardCount, card_info.DeskCard, card_info.DeskCardCount,
							bBasePosIsNext, iResultCard, iResultCardCount, Uplast_card, Downlast_card, out_record);
	}
	else if (card_info.NTposition == 1)//上家庄家
	{
		if (card_info.DeskPos == 1)
		{
			FamerFellowPreLD(card_info.UserCard, card_info.UserCardCount, card_info.DeskCard, card_info.DeskCardCount,
								bBasePosIsNext, iResultCard, iResultCardCount, Uplast_card, Downlast_card, out_record);
		}
		else
		{
			FamerFellowPostFM(card_info.UserCard, card_info.UserCardCount, card_info.DeskCard, card_info.DeskCardCount,
								bBasePosIsNext, iResultCard, iResultCardCount, Uplast_card, Downlast_card, out_record);
		}
	}
	else   //下家庄家
	{
		if (card_info.DeskPos == 2)
		{
			FamerFellowPostLD(card_info.UserCard, card_info.UserCardCount, card_info.DeskCard, card_info.DeskCardCount,
								bBasePosIsNext, iResultCard, iResultCardCount, Uplast_card, Downlast_card, out_record);
		}
		else
		{
			FamerFellowPreFM(card_info.UserCard, card_info.UserCardCount, card_info.DeskCard, card_info.DeskCardCount,
								bBasePosIsNext, iResultCard, iResultCardCount, Uplast_card, Downlast_card, out_record);
		}
	}

	if (!CanOutCard(iResultCard, iResultCardCount, card_info.DeskCard, card_info.DeskCardCount))
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
	}

	return true;
}

//地主跟牌
void CGameLogic::LandlordFellow(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);

	BYTE backup_cardlist[20] = { 0 }; //手牌数据
	BYTE backup_CardCount = 0;	      //手牌数目
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;

	Analysis_Result sPreRS;		 //跟牌前分析结果
	Analysis_Result sPostRS;	 //跟牌后分析结果

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//只剩炸弹+王炸(或单王)+一手牌
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (sPreRS.boom_count > 0
		&& (iBaseShape < UG_BOMB || (iBaseShape == UG_BOMB && m_boom_info[0] > GetCardBulk(iBaseCard[0])))
		&& (GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND))
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
		memset(byReslult, 0, sizeof(byReslult));
		iReslultcount = 0;

		for (size_t i = 0; i < 4; i++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[0]);
			if (return_card != 0x00)
			{
				byReslult[iReslultcount++] = return_card;
			}
		}

		//是否一手能出完,并且余牌中没有任何炸弹
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
			&& 0 == Uplast_card.IsHaveBoom() && 0 == Downlast_card.IsHaveBoom())
		{
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//如果手中只剩王炸家一手牌，直接王炸
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//是否一手能出完,并且余牌中没有任何炸弹
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			|| (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
			&& 0 == Uplast_card.IsHaveBoom() && 0 == Downlast_card.IsHaveBoom()))
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//桌面上牌型小于炸弹并且手中只剩王炸或者炸弹直接跟
	if ((iBaseShape <= UG_BOMB)
		&& ((GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB)
		|| (GetCardShape(backup_cardlist, backup_CardCount) == UG_BOMB)))
	{
		if (iBaseCard[0] < iHandCard[0] && iBaseShape == UG_BOMB || iBaseShape < UG_BOMB || (GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB))
		{
			memcpy(iResultCard, iHandCard, iHandCardCount);
			iResultCardCount = iHandCardCount;
			return;
		}
	}
	else
	{
		//对家只剩一张,需要跟单张情况
		if (PreHandCount == 1 || (PostHandCount == 1 && iBaseShape == UG_ONLY_ONE))
		{
			BYTE max_card = FindMaxCardInLast(backup_cardlist, backup_CardCount, iBaseCard[0], Uplast_card, Downlast_card);
			if (max_card != 0x00)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, max_card);
				iResultCard[0] = return_card;
				iResultCardCount = 1;
				return;
			}
		}

		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
		//匹配牌型可以跟牌
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, false);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//单牌是否从最大的出 
			if (PostHandCount == 1) bOutBigSingle = true;//如果下家只剩一张，从最大的单牌开始跟
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
		}

		if (iResultCardCount == iHandCardCount)
		{//能出完直接出
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//能出完就直接跟
			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Uplast_card.IsHaveBoom() < 2 && Downlast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//不出
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
			&& Uplast_card.IsHaveBoom() < 2 && Downlast_card.IsHaveBoom() < 2)
		{
			return;
		}

		AnalyzeHandCard(backup_cardlist, backup_CardCount, false);
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPostRS);
		
		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB) 
		{//不用炸弹跟,炸弹跟牌的策略在前面已经判断
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//如果对家只剩一张,不用炸弹跟的直接跟
		if (PreHandCount == 1 || PostHandCount == 1)
		{
			return;
		}

		//跟牌策略
		//单牌对牌
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//单小王比压2 , 单大王必压小王
			if (iBaseShape == UG_ONLY_ONE && GetCardBulk(iBaseCard[0], false) == 15 && iResultCard[0] == 0x4E && !m_have_large_king)
			{
				return;
			}
			if (iBaseShape == UG_ONLY_ONE && iBaseCard[0] == 0x4E && iResultCard[0] == 0x4F)
			{
				return;
			}
			if (iBaseShape == UG_ONLY_ONE && GetCardBulk(iBaseCard[0], false) == 15 && iResultCard[0] == 0x4F && !m_have_little_king)
			{
				return;
			}

			//跟牌需要拆顺子时,除非对手只剩一张牌
			if (sPostRS.iSeqCount < sPreRS.iSeqCount && (PreHandCount > 1 || PostHandCount > 1))
			{
				return;
			}
			//不拆顺子的前提下,两次不跟,这次就要跟
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && out_record.GetContinuePassCount(iBaseShape) >= 2)
			{
				return;
			}
			
			//手数减少或者相等
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				if (GetCardBulk(iResultCard[0], false) <= 14)
				{
					return;
				}
				else
				{
					//转换牌值
					BYTE temp[18] = { 0 };
					for (int i = 0; i < backup_CardCount; i++)
					{
						temp[GetCardBulk(backup_cardlist[i], false)]++;
					}

					if (iBaseShape == UG_ONLY_ONE)
					{
						//如果是拆王炸,对方手牌较多时可以先不跟
						if (iResultCard[0] == 0x4E && m_have_large_king && (PreHandCount > 6 && PostHandCount > 6))
						{
							memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
							iResultCardCount = 0;
							return;
						}
						//A以上的大牌个数大于等于单张个数
						int count_max = temp[15] + temp[16] + temp[17];
						if (count_max >= m_real_single_count)
						{
							return;
						}
					}
					if(iBaseShape == UG_DOUBLE)
					{
						//A以上的大牌个数大于等于单张个数
						int count_max = temp[15] + temp[16] + temp[17];
						if ((PreHandCount > 6 && PostHandCount > 6) && count_max >= m_real_single_count && sPostRS.iAloneDouble < 2)
						{
							return;
						}
					}
				}
			}

			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}
		
		//三张(带牌)
		if (iBaseShape == UG_THREE || iBaseShape == UG_THREE_ONE || iBaseShape == UG_THREE_TWO || iBaseShape == UG_THREE_DOUBLE)
		{
			//跟牌需要拆顺子时,除非对手只剩一张牌
			if (sPostRS.iSeqCount < sPreRS.iSeqCount && (PreHandCount == 1 || PostHandCount == 1))
			{
				return;
			}

			//不拆顺子的前提下,跟完手数相等或减少或者两次不跟,这次就要跟
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount)
			{
				if (sPostRS.total_outhand_count <= sPreRS.total_outhand_count)
				{
					return;
				}
				if (out_record.GetContinuePassCount(iBaseShape) >= 1)
				{
					return;
				}
			}

			//不拆顺子,手中单小于两张,小于A,要跟
			if (sPostRS.iSeqCount == sPreRS.iSeqCount
				&& m_real_single_count < 2 
				&& (GetCardBulk(iResultCard[0], false) < 14))
			{
				return;
			}

			//手数减少或者相等
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				if (GetCardBulk(iResultCard[0], false) < 14)
				{
					return;
				}
				else
				{
					if (sPostRS.iAloneSingle + sPostRS.iAloneDouble < 3)
					{
						return;
					}
				}
			}

			//其他情况不跟
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		if (iBaseShape == UG_STRAIGHT || iBaseShape == UG_DOUBLE_SEC || iBaseShape == UG_THREE_SEQUENCE 
			|| iBaseShape == UG_THREE_ONE_SEQUENCE || iBaseShape == UG_THREE_DOUBLE_SEC || iBaseShape == UG_THREE_TWO_SEQUENCE)
		{
			//只剩一张牌
			if ((PreHandCount == 1 || PostHandCount == 1))
			{
				return;
			}

			//跟完手数减少或相等
			if ((sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				return;
			}

			//其他情况不跟
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}
	}

	memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
	iResultCardCount = 0;
	return;
}

//农民跟(上家地主)
void CGameLogic::FamerFellowPreLD(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Downlast_card.ClearAllData();

	BYTE backup_cardlist[20] = { 0 }; //手牌数据
	BYTE backup_CardCount = 0;	      //手牌数目
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;

	Analysis_Result sPreRS;		 //跟牌前分析结果
	Analysis_Result sPostRS;	 //跟牌后分析结果

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//如果手中只剩王炸家一手牌，直接王炸
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//是否一手能出完,并且余牌中没有任何炸弹
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Uplast_card.IsHaveBoom())
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//下家只剩一张,用最大的牌去跟
	if (PostHandCount == 1 && m_bFeedCard)
	{
		if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
		{
			iResultCard[0] = 0x4e;
			iResultCard[1] = 0x4f;
			iResultCardCount = 2;
			return;
		}
		else if (m_boom_count > 0)
		{
			if (iBaseShape < UG_BOMB)
			{
				for (int i = 0; i < 4; i++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[m_boom_count - 1]);
					if (return_card != 0x00)
					{
						iResultCard[iResultCardCount++] = return_card;
					}
				}
			}
			else if (iBaseShape == UG_BOMB)
			{
				for (int i = 0; i < m_boom_count; i++)
				{
					if (m_boom_info[i] > mGetCardBulk(iBaseCard[0], false))
					{
						for (int j = 0; j < 4; j++)
						{
							BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[m_boom_count - 1]);
							if (return_card != 0x00)
							{
								iResultCard[iResultCardCount++] = return_card;
							}
						}
					}
				}
			}
			return;
		}
		else
		{
			UINT need_count = 5;
			//转换牌值
			BYTE feed_card_index[CARD_BULK_INDEX] = { 0 };
			for (int feed_index = 0; feed_index < iHandCardCount; feed_index++)
			{
				feed_card_index[mGetCardBulk(iHandCard[feed_index], false)]++;
			}

			if (iBaseShape == UG_ONLY_ONE)
			{
				need_count = 1;
			}
			else if (iBaseShape == UG_DOUBLE)
			{
				need_count = 2;
			}

			BYTE last_max_card = Uplast_card.GetLastMaxCard(need_count);

			for (int m = (CARD_BULK_INDEX - 1); m >= 0; m--)
			{
				if (feed_card_index[m] >= need_count)
				{
					if (m >= last_max_card)
					{
						for (size_t l = 0; l < need_count; l++)
						{
							BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m);
							iResultCard[iResultCardCount++] = return_card;							
						}
						return;
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	
	//桌面上牌型小于炸弹并且手中只剩王炸或者炸弹直接跟
	if ((iBaseShape <= UG_BOMB)
		&& ((GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB)
		|| (GetCardShape(backup_cardlist, backup_CardCount) == UG_BOMB)))
	{
		if (iBaseCard[0] < iHandCard[0] && iBaseShape == UG_BOMB || iBaseShape < UG_BOMB || (GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB))
		{
			memcpy(iResultCard, iHandCard, iHandCardCount);
			iResultCardCount = iHandCardCount;
			return;
		}
	}
	else
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
		//匹配牌型可以跟牌
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, false);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//单牌是否从最大的出 
			if (PostHandCount == 1) bOutBigSingle = true;//如果下家只剩一张，从最大的单牌开始跟
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
			//如果是王炸
			if (IsKingBomb(iResultCard, iResultCardCount))
			{
				BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
				int boomCardCount = 2;

				RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
				backup_CardCount -= 2;

				//是否一手能出完,并且余牌中没有任何炸弹
				if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
					&& 0 == Uplast_card.IsHaveBoom())
				{
					memcpy(iResultCard, byReslult, sizeof(byReslult));
					iResultCardCount = iReslultcount;
					return;
				}
				else
				{
					//还原
					memcpy(backup_cardlist, iHandCard, iHandCardCount);
					backup_CardCount = iHandCardCount;
				}
			}
		}

		if (iResultCardCount == iHandCardCount)
		{//能出完直接出
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//能出完就直接跟
			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Uplast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//不出
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		//如果对家只剩一张,不用炸弹跟的直接跟
		if (PreHandCount == 1)
		{
			return;
		}

		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
			&& Uplast_card.IsHaveBoom() < 2)
		{
			return;
		}

		AnalyzeHandCard(backup_cardlist, backup_CardCount, false);
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPostRS);

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//不用炸弹跟,炸弹跟牌的策略在前面已经判断
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//跟牌策略
		//单牌对牌
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//单小王比压2 , 单大王必压小王
			if (iBaseShape == UG_ONLY_ONE && GetCardBulk(iBaseCard[0], false) == 15 && iResultCard[0] == 0x4E && !m_have_large_king)
			{
				return;
			}
			if (iBaseShape == UG_ONLY_ONE && iBaseCard[0] == 0x4E && iResultCard[0] == 0x4F)
			{
				return;
			}
			if (iBaseShape == UG_ONLY_ONE && GetCardBulk(iBaseCard[0], false) == 15 && iResultCard[0] == 0x4F && !m_have_little_king)
			{
				return;
			}

			//跟牌需要拆顺子时,除非对手只剩一张牌,或者队友只剩一张牌
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPreRS.iSeqCount == 0) && (PreHandCount < 3 || PostHandCount == 1))
			{
				return;
			}
			//不拆顺子的前提下,两次不跟,地主手牌数小于等于6,这次就要跟
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && out_record.GetContinuePassCount(iBaseShape) >= 2 && PreHandCount <= 6)
			{
				return;
			}
			
			//手数减少或者相等
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				if (GetCardBulk(iResultCard[0], false) < 14)
				{
					return;
				}
				else
				{
					if (PreHandCount <= 3)
					{
						return;
					}

					//转换牌值
					BYTE temp[18] = { 0 };
					for (int i = 0; i < backup_CardCount; i++)
					{
						temp[GetCardBulk(backup_cardlist[i], false)]++;
					}

					if (iBaseShape == UG_ONLY_ONE)
					{
						//如果是拆王炸,对方手牌较多时可以先不跟
						if (iResultCard[0] == 0x4E && m_have_large_king && PreHandCount > 6)
						{
							memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
							iResultCardCount = 0;
							return;
						}
						//A以上的大牌个数大于等于单张个数
						int count_max = temp[15] + temp[16] + temp[17]+ m_boom_count;
						if ((PreHandCount <= 6 || iHandCardCount <= 6) && count_max >= m_real_single_count)
						{
							return;
						}
						//如果手中只剩下一张单牌,可以跟
						if (m_single_count == 1)
						{
							return;
						}
						//如果手里只剩单牌,能跟就跟
						if (m_single_count == (iHandCardCount - 1))
						{
							return;
						}
					}

					if (iBaseShape == UG_DOUBLE)
					{
						//A以上的大牌个数大于等于单张个数
						int count_max = temp[15] + temp[16] + temp[17];
						if (PreHandCount <= 6 || (sPostRS.iAloneSingle < 2 && sPostRS.iAloneDouble < 2))
						{
							return;
						}
					}
				}
			}			

			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//三张(带牌)
		if (iBaseShape == UG_THREE || iBaseShape == UG_THREE_ONE || iBaseShape == UG_THREE_TWO || iBaseShape == UG_THREE_DOUBLE)
		{
			//跟牌需要拆顺子时,除非对手只剩一张牌
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPostRS.total_outhand_count >= sPreRS.total_outhand_count) && (PreHandCount == 1))
			{
				return;
			}

			if (sPostRS.total_outhand_count <= sPreRS.total_outhand_count)
			{
				//地主手牌大于6张,并且子还有单,还有小对子就不跟
				if ((sPostRS.iAloneDouble + sPostRS.iAloneSingle > 1) && PreHandCount > 6 && (GetCardBulk(iResultCard[0], false) > 14))
				{
					memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
					iResultCardCount = 0;
					return;
				}
				if (PostHandCount < 6)
				{//地主手牌少于6张要跟了
					return;
				}
			}
			if (out_record.GetContinuePassCount(iBaseShape) >= 1 && PostHandCount < 6)
			{
				return;
			}

			//不拆顺子,手中单小于两张,小于A,要跟
			if (sPostRS.iSeqCount == sPreRS.iSeqCount
				&& m_real_single_count < 2
				&& (GetCardBulk(iResultCard[0], false) < 14))
			{
				return;
			}

			//手数减少或者相等
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				return;
			}

			//其他情况不跟
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		if (iBaseShape == UG_STRAIGHT || iBaseShape == UG_DOUBLE_SEC || iBaseShape == UG_THREE_SEQUENCE
			|| iBaseShape == UG_THREE_ONE_SEQUENCE || iBaseShape == UG_THREE_DOUBLE_SEC || iBaseShape == UG_THREE_TWO_SEQUENCE)
		{
			//只剩一张牌
			if (PreHandCount == 1 || PostHandCount == 1)
			{
				return;
			}

			//跟完手数减少或相等
			if ((sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				return;
			}

			//其他情况不跟
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}
	}

	memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
	iResultCardCount = 0;
	return;
}

//农民跟(下家地主)
void CGameLogic::FamerFellowPostLD(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Uplast_card.ClearAllData();

	//同地主跟牌
	BYTE backup_cardlist[20] = { 0 }; //手牌数据
	BYTE backup_CardCount = 0;	      //手牌数目
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;

	Analysis_Result sPreRS;		 //跟牌前分析结果
	Analysis_Result sPostRS;	 //跟牌后分析结果

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//如果手中只剩王炸家一手牌，直接王炸
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//是否一手能出完,并且余牌中没有任何炸弹
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Downlast_card.IsHaveBoom())
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//只剩炸弹+王炸(或单王)+一手牌
	if (sPreRS.boom_count > 0
		&& (iBaseShape < UG_BOMB || (iBaseShape == UG_BOMB && m_boom_info[0] > GetCardBulk(iBaseCard[0])))
		&& (GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND))
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
		memset(byReslult, 0, sizeof(byReslult));
		iReslultcount = 0;

		for (size_t i = 0; i < 4; i++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[0]);
			if (return_card != 0x00)
			{
				byReslult[iReslultcount++] = return_card;
			}
		}

		//是否一手能出完,并且余牌中没有任何炸弹
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
			&& 0 == Downlast_card.IsHaveBoom())
		{
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//桌面上牌型小于炸弹并且手中只剩王炸或者炸弹直接跟
	if ((iBaseShape <= UG_BOMB)
		&& ((GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB)
		|| (GetCardShape(backup_cardlist, backup_CardCount) == UG_BOMB)))
	{
		if (iBaseCard[0] < iHandCard[0] && iBaseShape == UG_BOMB || iBaseShape < UG_BOMB || (GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB))
		{
			memcpy(iResultCard, iHandCard, iHandCardCount);
			iResultCardCount = iHandCardCount;
			return;
		}
	}
	else
	{
		//匹配牌型可以跟牌
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, false);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//单牌是否从最大的出 
			if (PostHandCount == 1) bOutBigSingle = true;//如果下家只剩一张，从最大的单牌开始跟
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
			//如果是王炸
			if (IsKingBomb(iResultCard, iResultCardCount))
			{
				BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
				int boomCardCount = 2;

				RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
				backup_CardCount -= 2;

				//是否一手能出完,并且余牌中没有任何炸弹
				if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
					&& 0 == Downlast_card.IsHaveBoom())
				{
					memcpy(iResultCard, byReslult, sizeof(byReslult));
					iResultCardCount = iReslultcount;
					return;
				}
				else
				{
					//还原
					memcpy(backup_cardlist, iHandCard, iHandCardCount);
					backup_CardCount = iHandCardCount;
				}
			}
		}

		if (iResultCardCount == iHandCardCount)
		{//能出完直接出
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//能出完就直接跟,如果地主只剩一张的时候需要跟牌(王炸要跟)
			//庄家大于一张,不炸
			if (PostHandCount <= 2)
			{
				return;
			}

			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Downlast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//清空出牌数据
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		//跟牌直接能直接出完直接跟
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
			&& Downlast_card.IsHaveBoom() < 2)
		{
			return;
		}

		//如果对家只剩一张,不用炸弹跟的直接跟
		if (PostHandCount == 1)
		{
			return;
		}

		AnalyzeHandCard(backup_cardlist, backup_CardCount, false);
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPostRS);

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//不用炸弹跟,炸弹跟牌的策略在前面已经判断
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//跟牌策略
		//单牌对牌
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//单小王比压2 , 单大王必压小王
			if (iBaseShape == UG_ONLY_ONE && GetCardBulk(iBaseCard[0], false) == 15 && iResultCard[0] == 0x4E && !m_have_large_king)
			{
				return;
			}
			if (iBaseShape == UG_ONLY_ONE && iBaseCard[0] == 0x4E && iResultCard[0] == 0x4F)
			{
				return;
			}
			if (iBaseShape == UG_ONLY_ONE && GetCardBulk(iBaseCard[0], false) == 15 && iResultCard[0] == 0x4F && !m_have_little_king)
			{
				return;
			}

			//跟牌需要拆顺子时,除非对手只剩一张牌
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPostRS.total_outhand_count >= sPreRS.total_outhand_count) && (PostHandCount == 1))
			{
				return;
			}
			//不拆顺子的前提下,两次不跟,这次就要跟
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && out_record.GetContinuePassCount(iBaseShape) >= 2)
			{
				return;
			}
			//手数减少或者相等
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				if ((iResultCard[0] == 0x4E || iResultCard[0] == 0x4F) && (PostHandCount > 6 || m_single_count == 0))
				{
					if (sPostRS.iAloneSingle + sPostRS.iAloneDouble < 2 && m_triple_count == 0)
					{//如果跟大小王之后手里
						return;
					}
					memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
					iResultCardCount = 0;
				}
				return;
			}

			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//三张(带牌)
		if (iBaseShape == UG_THREE || iBaseShape == UG_THREE_ONE || iBaseShape == UG_THREE_TWO || iBaseShape == UG_THREE_DOUBLE)
		{
			//跟牌需要拆顺子时,除非对手只剩一张牌
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPostRS.total_outhand_count >= sPreRS.total_outhand_count) && (PostHandCount == 1))
			{
				return;
			}

			//不拆顺子的前提下,跟完手数相等或减少或者两次不跟,这次就要跟
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount)
			{
				if (sPostRS.total_outhand_count <= sPreRS.total_outhand_count)
				{
					//地主手牌大于6张,并且子还有单,还有小对子就不跟
					if ((sPostRS.iAloneDouble + sPostRS.iAloneSingle > 1) && PreHandCount > 6 && (GetCardBulk(iResultCard[0], false) > 14))
					{
						memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
						iResultCardCount = 0;
						return;
					}
					if (PostHandCount < 6)
					{//地主手牌少于6张要跟了
						return;
					}
				}
				if (out_record.GetContinuePassCount(iBaseShape) >= 2 && PostHandCount < 6)
				{
					return;
				}
			}

			//不拆顺子,手中单小于两张,小于A,要跟
			if (sPostRS.iSeqCount == sPreRS.iSeqCount
				&& m_real_single_count < 2
				&& (GetCardBulk(iResultCard[0], false) < 14))
			{
				return;
			}

			//手数减少或者相等
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				if (GetCardBulk(iResultCard[0], false) < 14)
				{
					return;
				}
				else
				{
					if (sPostRS.iAloneSingle < 4 && sPostRS.iAloneDouble < 4)
					{
						return;
					}
				}
			}

			//其他情况不跟
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		if (iBaseShape == UG_STRAIGHT || iBaseShape == UG_DOUBLE_SEC || iBaseShape == UG_THREE_SEQUENCE
			|| iBaseShape == UG_THREE_ONE_SEQUENCE || iBaseShape == UG_THREE_DOUBLE_SEC || iBaseShape == UG_THREE_TWO_SEQUENCE)
		{
			//只剩一张牌
			if (PostHandCount == 1)
			{
				return;
			}

			//跟完手数减少或相等
			if ((sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				return;
			}

			//其他情况不跟
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}
	}

	memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
	iResultCardCount = 0;
	return;
}

//农民跟(上家队友)
void CGameLogic::FamerFellowPreFM(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Uplast_card.ClearAllData();

	//有单/对子 就跟用较大的去跟(A以下),逼地主的大牌
	//其他牌型不跟,除非自己能出完

	BYTE backup_cardlist[20] = { 0 }; //手牌数据
	BYTE backup_CardCount = 0;	      //手牌数目
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;
	Analysis_Result sPreRS;		 //跟牌前分析结果
	Analysis_Result sPostRS;	 //跟牌后分析结果

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//如果手中只剩王炸家一手牌，直接王炸
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//是否一手能出完,并且余牌中没有任何炸弹
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Downlast_card.IsHaveBoom())
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//只剩炸弹+一手牌
	if (sPreRS.boom_count > 0
		&& (iBaseShape < UG_BOMB || (iBaseShape == UG_BOMB && m_boom_info[0] > GetCardBulk(iBaseCard[0])))
		&& (GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND))
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
		memset(byReslult, 0, sizeof(byReslult));
		iReslultcount = 0;

		for (size_t i = 0; i < 4; i++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[0]);
			if (return_card != 0x00)
			{
				byReslult[iReslultcount++] = return_card;
			}
		}

		//是否一手能出完,并且余牌中没有任何炸弹
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
			&& 0 == Downlast_card.IsHaveBoom())
		{
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//桌面上牌型小于炸弹并且手中只剩王炸或者炸弹直接跟
	if ((iBaseShape <= UG_BOMB)
		&& ((GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB)
		|| (GetCardShape(backup_cardlist, backup_CardCount) == UG_BOMB)))
	{
		if (iBaseCard[0] < iHandCard[0] && iBaseShape == UG_BOMB || iBaseShape < UG_BOMB || (GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB))
		{
			memcpy(iResultCard, iHandCard, iHandCardCount);
			iResultCardCount = iHandCardCount;
			return;
		}
	}
	else
	{
		//下家地主只剩一张,并且有可能放走地主,需要跟单张情况
		if (PostHandCount == 1 && iBaseShape == UG_ONLY_ONE && Downlast_card.GetLastMaxCard(1) > iBaseCard[0])
		{
			BYTE max_card = FindMaxCardInLast(backup_cardlist, backup_CardCount, iBaseCard[0], Uplast_card, Downlast_card);
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, max_card);
			if (return_card != 0x00)
			{
				iResultCard[0] = return_card;
				iResultCardCount = 1;
			}
			return;
		}

		//下家地主只剩一张,并且有可能放走地主,需要跟单张情况
		if (PostHandCount == 2 && iBaseShape == UG_DOUBLE && Downlast_card.GetLastMaxCard(2) > iBaseCard[0])
		{
			BYTE max_card = FindMaxCardInLast(backup_cardlist, backup_CardCount, iBaseCard[0], Uplast_card, Downlast_card, 2);
			for (size_t j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, max_card);
				if (return_card != 0x00)
				{
					iResultCard[0] = return_card;
					iResultCardCount = 1;
				}
			}
			return;
		}


		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
		//匹配牌型可以跟牌
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, true);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//单牌是否从最大的出 
			if (PostHandCount == 1) bOutBigSingle = true;//如果下家只剩一张，从最大的单牌开始跟
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
			//如果是王炸
			if (IsKingBomb(iResultCard, iResultCardCount))
			{
				BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
				int boomCardCount = 2;

				RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
				backup_CardCount -= 2;

				//是否一手能出完,并且余牌中没有任何炸弹
				if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
					&& 0 == Downlast_card.IsHaveBoom())
				{
					memcpy(iResultCard, byReslult, sizeof(byReslult));
					iResultCardCount = iReslultcount;
					return;
				}
				else
				{
					//还原
					memcpy(backup_cardlist, iHandCard, iHandCardCount);
					backup_CardCount = iHandCardCount;
				}
			}
		}

		if (iResultCardCount == iHandCardCount)
		{//能出完直接出
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//能出完就直接跟
			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Downlast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//不出
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		//跟牌直接能直接出完直接跟
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
			&& Downlast_card.IsHaveBoom() < 2)
		{
			return;
		}

		//下家地主只剩一张,并且上家出的单牌
		if (PostHandCount == 1 && iBaseShape == UG_ONLY_ONE)
		{
			//寻找余牌中的最大牌,炸弹不算
			BYTE max_card = FindMaxSingleInHand(iHandCard, iHandCardCount, Downlast_card);

			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, max_card);
			if (return_card != 0x00)
			{
				iResultCard[iResultCardCount++] = return_card;
			}
			return;
		}

		memcpy(backup_cardlist, iHandCard, iHandCardCount);
		AnalyzeHandCard(backup_cardlist, backup_CardCount, false);
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPostRS);

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//不用炸弹跟,炸弹跟牌的策略在前面已经判断
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//跟牌策略
		//单牌对牌
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//跟单牌
			if (iBaseShape == UG_ONLY_ONE)
			{
				for (INT i = 0; i < m_single_count; i++)
				{
					//上家出单牌小于10,自己抬一手
					if (m_single_info[i] > 10 && m_single_info[i] > GetCardBulk(iBaseCard[0], false) && GetCardBulk(iBaseCard[0], false) <= 10)
					{
						BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[i]);
						if (return_card != 0x00)
						{
							iResultCard[0] = return_card;
							iResultCardCount = 1;
							return;
						}
					}
				}
			}

			//对子大于10都不跟
			if (iBaseShape == UG_DOUBLE && (GetCardBulk(iResultCard[0], false) > 10))
			{
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}

			//跟牌需要拆顺子时,除非对手只剩一张牌
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPostRS.total_outhand_count >= sPreRS.total_outhand_count) && (PostHandCount == 1))
			{
				return;
			}

			//手数减少或者相等
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && sPostRS.total_outhand_count < sPreRS.total_outhand_count && (GetCardBulk(iResultCard[0], false) < 14))
			{
				return;
			}

			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}		
	}

	memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
	iResultCardCount = 0;
	return;
}

//农民跟(下家队友)
void CGameLogic::FamerFellowPostFM(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Downlast_card.ClearAllData();

	//单对有牌能要就跟,队友手牌少于5张什么都不跟了,除非自己能出完

	BYTE backup_cardlist[20] = { 0 }; //手牌数据
	BYTE backup_CardCount = 0;	      //手牌数目
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;
	Analysis_Result sPreRS;		 //跟牌前分析结果
	Analysis_Result sPostRS;	 //跟牌后分析结果

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//如果手中只剩王炸家一手牌，直接王炸
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//是否一手能出完,并且余牌中没有任何炸弹
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Uplast_card.IsHaveBoom())
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//只剩炸弹+一手牌
	if (sPreRS.boom_count > 0
		&& (iBaseShape < UG_BOMB || (iBaseShape == UG_BOMB && m_boom_info[0] > GetCardBulk(iBaseCard[0])))
		&& (GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND))
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
		memset(byReslult, 0, sizeof(byReslult));
		iReslultcount = 0;

		for (size_t i = 0; i < 4; i++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[0]);
			if (return_card != 0x00)
			{
				byReslult[iReslultcount++] = return_card;
			}
		}

		//是否一手能出完,并且余牌中没有任何炸弹
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
			&& 0 == Uplast_card.IsHaveBoom())
		{
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//还原
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//桌面上牌型小于炸弹并且手中只剩王炸或者炸弹直接跟
	if ((iBaseShape <= UG_BOMB)
		&& ((GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB)
		|| (GetCardShape(backup_cardlist, backup_CardCount) == UG_BOMB)))
	{
		if (iBaseCard[0] < iHandCard[0] && iBaseShape == UG_BOMB || iBaseShape < UG_BOMB || (GetCardShape(backup_cardlist, backup_CardCount) == UG_KING_BOMB))
		{
			memcpy(iResultCard, iHandCard, iHandCardCount);
			iResultCardCount = iHandCardCount;
			return;
		}
	}
	else
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
		//匹配牌型可以跟牌
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, true);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//单牌是否从最大的出 
			if (PostHandCount == 1) bOutBigSingle = true;//如果下家只剩一张，从最大的单牌开始跟
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
			//如果是王炸
			if (IsKingBomb(iResultCard, iResultCardCount))
			{
				BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
				int boomCardCount = 2;

				RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
				backup_CardCount -= 2;

				//是否一手能出完,并且余牌中没有任何炸弹
				if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
					&& 0 == Uplast_card.IsHaveBoom())
				{
					memcpy(iResultCard, byReslult, sizeof(byReslult));
					iResultCardCount = iReslultcount;
					return;
				}
				else
				{
					//还原
					memcpy(backup_cardlist, iHandCard, iHandCardCount);
					backup_CardCount = iHandCardCount;
				}
			}
		}

		if (iResultCardCount == iHandCardCount)
		{//能出完直接出
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//能出完就直接跟
			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Uplast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//不出
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		//跟牌直接能直接出完直接跟
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
			&& Uplast_card.IsHaveBoom() < 3)
		{
			return;
		}

		//如果上家庄家只剩一张,此时不能出完就不跟
		if (PreHandCount == 1)
		{
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		AnalyzeHandCard(backup_cardlist, backup_CardCount, false);
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPostRS);

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//不用炸弹跟,炸弹跟牌的策略在前面已经判断
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//跟牌策略
		//单牌对牌
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//对子大于10都不跟
			if (iBaseShape == UG_DOUBLE && (GetCardBulk(iResultCard[0], false) > 10))
			{
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}

			//手数减少,并且小于K的单对子
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && sPostRS.total_outhand_count < sPreRS.total_outhand_count && (GetCardBulk(iResultCard[0], false) < 14) && PostHandCount > 5)
			{
				return;
			}

			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}
	}

	memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
	iResultCardCount = 0;
	return;
}

//获取顺子 bCanSplite:是否拆牌找
int CGameLogic::GetSequence(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bCanSplite)
{
	BYTE backup_cardlist[20] = { 0 }; //手牌数据
	BYTE backup_CardCount = 0;	      //手牌数目
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;
	//获取出牌牌型的信息 
	bool find_match = false;
	BYTE base_start = 0x0F;
	BYTE base_count = 0x10 + (BYTE)iBaseCardCount;

	for (int i = 0; i < iBaseCardCount; i++)
	{
		if (mGetCardBulk(iBaseCard[i], false) < base_start)
		{//找到起始牌大小
			base_start = mGetCardBulk(iBaseCard[i], false);
		}
	}

	//先找可以匹配的张数
	for (int i = 0; i < 4; i++)
	{
		if (m_SequenceListStart[i] > base_start && m_SequenceCount[i] == base_count)
		{
			//找到匹配顺子
			int count = base_count & 0x0f;
			for (int j = 0; j < count; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[i] + j));
				if (return_card != 0x00)
				{
					iResultCard[iResultCardCount++] = return_card;					
				}
			}
			find_match = true;
			break;
		}
	}

	if (bCanSplite && !find_match)
	{

	}

	if (find_match)
	{
		return 0;
	}

	return -1;
}
//提起三张 bCanSplite:是否拆牌找 
int CGameLogic::GetTripleCards(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite)
{
	BYTE byStartBulk = byStartValue;

	for (int i = 0; i < m_triple_count; i++)
	{
		if (m_triple_info[i] > byStartBulk)
		{
			for (int j = 0; j < 3; j++)
			{
				BYTE return_card = GetRealCardValue(iHandCard, iHandCardCount, m_triple_info[i]);
				if (return_card != 0x00)
				{
					iResultCard[iResultCardCount++] = return_card;
				}
			}
			//删除三条信息
			for (int k = 0; k < m_triple_count - 1; k++)
			{
				m_triple_info[k] = m_triple_info[k + 1];
			}
			m_triple_info[m_triple_count - 1] = 0x00;
			return 0;
		}
	}

	if (bCanSplite)
	{}
	return -1;
}
//提取对子 bCanSplite:是否拆牌找 
int CGameLogic::GetDoubleCards(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite)
{
	for (int i = 0; i < m_double_count; i++)
	{
		if (m_double_info[i] > byStartValue)
		{
			for (int j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(iHandCard, iHandCardCount, m_double_info[i]);
				if (return_card != 0x00)
				{
					iResultCard[iResultCardCount++] = return_card;
				}
			}
			return 0;
		}
	}

	if (bCanSplite)
	{
		//拆三条
		for (int i = 0; i < m_triple_count; i++)
		{
			if (m_triple_info[i] > byStartValue)
			{
				int j = 0;
				while (j < 4)
				{
					if ((m_SequenceCount[j] & 0xF0) == 0x10)
					{
						BYTE byGetCard = 0x00;
						//拆了三条，剩下的单张能不能添加到已知单顺当中
						if (m_triple_info[i] == (m_SequenceListStart[j] - 1))
						{
							byGetCard = m_triple_info[i];
							//将拆开的单牌添加到单顺当中
							m_SequenceListStart[j] = m_triple_info[i];
							m_SequenceCount[j] += 1;
							//删除掉三条信息
							int k = i;
							while (k < 16)
							{
								m_triple_info[k] = m_triple_info[k + 1];
								m_triple_info[k + 1] = 0x00;
								k++;
							}
							m_triple_count -= 1;
						}
						else if (m_triple_info[i] == (m_SequenceListStart[j] + (m_SequenceCount[j] & 0x0F)))
						{
							byGetCard = m_triple_info[i];
							//将拆开的单牌添加到顺子当中
							m_SequenceCount[j] += 1;
							//删除掉对子信息
							int k = i;
							while (k < 16)
							{
								m_triple_info[k] = m_triple_info[k + 1];
								m_triple_info[k + 1] = 0x00;
								k++;
							}
							m_triple_count -= 1;							
						}
						for (int j = 0; j < 2; j++)
						{
							BYTE return_card = GetRealCardValue(iHandCard, iHandCardCount, byGetCard);
							if (return_card != 0x00)
							{
								iResultCard[iResultCardCount++] = return_card;
								return 1;
							}
						}
					}
					j++;
				}
			}
		}
		//在双顺中查找
		int iNum = 0;
		while (iNum < 4)
		{
			//超过三连，查找最大的那一张能否提出
			BYTE byMaxCardBulk = (m_SequenceListStart[iNum] + m_SequenceCount[iNum] - 1);
			if (((m_SequenceCount[iNum] & 0x0F) > 0x03) && (byMaxCardBulk > byStartValue) && ((m_SequenceCount[iNum] & 0xF0) == 0x20))
			{
				BYTE return_card = GetRealCardValue(iHandCard, iHandCardCount, byMaxCardBulk);
				for (int j = 0; j < 2; j++)
				{
					if (return_card != 0x00)
					{
						m_SequenceCount[iNum] -= 1;
						iResultCard[iResultCardCount++] = return_card;
						return 2;
					}
				}
			}
			iNum++;
		}
	}
	return -1;
}
//提起单牌 bCanSplite:是否拆牌找 
int CGameLogic::GetSingleCard(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE byStartValue, bool bCanSplite)
{
	for (int i = 0; i < m_single_count; i++)
	{
		if (m_single_info[i] > byStartValue)
		{
			BYTE return_card = GetRealCardValue(iHandCard, iHandCardCount, m_single_info[i]);
			if (return_card != 0x00)
			{
				iResultCard[iResultCardCount++] = return_card;
			}
			return 0;
		}
	}
	
	if (bCanSplite)
	{
		//拆对子
		for (int i = 0; i < m_double_count; i++)
		{
			int j = 0;
			if (m_double_info[i] > byStartValue)
			{
				while (j < 4)
				{
					if ((m_SequenceCount[j] & 0xF0) == 0x10)
					{
						BYTE byGetCard = 0x00;
						//拆了对子，剩下的单张能不能添加到已知单顺当中
						if (m_double_info[i] == (m_SequenceListStart[j] - 1))
						{
							byGetCard = m_double_info[i];
							//将拆开的单牌添加到顺子当中
							m_SequenceListStart[j] = m_double_info[i];
							m_SequenceCount[j] += 1;
							//删除掉对子信息
							int k = i;
							while (k < 16)
							{
								m_double_info[k] = m_double_info[k + 1];
								m_double_info[k + 1] = 0x00;
								k++;
							}
							m_double_count -= 1;
						}
						else if (m_double_info[i] == (m_SequenceListStart[j] + (m_SequenceCount[j] & 0x0F)))
						{
							byGetCard = m_double_info[i];
							//将拆开的单牌添加到顺子当中
							m_SequenceCount[j] += 1;
							//删除掉对子信息
							int k = i;
							while (k < 16)
							{
								m_double_info[k] = m_double_info[k + 1];
								m_double_info[k + 1] = 0x00;
								k++;
							}
							m_double_count -= 1;
						}
						BYTE return_card = GetRealCardValue(iHandCard, iHandCardCount, byGetCard);
						if (return_card != 0x00)
						{
							iResultCard[iResultCardCount++] = return_card;
							return 1;
						}
					}
					j++;
				}
			}
		}
		//找顺子中能不能拿出来一张
		int iNum = 0;
		while (iNum < 4)
		{
			//超过五张，查找最大的那一张能否提出
			BYTE byMaxCardBulk = (m_SequenceListStart[iNum] + m_SequenceCount[iNum] - 1);
			if (((m_SequenceCount[iNum] & 0x0F) > 0x05) && (byMaxCardBulk > byStartValue) && ((m_SequenceCount[iNum] & 0xF0) == 0x10))
			{
				BYTE return_card = GetRealCardValue(iHandCard, iHandCardCount, byMaxCardBulk);
				if (return_card != 0x00)
				{
					m_SequenceCount[iNum] -= 1;
					iResultCard[iResultCardCount++] = return_card;
					return 2;
				}
			}
			iNum++;
		}
		//拆三条
		for (int i = 0; i < m_triple_count; i++)
		{
			if (m_triple_info[i] > byStartValue)
			{
				int j = 0;
				while (j < 4)
				{
					//找双顺
					if ((m_SequenceCount[j] & 0xF0) == 0x20)
					{
						BYTE byGetCard = 0x00;
						//拆了三条，剩下的对子能不能添加到已知双顺当中
						if ((m_triple_info[i] == (m_SequenceListStart[j] - 1)))
						{
							//将拆开的对子添加到双顺当中
							m_SequenceListStart[j] = m_triple_info[i];
							m_SequenceCount[j] += 1;
							//删除掉三条信息
							int k = i;
							while (k < 16)
							{
								m_triple_info[k] = m_triple_info[k + 1];
								m_triple_info[k + 1] = 0x00;
								k++;
							}
							m_triple_count -= 1;
							byGetCard = m_triple_info[i];
						}
						else if (m_triple_info[i] == (m_SequenceListStart[j] + (m_SequenceCount[j] & 0x0F)))
						{
							//将拆开的对子添加到双顺当中
							m_SequenceCount[j] += 1;
							//删除掉三条信息
							int k = i;
							while (k < 16)
							{
								m_triple_info[k] = m_triple_info[k + 1];
								m_triple_info[k + 1] = 0x00;
								k++;
							}
							m_triple_count -= 1;
							byGetCard = m_triple_info[i];
						}
						BYTE return_card = GetRealCardValue(iHandCard, iHandCardCount, byGetCard);
						if (return_card != 0x00)
						{
							iResultCard[iResultCardCount++] = return_card;
							return 1;
						}
					}
					j++;
				}
			}
		}

		//找三张或者对子中的A或者2
		if (m_triple_count > 0)
		{
			for (int i = 0; i < m_triple_count; i++)
			{
				if (m_triple_info[i] > 13 && m_triple_info[i] > byStartValue)
				{//从对A开始找
					iResultCard[iResultCardCount++] = GetRealCardValue(iHandCard, iHandCardCount, m_triple_info[i]);
					return 1;
				}
			}
		}

		if (m_double_count > 0)
		{
			for (int i = 0; i < m_double_count; i++)
			{
				if (m_double_info[i] > 13 && m_double_info[i] > byStartValue)
				{//从对A开始找
					iResultCard[iResultCardCount++] = GetRealCardValue(iHandCard, iHandCardCount, m_double_info[i]);
					return 1;
				}
			}
		}
	}
	return -1;
}
bool CGameLogic::CanOutAllCardOnce(BYTE byCardList[], BYTE byCardCount, 
									BYTE byOutCardList[], int &byOutCardCount, 
									CRecordTool Uplast_card, CRecordTool Downlast_card)
{
	AnalyzeHandCard(byCardList, byCardCount, false);
	BYTE backup_cardlist[20] = { 0 };
	BYTE backup_CardCount = byCardCount;
	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
	//手数统计
	int hand_count = 0;

	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	UINT up_card_count = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	UINT down_card_count = Downlast_card.GetAllCardIndex(down_card_index);


	//余牌统计
	UINT max_boom_count = 0;
	BYTE last_max_boom = 0x00;//最大的炸弹
	int last_boom_count = 0;
	if (1 == up_card_index[16] && 1 == up_card_index[17]
		|| 1 == down_card_index[16] && 1 == down_card_index[17])
	{
		last_max_boom = 0x10;
		last_boom_count++;
	}
	else
	{
		for (UINT i = 0; i < CARD_BULK_INDEX; i++)
		{
			if (up_card_index[i] == 4 || down_card_index[i] == 4)
			{//炸弹
				last_max_boom = i;
				last_boom_count++;
			}
		}
	}

	//如果只剩大王加一手牌,并且没有炸弹的情况,先出大王
	if (m_have_large_king)
	{
		BYTE iRemoveCard[20] = { 0x4f };
		RemoveCard(iRemoveCard, 1, backup_cardlist, backup_CardCount);
		backup_CardCount -= 1;

		if (GetCardShape(backup_cardlist, backup_CardCount) == UG_THREE_DOUBLE && !gameconfig.bTakePair)
		{
		}
		else if ((last_max_boom == 0x00) && (GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND))
		{
			byOutCardList[0] = 0x4f;
			byOutCardCount = 1;
			return true;
		}
		//还原数据
		memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
		byOutCardCount = 0;
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
	}
	//或有小王家一手牌,没有大王和炸弹的情况
	if (m_have_little_king && (up_card_index[17] == 0 && down_card_index[17] == 0))
	{
		BYTE iRemoveCard[20] = { 0x4e };
		RemoveCard(iRemoveCard, 1, backup_cardlist, backup_CardCount);
		backup_CardCount -= 1;

		if ((last_max_boom == 0x00) 
			&&((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			|| (GetCardShape(backup_cardlist, backup_CardCount) == UG_THREE_DOUBLE && gameconfig.bTakePair)))
		{
			byOutCardList[0] = 0x4e;
			byOutCardCount = 1;
			return true;
		}
		//还原数据
		memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
		byOutCardCount = 0;
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
	}

	//如果手中只剩王炸家一手牌，直接王炸
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//要处理的数组
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//是否一手能出完,并且余牌中没有任何炸弹
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Uplast_card.IsHaveBoom() && 0 == Downlast_card.IsHaveBoom())
		{
			memcpy(byOutCardList, byReslult, sizeof(byReslult));
			byOutCardCount = iReslultcount;
			return true;
		}
		else
		{
			//还原数据
			memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
			byOutCardCount = 0;
			memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
			backup_CardCount = byCardCount;
			AnalyzeHandCard(byCardList, byCardCount, false);
		}
	}

	//如果只剩一手牌,就直接出完(四带二特殊处理)
	if (GetCardShape(byCardList, byCardCount) != UG_ERROR_KIND)
	{
		if (GetCardShape(byCardList, byCardCount) == UG_FOUR_TWO
			|| GetCardShape(byCardList, byCardCount) == UG_FOUR_TWO_DOUBLE)
		{//四带两单(对)，不直接出，先出一个单张(对)
			//判断炸弹是不是最大的,如果是就分开出
			if (mGetCardBulk(m_boom_info[0], false) > last_max_boom
				&& ((m_single_count > 0 && up_card_count>1 && down_card_count>1)
					|| (m_double_count > 0 && up_card_count>2 && down_card_count>2)))
			{
				if (m_single_count > 0)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
					byOutCardList[0] = return_card;
					byOutCardCount = 1;
				}
				if (m_double_count > 0)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
					byOutCardList[0] = return_card;
					return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
					byOutCardList[1] = return_card;
					byOutCardCount = 2;
				}
				return true;
			}
			else
			{
				memcpy(byOutCardList, byCardList, byCardCount);
				byOutCardCount = byCardCount;
				return true;
			}
		}
		else
		{
			memcpy(byOutCardList, byCardList, byCardCount);
			byOutCardCount = byCardCount;
			return true;
		}
	}

	//如果只剩炸弹加一手牌,并且炸弹是最大的或者只有一个炸弹比自己的大(并且不是王炸)
	for (int i = 0; i < m_boom_count; i++)
	{
		BYTE return_card[4] = {0};
		for (int j = 0; j < 4; j++)
		{
			return_card[j] = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[i]);
		}
	}
	if (GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND
		&& GetCardShape(backup_cardlist, backup_CardCount) != UG_FOUR_TWO
		&& GetCardShape(backup_cardlist, backup_CardCount) != UG_FOUR_TWO_DOUBLE)
	{
		//还原数据
		memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
		byOutCardCount = 0;
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		if (m_boom_info[m_boom_count - 1] > last_max_boom)
		{
			for (int j = 0; j < 4; j++)
			{
				byOutCardList[byOutCardCount++] = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[0]);
			}
			return true;
		}
		else if (last_max_boom != 0x10 && last_boom_count < 2)
		{
			for (int j = 0; j < 4; j++)
			{
				byOutCardList[byOutCardCount++] = GetRealCardValue(backup_cardlist, backup_CardCount, m_boom_info[0]);
			}
			return true;
		}
	}
	//还原数据
	memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
	byOutCardCount = 0;
	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
	backup_CardCount = byCardCount;


	for (int i = 0; i < m_boom_count; i++)
	{
		if (m_boom_info[i] > last_max_boom)
		{
			hand_count--;
			max_boom_count++;
		}
		else
		{
			hand_count++;
		}
	}

	//需要先组合手牌
	UINT Ssin_count = 0;//单顺个数
	UINT Sdui_count = 0;//连对个数
	UINT Stri_count = 0;//飞机个数
	UINT take_count[5] = { 0 };//可带的种类个数---(可带两个单/对子) 三带1最多有5个 4*5 = 20
	UINT countfortake = 0;		//take_count的计数

	//统计顺子类型
	for (int i = 0; i < 4; i++)
	{
		BYTE t_type = m_SequenceCount[i] & 0xF0;
		BYTE t_count = m_SequenceCount[i] & 0x0F;
		BYTE seq_count_t = ((t_type>>4) * t_count);

		if (t_count == 0x00)
		{
			break;
		}
		if (0x10 == t_type)
		{
			Ssin_count++;
		}
		else if (0x20 == t_type)
		{
			Sdui_count++;
		}
		else if (0x30 == t_type)
		{
			Stri_count++;
			take_count[countfortake++] = t_count;
		}

		//寻找剩余牌中是否有大的
		if (up_card_count < seq_count_t && down_card_count < seq_count_t)
		{//先判断其他玩家手牌中是否够张数(不考虑炸弹)不够直接continue
			continue;
		}

		BYTE tt_count = 0;
		int tt_start_index = mGetCardBulk(m_SequenceListStart[i], false) + 1;
		bool find_big = false;
		//查找上家
		while (tt_start_index + tt_count <= 14)
		{//大于等于2就不找了,不能组成顺子
			if (up_card_index[tt_start_index + tt_count] >= (t_type >> 4))
			{
				tt_count++;
				if (tt_count == t_count)
				{//找到可以压住的牌
					hand_count++;
					find_big = true;
					break;
				}
			}
			else
			{//中断顺子,从下一个继续找
				tt_start_index = tt_start_index + tt_count + 1;
				tt_count = 0;
			}
		}
		//查找下家
		while (tt_start_index + tt_count <= 14 && find_big)
		{//大于等于2就不找了,不能组成顺子
			if (down_card_index[tt_start_index + tt_count] >= (t_type >> 4))
			{
				tt_count++;
				if (tt_count == t_count)
				{//找到可以压住的牌
					hand_count++;
					break;
				}
			}
			else
			{//中断顺子,从下一个继续找
				tt_start_index = tt_start_index + tt_count + 1;
				tt_count = 0;
			}
		}
	}

	UNIT triple_count = 0;
	//三带
	for (int i = 0; i < m_triple_count; i++)
	{
		for (size_t j = m_triple_info[i]; j < 16; j++)
		{//余牌中2以下找
			if (up_card_index[j + 1] >= 3 || down_card_index[j + 1] >= 3)
			{
				hand_count++;
				triple_count++;
				break;
			}
		}
		//可以带
		take_count[countfortake++] = 1;
	}

	UNIT double_count = 0;
	//对子
	for (int i = 0; i < m_double_count; i++)
	{
		for (size_t j = m_double_info[i]; j < 16; j++)
		{//余牌中2下找
			if (up_card_index[j + 1] >= 2 || down_card_index[j + 1] >= 2)
			{
				hand_count++;
				double_count++;
				break;
			}
		}
	}

	UNIT single_count = 0;
	//单
	for (int i = 0; i < m_single_count; i++)
	{
		for (size_t j = m_single_info[i]; j < 18; j++)
		{//余牌中大王以下找
			if (j >= 17)//大王不考虑炸弹最大
			{
				break;
			}
			else
			{
				if (up_card_index[j + 1] >= 1 || down_card_index[j + 1] >= 1)
				{
					hand_count++;
					single_count++;
					break;
				}
			}
		}
	}

	for (UINT i = 0; i < countfortake; i++)
	{
		//先用单牌找
		if (take_count[i] <= single_count && single_count > 0)
		{
			hand_count -= take_count[i];
			single_count -= take_count[i];
			continue;
		}
		//再用对子找
		if (take_count[i] <= double_count && double_count > 0)
		{
			hand_count -= take_count[i];
			double_count -= take_count[i];
			continue;
		}
	}

	//获取最大牌型个数,没有要的起的牌型不算手数,剩余的算手数,手中别人要不起的炸弹要减一手
	if (m_have_large_king && m_have_little_king)
	{
		hand_count--;
		if (m_SequenceCount[0] == 0 && m_triple_count == 0 && m_double_count == 0)
		{//如果只剩下王和单的情况下,王炸可以拆开算两个最大
			hand_count--;
		}
	}
	else if (m_have_large_king )
	{
		if (last_max_boom == 0x00)
		{
			hand_count--;
			single_count--;
		}
	}
	else if (m_have_little_king)
	{
		//小王
		if (up_card_index[17] == 0 && down_card_index[17] == 0 && last_max_boom == 0x00)
		{
			hand_count--;
			if (single_count > 0)
			{
				single_count--;
			}
			else if (double_count > 0)
			{
				double_count--;
			}
			else if (triple_count > 0)
			{
				triple_count--;
			}
		}
		else
		{
			hand_count++;
		}
	}

	if (hand_count > 1)
	{//手数大于1,不能一手出完
		memset(byOutCardList, 0, 20 * sizeof(BYTE));
		byOutCardCount = 0;
		return false;
	}


	int t_double_count = m_double_count;
	int t_single_count = m_single_count;
	int t_triple_count = m_triple_count;
	BYTE t_double_info[18] = { 0x00 };
	BYTE t_single_info[18] = { 0x00 };
	BYTE t_triple_info[18] = { 0x00 };
	memcpy(t_double_info, m_double_info, sizeof(m_double_info));
	memcpy(t_single_info, m_single_info, sizeof(m_single_info));
	memcpy(t_triple_info, m_triple_info, sizeof(m_triple_info));
	//如果剩余一手,剔除这一手最后一手出
	if (hand_count == 1)
	{
		if (double_count == 1 && m_double_count > 0)
		{
			//如果对子有最大的可以管,并且有单张,就先出单张(当上下家报单的时候出对)
			if ((m_double_count > 1 || (up_card_count == 1 || down_card_count == 1)) && single_count == 1)
			{
				for (int i = 1; i < m_single_count; i++)
				{
					t_single_info[i - 1] = m_single_info[i];
				}
				t_single_count--;
			}
			else
			{
				for (int i = 1; i < m_double_count; i++)
				{
					t_double_info[i - 1] = m_double_info[i];
				}
				t_double_count--;
			}			
		}
		if (single_count == 1 && m_single_count > 1)
		{
			for (int i = 1; i < m_single_count; i++)
			{
				t_single_info[i - 1] = m_single_info[i];
			}
			t_single_count--;
		}
		if (triple_count == 1 && t_triple_count > 0)
		{
			for (int i = 1; i < t_triple_count; i++)
			{
				t_triple_info[i - 1] = m_triple_info[i];
			}
			t_triple_count--;
		}
	}


	//寻找出牌
	//提取张数较多的牌型出牌--------------------------------------------------------------------------------
	BYTE seq_count = 0x00;
	int imax_pos = 255;
	BYTE type = 0x00;
	BYTE count = 0x00;
	for (int i = 0; i < 4; i++)
	{
		BYTE t_type = (m_SequenceCount[i] & 0xF0) >> 4;
		BYTE t_count = m_SequenceCount[i] & 0x0F;
		BYTE seq_count_t = (t_type * t_count);
		if (t_type == 0x03)
		{
			seq_count_t += count * 0x01;//飞机可以带
		}
		if (seq_count < seq_count_t)
		{
			type = t_type;
			count = t_count;
			seq_count = seq_count_t;
			imax_pos = i;
		}
	}

	if (type == 0x03)
	{
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		memset(byOutCardList, 0, 20);
		byOutCardCount = 0;
		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[imax_pos] + i));
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
		if (m_single_count >= count)//带单
		{
			for (int i = 0; i < count; i++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[i]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
		else if (m_double_count >= count)//带双
		{
			for (int i = 0; i < count; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[i]);
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
		}
		else if (m_single_count == 1 && m_double_count == 1 && count == 2)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
			return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		return true;
	}
	else if (type == 0x02 || type == 0x01)//单双顺
	{
		for (int i = 0; i < count; i++)
		{
			for (int j = 0; j < type; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[imax_pos] + i));
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
		return true;
	}

	//三带,m_total_outhand_count == 2 表示只剩下三带加一手牌
	if (t_triple_count > 0 && (triple_count == 0 || t_double_count == 0 || t_single_count == 0 || m_total_outhand_count == 2))
	{
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		memset(byOutCardList, 0, 20);
		byOutCardCount = 0;
		for (int j = 0; j < 3; j++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_triple_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}

		//判断带单还是带对子,谁小带谁
		bool three_take_single = true;
		if (m_single_count > 0 && gameconfig.bTakePair && m_double_count > 0)
		{
			//如果单大于对子,带对子
			if (m_single_info[0] > m_double_info[0] && (up_card_count > 1 || up_card_count == 0) && (down_card_count > 1 || down_card_count == 0))
			{
				three_take_single = false;
			}
		}

		if (m_single_count == 0)
		{
			three_take_single = false;
		}

		if (three_take_single)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		else
		{
			for (int j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}

		if (mGetCardBulk(byOutCardList[0], false) == 15 && backup_CardCount > 0 && Uplast_card.IsHaveBoom() > 0 && Downlast_card.IsHaveBoom() > 0)
		{
			//出国出完这些牌,只剩一对或者一张单的话就出
			if ((backup_CardCount == 1) || (backup_CardCount == 2 && backup_cardlist[0] == backup_cardlist[1]))
			{
				return true;
			}
			if (backup_CardCount == 2 && m_have_large_king && m_have_little_king)
			{
				return true;
			}
			//还原数据
			memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
			byOutCardCount = 0;
			memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
			backup_CardCount = byCardCount;
		}
		else
		{
			return true;
		}
	}

	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
	backup_CardCount = byCardCount;
	memset(byOutCardList, 0, 20);
	byOutCardCount = 0;

	//如果只剩大王加一对,直接出大王
	if (m_have_large_king && m_double_count == 1 && byCardCount == 3)
	{
		byOutCardList[byOutCardCount++] = 0x4f;
		return true;
	}

	if ((t_double_count > 0 && double_count <= 0 && t_triple_count == 0) || t_single_count == 0)
	{
		for (int j = 0; j < 2; j++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, t_double_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		return true;
	}

	if (t_single_count > 0)
	{
		BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, t_single_info[0]);
		if (return_card != 0x00)
		{
			byOutCardList[0] = return_card;
			byOutCardCount = 1;
		}
		return true;
	}

	return false;
}

//寻找手中的大单牌(用做下家地主一张,且上家出了一张单的情况)
BYTE CGameLogic::FindMaxSingleInHand(BYTE byCardList[], BYTE byCardCount, CRecordTool last_card)
{
	BYTE maxcard_inhand = 0x00;
	BYTE last_max_card = 0x00;

	BYTE handcard_index[CARD_BULK_INDEX] = { 0 };
	for (UINT i = 0; i < byCardCount; i++)
	{
		handcard_index[mGetCardBulk(byCardList[i], false)]++;
	}

	BYTE lastcard_index[CARD_BULK_INDEX] = { 0 };
	UINT lastcard_count = last_card.GetAllCardIndex(lastcard_index);
	
	for (INT i = CARD_BULK_INDEX - 1; i >= 0; i--)
	{
		if (lastcard_index[i] == 4)
		{//炸弹
			last_max_card = i;
			break;
		}
	}
	
	for (int i = 0; i < m_single_count; i++)
	{
		if (m_single_info[i] >= last_max_card)
		{
			return m_single_info[i];
		}
	}

	if (m_have_large_king && !m_have_little_king)
	{
		return 17;
	}

	if (!m_have_large_king && m_have_little_king)
	{
		return 16;
	}

	if (m_have_large_king && m_have_little_king && last_max_card == 15)
	{
		return 16;
	}

	if (handcard_index[15] > 0)
	{//2
		return 15;
	}

	return 0x00;
}

//保存手牌分析数据
bool CGameLogic::SaveAnalysisResult(BYTE byCardList[], BYTE byCardCount, Analysis_Result &result)
{
	//转换牌值
	BYTE temp[18] = { 0 };
	for (int i = 0; i < byCardCount; i++)
	{
		temp[GetCardBulk(byCardList[i], false)]++;
	}
	result.total_outhand_count = m_total_outhand_count;
	result.real_single_count = m_real_single_count;
	result.have_large_king = m_have_large_king;
	result.have_little_king = m_have_little_king;
	result.boom_count = m_boom_count;
	result.single_count = m_single_count;
	result.double_count = m_double_count;
	result.triple_count = m_triple_count;
	memcpy(result.boom_info, m_boom_info, sizeof(m_boom_info));
	memcpy(result.single_info, m_boom_info, sizeof(m_boom_info));
	memcpy(result.double_info, m_boom_info, sizeof(m_boom_info));
	memcpy(result.triple_info, m_boom_info, sizeof(m_boom_info));
	memcpy(result.SequenceListStart, m_SequenceListStart, sizeof(m_SequenceListStart));
	memcpy(result.SequenceCount, m_SequenceCount, sizeof(m_SequenceCount));
	for (int i = 0; i < 4; i++)
	{
		if (m_SequenceCount[i] > 0x00)
		{
			result.iSeqCount++;
		}
	}
	for (int i = m_single_count - m_real_single_count; i < m_single_count; i++)
	{
		if (m_single_info[i] < 14)
		{
			result.iAloneSingle++;
			if (temp[15] > 0)
			{
				temp[15]--;
				result.iAloneSingle--;
			}
		}
	}
	if (m_have_large_king)result.iAloneSingle--;
	if (m_have_little_king)result.iAloneSingle--;
	for (int i = 0; i < m_double_count; i++)
	{
		if (m_double_info[i] <= 10)
		{
			result.iAloneDouble++;
			if (temp[15] > 1)
			{
				temp[15] -= 2;
				result.iAloneDouble--;
			}
		}
		else if (m_double_info[i] < 15)
		{
			result.iAloneDouble--;
		}
	}
	return true;
}

//寻找先手出牌类型
BYTE CGameLogic::FindOutCardType(Analysis_Result result, BYTE PreHandCount, BYTE PostHandCount, COperateRecord out_record, 
									shun_info &shun_type, CRecordTool Uplast_card, CRecordTool Downlast_card)
{
	BYTE out_type = UG_ERROR_KIND;

	/*BYTE lastcard_index[CARD_BULK_INDEX] = { 0 };
	UINT lastcard_count = last_card.GetAllCardIndex(lastcard_index);*/

	//提取张数较多的顺子牌型
	int seq_count = 0;// 顺子的个数
	for (int i = 0; i < 4; i++)
	{
		if (m_SequenceCount[i]>0x00)
		{
			seq_count++;
			BYTE t_type = (m_SequenceCount[i] & 0xF0) >> 4;
			BYTE t_count = m_SequenceCount[i] & 0x0F;
			BYTE seq_count_t = (t_type * t_count);
			if (t_type == 0x03)
			{
				seq_count_t += t_count * 0x01;//飞机可以带单或者对子
			}
			if (shun_type.seq_count < seq_count_t 
				|| (shun_type.seq_count == seq_count_t 
					&& shun_type.type == t_type 
					&& shun_type.seq_end >(m_SequenceListStart[i] + t_count - 0x01)))
			{
				shun_type.type = t_type;
				shun_type.count = t_count;
				shun_type.seq_count = seq_count_t;
				shun_type.imax_pos = i;
				shun_type.seq_end = m_SequenceListStart[i] + t_count - 0x01;
			}
		}
	}

	//顺子取多的牌型先出
	if (shun_type.seq_count > 0)
	{
		if (shun_type.type == 0x03 || shun_type.type == 0x02)
		{//双顺或者飞机就直接出
			out_type = UG_STRAIGHT;
			return out_type;
		}
		else
		{//单顺
			if (shun_type.seq_end < 0x0A)//小于10的顺子
			{	
				//如果单顺个数超过六张,或者双顺,三顺就直接出
				if ((m_SequenceCount[0] & 0xF0) == 0x30 
					|| (m_SequenceCount[0] & 0xF0) == 0x20 
					|| ((m_SequenceCount[0] & 0xF0) == 0x10
						&& (m_SequenceCount[0] & 0x0F) > 0x06))
				{
					out_type = UG_STRAIGHT;
					return out_type;
				}

				//如果只剩顺子只有这一个,就直接出
				if (m_SequenceCount[1] == 0x00)
				{
					out_type = UG_STRAIGHT;
					return out_type;
				}
				//判断有没有的管
				for (int i = 0; i < 4; i++)
				{
					BYTE other_type = (m_SequenceCount[i] & 0xF0) >> 4;
					BYTE other_count = m_SequenceCount[i] & 0x0F;
					if (other_type == shun_type.type //相同类型
						&& (other_count == shun_type.count || other_count == shun_type.count + 1)//张数相同或者多一张的都算
						&& (m_SequenceListStart[i] + other_count - 0x01) >(shun_type.seq_end + 0x01)) // 并且最大张比这个顺子大两个
					{//出这个顺子
						out_type = UG_STRAIGHT;
						return out_type;
					}
				}
				//判断余牌中有没有管的上的顺子
				if ((!Uplast_card.IsHaveBigShun((shun_type.seq_end - shun_type.count + 1), shun_type.count))
					&& (!Downlast_card.IsHaveBigShun((shun_type.seq_end - shun_type.count + 1), shun_type.count)))
				{
					out_type = UG_STRAIGHT;
					return out_type;
				}
				//如果除了顺子没有单对三张,还是出顺
				if (m_single_count == 0 && m_double_count == 0 && m_triple_count == 0)
				{
					out_type = UG_STRAIGHT;
					return out_type;
				}
			}
			else
			{
				out_type = UG_STRAIGHT;
				return out_type;
			}
		}
	}

	//三带
	if (m_triple_count > 0)
	{
		//如果出完三带就只剩下一对或者一张,也可以直接出
		if (m_triple_count == 1 && (m_double_count + m_single_count == 2))
		{
			out_type = UG_THREE;
			shun_type.seq_count = 4;
		}

		if (m_triple_count == 1 && m_triple_info[0] > 13 && m_double_count <= 1 && m_single_count <= 1)
		{
			if (m_single_count == 1 && m_single_info[0] < 10)
			{
				out_type = UG_THREE;
				shun_type.seq_count = 4;
			}
		}
		else
		{
			//三带小于10 并且有大的三带管可以出
			if (m_triple_info[0] < 10)
			{
				for (int i = 1; i < m_triple_count; i++)
				{
					if (m_triple_info[i] > m_triple_info[0] + 2)
					{
						out_type = UG_THREE;
						shun_type.seq_count = 4;
					}
				}
			}
			else if (m_double_count <= 1 && m_single_count <= 1)
			{//大于直接出
				out_type = UG_THREE;
				shun_type.seq_count = 4;
			}
		}

		if (m_real_single_count == 0 && m_triple_count >= m_double_count + m_single_count)
		{
			out_type = UG_THREE;
			shun_type.seq_count = 4;
		}

		//三带的第一张小于对子或者单张的第二张就直接出
		if ((m_triple_info[0] < m_single_info[1] && m_single_count > 1) || (m_triple_info[0] < m_double_info[1] && m_double_count > 1))
		{
			out_type = UG_THREE;
			shun_type.seq_count = 4;
		}
	}

	return out_type;
}

//寻找手中在余牌中的最大牌(不是手中最大),只能找单和对子
BYTE CGameLogic::FindMaxCardInLast(BYTE byCardList[], BYTE byCardCount, BYTE mix_value, CRecordTool Uplast_card, CRecordTool Downlast_card, UINT out_count)
{
	BYTE backup_cardlist[20] = { 0 };
	BYTE backup_CardCount = byCardCount;
	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
	   
	BYTE max_card = 0x00;
	BYTE max_up = Uplast_card.GetLastMaxCard(out_count);
	BYTE max_down = Downlast_card.GetLastMaxCard(out_count);

	if (max_up >= max_down)
	{
		max_card = max_up;
	}
	else
	{
		max_card = max_down;
	}

	if (max_card < mix_value)
	{
		max_card = mix_value;
	}

	if (out_count == 1)
	{ 
		for (int i = 0; i < m_single_count; i++)
		{
			if (m_single_info[i] >= max_card)
			{
				return m_single_info[i];
			}
		}
	}


	for (int i = 0; i < m_double_count; i++)
	{
		if (m_double_info[i] >= max_card)
		{
			return m_double_info[i];
		}
	}

	if (out_count == 1)
	{
		if (m_have_little_king && !m_have_large_king)
		{
			return 16;
		}

		if (m_have_large_king && !m_have_little_king)
		{
			return 17;
		}

		for (int i = 0; i < m_triple_count; i++)
		{
			if (m_triple_info[i] >= max_card)
			{
				return m_triple_info[i];
			}
		}

		if (m_have_little_king)
		{
			return 16;
		}

		if (m_have_large_king)
		{
			return 17;
		}
	}

	return 0x00;
}

//寻找应该出的牌
bool CGameLogic::FindOutCard(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount,
								CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record, BYTE NTpos)
{
	//备份
	BYTE t_UserCard[HAND_CARD_MAX] = { 0 };
	BYTE t_UserCardCount = 0;
	BYTE t_UpUserCard[HAND_CARD_MAX] = { 0 };
	BYTE t_UpUserCardCount = 0;
	BYTE t_DownUserCard[HAND_CARD_MAX] = { 0 };
	BYTE t_DownUserCardCount = 0;

	memcpy(t_UserCard, byCardList, sizeof(t_UserCard));
	t_UserCardCount = byCardCount;
	t_UpUserCardCount = Uplast_card.GetAllCard(t_UpUserCard);
	t_DownUserCardCount = Uplast_card.GetAllCard(t_DownUserCard);

	//判断上下家标志位
	bool Up_flag = (NTpos != 2 ? true : false);
	bool Down_flag = (NTpos != 1 ? true : false);

	//6张起开始判断
	if ((t_UpUserCardCount >= 6 && Up_flag) || (t_DownUserCardCount >= 6 && Down_flag))
	{
		return false;
	}

	//分析玩家手牌
	Analysis_Result UpAnalyResult;
	UpAnalyResult = AnalyzeHandCard(t_UpUserCard, t_UpUserCardCount, false, false);
	
	Analysis_Result DownAnalyResult;
	DownAnalyResult = AnalyzeHandCard(t_DownUserCard, t_DownUserCardCount, false, false);

	//寻找玩家没有的牌型或者牌型点数较小的牌型先出
	BYTE out_type = UG_ERROR_KIND;

	//循环找四个顺子
	for (UINT i = 0; i < 4; i++)
	{
		if (m_SequenceCount[i] != 0x00)
		{
			//6张以上的顺子并且对手没有顺子直接出(6张单顺,双顺和飞机)
			if ((((m_SequenceCount[i] & 0x0F) > 5
				|| (m_SequenceCount[i] & 0xF0) > 0x10))
				&& (Up_flag && UpAnalyResult.SequenceCount[0] == 0x00
					|| Down_flag && DownAnalyResult.SequenceCount[0] == 0x00))
			{//自己有顺子,对手没有,直接出顺子
				out_type = UG_STRAIGHT;
			}

			if (Up_flag && UpAnalyResult.SequenceCount[0] != 0x00)
			{
				int j = 0;
				out_type = UG_STRAIGHT;
				while (UpAnalyResult.SequenceCount[j++] != 0x00)
				{
					if (((m_SequenceCount[0] & 0xF0) == (UpAnalyResult.SequenceCount[j] & 0xF0)
						|| (m_SequenceCount[0] & 0x0F) == (UpAnalyResult.SequenceCount[j] & 0x0F))
							&& (m_SequenceListStart[i] < UpAnalyResult.SequenceListStart[j]))
					{//会被压制,不能出
						out_type = UG_ERROR_KIND;
						break;
					}
				}
			}

			if (Down_flag && DownAnalyResult.SequenceCount[0] != 0x00)
			{
				int j = 0;
				out_type = UG_STRAIGHT;
				while (DownAnalyResult.SequenceCount[j++] != 0x00)
				{
					if (((m_SequenceCount[0] & 0xF0) == (DownAnalyResult.SequenceCount[j] & 0xF0)
						|| (m_SequenceCount[0] & 0x0F) == (DownAnalyResult.SequenceCount[j] & 0x0F))
						&& (m_SequenceListStart[i] < DownAnalyResult.SequenceListStart[j]))
					{//会被压制,不能出
						out_type = UG_ERROR_KIND;
						break;
					}
				}
			}
			//可以出顺子就直接出顺子
			if (out_type == UG_STRAIGHT)
			{
				//---------------------------------------------------------------------------------
			}
		}
	}

	if (m_triple_count > 0 && out_type == UG_ERROR_KIND && m_triple_info[0] < 14)
	{
		out_type = UG_THREE;
		//判断对手能不能要的起
		if ((Up_flag && UpAnalyResult.triple_count > 0)
			|| (Down_flag && DownAnalyResult.triple_count > 0))
		{
			if (Up_flag && m_triple_info[0] < UpAnalyResult.triple_info[0])
			{//对手能要的起,
				//判断是否跟了就出完了
				out_type = UG_THREE_ONE;
				if (m_single_count > 0 && t_UpUserCardCount == 4)
				{
					out_type = UG_ERROR_KIND;
				}
				//判断自己能不能压
				if (m_triple_info[m_triple_count - 1] <= UpAnalyResult.triple_info[0])
				{
					out_type = UG_ERROR_KIND;
				}
				if (m_single_count == 0 && m_double_count > 0)
				{
					out_type = UG_THREE_DOUBLE;
					//判断自己能不能压
					if (m_triple_info[m_triple_count - 1] <= UpAnalyResult.triple_info[0] && UpAnalyResult.double_count == 1)
					{
						out_type = UG_ERROR_KIND;
					}
				}
			}
			if (Down_flag && m_triple_info[0] < DownAnalyResult.triple_info[0])
			{//对手能要的起,
				//判断是否跟了就出完了
				out_type = UG_THREE_ONE;
				if (m_single_count > 0 && t_DownUserCardCount == 4)
				{
					out_type = UG_ERROR_KIND;
				}
				//判断自己能不能压
				if (m_triple_info[m_triple_count - 1] <= DownAnalyResult.triple_info[0])
				{
					out_type = UG_ERROR_KIND;
				}
				if (m_single_count == 0 && m_double_count > 0)
				{
					out_type = UG_THREE_DOUBLE;
					//判断自己能不能压
					if (m_triple_info[m_triple_count - 1] <= DownAnalyResult.triple_info[0] && DownAnalyResult.double_count == 1)
					{
						out_type = UG_ERROR_KIND;
					}
				}
			}
		}
	}
	
	if (m_double_count > 0 && out_type == UG_ERROR_KIND && m_double_info[0] < 14)
	{
		out_type = UG_DOUBLE;
		//对子(用对子跟,拆三条跟)
		if (Up_flag && UpAnalyResult.double_count > 0)
		{
			//对手的对子大于自己的对子
			if (m_double_info[0] < UpAnalyResult.double_info[0])
			{
				for (int i = 1; i < m_double_count; i++)
				{
					//自己的对子大于对手的对子
					if (m_double_info[i] > UpAnalyResult.double_info[0])
					{
						out_type = UG_DOUBLE;
						//判断对手还有没有对子可以跟
						if (m_double_info[i] < UpAnalyResult.double_info[1])
						{
							for (int j = i + 1; j < m_double_count; j++)
							{
								if (m_double_info[j] < UpAnalyResult.double_info[1])
								{//要不起
									out_type = UG_ERROR_KIND;
								}
								else
								{//要的起
									out_type = UG_DOUBLE;
									break;
								}
							}
						}
						//判断对手拆三条可以跟
						if (UpAnalyResult.triple_count > 0)
						{
							if (m_double_info[i] < UpAnalyResult.triple_info[0])
							{//对手的对子大
								//自己的还能不能用对子跟
								if (m_double_info[m_double_count - 1] > UpAnalyResult.triple_info[0])
								{
									out_type = UG_DOUBLE;
									break;
								}
								else
								{//自己的对子管不起了,拆自己的三条
									if (m_triple_count > 0 && m_triple_info[m_triple_count - 1] > UpAnalyResult.triple_info[0])
									{
										out_type = UG_DOUBLE;
										break;
									}
									else
									{
										out_type = UG_ERROR_KIND;
										break;
									}
								}
							}
						}
					}
					else
					{
						out_type = UG_ERROR_KIND;
						//不考虑拆自己三条
					}
				}
			}
		}
		if (Down_flag && DownAnalyResult.double_count > 0)
		{
			//对手的对子大于自己的对子
			if (m_double_info[0] < DownAnalyResult.double_info[0])
			{
				for (int i = 1; i < m_double_count; i++)
				{
					//自己的对子大于对手的对子
					if (m_double_info[i] > DownAnalyResult.double_info[0])
					{
						out_type = UG_DOUBLE;
						//判断对手还有没有对子可以跟
						if (m_double_info[i] < DownAnalyResult.double_info[1])
						{
							for (int j = i + 1; j < m_double_count; j++)
							{
								if (m_double_info[j] < DownAnalyResult.double_info[1])
								{//要不起
									out_type = UG_ERROR_KIND;
								}
								else
								{//要的起
									out_type = UG_DOUBLE;
									break;
								}
							}
						}
						//判断对手拆三条可以跟
						if (DownAnalyResult.triple_count > 0)
						{
							if (m_double_info[i] < DownAnalyResult.triple_info[0])
							{//对手的对子大
								//自己的还能不能用对子跟
								if (m_double_info[m_double_count - 1] > DownAnalyResult.triple_info[0])
								{
									out_type = UG_DOUBLE;
									break;
								}
								else
								{//自己的对子管不起了,拆自己的三条
									if (m_triple_count > 0 && m_triple_info[m_triple_count - 1] > DownAnalyResult.triple_info[0])
									{
										out_type = UG_DOUBLE;
										break;
									}
									else
									{
										out_type = UG_ERROR_KIND;
										break;
									}
								}
							}
						}
					}
					else
					{
						out_type = UG_ERROR_KIND;
						//不考虑拆自己三条
					}
				}
			}
		}
	}

	if (m_single_count > 0 && out_type == UG_ERROR_KIND)
	{
		out_type = UG_ONLY_ONE;
		//单张(用单牌跟,拆三条,拆对子)
		BYTE temp[18] = { 0 };
		BYTE max_card = 0x00;
		for (int i = 0; i < t_UserCardCount; i++)
		{
			temp[GetCardBulk(t_UserCard[i], false)]++;
		}
		for (int i = 17; i > 0; i--)
		{
			if (temp[i] != 0)
			{
				max_card = i;
				break;
			}
		}
		if (Up_flag && UpAnalyResult.real_single_count > m_real_single_count)
		{
			if (max_card > Uplast_card.GetLastMaxCard())
			{
				out_type = UG_ONLY_ONE;
			}
			else
			{
				out_type = UG_ERROR_KIND;
			}
		}
		if (Down_flag && UpAnalyResult.real_single_count > m_real_single_count)
		{
			if (max_card > Uplast_card.GetLastMaxCard())
			{
				out_type = UG_ONLY_ONE;
			}
			else
			{
				out_type = UG_ERROR_KIND;
			}
		}
	}

	switch (out_type)
	{
	case UG_ERROR_KIND:
	{
		return false;
	}
		break;
	case UG_ONLY_ONE:
	{
		BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_single_info[0]);
		if (return_card != 0x00)
		{
			byOutCardList[byOutCardCount++] = return_card;
		}
	}
		break;
	case UG_DOUBLE:
	{
		for (int j = 0; j < 2; j++)
		{
			BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_double_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
	}
		break;
	case UG_THREE:
	case UG_THREE_ONE:
	{
		for (int j = 0; j < 3; j++)
		{
			BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_triple_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_single_info[0]);
		if (return_card != 0x00)
		{
			byOutCardList[byOutCardCount++] = return_card;
		}
	}
		break;
	case UG_THREE_DOUBLE:
	{
		for (int j = 0; j < 3; j++)
		{
			BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_triple_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		for (int j = 0; j < 2; j++)
		{
			BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_double_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
	}
		break;
	case UG_STRAIGHT:
	{
		BYTE shun_type = ((m_SequenceCount[0] & 0xF0) >> 4);
		BYTE shun_count = (m_SequenceCount[0] & 0x0F);
		if (shun_type == 0x03)
		{
			memcpy(t_UserCard, byCardList, sizeof(t_UserCard));
			t_UserCardCount = byCardCount;
			memset(byOutCardList, 0, 20*sizeof(BYTE));
			byOutCardCount = 0;
			for (int i = 0; i < shun_count; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_SequenceListStart[i]);
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
			///---------带单双-----------------------------
			if (shun_count == 0x03)
			{//三联以上
				//带最小的三张牌(三张单,两张单,一单一对)
				if ((m_single_count + m_double_count * 2 >= shun_count && m_single_count != 0)
					|| (m_single_count == 0 && m_double_count >= shun_count))
				{
					int single_index = 0;
					int double_index = 0;
					bool type_single = false;
			
					for (int i = 0; i < shun_count; i++)
					{
						if (m_single_info[single_index] < 15 && m_single_info[single_index] != 0x00 || (m_double_count < 3 && i == 0))
						{
							BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_single_info[single_index]);
							if (return_card != 0x00)
							{
								byOutCardList[byOutCardCount++] = return_card;
							}
							single_index++;
							type_single = true;
						}
						else
						{
							if ((shun_count - i > 1) && type_single)
							{
								//没有单直接用对子
								byOutCardList[--byOutCardCount] = 0x00;
							}
							BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_double_info[double_index]);
							if (return_card != 0x00)
							{
								byOutCardList[byOutCardCount++] = return_card;
							}
							return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_double_info[double_index]);
							if (return_card != 0x00)
							{
								byOutCardList[byOutCardCount++] = return_card;
							}
							double_index++;
							if (type_single)
							{
								i++;
							}
						}
					}
				}
			}
			else
			{//二连三顺
				shun_count = 0x02;//可能是超过三顺的拆分
				if (m_single_count >= shun_count)//带单
				{
					for (int i = 0; i < shun_count; i++)
					{
						BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_single_info[i]);
						if (return_card != 0x00)
						{
							byOutCardList[byOutCardCount++] = return_card;
						}
					}
				}
				else if (m_double_count >= shun_count)//带双
				{
					for (int i = 0; i < shun_count; i++)
					{
						for (int j = 0; j < 2; j++)
						{
							BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_double_info[i]);
							if (return_card != 0x00)
							{
								byOutCardList[byOutCardCount++] = return_card;
							}
						}
					}
				}
				else if (m_single_count == 1 && m_double_count == 1)
				{
					BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_single_info[0]);
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
					return_card = GetRealCardValue(t_UserCard, t_UserCardCount, m_double_info[0]);
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
		}
		else if (shun_type == 0x02 || shun_type == 0x01)//单双顺
		{
			for (int i = 0; i < shun_count; i++)
			{
				for (int j = 0; j < shun_type; j++)
				{
					BYTE return_card = GetRealCardValue(t_UserCard, t_UserCardCount, (m_SequenceListStart[i]));
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
		}
	}
		break;
	default:
		break;
	}

	return true;
}

//----------------------------出牌策略------------------------------------------


//先手出牌
void CGameLogic::FirstOutCard(Card_Info card_info, BYTE byOutCardList[], int &byOutCardCount, COperateRecord out_record)
{
	AnalyzeHandCard(card_info.UserCard, card_info.UserCardCount, false);

	memset(byOutCardList, 0, 20 * sizeof(BYTE));
	byOutCardCount = 0;

	CRecordTool Uplast_card;
	CRecordTool Downlast_card;
	

	if (0 == card_info.NTposition)
	{
		Uplast_card.SetLastCard(card_info.UpUserCard, card_info.UpUserCardCount);
		Downlast_card.SetLastCard(card_info.DownUserCard, card_info.DownUserCardCount);
		//分析是否能一次性出完
		if (!CanOutAllCardOnce(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card))
		{
			LandlordFirst(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record);
		}		
	}
	else if (1 == card_info.NTposition)
	{
		Uplast_card.SetLastCard(card_info.UpUserCard, card_info.UpUserCardCount);
		Downlast_card.SetLastCard(card_info.DownUserCard, card_info.DownUserCardCount);
		//分析是否能一次性出完
		if (!CanOutAllCardOnce(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card))
		{
			FamerFirstBefFM(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record);
		}
	}
	else if (2 == card_info.NTposition)
	{
		Uplast_card.SetLastCard(card_info.UpUserCard, card_info.UpUserCardCount);
		Downlast_card.SetLastCard(card_info.DownUserCard, card_info.DownUserCardCount);
		//分析是否能一次性出完
		if (!CanOutAllCardOnce(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card))
		{
			FamerFirstBefLD(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record);
		}
	}
	if (!CanOutCard(byOutCardList, byOutCardCount, byOutCardList, byOutCardCount, true))
	{
		printf("error byOutCardList[%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x],count =  %d"
			, byOutCardList[0], byOutCardList[1], byOutCardList[2], byOutCardList[3], byOutCardList[4]
			, byOutCardList[5], byOutCardList[6], byOutCardList[7], byOutCardList[8], byOutCardList[9]
			, byOutCardList[10], byOutCardList[11], byOutCardList[12], byOutCardList[13], byOutCardList[14]
			, byOutCardList[15], byOutCardList[16], byOutCardList[17], byOutCardList[18], byOutCardList[19], byOutCardCount);
		memset(byOutCardList, 0, (MAX_COUNT*sizeof(BYTE)));
		byOutCardCount = 0;
	}
	return;
}

//地主先手
void CGameLogic::LandlordFirst(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount, 
									CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);

	BYTE backup_cardlist[20] = { 0 };
	BYTE backup_CardCount = byCardCount;
	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));

	AnalyzeHandCard(byCardList, byCardCount, false);

	//寻找是否会导致对手一次出完()
	bool already_find = false;
	//already_find = m_DeskPositon(backup_cardlist, backup_CardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record, 0);

	if (already_find)
	{
		//寻找到出牌,直接出
		return;
	}

	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	//获取大于等于2的牌张数
	int temp_king_hand_count = 0;//手中王的手数
	if (m_have_little_king != m_have_large_king) temp_king_hand_count = 1;


	int bigcardcount = temp[15] + temp[16] + temp[17];
	if ((m_real_single_count > 0 && bigcardcount > 3)
		&& (PreHandCount > 1 && PostHandCount > 1))
	{
		if (m_single_info[0] < 14)//大于A先不出
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[0] = return_card;
				byOutCardCount = 1;
			}
			return;
		}
	}

	int out_type = UG_ERROR_KIND;

	//提取张数较多的顺子牌型
	BYTE seq_count = 0x00;
	shun_info shun_data;
	Analysis_Result analy_re;
	SaveAnalysisResult(backup_cardlist, backup_CardCount, analy_re);
	out_type = FindOutCardType(analy_re, PreHandCount, PostHandCount, out_record, shun_data, Uplast_card, Downlast_card);

	if (out_type == UG_ERROR_KIND)
	{
		bool inverted = false;
		if (seq_count < (m_double_count * 2))
		{
			//如果有一对A或对2并且单牌大于2张,就先出单牌
		if ((PreHandCount != 1 && PostHandCount != 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				out_type = UG_ONLY_ONE;
			}
		else if ((PreHandCount == 1 || PostHandCount == 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				inverted = true;//此时要逆序出牌
				out_type = UG_ONLY_ONE;
			}
			else
			{
				out_type = UG_DOUBLE;
				seq_count = m_double_count * 2;
			}
		}
		if ((PostHandCount != 1 && PreHandCount != 1) || (out_type == UG_ERROR_KIND))
		{
			if (seq_count < m_single_count)
			{
				out_type = UG_ONLY_ONE;
			}
			else if (seq_count == m_single_count && m_single_count == 2)
			{
				if (m_single_info[m_single_count - 1] == 15 || (m_have_large_king || m_have_little_king))
				{
					out_type = UG_ONLY_ONE;
				}
			}
		}
		if (PostHandCount == 1 || PreHandCount == 1)
		{
			if (m_double_count > 0)
			{
				out_type = UG_DOUBLE;
			}
			if (m_triple_count > 0)
			{
				out_type = UG_THREE;
			}
			if (m_SequenceListStart[0] > 0)
			{
				out_type = UG_STRAIGHT;
			}
		}
		//如果单个数较少并且对子是小于10的就先出对子
		if (m_real_single_count <= 2 && m_double_count > 0 && m_double_info[0] < 10)
		{
			out_type = UG_DOUBLE;
		}
	}

	BYTE last_out_type = out_record.GetLastOutType();
	//如果上次其他玩家都不要的牌型还有就继续出这种牌型
	if (last_out_type != UG_ERROR_KIND)
	{
		switch (last_out_type)
		{
		case UG_STRAIGHT:
		{
			if (m_SequenceCount[0] > 0)
			{
				out_type = UG_STRAIGHT;
			}
		}
			break;
		case UG_THREE:
		case UG_THREE_ONE:
		case UG_THREE_TWO:
		case UG_THREE_DOUBLE:
		{
			if (m_triple_count > 0 && m_triple_info[0] < 14)
			{//有且小于A
				out_type = UG_THREE;
			}
		}
			break;
		case UG_DOUBLE:
		{
			if (m_double_count > 0 && m_double_info[0] < 14 && PreHandCount != 2 && PostHandCount != 2)
			{
				out_type = UG_DOUBLE;
			}
		}
			break;
		case UG_ONLY_ONE:
		{
			if (m_real_single_count > 0 && m_single_info[0] < 15)
			{
				out_type = UG_ONLY_ONE;
			}
		}
			break;
		default:
			break;
		}
	}

	//如果还是没有
	if (out_type == UG_ERROR_KIND)
	{
		if (m_single_count > 0)
		{
			out_type = UG_ONLY_ONE;
		}
		else if (m_have_large_king)
		{
			byOutCardList[byOutCardCount++] = 0x4F;
			return;
		}
		else if (m_have_little_king)
		{
			byOutCardList[byOutCardCount++] = 0x4E;
			return;
		}
	}

	//根据出牌类型出牌
	switch (out_type)
	{
	case UG_STRAIGHT:
	{
		if (shun_data.type == 0x03)
		{
			memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
			backup_CardCount = byCardCount;
			memset(byOutCardList, 0, 20);
			byOutCardCount = 0;
			for (int i = 0; i < shun_data.count; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[shun_data.imax_pos] + i));
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
			if (m_single_count >= shun_data.count)//带单
			{
				for (int i = 0; i < shun_data.count; i++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[i]);
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
			else if (m_double_count >= shun_data.count && (m_triple_info[0] < 15 || ((m_total_outhand_count - m_boom_count - temp_king_hand_count) == 1)))//带双
			{
				for (int i = 0; i < shun_data.count; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[i]);
						if (return_card != 0x00)
						{
							byOutCardList[byOutCardCount++] = return_card;
						}
					}
				}
			}
			else if (m_single_count == 1 && m_double_count == 1 && shun_data.count == 2)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
				return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
		else if (shun_data.type == 0x02 || shun_data.type == 0x01)//单双顺
		{
			for (int i = 0; i < shun_data.count; i++)
			{
				for (int j = 0; j < shun_data.type; j++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[shun_data.imax_pos] + i));
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
		}
	}
		break;
	case UG_THREE:
	{
		//三带
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		memset(byOutCardList, 0, 20);
		byOutCardCount = 0;
		for (int j = 0; j < 3; j++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_triple_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}

		//判断带单还是带对子
		bool three_take_single = true;
		if (m_single_count > 0 && gameconfig.bTakePair && m_double_count > 0)
		{
			//如果对子大于单,带单
			if (m_single_info[0] > m_double_info[0])
			{
				three_take_single = false;
			}
		}

		if (m_single_count == 0)
		{
			three_take_single = false;
		}

		if (three_take_single)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		else
		{
			for (int j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
	}
		break;
	case UG_DOUBLE:
	{		
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;

		for (int j = 0; j < 2; j++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}

	}
		break;
	case UG_ONLY_ONE:
	{
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		memset(byOutCardList, 0, 20);
		byOutCardCount = 0;

		BYTE t_single_info = m_single_info[0];

		if (PreHandCount == 1 || PostHandCount == 1)
		{//对手只剩一张牌时
			t_single_info = m_single_info[m_single_count - 1];				//此时单排从最大的出
		}

		BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, t_single_info);
		if (return_card != 0x00)
		{
			byOutCardList[0] = return_card;
			byOutCardCount = 1;
		}
	}
		break;
	default:
	{
		//printf("error switch first out card!");
	}
		break;
	}

	return;
}

//农民先手(下家地主)
void CGameLogic::FamerFirstBefLD(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount,
									CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	CRecordTool backup_last_card;
	backup_last_card.SetLastCard(Uplast_card);
	Uplast_card.ClearAllData();

	BYTE backup_cardlist[20] = { 0 };
	BYTE backup_CardCount = byCardCount;
	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));

	//寻找是否会导致对手一次出完()
	bool already_find = false;
	//already_find = FindOutCard(backup_cardlist, backup_CardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record, 0);

	if (already_find)
	{
		//寻找到出牌,直接出
		return;
	}

	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	//获取大于等于2的牌张数
	int temp_king_hand_count = 0;//手中王的手数
	if (m_have_little_king != m_have_large_king) temp_king_hand_count = 1;

	//如果队友家只有一张就送下家走,并且地主不止一张
	if (PreHandCount == 1 && m_bFeedCard && PostHandCount > 1)
	{
		m_bFeedCard = false;
		if (m_single_count > 0)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[0] = return_card;
				byOutCardCount = 1;
			}
			return;
		}
	}

	int bigcardcount = temp[15] + temp[16] + temp[17];
	if ((m_real_single_count > 0 && bigcardcount > 3)
		&& (PreHandCount > 1 && PostHandCount > 1))
	{
		if (m_single_info[0] < 14)//大于A先不出
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[0] = return_card;
				byOutCardCount = 1;
			}
			return;
		}
	}

	int out_type = UG_ERROR_KIND;

	//提取张数较多的顺子牌型
	BYTE seq_count = 0x00;
	shun_info shun_data;
	Analysis_Result analy_re;
	SaveAnalysisResult(backup_cardlist, backup_CardCount, analy_re);
	out_type = FindOutCardType(analy_re, PreHandCount, PostHandCount, out_record, shun_data, Uplast_card, Downlast_card);

	if (out_type == UG_ERROR_KIND)
	{
		bool inverted = false;
		if (seq_count < (m_double_count * 2))
		{
			//如果有一对A或对2并且单牌大于2张,就先出单牌
			if ((PostHandCount != 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				out_type = UG_ONLY_ONE;
			}
			else if ((PostHandCount == 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				inverted = true;//此时要逆序出牌
				out_type = UG_ONLY_ONE;
			}
			else
			{
				out_type = UG_DOUBLE;
				seq_count = m_double_count * 2;
			}
		}
		if ((PostHandCount != 1) || (out_type == UG_ERROR_KIND))
		{
			if (seq_count < m_single_count)
			{
				out_type = UG_ONLY_ONE;
			}
			else if (seq_count == m_single_count && m_single_count == 2)
			{
				if (m_single_info[m_single_count - 1] == 15 || (m_have_large_king || m_have_little_king))
				{
					out_type = UG_ONLY_ONE;
				}
			}
		}		

		//如果单个数较少并且对子是小于10的就先出对子
		if (m_real_single_count <= 2 && m_double_count > 0 && m_double_info[0] < 10)
		{
			out_type = UG_DOUBLE;
		}

		//如果单个数已经超过大牌个数2个以上就先出其他牌型
		Analysis_Result sPreRS;
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPreRS);
		if (sPreRS.iAloneSingle > 2 && out_type == UG_ONLY_ONE)
		{
			if (m_double_info[0] < 13 && m_double_count > 0)
			{
				out_type = UG_DOUBLE;
			}
			else if (m_triple_info[0] < 13 && m_triple_count > 0)
			{
				out_type = UG_THREE;
			}
			else if (m_SequenceListStart[0] > 0x00)
			{
				out_type = UG_STRAIGHT;
			}
			//如果其他牌型都是大于K的牌型就继续出单
		}

		if (PostHandCount == 2 && m_single_count > 0)
		{//如果地主只剩两张,先出单
			out_type = UG_ONLY_ONE;
		}

		if (PostHandCount == 1)
		{
			if (m_double_count > 0)
			{
				out_type = UG_DOUBLE;
			}
			if (m_triple_count > 0)
			{
				out_type = UG_THREE;
			}
			if (m_SequenceListStart[0] > 0)
			{
				out_type = UG_STRAIGHT;
			}
		}
	}

	BYTE last_out_type = out_record.GetLastOutType();
	//如果上次其他玩家都不要的牌型还有就继续出这种牌型
	if (last_out_type != UG_ERROR_KIND)
	{
		switch (last_out_type)
		{
		case UG_STRAIGHT:
		{
			if (m_SequenceCount[0] > 0)
			{
				out_type = UG_STRAIGHT;
			}
		}
			break;
		case UG_THREE:
		case UG_THREE_ONE:
		case UG_THREE_TWO:
		case UG_THREE_DOUBLE:
		{
			if (m_triple_count > 0 && m_triple_info[0] < 14)
			{//有且小于A
				out_type = UG_THREE;
			}
		}
			break;
		case UG_DOUBLE:
		{
			if (m_double_count > 0 && m_double_info[0] < 14 && PostHandCount != 2)
			{
				out_type = UG_DOUBLE;
			}
		}
			break;
		case UG_ONLY_ONE:
		{
			if (m_real_single_count > 0 && m_single_info[0] < 15)
			{
				out_type = UG_ONLY_ONE;
			}
		}
			break;
		default:
			break;
		}
	}

	//如果上家队友只剩一手牌,并且手牌数小于2张的情况
	bool bSpliteCard = false;//是否拆牌
	if (PreHandCount == 2)
	{
		BYTE last_card[HAND_CARD_MAX] = { 0 };
		BYTE LastHandCount = backup_last_card.GetAllCard(last_card);
		
		if (last_card[0] != last_card[1])
		{//两张单
			if (backup_last_card.GetLastMaxCard() >= Downlast_card.GetLastMaxCard())
			{//队友有机会出完,倒叙拆牌出
				out_type = UG_ONLY_ONE;
				bSpliteCard = true;
			}
		}
		else
		{//对子
			if (m_double_count > 0 && m_double_info[m_double_count - 1] < last_card[0])
			{
				out_type = UG_DOUBLE;
			}
			else if (m_triple_count > 0 && m_triple_info[m_triple_count - 1] < last_card[0])
			{//队友有机会出完,倒叙拆牌出
				out_type = UG_DOUBLE;
				bSpliteCard = true;
			}
		}
	}
	else if (PreHandCount == 1)
	{
		BYTE last_card[HAND_CARD_MAX] = { 0 };
		BYTE LastHandCount = backup_last_card.GetAllCard(last_card);
		if (backup_last_card.GetLastMaxCard() >= Downlast_card.GetLastMaxCard())
		{//队友有机会出完,倒叙拆牌出
			out_type = UG_ONLY_ONE;
			bSpliteCard = true;
		}
	}


	//如果还是没有
	if (out_type == UG_ERROR_KIND)
	{
		if (m_single_count > 0)
		{
			out_type = UG_ONLY_ONE;
		}
		else if (m_have_large_king)
		{
			byOutCardList[byOutCardCount++] = 0x4F;
			return;
		}
		else if (m_have_little_king)
		{
			byOutCardList[byOutCardCount++] = 0x4E;
			return;
		}
	}


	//根据出牌类型出牌
	switch (out_type)
	{
	case UG_STRAIGHT:
	{
		if (shun_data.type == 0x03)
		{
			memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
			backup_CardCount = byCardCount;
			memset(byOutCardList, 0, 20);
			byOutCardCount = 0;
			for (int i = 0; i < shun_data.count; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[shun_data.imax_pos] + i));
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
			if (m_single_count >= shun_data.count)//带单
			{
				for (int i = 0; i < shun_data.count; i++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[i]);
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
			else if (m_double_count >= shun_data.count && (m_triple_info[0] < 15 || ((m_total_outhand_count - m_boom_count - temp_king_hand_count) == 1)))//带双
			{
				for (int i = 0; i < shun_data.count; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[i]);
						if (return_card != 0x00)
						{
							byOutCardList[byOutCardCount++] = return_card;
						}
					}
				}
			}
			else if (m_single_count == 1 && m_double_count == 1 && shun_data.count == 2)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
				return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
		else if (shun_data.type == 0x02 || shun_data.type == 0x01)//单双顺
		{
			for (int i = 0; i < shun_data.count; i++)
			{
				for (int j = 0; j < shun_data.type; j++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[shun_data.imax_pos] + i));
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
		}
	}
		break;
	case UG_THREE:
	case UG_THREE_ONE:
	case UG_THREE_TWO:
	case UG_THREE_DOUBLE:
	{
		//三带
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		memset(byOutCardList, 0, 20);
		byOutCardCount = 0;
		for (int j = 0; j < 3; j++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_triple_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		//判断带单还是带对子
		bool three_take_single = true;
		if (m_single_count > 0 && gameconfig.bTakePair && m_double_count > 0)
		{
			//如果对子大于单,带单
			if (m_single_info[0] > m_double_info[0])
			{
				three_take_single = false;
			}
		}
		if (m_single_count == 0)
		{
			three_take_single = false;
		}

		if (three_take_single)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		else
		{
			for (int j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
	}
		break;
	case UG_DOUBLE:
	{		
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;

		if (bSpliteCard)
		{
			for (int j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_triple_info[m_triple_count - 1]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
		else
		{
			for (int j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
	}
		break;
	case UG_ONLY_ONE:
	{
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		memset(byOutCardList, 0, 20);
		byOutCardCount = 0;

		BYTE t_single_info = m_single_info[0];

		if (m_single_count == 0)
		{
			t_single_info = m_double_info[0];
		}

		if (PostHandCount == 1 && m_single_count > 0)
		{//如果地主只剩一张,从最大的出
			t_single_info = m_single_info[m_single_count - 1];				//此时单排从最大的出
		}

		if (bSpliteCard)
		{//拆牌
			BYTE card_val[HAND_CARD_MAX] = { 0 };
			BYTE card_count = 0;
			if (UG_ONLY_ONE == out_record.GetLastOutType())
			{
				bool re = out_record.GetLastOutCardbyType(UG_ONLY_ONE, card_val, card_count);
				//查找比这张牌小的最大牌
				for (INT m = 17; m > 0; m--)
				{
					if (temp[m] > 0 && m < GetCardBulk(card_val[0], false))
					{
						t_single_info = m;
						break;
					}
				}
			}
			else
			{//从第二大牌开始出
				BYTE first_card = 0x00;
				for (INT m = 17; m > 0; m--)
				{
					if (temp[m] > 0 && first_card == 0x00)
					{
						first_card = m;
					}
					if (temp[m] > 0 && m < first_card)
					{
						t_single_info = m;
						break;
					}
				}
			}
		}

		BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, t_single_info);
		if (return_card != 0x00)
		{
			byOutCardList[0] = return_card;
			byOutCardCount = 1;
		}
	}
		break;
	default:
	{
		//printf("error switch first out card!");
	}
		break;
	}

	return;
}

//农民先手(下家队友)
void CGameLogic::FamerFirstBefFM(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount,
									CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Downlast_card.ClearAllData();

	BYTE backup_cardlist[20] = { 0 };
	BYTE backup_CardCount = byCardCount;
	memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
	AnalyzeHandCard(backup_cardlist, backup_CardCount, false);

	//寻找是否会导致对手一次出完()
	bool already_find = false;
	//already_find = FindOutCard(backup_cardlist, backup_CardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record, 0);

	if (already_find)
	{
		//寻找到出牌,直接出
		return;
	}

	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	//如果下家只有一张就送下家走
	if (PostHandCount == 1 && m_bFeedCard)
	{
		m_bFeedCard = false;
		for (int i = 0; i < 18; i++)
		{
			if (temp[i] > 0)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, i);
				if (return_card != 0x00)
				{
					byOutCardList[0] = return_card;
					byOutCardCount = 1;
					return;
				}
			}
		}
	}


	//获取大于等于2的牌张数
	int temp_king_hand_count = 0;//手中王的手数
	if (m_have_little_king != m_have_large_king) temp_king_hand_count = 1;


	int bigcardcount = temp[15] + temp[16] + temp[17];
	if ((m_real_single_count > 0 && bigcardcount > 3)
		&& (PreHandCount > 1 && PostHandCount > 1))
	{
		if (m_single_info[0] < 14)//大于A先不出
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[0] = return_card;
				byOutCardCount = 1;
			}
			return;
		}
	}

	int out_type = UG_ERROR_KIND;

	//提取张数较多的顺子牌型
	BYTE seq_count = 0x00;
	shun_info shun_data;
	Analysis_Result analy_re;
	SaveAnalysisResult(backup_cardlist, backup_CardCount, analy_re);
	out_type = FindOutCardType(analy_re, PreHandCount, PostHandCount, out_record, shun_data, Uplast_card, Downlast_card);

	if (out_type == UG_ERROR_KIND)
	{
		bool inverted = false;
		if (seq_count < (m_double_count * 2))
		{
			//如果有一对A或对2并且单牌大于2张,就先出单牌
			if ((PreHandCount != 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				out_type = UG_ONLY_ONE;
			}
			else if ((PostHandCount == 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				inverted = true;//此时要逆序出牌
				out_type = UG_ONLY_ONE;
			}
			else
			{
				out_type = UG_DOUBLE;
				seq_count = m_double_count * 2;
			}
			if (m_double_count == 1 && m_single_count == 1 && m_have_large_king != m_have_little_king && (PreHandCount != 1))
			{
				out_type = UG_ONLY_ONE;
			}
		}
		if ((PreHandCount != 1) || (out_type == UG_ERROR_KIND))
		{
			if (seq_count < m_single_count)
			{
				out_type = UG_ONLY_ONE;
			}
			else if (seq_count == m_single_count && m_single_count == 2)
			{
				if (m_single_info[m_single_count - 1] == 15 || (m_have_large_king || m_have_little_king))
				{
					out_type = UG_ONLY_ONE;
				}
			}
		}

		//如果单个数较少并且对子是小于10的就先出对子
		if (m_real_single_count <= 2 && m_double_count > 0 && m_double_info[0] < 10)
		{
			out_type = UG_DOUBLE;
		}

		//如果单个数已经超过大牌个数2个以上就先出其他牌型
		Analysis_Result sPreRS;
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPreRS);
		if (sPreRS.iAloneSingle > 2 && out_type == UG_ONLY_ONE)
		{
			if (m_double_info[0] < 13 && m_double_count > 0)
			{
				out_type = UG_DOUBLE;
			}
			else if (m_triple_info[0] < 13 && m_triple_count > 0)
			{
				out_type = UG_THREE;
			}
			else if (m_SequenceListStart[0] > 0x00)
			{
				out_type = UG_STRAIGHT;
			}
			//如果其他牌型都是大于K的牌型就继续出单
		}

		if (PreHandCount == 2 && m_single_count > 0)
		{//如果地主只剩两张,先出单
			out_type = UG_ONLY_ONE;
		}
	}

	BYTE last_out_type = out_record.GetLastOutType();
	//如果上次其他玩家都不要的牌型还有就继续出这种牌型
	if (last_out_type != UG_ERROR_KIND)
	{
		switch (last_out_type)
		{
		case UG_STRAIGHT:
		{
			if (m_SequenceCount[0] > 0)
			{
				out_type = UG_STRAIGHT;
			}
		}
			break;
		case UG_THREE:
		case UG_THREE_ONE:
		case UG_THREE_TWO:
		case UG_THREE_DOUBLE:
		{
			if (m_triple_count > 0 && m_triple_info[0] < 14)
			{//有且小于A
				out_type = UG_THREE;
			}
		}
			break;
		case UG_DOUBLE:
		{
			if (m_double_count > 0 && m_double_info[0] < 14 && PreHandCount != 2)
			{
				out_type = UG_DOUBLE;
			}
		}
			break;
		case UG_ONLY_ONE:
		{
			if (m_real_single_count > 0 && m_single_info[0] < 15)
			{
				out_type = UG_ONLY_ONE;
			}
		}
			break;
		default:
			break;
		}
	}

	//如果还是没有
	if (out_type == UG_ERROR_KIND)
	{
		if (m_single_count > 0)
		{
			out_type = UG_ONLY_ONE;
		}
		else if (m_have_large_king)
		{
			byOutCardList[byOutCardCount++] = 0x4F;
			return;
		}
		else if (m_have_little_king)
		{
			byOutCardList[byOutCardCount++] = 0x4E;
			return;
		}
	}

	//根据出牌类型出牌
	switch (out_type)
	{
	case UG_STRAIGHT:
	{
		if (shun_data.type == 0x03)
		{
			memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
			backup_CardCount = byCardCount;
			memset(byOutCardList, 0, 20);
			byOutCardCount = 0;
			for (int i = 0; i < shun_data.count; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[shun_data.imax_pos] + i));
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
			if (m_single_count >= shun_data.count)//带单
			{
				for (int i = 0; i < shun_data.count; i++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[i]);
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
			else if (m_double_count >= shun_data.count && (m_triple_info[0] < 15 || ((m_total_outhand_count - m_boom_count - temp_king_hand_count) == 1)))//带双
			{
				for (int i = 0; i < shun_data.count; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[i]);
						if (return_card != 0x00)
						{
							byOutCardList[byOutCardCount++] = return_card;
						}
					}
				}
			}
			else if (m_single_count == 1 && m_double_count == 1 && shun_data.count == 2)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
				return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
		else if (shun_data.type == 0x02 || shun_data.type == 0x01)//单双顺
		{
			for (int i = 0; i < shun_data.count; i++)
			{
				for (int j = 0; j < shun_data.type; j++)
				{
					BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, (m_SequenceListStart[shun_data.imax_pos] + i));
					if (return_card != 0x00)
					{
						byOutCardList[byOutCardCount++] = return_card;
					}
				}
			}
		}
	}
		break;
	case UG_THREE:
	{
		//三带
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		memset(byOutCardList, 0, 20);
		byOutCardCount = 0;
		for (int j = 0; j < 3; j++)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_triple_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		//判断带单还是带对子
		bool three_take_single = true;
		if (m_single_count > 0 && gameconfig.bTakePair && m_double_count > 0)
		{
			//如果对子大于单,带单
			if (m_single_info[0] > m_double_info[0])
			{
				three_take_single = false;
			}
		}

		if (m_single_count == 0)
		{
			three_take_single = false;
		}

		if (three_take_single)
		{
			BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_single_info[0]);
			if (return_card != 0x00)
			{
				byOutCardList[byOutCardCount++] = return_card;
			}
		}
		else
		{
			for (int j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
	}
		break;
	case UG_DOUBLE:
	{
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		if (m_double_info[0] < 14 || m_single_count <= 1)//对2、对A先不出；单牌只剩一张时再出对这些大对子
		{
			for (int j = 0; j < 2; j++)
			{
				BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, m_double_info[0]);
				if (return_card != 0x00)
				{
					byOutCardList[byOutCardCount++] = return_card;
				}
			}
		}
	}
		break;
	case UG_ONLY_ONE:
	{
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
		memset(byOutCardList, 0, 20);
		byOutCardCount = 0;

		BYTE t_single_info = m_single_info[0];

		if (m_single_count == 0)
		{
			t_single_info = m_double_info[0];
		}

		BYTE return_card = GetRealCardValue(backup_cardlist, backup_CardCount, t_single_info);
		if (return_card != 0x00)
		{
			byOutCardList[0] = return_card;
			byOutCardCount = 1;
		}
	}
		break;
	default:
	{
		//printf("error switch first out card!");
	}
		break;
	}

	return;
}

//---------------------------------------------------------------------------------------------------------------
//构造函数
CRecordTool::CRecordTool(void)
{
	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;
}
//析造函数
CRecordTool::~CRecordTool()
{
	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;
}
//初始化记牌器
bool CRecordTool::InitLastCard()
{
	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;

	BYTE color = 0x10;
	for (int i = 0; i < 4; i++)
	{
		m_LastCard[m_LastCount++] = 0x01 + color * i;
		m_LastCard[m_LastCount++] = 0x02 + color * i;
		m_LastCard[m_LastCount++] = 0x03 + color * i;
		m_LastCard[m_LastCount++] = 0x04 + color * i;
		m_LastCard[m_LastCount++] = 0x05 + color * i;
		m_LastCard[m_LastCount++] = 0x06 + color * i;
		m_LastCard[m_LastCount++] = 0x07 + color * i;
		m_LastCard[m_LastCount++] = 0x08 + color * i;
		m_LastCard[m_LastCount++] = 0x09 + color * i;
		m_LastCard[m_LastCount++] = 0x0a + color * i;
		m_LastCard[m_LastCount++] = 0x0b + color * i;
		m_LastCard[m_LastCount++] = 0x0c + color * i;
		m_LastCard[m_LastCount++] = 0x0d + color * i;
	}

	m_LastCard[m_LastCount++] = 0x4E;
	m_LastCard[m_LastCount++] = 0x4F;

	return true;
}

//设置记牌器牌值
bool CRecordTool::SetLastCard(CRecordTool src)
{
	if (src.m_LastCount > MAX_COUNT)
	{
		return false;
	}
	memcpy(m_LastCard, src.m_LastCard, sizeof(m_LastCard));
	m_LastCount = src.m_LastCount;
	return true;
}
//设置记牌器牌值
bool CRecordTool::SetLastCard(BYTE card_value[], unsigned int card_count)
{
	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;
	for (UINT i = 0; i < card_count; i++)
	{
		if (IsCardValid(card_value[i]))
		{
			m_LastCard[m_LastCount++] = card_value[i];
		}
	}
	return true;
}
//删除出掉的牌
UNIT CRecordTool::RemoveCard(BYTE out_card_list[], unsigned int out_card_count)
{
	if (out_card_count > 20)
	{
		return 0;
	}
	unsigned int count = 0;

	for (unsigned int i = 0; i < out_card_count; i++)
	{
		if (IsCardValid(out_card_list[i]))
		{
			for (unsigned int j = 0; j < m_LastCount; j++)
			{
				if (m_LastCard[j] == out_card_list[i])
				{
					m_LastCard[j] = 0x00;
					count++;
				}
			}
		}
	}

	Sort();

	return count;
}

//是否有某张牌
bool CRecordTool::IsHaveCard(BYTE byCard, unsigned int count)
{
	unsigned int card_count = 0;
	if (IsCardValid(byCard))
	{
		return false;
	}

	for (unsigned int j = 0; j < m_LastCount; j++)
	{
		if (m_LastCard[j] == byCard)
		{
			card_count++;
		}
	}

	if (count != card_count)
	{
		return false;
	}

	return true;
}
////是否有炸弹 1;只有普通炸弹 2:有王炸 3:有王炸加普通炸弹
int CRecordTool::IsHaveBoom(BYTE start_card)
{
	int re_code = 0;

	BYTE temp[18] = { 0 };
	for (UNIT i = 0; i < m_LastCount; i++)
	{
		temp[mGetCardBulk(m_LastCard[i], false)]++;
	}

	for (UNIT i = 0; i < 18; i++)
	{
		if (temp[i] == 4 && i > start_card)
		{
			re_code = 1;
			break;
		}
	}

	if (temp[16] == 1 && temp[17] == 1)
	{
		re_code += 2;
	}
	return re_code;
}

//排序
bool CRecordTool::Sort()
{
	unsigned int card_index[MAX_CARD_INDEX] = { 0 };
	ChangeCardToIndex(m_LastCard, CARD_MAX_COUNT, card_index);

	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;

	for (int i = 0; i < MAX_CARD_INDEX; i++)
	{
		if (card_index[i] == 1)
		{
			m_LastCard[m_LastCount++] = i;
			card_index[i]--;
		}
	}

	return true;
}

//获取所有牌值
UNIT CRecordTool::GetAllCard(BYTE byCardList[])
{
	if (m_LastCount > HAND_CARD_MAX)
	{
		return 0;
	}

	memcpy(byCardList, m_LastCard, HAND_CARD_MAX * sizeof(BYTE));

	return m_LastCount;
}

//获取所有余牌牌值index
UNIT CRecordTool::GetAllCardIndex(BYTE byIndex[])
{
	//转换牌值
	BYTE temp[CARD_BULK_INDEX] = { 0 };
	for (UINT i = 0; i < m_LastCount; i++)
	{
		temp[mGetCardBulk(m_LastCard[i], false)]++;
	}

	memcpy(byIndex, temp, sizeof(temp));

	return m_LastCount;
}

//获取剩余的最大的牌
BYTE  CRecordTool::GetLastMaxCard(UINT count)
{
	BYTE temp[CARD_BULK_INDEX] = { 0 };

	GetAllCardIndex(temp);

	for (int i = CARD_BULK_INDEX - 1; i > 0; i--)
	{
		if (temp[i] >= count)
		{
			return i;
		}
	}
	return 0x00;
}

//是否有大的过的顺子
bool CRecordTool::IsHaveBigShun(BYTE start_val, BYTE count)
{
	if (count < 5)
	{
		return true;
	}

	if (start_val + count >= 0x0E)
	{
		return false;
	}
	BYTE temp[CARD_BULK_INDEX] = { 0 };

	GetAllCardIndex(temp);

	int shun_count = 0;
	for (size_t i = start_val + 1; i < CARD_BULK_INDEX - 3; i++)
	{
		if (temp[i] > 0)
		{
			shun_count++;
		}
		else
		{
			shun_count = 0;
		}

		if (shun_count == count)
		{
			return true;
		}
	}

	return false;
}

//清理数据
bool CRecordTool::ClearAllData()
{
	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;
	
	return true;
}

//---------------------------------------------------------------------------------------------------------

//构造函数
COperateRecord::COperateRecord(void)
{
	for (unsigned int i = 0; i < HAND_CARD_MAX; i++)
	{
		m_out_info[i].clear();
	}
	for (unsigned int i = 0; i < (2* HAND_CARD_MAX); i++)
	{
		m_pass_info[i].clear();
	}
	m_out_count = 0;
	m_pass_count = 0;
	m_continue_pass_count = 0;
}
//析造函数
COperateRecord::~COperateRecord()
{
	for (unsigned int i = 0; i < HAND_CARD_MAX; i++)
	{
		m_out_info[i].clear();
	}
	for (unsigned int i = 0; i < (2 * HAND_CARD_MAX); i++)
	{
		m_pass_info[i].clear();
	}
	m_out_count = 0;
	m_pass_count = 0;
	m_continue_pass_count = 0;
}

//添加一组出牌信息
bool COperateRecord::AddOutCardInfo(BYTE byCardList[], BYTE	byCardCount, BYTE byCardType, BYTE byFirstOut)
{
	if (m_out_count >= HAND_CARD_MAX)
	{
		return false;
	}
	if (byCardCount > HAND_CARD_MAX)
	{
		return false;
	}
	for (unsigned int i = 0; i < byCardCount; i++)
	{
		if (!IsCardValid(byCardList[i]))
		{
			return false;
		}
	}
	memcpy(m_out_info[m_out_count].byCardList, byCardList, HAND_CARD_MAX*sizeof(BYTE));
	m_out_info[m_out_count].byCardCount = byCardCount;
	m_out_info[m_out_count].byCardType = byCardType;
	m_out_info[m_out_count].byExInfo = byFirstOut;
	m_out_count++;
	m_continue_pass_count = 0;

	return true;
}
//添加一组过牌信息 byFriend 0x00 队友  0x01对手
bool COperateRecord::AddPassCardInfo(BYTE byCardList[], BYTE byCardCount, BYTE byCardType, BYTE byFriend)
{
	if (m_pass_count >= 2 * HAND_CARD_MAX)
	{
		return false;
	}
	if (byCardCount > HAND_CARD_MAX)
	{
		return false;
	}
	for (unsigned int i = 0; i < byCardCount; i++)
	{
		if (!IsCardValid(byCardList[i]))
		{
			return false;
		}
	}
	memcpy(m_pass_info[m_pass_count].byCardList, byCardList, HAND_CARD_MAX*sizeof(BYTE));
	m_pass_info[m_pass_count].byCardCount = byCardCount;
	m_pass_info[m_pass_count].byCardType = byCardType;
	m_pass_info[m_pass_count].byExInfo = byFriend;//不跟队友
	m_pass_count++;

	//和上一次的牌型一样,并且还是不跟
	if (byCardType == m_pass_info[m_pass_count - 2].byCardType)
	{
		if (byFriend == 0x01)
		{
			m_continue_pass_count++;
		}
	}
	else
	{
		m_continue_pass_count = 0;
	}

	return true;
}
//获取此牌型上次出的牌值
bool COperateRecord::GetLastOutCardbyType(BYTE byCardType, BYTE byCardList[], BYTE byCardCount)
{
	if (byCardType == UG_ERROR_KIND)
	{
		return false;
	}
	for (int i = byCardType; i >= 0; i--)
	{
		if (m_out_info[i].byCardType == byCardType)
		{
			memcpy(byCardList, m_out_info[i].byCardList, sizeof(m_out_info[i].byCardList));
			byCardCount = m_out_info[i].byCardCount;
			return true;
		}
	}

	return false;
}
//获取此牌型上次出的牌型
BYTE COperateRecord::GetLastOutType()
{
	if (m_out_count == 0)
	{
		return UG_ERROR_KIND;
	}
	else
	{
		return m_out_info[m_out_count - 1].byCardType;
	}
}
//获取此牌型上次不跟的牌值
bool COperateRecord::GeteLastPassCardbyType(BYTE byCardType, BYTE byCardList[], BYTE &byCardCount)
{
	if (byCardType == UG_ERROR_KIND)
	{
		return false;
	}
	for (int i = m_pass_count; i >= 0; i--)
	{
		if (m_pass_info[i].byCardType == byCardType)
		{
			memcpy(byCardList, m_pass_info[i].byCardList, sizeof(m_pass_info[i].byCardList));
			byCardCount = m_pass_info[i].byCardCount;
			return true;
		}
	}

	return false;
}
//获取此牌型出牌的次数
unsigned int COperateRecord::GetCountOutbyType(BYTE byCardType)
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < m_out_count; i++)
	{
		if (m_out_info[i].byCardType == byCardType)
		{
			count++;
		}
	}
	return count;
}
//获取此牌型不跟的次数
unsigned int COperateRecord::GetCountPassbyType(BYTE byCardType)
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < m_pass_count; i++)
	{
		if (m_pass_info[i].byCardType == byCardType)
		{
			count++;
		}
	}
	return count;
}
//获取连续同类型不跟次数
unsigned int COperateRecord::GetContinuePassCount(BYTE iBaseShape)
{
	if (m_pass_info[m_pass_count].byCardType == iBaseShape)
	{
		return m_continue_pass_count;
	}
	return 0;
}
//清空所有数据
bool COperateRecord::ClearAllData()
{
	for (unsigned int i = 0; i < HAND_CARD_MAX; i++)
	{
		m_out_info[i].clear();
	}
	for (unsigned int i = 0; i < (2 * HAND_CARD_MAX); i++)
	{
		m_pass_info[i].clear();
	}
	m_out_count = 0;
	m_pass_count = 0;
	m_continue_pass_count = 0;

	return true;
}
