
#include "RobortLogic.h"
#include <string>
#include <random>
#include <time.h>

//����㷨
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



//ת��������ֵ
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
//��ȡ�˿˻�ɫ
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

//��ȡ�˿˴�С ��2 - 18 �� 15 ���������� �� 2 - 21 �� 15 ����������
int mGetCardBulk(BYTE iCard, bool bExtVal)
{
	if ((iCard == 0x4E) || (iCard == 0x4F))
	{
		return bExtVal ? (iCard - 14) : (iCard - 62); //��С��64+14-62=16	ֻ���ش�Сè��ֵ
	}

	int iCardNum = mGetCardNum(iCard);
	int iHuaKind = mGetCardHuaKind(iCard, TRUE);

	return ((bExtVal) ? ((iHuaKind >> 4) + (iCardNum * 4)) : (iCardNum));
}

static const BYTE m_CardArray[CARD_MAX_COUNT] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, //���� A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, //÷�� A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, //���� A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, //���� A - K
	0x4E, 0x4F																	  //С�����
};
//���캯��
CGameLogic::CGameLogic(void)
{
	m_bSortCardStyle = 0;			//0 �ư���С����;1 ����������
	m_bKingCanReplace = false;
	m_bFeedCard = true;
	gameconfig.InitConfig();
	ClearTempAnalyseInfo();
	InitAllData();
}
//���캯��
CGameLogic::~CGameLogic()
{
}
//��ʼ����ر���
void CGameLogic::InitAllData()
{
	m_bSortCardStyle = 0;			//0 �ư���С����;1 ����������
	m_bKingCanReplace = false;
	gameconfig.InitConfig();
	ClearTempAnalyseInfo();
	m_bFeedCard = true;
}

//[�������]
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
//��ȡ�˿˻�ɫ
BYTE CGameLogic::GetCardHuaKind(BYTE iCard, BOOL bTrueHua)
{
	int iHuaKind = (iCard&UG_HUA_MASK);
	if (!bTrueHua)
	{
		return iHuaKind = UG_NT_CARD;
	}
	return iHuaKind;
}

//��ȡ�˿˴�С ��2 - 18 �� 15 ���������� �� 2 - 21 �� 15 ����������
int CGameLogic::GetCardBulk(BYTE iCard, bool bExtVal)
{
	if ((iCard == 0x4E) || (iCard == 0x4F))
	{
		return bExtVal ? (iCard - 14) : (iCard - 62); //��С��64+14-62=16	ֻ���ش�Сè��ֵ
	}

	int iCardNum = GetCardNum(iCard);
	int iHuaKind = GetCardHuaKind(iCard, TRUE);

	return ((bExtVal) ? ((iHuaKind >> 4) + (iCardNum * 4)) : (iCardNum));
}
//�����ֵ�Ƿ�Ϸ�
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
//���������ִӴ�С�����˿�
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
	//��ȡλ����ֵ
	for (int i = 0; i < iCardCount; i++)
	{
		iStationVol[i] = GetCardBulk(iCardList[i], true);
	}
	//�������(���Ӵ�С����)
	do
	{
		bSorted = TRUE;
		for (int i = 0; i < iLast; i++)
		{
			if (iStationVol[i] < iStationVol[i + 1])
			{
				//����λ��				//==ð������
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

	//ϵͳ���в����ǻ�ɫ��������
	if (bSysSort)
	{
		ReverseCard(iCardList, bUp, iCardCount);
		return TRUE;
	}
	if (GetSortCardStyle() == 1) //����������
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
//����������
BOOL CGameLogic::SortCardByStyle(BYTE iCardList[], BYTE iCardCount)
{
	//�������������Ҫ�󰴴�С����
	if (m_bSortCardStyle == 0)
	{
		SortCard(iCardList, NULL, iCardCount);
		return TRUE;
	}
	//����Ĵ���==�������Ŵ�С
	int iStationVol[45];
	memset(iStationVol, 0, sizeof(iStationVol));
	for (int i = 0; i < iCardCount; i++)
	{
		iStationVol[i] = GetCardBulk(iCardList[i], false);
	}
	int Start = 0;
	int j, step;
	BYTE CardTemp[8];					//��������Ҫ��λ������
	int CardTempVal[8];					//����������λ������ֵ
	for (int i = 8; i > 1; i--)				//����������һ������i����ͬ��ֵ
	{
		for (j = Start; j < iCardCount; j++)
		{
			CardTemp[0] = iCardList[j];			//���浱ǰi��������ͬ��ֵ
			CardTempVal[0] = iStationVol[j];
			for (step = 1; step < i&&j + step < iCardCount;)			//��һ������i��ֵ��ȵ�����(����������ʱ������)
			{
				if (iStationVol[j] == iStationVol[j + step])
				{
					CardTemp[step] = iCardList[j + step];			//������������
					CardTempVal[step] = iStationVol[j + step];		//��ֵ
					step++;
				}
				else
				{
					break;
				}
			}
			if (step >= i)	//�ҵ�һ������i����ȵ����鴮��ʼλ��Ϊj,����λ��Ϊj+setp-1
			{			//����Start��ʼ��j���������setp��
				if (j != Start) //�ų���ʼ��������
				{
					for (; j >= Start; j--) //��Start����j�ź��ƶ�i��
					{
						iCardList[j + i - 1] = iCardList[j - 1];
						iStationVol[j + i - 1] = iStationVol[j - 1];
					}
					for (int k = 0; k < i; k++)
					{
						iCardList[Start + k] = CardTemp[k];	//��Start��ʼ���ó�CardSave
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

//�����˿�,������ʹ��
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
//ɾ���˿�
int CGameLogic::RemoveCard(BYTE iRemoveCard[],   //Ҫɾ��������
						   int iRemoveCount,		//Ҫɾ����������
						   BYTE iCardList[],		//Ҫ���������
						   int iCardCount)		//�������������
{
	if (iRemoveCount == 0) return 0;
	//��������
	if (iRemoveCount > iCardCount) return 0;

	int iRecount = 0;
	int iDeleteCount = 0; //��Ҫɾ����������

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
	iRecount = RemoveNummCard(iCardList, iCardCount); //ɾ�����˱�ǵ���

	if (iDeleteCount != iRecount)
	{
		return 0;
	}

	return iDeleteCount;
}
//��� 0 λ�˿�
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
//�Ƚϵ���
BOOL CGameLogic::CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard)
{
	//��һ����ʾ�����������, �ڶ�����ʾҪ������
	return GetCardBulk(iFirstCard) < GetCardBulk(iNextCard);
}
//�������Ƿ�����ͬ����
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
		if (irank < 16)//����������
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

//�Ƿ�Ϊͬ��
BOOL CGameLogic::IsSameHuaKind(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount <= 0) return FALSE;

	BYTE iFirstHua = GetCardHuaKind(iCardList[0], TRUE); //ȡ�õ�һ���ƵĻ�ɫ

	for (int i = 1; i < iCardCount; i++) //����Ķ��͵�һ�ŵĻ�ɫ��
	{
		if (GetCardHuaKind(iCardList[i], TRUE) != iFirstHua)
		{
			return FALSE;
		}
	}
	return TRUE;
}

//�����û�����ը����
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

//��ȡָ�������Ƹ���
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

//��ȡָ���Ƹ���
BYTE CGameLogic::GetCountBySpecifyCard(BYTE iCardList[], int iCardCount, BYTE bCard)
{
	int count = 0;
	for (int i = 0; i < iCardCount; i++)
		if (iCardList[i] == bCard)
			count++;

	return count;
}
//��ȡָ������ֵ�Ƶĸ���
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
//��ȡָ���������ƴ�С
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

//�Ƿ�Ϊ����˳��
BOOL CGameLogic::IsVariationSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	int iValue = iCardCount / iCount;
	if (iCardCount != iCount *iValue)						 //����������
		return FALSE;

	int iFirstMax = 0, iSecondMax = 0, iThirdMax = 0, iMin = 18;//�ҳ���һ��,�ڶ���,���������,����С��
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)						//�ƶ���
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int i = 0; i < 18; i++)
	{
		if (temp[i] != 0 && temp[i] != iCount)	//���Ҷ�˳��
			return false;
	}

	for (int i = 0; i < 18; i++)						//��С�������ܵ�A
	{
		if (temp[i] != 0)
			iMin = i;
	}

	for (int i = 17; i > 0; i--)
	{
		if (temp[i] != 0)
		{
			iFirstMax = i;						//������2Ҳ������A
			for (int j = i - 1; j > 0; j--)
			{
				if (temp[j] != 0)//�ҵ��ڶ�����˳�ѭ��(�޵������)//������AҲ���Է�A
				{
					iSecondMax = j;
					for (int k = j - 1; j > 0; j--)
					{
						if (temp[k] != 0)//��ڵ�������˳�ѭ��	//���Ǵ���Ҳ���Բ�����
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

	if (iFirstMax < 15)	//������2�����,���������
	{
		return (iFirstMax - iMin + 1 == iValue);
	}

	if (iFirstMax == 15)	//����2,�ٿ��Ƿ����A
	{
		if (iSecondMax == 14)		//����A
		{
			if (iThirdMax == 0)		//�����ڵ�����,Ҳֻ��A2������
				return true;

			return (iThirdMax - iMin + 1 == iValue - 2);		//���� A2�����������AA2233
		}
		return (iSecondMax - iMin + 1 == iValue - 1);
	}

	return false;
}

//�Ƿ�����
BOOL CGameLogic::IsSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int i = 0; i < 15; i++)
	{
		if (temp[i] != 0 && temp[i] != iCount)	//��ָ��˳
			return false;
	}

	int len = iCardCount / iCount;

	for (int i = 0; i < 15; i++)
	{
		if (temp[i] != 0)//��ֵ
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
//�Ƿ���ָ����˳��(��������Ƶļ��)
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
			//ĳ���Ʋ��ܴ�������Ƶ�����
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

//��ȡָ���Ʒ����ҵ��Ƃ���
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


//��ȡ1,2,3 or 4����ͬ���ֵ���
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
		if (temp[i] == bCardNum && bCardNum > 0) //����Ҫ���ҵ�����:one?double?three?four_bomb?
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

//��ȡָ����ɫ��
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

//���(�������ƶ�Ĳ���ٵ�)
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

//��ȡĳ��ָ����С����
BOOL CGameLogic::TackOutCardBySpecifyCardNum(BYTE iCardList[], int iCardCount, BYTE iBuffer[], int &iBufferCardCount, BYTE iCard, BOOL bExtVal)
{
	iBufferCardCount = 0;
	BYTE iCardNum = GetCardBulk(iCard); //�õ��������
	for (int i = 0; i < iCardCount; i++)
	{
		if (GetCardBulk(iCardList[i]) == iCardNum) //����Ҫ���ҵ��Ƶ�����
		{
			iBuffer[iBufferCardCount++] = iCardList[i];
		}
	}
	return iBufferCardCount;
}

//���Ҵ���iCard�ĵ�������iCardList�е����
BYTE  CGameLogic::GetSerialByMoreThanSpecifyCard(BYTE iCardList[], int iCardCount,
												 BYTE iCard, BYTE iBaseCardCount,
												 bool bExtValue)
{
	BYTE MaxCard = 0;
	BYTE Serial = 0;
	BYTE MaxCardNum = 255;
	int BaseCardNum = GetCardBulk(iCard);	//��ǰ�Ƚ�ֵ

	for (BYTE i = 0; i < iCardCount; i += iBaseCardCount)
	{
		int temp = GetCardBulk(iCardList[i]);
		if (temp<MaxCardNum && temp>BaseCardNum)
		{
			MaxCardNum = temp;
			Serial = i; //�õ����
			break;
		}
	}
	return Serial;
}


//����==iCard�ĵ�������iCardList�е����
int  CGameLogic::GetSerialBySpecifyCard(BYTE iCardList[], int iStart, int iCardCount, BYTE iCard)
{
	for (int i = iStart; i < iCardCount; i++)
	{
		if (iCardList[i] == iCard)
			return i;
	}
	return -1;
}

//����˳��������
BYTE CGameLogic::GetBulkBySpecifyVariationSequence(BYTE iCardList[], int iCardCount, int iSequence)
{
	int iFirstMax = 0, iSecondMax = 0, iThirdMax = 0;//�ҳ���һ��,�ڶ���,���������,����С��
	BYTE temp[18] = { 0 };
	for (int i = 0; i < iCardCount; i++)						//�ƶ���
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for (int i = 17; i > 0; i--)
	{
		if (temp[i] == iSequence)
		{
			iFirstMax = i;						//������2Ҳ������A
			for (int j = i - 1; j > 0; j--)
			{
				if (temp[j] == iSequence)//�ҵ��ڶ�����˳�ѭ��(�޵������)//������AҲ���Է�A
				{
					iSecondMax = j;
					for (int k = j - 1; j > 0; j--)
					{
						if (temp[k] == iSequence)//��ڵ�������˳�ѭ��	//���Ǵ���Ҳ���Բ�����
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

	if (iFirstMax == 15)	//����2,�ٿ��Ƿ����A
	{
		if (iSecondMax == 14)		//����A
		{
			if (iThirdMax == 0)		//�����ڵ�����,Ҳֻ��A2������
				return 2;

			return iThirdMax;		//���� A2�����������AA2233
		}
		return iSecondMax;
	}
	return 0;
}

//��ȡָ��˳�����Ƶ���Сֵ(iSequence ����˳�ӵ��������Ϊ
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

//�ҳ�һ����С��������
int  CGameLogic::GetBulkBySepcifyMinOrMax(BYTE iCardList[], int iCardCount, int MinOrMax/*1 or 255*/, bool bExtVal)
{
	int CardNum = GetCardBulk(iCardList[0], false);

	if (MinOrMax == 1) //����С��
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

	//���ص��� GetCardBulk() �õ���ֵ
	return CardNum;
}

/////////////////////////////////////////////////////////////////////////
/**
* @info ��ȡ����
* @param iCardList[] ��
* @param iCardCount �Ƶ�����
* @param bExlVol ?
*/
BYTE CGameLogic::GetCardShape(BYTE iCardList[], int iCardCount)
{
	if (IsOnlyOne(iCardList, iCardCount)) return UG_ONLY_ONE;							//����
	if (IsDouble(iCardList, iCardCount)) return UG_DOUBLE;								//����
	if (IsThreeX(iCardList, iCardCount, 0)) return UG_THREE;							//����
	if (IsKingBomb(iCardList, iCardCount)) return UG_KING_BOMB;							//��ը
	if (IsBomb(iCardList, iCardCount)) return UG_BOMB;									//4������ͬ���ƣ�ը��
	if (IsThreeX(iCardList, iCardCount, 3))	return UG_THREE_DOUBLE;						//������
	if (IsThreeX(iCardList, iCardCount, 1)) return UG_THREE_ONE;						//����һ
	if (IsFourX(iCardList, iCardCount, 4)) return UG_FOUR_TWO_DOUBLE;					//�Ĵ�����(Ҫ���Ƕ���)
	if (IsFourX(iCardList, iCardCount, 2)) return UG_FOUR_TWO;							//�Ĵ���(��Ҫ��ɶ�)
	if (IsStraight(iCardList, iCardCount)) return UG_STRAIGHT;							//˳��
	if (IsDoubleSequence(iCardList, iCardCount)) return UG_DOUBLE_SEC;					//����
	if (IsThreeXSequence(iCardList, iCardCount, 0)) return UG_THREE_SEQUENCE;			//����
	if (IsThreeXSequence(iCardList, iCardCount, 3)) return UG_THREE_DOUBLE_SEC;			//����������
	if (IsThreeXSequence(iCardList, iCardCount, 1)) return UG_THREE_ONE_SEQUENCE;		//��������һ

	return UG_ERROR_KIND;
}

//����
BOOL CGameLogic::IsDouble(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount != 2)
		return FALSE;
	return IsSameNumCard(iCardList, iCardCount, bExtVal);
}

//3 �� 0,1or2,or3
BOOL CGameLogic::IsThreeX(BYTE iCardList[], int iCardCount, int iX, bool bExtVal)
{
	if (iCardCount > 5 || iCardCount < 3)
		return FALSE;

	if (GetCountBySpecifyNumCount(iCardList, iCardCount, 3) != 1)//�Ƿ��������
		return false;
	switch (iX)
	{
		case 0:
			return iCardCount == 3;//IsSameNumCard(iCardList, iCardCount, bExtVal);//����
		case 1:
			return iCardCount == 4;//������
		case 2:
			return iCardCount == 5;//�����ţ����ԷǶ��ӣ�
		case 3:					//��һ��
			return GetCountBySpecifyNumCount(iCardList, iCardCount, 2) == 1;//�Ƿ���ڶ���
		default:
			break;
	}
	return FALSE;
}

//�Ĵ�1or2
BOOL CGameLogic::IsFourX(BYTE iCardList[], int iCardCount, int iX)
{
	if (iCardCount > 8 || iCardCount < 4)
		return FALSE;

	if (GetCountBySpecifyNumCount(iCardList, iCardCount, 4) != 1)//�Ƿ����ĸ�����
		return false;

	switch (iX)
	{
		case 0:
			return iCardCount == 4;//����
		case 1:
			return iCardCount == 5;//�Ĵ�1��
		case 2:
			return iCardCount == 6;//�Ĵ�2(��Ҫ��ɶ�)
		case 3:
			return (iCardCount == 6 && 1 == GetCountBySpecifyNumCount(iCardList, iCardCount, 2));//Ҫ��ɶ�
		case 4:
			return (iCardCount == 8 && 2 == GetCountBySpecifyNumCount(iCardList, iCardCount, 2));	//�Ĵ�2��
	}

	return FALSE;
}

//��ը
BOOL CGameLogic::IsKingBomb(BYTE iCardList[], int iCardCount)			//�Ƿ�Ϊ��ը(ץ��������)
{
	if (iCardCount != KING_COUNT)
		return FALSE;

	for (int i = 0; i < iCardCount; i++)
		if (iCardList[i] != 0x4e && iCardList[i] != 0x4f)
			return FALSE;
	return TRUE;
}

//4+���� ը��
BOOL CGameLogic::IsBomb(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 4)
		return FALSE;

	return IsSameNumCard(iCardList, iCardCount, bExtVal); //�Ƿ�����ͬ����
}

//ͬ��ը��
BOOL CGameLogic::IsBombSameHua(BYTE iCardList[], int iCardCount)
{
	if (!IsBomb(iCardList, iCardCount)) return false;
	if (!IsSameHuaKind(iCardList, iCardCount)) return false;
	return TRUE;
}

//ͬ��(��ͬ��)
BOOL CGameLogic::IsFlush(BYTE iCardList[], int iCardCount)
{
	return IsSameHuaKind(iCardList, iCardCount);
}

//ͬ��˳ 5��ͬ��������
BOOL CGameLogic::IsStraightFlush(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (!IsSameHuaKind(iCardList, iCardCount, bExtVal)) return FALSE; //ͬ����

	if (!IsStraight(iCardList, iCardCount, bExtVal)) return FALSE; //˳�ӣ�
	return TRUE;
}

//�Ƿ��Ǳ���˳��(A2345)��23456
BOOL CGameLogic::IsVariationStraight(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 5)
		return false;
	return IsVariationSequence(iCardList, iCardCount, 1);
}

//�Ƿ���˳��ָ������
BOOL CGameLogic::IsStraight(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 5)
		return false;
	return IsSequence(iCardList, iCardCount, 1);
}

//�Ƿ��Ǳ�������AA22��2233��
BOOL CGameLogic::IsVariationDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount % 2 != 0 || iCardCount < 4)
		return false;

	return IsVariationSequence(iCardList, iCardCount, 2);
}

//�Ƿ�������
BOOL CGameLogic::IsDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount % 2 != 0 || iCardCount < 6)
		return false;

	return IsSequence(iCardList, iCardCount, 2);
}

//������˳����˳
BOOL CGameLogic::IsVariationThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 10)		//��˳����2��˳Ҳ���ٶ�
		return false;

	BYTE iBuffer3[54], iBuffer2[54];
	BOOL bValue3 = false, bValue2 = false;	//��˳,��˳�Ƿ�Ϊ˳,
	int TackOutCount3 = 0, TackOutCount2 = 0;

	TackOutCount3 = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer3, 3);//����
	TackOutCount2 = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer2, 2);//����

	if (TackOutCount3 <= 0 || TackOutCount2 <= 0 || TackOutCount3 + TackOutCount2 != iCardCount || TackOutCount3 / 3 != TackOutCount2 / 2)
		return FALSE;

	bValue3 = IsVariationSequence(iBuffer3, TackOutCount3, 3);
	bValue2 = (IsVariationSequence(iBuffer2, TackOutCount2, 2) || IsSequence(iBuffer2, TackOutCount2, 2));
	return bValue3&&bValue2;
}

//��˳����˳
BOOL CGameLogic::IsThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount < 10)		//��˳����2��˳Ҳ���ٶ�
		return false;

	BYTE iBuffer3[54], iBuffer2[54];
	BOOL bValue3 = false, bValue2 = false;	//��˳,��˳�Ƿ�Ϊ˳,
	int TackOutCount3 = 0, TackOutCount2 = 0;

	TackOutCount3 = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer3, 3);//����
	TackOutCount2 = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer2, 2);//����
	if (TackOutCount3 <= 0 || TackOutCount2 <= 0 || TackOutCount3 + TackOutCount2 != iCardCount || TackOutCount3 / 3 != TackOutCount2 / 2)
		return FALSE;
	bValue3 = IsSequence(iBuffer3, TackOutCount3, 3);
	//	TCHAR sz[200];
	//wsprintf(sz,"%d",bValue3);
	//	WriteStr(sz);
	bValue2 = (IsVariationSequence(iBuffer2, TackOutCount2, 2) || IsSequence(iBuffer2, TackOutCount2, 2));
	//	TCHAR sz[200];
	//	wsprintf(sz,"bValue3=%d,bValue2=%d==����2˳%d,��׼��˳%d",bValue3,bValue2,IsVariationSequence(iBuffer2,TackOutCount2,2),IsSequence(iBuffer2,TackOutCount2,2));
	//	WriteStr(sz);
	return bValue3&&bValue2;
}


//����������x
BOOL CGameLogic::IsVariationThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX, bool bExtVal)
{
	if (iCardCount < 6)		//��˳����2
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
		case 1://����
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount>0 && TackOutCount / 3 * 4 == iCardCount)
				return IsVariationSequence(iBuffer, TackOutCount, 3);
			break;
		case 2://������
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount > 0 && TackOutCount / 3 * 5 == iCardCount)
				return IsVariationSequence(iBuffer, TackOutCount, 3);
		case 3://��һ��
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount > 0 && TackOutCount / 3 * 5 == iCardCount
				&&GetCountBySpecifyNumCount(iCardList, iCardCount, 2))
				return IsVariationSequence(iBuffer, TackOutCount, 3);

			break;
	}
	return false;
}

//�������� 0,1 or 2
BOOL CGameLogic::IsThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX, bool bExtVal)
{
	if (iCardCount < 6)		//��˳����2
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
		case 1://����
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
		case 2://������
		{
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 3);
			if (TackOutCount > 0 && TackOutCount / 3 * 5 == iCardCount)
				return IsSequence(iBuffer, TackOutCount, 3);
			break;
		}
		case 3://����
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

//������˳����
BOOL CGameLogic::IsVariationFourXSequence(BYTE iCardList[], int iCardCount, int iSeqX)
{
	if (iCardCount < 8)		//��˳����2
		return false;

	BYTE iBuffer[54];
	int TackOutCount = 0;
	switch (iSeqX)
	{
		case 0:
			if (iCardCount % 4 != 0)
				return false;
			return IsVariationSequence(iCardList, iCardCount, 4);
		case 1://������
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount>0 && TackOutCount / 4 * 5 == iCardCount)
				return IsVariationSequence(iBuffer, TackOutCount, 4);
			break;
		case 2://������(���ԷǶ��ӣ�
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount)
				return IsVariationSequence(iBuffer, TackOutCount, 4);
			break;
		case 3://��һ��
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount
				&&TackOutCount / 4 == GetBulkBySpecifyCardCount(iCardList, iCardCount, 2))
				return IsVariationSequence(iBuffer, TackOutCount, 4);
			break;
		case 4://(�����ԣ�
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount
				&&TackOutCount / 2 == GetBulkBySpecifyCardCount(iCardList, iCardCount, 2))
				return IsVariationSequence(iBuffer, TackOutCount, 4);
			break;
	}
	return FALSE;
}

//��˳����
BOOL CGameLogic::IsFourXSequence(BYTE iCardList[], int iCardCount, int iSeqX)
{
	if (iCardCount < 8)		//��˳����2
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
		case 1://������
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 5 == iCardCount)
			{
				return IsSequence(iBuffer, TackOutCount, 4);
			}
			break;
		case 2://������(���ԷǶ��ӣ�
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount)
			{
				return IsSequence(iBuffer, TackOutCount, 4);
			}
			break;
		case 3://��һ��
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount 	&&TackOutCount / 4 == GetBulkBySpecifyCardCount(iCardList, iCardCount, 2))
			{
				return IsSequence(iBuffer, TackOutCount, 4);
			}
			break;
		case 4://(�����ԣ�
			TackOutCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iBuffer, 4);
			if (TackOutCount > 0 && TackOutCount / 4 * 6 == iCardCount&&TackOutCount / 2 == GetBulkBySpecifyCardCount(iCardList, iCardCount, 2))
			{
				return IsSequence(iBuffer, TackOutCount, 4);
			}
			break;
	}
	return FALSE;
}

//����һ���ȵ�ǰ���
BOOL CGameLogic::TackOutCardMoreThanLast(BYTE iHandCard[], int iHandCardCount,
										 BYTE iBaseCard[], int iBaseCardCount,
										 BYTE iResultCard[], int &iResultCardCount,
										 bool bSingleOrder)
{
	BYTE iTempCard[54];
	iResultCardCount = 0;
	int iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //�������Ƶ�����

	switch (iBaseShape)
	{
		case UG_ONLY_ONE: //����
		{
			//����ֻʣը����һ���ƣ�ֱ�Ӹ�ը��
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

			//���Ƹ���--���ж����еĵ����ܲ��ܴ��
			BYTE tSingleCard[20] = { 0 };
			BYTE tSingleCardCount = 0;
			memcpy(tSingleCard, m_single_info, sizeof(m_single_info));
			tSingleCardCount = m_single_count;

			BYTE tOutCard = 0x00;
			//����Լ�ֻʣһ���ƣ������ĵ��ƿ�ʼ��
			if (bSingleOrder)//�¼���ׯ��ֻʣһ����
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
		case UG_DOUBLE:   //����
		case UG_THREE:    //����
		{
			//����ֻʣը����һ���ƣ�ֱ�Ӹ�ը��
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
		case UG_BOMB:	//���� ը��
		{
			//����1,2,3,or4����
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
		case UG_THREE_ONE: //����һ
		{
			if (TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 1))
				return TRUE;
			break;
		}
		case UG_THREE_TWO: //��������
		{
			if (TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 2))
				return TRUE;
			break;
		}
		case UG_THREE_DOUBLE:	//����һ��
		{
			if (TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 3))
				return TRUE;
			break;
		}
		case UG_STRAIGHT: //˳��
		{
			if (TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 1))
			{
				return TRUE; //�����Ƶ���˳��
			}
			break;
		}
		case UG_STRAIGHT_FLUSH: //ͬ��˳
		{
			if (TackOutStraightFlush(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
				return TRUE;
			break;
		}
		case UG_DOUBLE_SEC: //����
		{
			if (TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 2))
			{
				return TRUE; //�����Ƶ���˳��
			}
			break;
		}
		case UG_THREE_SEQUENCE: //����
		{
			if (TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 3))
			{
				return TRUE;
			}
			break;
		}
		case UG_THREE_ONE_SEQUENCE: //����һ������
		{
			if (TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 1))
			{
				return TRUE;
			}
			break;
		}
		case UG_THREE_TWO_SEQUENCE: //������������
		{
			if (TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, 2))
			{
				return TRUE;
			}
			break;
		}
		case UG_THREE_DOUBLE_SEC://����������
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
	if (iResultCardCount == 0) //û�ҵ�ͬ���͵Ĵ���,���Ҵ�һ�������
	{
		switch (iBaseShape)
		{
			case UG_ONLY_ONE: //���Բ����,��������ѹ���ƻ��߶���
			case UG_DOUBLE:
			{
				if (TackOutCardByNoSameShape(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount))
					return TRUE;
				break;
			}
			case UG_BOMB:
			{
				//����û�ҵ���ͬ�����Ĵ�ը��,���������������
				if (TackOutBomb(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCardCount + 1))
					return TRUE;
			}
				break;
			default: //����Ҳ�����Ķ��ӵ��ƾ��Ҵ������,warning�˴�����break;
				break;
		}
	}
	if (iResultCardCount == 0)
	{
		TackOutMoreThanLastShape(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount);
	}
	return TRUE;
}

//ֱ����ȡ�������ϴ������
BOOL  CGameLogic::TackOutMoreThanLastShape(BYTE iCardList[], int iCardCount,
										   BYTE iResultCard[], int &iResultCardCount,
										   BYTE iBaseCard[], int iBaseCardCount)
{
	iResultCardCount = 0;
	TackOutBomb(iCardList, iCardCount, iResultCard, iResultCardCount); //��ը��

	return true;
}

//��ȡ����������0, 1 or 2 ���״����Ǽ�,�� iBaseCount-3 ������
BYTE CGameLogic::TackOutThreeX(BYTE iCardList[], int iCardCount,
							   BYTE iBaseCard[], int iBaseCount,
							   BYTE iResultCard[], int &iResultCount, int iValue)
{
	iResultCount = 0;
	if (iCardCount < iBaseCount)
		return FALSE;
	BYTE iTempCard[54];
	int threecard = GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 3);//���������ŵĵ���
	//3�����ܸ���
	BYTE iCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iTempCard, 3);

	if (iCount > 0)//��ȡ�������������
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

		BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, byCardTemp, 3, true);//����ֵ��ȥ
		//if(Step == 0)
		//	return FALSE;
		memcpy(iResultCard, &iTempCard[Step], sizeof(BYTE) * 3);
		//TCHAR sz[200];
		//wsprintf(sz,"Step=%d,iBaseCount=%d",Step,iBaseCount);
		//WriteStr(sz,7,7);

		//if(CompareOnlyOne(iBaseCard[0], iResultCard[0]))			//���ڴ�������step����Ϊ0�ý���һ�αȽϴ���
		if (threecard >= GetBulkBySpecifyCardCount(iResultCard, 3, 3))
			return FALSE;
		//iResultCount = 3;
		//else
		//	return FALSE;
	}
	else
		return FALSE;
	//��ԭֵ����
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
			if (iCount >= destCount)//���ҵ�����
			{
				memcpy(&iResultCard[3], iTempCard, sizeof(BYTE)*destCount);
				iResultCount = iBaseCount;
				break;
			}
			//�����������
			iCount = TackOutBySepcifyCardNumCount(Tmp, iTempCount, iTempCard, 2);
			if (iCount >= destCount)
			{
				memcpy(&iResultCard[3], iTempCard, sizeof(BYTE)*destCount);
				iResultCount = iBaseCount;
				break;
			}

			//��������������
			iCount = TackOutBySepcifyCardNumCount(Tmp, iTempCount, iTempCard, 3);
			if (iCount < 3)//��һ�����޷���
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
			//��������������
			iCount = TackOutBySepcifyCardNumCount(Tmp, iTempCount, iTempCard, 3);
			if (iCount < 3)//��һ�����޷���
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
//��ȡ����
BOOL  CGameLogic::TrackOut3Sequence2Sequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount)
{
	iResultCardCount = 0;
	if (iCardCount < iBaseCount)	//��������
		return false;
	BYTE tmpBaseCard[54];//,destCard[54];
	int tmpbaseCardCount = 0, destCardCount = 0;
	//�������Ƶ������������
	tmpbaseCardCount = TackOutBySepcifyCardNumCount(iBaseCard, iBaseCount, tmpBaseCard, 3);
	if (tmpbaseCardCount < 6)	//������������
		return FALSE;
	//����ȡ����������˳
	if (!TackOutSequence(iCardList, iCardCount, tmpBaseCard, tmpbaseCardCount, iResultCard, iResultCardCount, 3))
		return FALSE;
	//�����Ƹ���һ��(�Ƴ���˳��)
	BYTE TMP[54];
	int TmpCount = iCardCount;
	memcpy(TMP, iCardList, sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard, iResultCardCount, TMP, TmpCount);
	TmpCount -= iResultCardCount;
	destCardCount = iBaseCount - iResultCardCount;	//��������

	BYTE twoList[54];
	int twoCount;
	//�������ƵĶ�˳�������
	tmpbaseCardCount = TackOutBySepcifyCardNumCount(iBaseCard, iBaseCount, tmpBaseCard, 2);
	if (!TackOutSequence(TMP, TmpCount, tmpBaseCard, tmpbaseCardCount, twoList, twoCount, 2, true))
		return false;
	//	int TwoSequenceLen = (iBaseCount- tmpbaseCardCount)/2;
	//	tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,3);
	memcpy(&iResultCard[iResultCardCount], twoList, sizeof(BYTE)*twoCount);
	iResultCardCount += twoCount;
	return true;
}
//��ȡָ��������˳
BOOL  CGameLogic::TrackOut3XSequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int xValue)
{
	iResultCardCount = 0;
	if (iCardCount < iBaseCount)	//��������
		return false;
	BYTE tmpBaseCard[54];//,destCard[54];
	int tmpbaseCardCount = 0, destCardCount = 0;
	//�������Ƶ������������
	tmpbaseCardCount = TackOutBySepcifyCardNumCount(iBaseCard, iBaseCount, tmpBaseCard, 3);
	if (tmpbaseCardCount < 6)	//������������
		return FALSE;
	//TCHAR sz[200];
	//wsprintf(sz,"��˳����ȡ֮ǰ%d",iResultCardCount);
	//WriteStr(sz);	
	//����ȡ����������˳
	if (!TackOutSequence(iCardList, iCardCount, tmpBaseCard, tmpbaseCardCount, iResultCard, iResultCardCount, 3))
		return FALSE;
	//TCHAR sz[200];
	//wsprintf(sz,"��˳����ȡ�ɹ�%d",iResultCardCount);
	//WriteStr(sz);
	//�����Ƹ���һ��
	BYTE TMP[54];
	int TmpCount = iCardCount;
	memcpy(TMP, iCardList, sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard, iResultCardCount, TMP, TmpCount);
	TmpCount -= iResultCardCount;
	destCardCount = iBaseCount - iResultCardCount;	//��������

	switch (xValue)
	{
		case 1:
		case 2:
		{
			tmpbaseCardCount = TackOutBySepcifyCardNumCount(TMP, TmpCount, tmpBaseCard, 1);//�յ���
			if (tmpbaseCardCount >= destCardCount)
			{
				memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*destCardCount);//����
				iResultCardCount += destCardCount;
			}
			else
			{
				memcpy(&iResultCard[iResultCardCount], tmpBaseCard, sizeof(BYTE)*tmpbaseCardCount);
				iResultCardCount += tmpbaseCardCount;
				destCardCount -= tmpbaseCardCount;
				tmpbaseCardCount = TackOutBySepcifyCardNumCount(TMP, TmpCount, tmpBaseCard, 2);//�ö��Ʋ�
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
					tmpbaseCardCount = TackOutBySepcifyCardNumCount(TMP, TmpCount, tmpBaseCard, 3);//��������
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
			tmpbaseCardCount = TackOutBySepcifyCardNumCount(TMP, TmpCount, tmpBaseCard, 2);//�ն���
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
				//tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,3);//����������
				TackOutMuchToFew(TMP, TmpCount, tmpBaseCard, tmpbaseCardCount, 3, 2);	//������������ɶ�����
				if (tmpbaseCardCount >= destCardCount)//������Թ���
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

//��д��ȡ���ŵ�˳��,���� or ����
BOOL CGameLogic::TackOutSequence(BYTE iCardList[], int iCardCount, //���е���
								 BYTE iBaseCard[], int iBaseCount,   //������������, �Ƶĸ���
								 BYTE iResultCard[], int &iResultCount, //�ҵ�����
								 int xSequence, BOOL bNoComp)							//˳�ӵĸ���
{
	iResultCount = 0;
	BYTE iTack[54];
	int iTackCount = iCardCount;
	//����һ��
	memcpy(iTack, iCardList, sizeof(BYTE)*iCardCount);
	BYTE iBuffer[54];
	int iBufferCount = 0;
	int iBaseStart, iDestStart = 0, iDestEnd = 0;
	int iSequenceLen = iBaseCount;
	int temp[18] = { 0 };
	int num = 0;
	//��ȡ����ը��(������ɾ������ը��)
	TackOutAllBomb(iTack, iTackCount, iBuffer, iBufferCount);
	RemoveCard(iBuffer, iBufferCount, iTack, iTackCount);
	iTackCount -= iBufferCount;
	//����һ��ϵͳ����������(����������С->�����
	SortCard(iTack, NULL, iTackCount, TRUE);
	//�û����������
	for (int i = 0; i < iTackCount; i++)
	{
		temp[GetCardBulk(iTack[i])]++;
	}

	switch (xSequence)
	{
		//��˳
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
			//��ȡ����
			for (int j = 0; j < iTackCount; j++)
			{
				if (GetCardBulk(iTack[j]) == iDestStart)//�ҵ�һ����
				{
					iResultCard[iResultCount++] = iTack[j];
					iDestStart++;
					iDestEnd--;
					//break;
				}
				//�Ѿ���ȫ
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
			//��ȡ����
			for (int j = 0; j < iTackCount; j++)
			{
				if (GetCardBulk(iTack[j]) == iDestStart)
				{
					iResultCard[iResultCount++] = iTack[j];
					num++;
				}

				if (num == 2)//һ���Ѿ��ҵ�
				{
					num = 0;
					iDestStart++;
					iDestEnd--;
					//�Ѿ���ȫ
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
			//��ȡ����
			for (int j = 0; j < iTackCount; j++)
			{
				if (GetCardBulk(iTack[j]) == iDestStart)
				{
					iResultCard[iResultCount++] = iTack[j];
					num++;

					if (num == 3)//�ҵ�����
					{
						num = 0;
						iDestStart++;
						iDestEnd--;
						//�Ѿ���ȫ
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

//��ȡͬ���
BOOL CGameLogic::TackOutStraightFlush(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount)
{
	iResultCardCount = 0;
	if (iCardCount < iBaseCount)
		return false;
	BYTE iBaseMinCard = GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1);//�����˳������С����
	BYTE iTack[54];
	int iTackCount = iCardCount;
	//����һ��
	memcpy(iTack, iCardList, sizeof(BYTE)*iCardCount);
	BYTE iBuffer[54];
	int iBufferCount = 0;
	int iDestStart = 0, iDestEnd = 0;
	int iSequenceLen = iBaseCount;
	int temp[18] = { 0 };
	int num = 0;
	//��ȡ����ը��(������ɾ������ը��)
	TackOutAllBomb(iTack, iTackCount, iBuffer, iBufferCount);
	RemoveCard(iBuffer, iBufferCount, iTack, iTackCount);
	iTackCount -= iBufferCount;

	SortCard(iTack, NULL, iTackCount, TRUE);

	BYTE iTempKind[54];
	int iTempKindCount = 0;
	//TCHAR sz[200];
	//wsprintf(sz,"iTackCount=%d,iBaseCount=%d",iTackCount,iBaseCount);
	//WriteStr(sz);
	//�û����������
	for (int kind = 0; kind <= 48; kind += 16)
	{	//��ȡ����
		iResultCardCount = 0;
		iTempKindCount = TackOutByCardKind(iTack, iTackCount, iTempKind, kind);
		//wsprintf(sz,"kind=%d,iTempKindCount=%d",kind,iTempKindCount);
		//WriteStr(sz);
		if (iTempKindCount >= iBaseCount)					//��������
		{
			for (int i = 0; i < iTempKindCount; i++)
			{
				temp[GetCardBulk(iTempKind[i])]++;
			}

			for (int i = iBaseMinCard + 1; i < 15; i++)//�Զ������б���
			{
				if (temp[i] >= 1)		//ĳ��ɫ����
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

			if (iDestEnd != iBaseCount)	//ĳ�ֻ�ɫ������,������һ�ֻ�ɫ
				continue;
			//��ȡ����
			for (int j = 0; j < iTempKindCount; j++)
			{
				if (GetCardBulk(iTempKind[j]) == iDestStart)
				{
					iResultCard[iResultCardCount++] = iTempKind[j];
					iDestStart++;
					iDestEnd--;
				}
				//�Ѿ���ȫ
				if (iDestEnd == 0)
					return true;
			}
		}

	}

	return FALSE;
}

//�õ�˳�ӵ���ʼλ��
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

//���
BOOL CGameLogic::TackOutCardByNoSameShape(BYTE iCardList[], int iCardCount, BYTE iResultCard[], int &iResultCardCount, BYTE iBaseCard[], int iBaseCardCount)
{
	iResultCardCount = 0;
	BYTE temp[18] = { 0 };
	int t = GetCardBulk(iBaseCard[0], false); //�õ��������Ǹ��Ƶ�ֵ
	for (int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i], false)]++;
	}

	//��(ը�Ʋ���)
	for (int i = 0; i < 18; i++)
	{
		if (temp[i] < 4 &&               //��ը����
			temp[i] > iBaseCardCount &&  //�����������ƶ�
			i > t)                      //�����ִ�
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
//�Ƿ���Գ���
BOOL CGameLogic::CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], int iBaseCount, bool bFirstOut)
{
	BYTE iOutCardShape = GetCardShape(iOutCard, iOutCount);

	if (iOutCardShape == UG_ERROR_KIND) //���Ͳ���
		return FALSE;

	if (bFirstOut)
		return TRUE;
	BYTE iBaseCardShape = GetCardShape(iBaseCard, iBaseCount); //�����ϵ�����

	if (iBaseCardShape > iOutCardShape)						//����<
	{
		return FALSE;
	}

	if (iBaseCardShape < iOutCardShape)						//����>
	{
		if (UG_BOMB <= iOutCardShape)					//ը��
		{
			return TRUE;
		}
		//����һ��������Ҳ���Դ����˳�Ӻ�˳�Ӵ�С�Ƚ�
		if (iBaseCount != iOutCount) //��������
			return FALSE;

		switch (iBaseCardShape)
		{
			case UG_STRAIGHT:									//ͬ��혴������
			{
				if (iOutCardShape == UG_STRAIGHT_FLUSH)
					return true;
			}
			case UG_VAR_STRAIGHT:							//��С����˳��
			{
				if (iOutCardShape == UG_STRAIGHT)			//����˳������Ч���ֵС������˳���������
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 1) < GetBulkBySpecifySequence(iOutCard, iOutCount, 1);
				return false;
			}
			case UG_VAR_DOUBLE_SEC://��С����˳��
			{
				if (iOutCardShape == UG_DOUBLE_SEC)	//����˳������Ч���ֵС������˳���������
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 2) < GetBulkBySpecifySequence(iOutCard, iOutCount, 2);
				return false;
			}
			case UG_VAR_THREE_SEC:		//������˳
			case UG_VAR_THREE_ONE_SEC://������˳
			case UG_VAR_THREE_TWO_SEC://����������˳
			case UG_VAR_THR_DOUBLE_SEC_1://����������˳
			case UG_VAR_THR_DOUBLE_SEC_2://������˳����˳
			{
				if (iOutCardShape == iBaseCardShape + 1)
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 3) < GetBulkBySpecifySequence(iOutCard, iOutCount, 3);
				return false;
			}
			case UG_VAR_FOUR_SEC:		//������˳
			case UG_VAR_FOUR_ONE_SEC:	//�����Ĵ�һ˳
			case UG_VAR_FOUR_TWO_SEC:	//�����Ĵ���˳
			case UG_VAR_FOUR_ONE_DOUBLE_SEC://�����Ĵ�һ��˳
			case UG_VAR_FOUR_TWO_DOUBLE_SEC://�����Ĵ�����˳
			{
				if (iOutCardShape == iBaseCardShape + 1)
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 4) < GetBulkBySpecifySequence(iOutCard, iOutCount, 4);
				return false;
			}
			case UG_THREE_TWO://����һ������������
			{
				if (iOutCardShape == UG_THREE_DOUBLE)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 3);
				return false;
			}
			case UG_THREE_TWO_SEQUENCE://����һ���(�����)>���������
			{
				if (iOutCardShape == UG_THREE_DOUBLE_SEC || iOutCardShape == UG_THREE_SEC_DOUBLE_SEC)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 3);
				return false;
			}
			case UG_FOUR_TWO://�Ď�һ�����Ď�����
			{
				if (iOutCardShape == UG_FOUR_ONE_DOUBLE)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4);
				return false;
			}
			case UG_FOUR_TWO_SEC://�Ď�һ��혣��Ď������
			{
				if (iOutCardShape == UG_FOUR_ONE_DOUBLE_SEC)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4);
				return false;
			}
			case UG_THREE_DOUBLE_SEC:	//����������혎���
			{
				if (iOutCardShape == UG_THREE_SEC_DOUBLE_SEC)
					return GetBulkBySpecifySequence(iBaseCard, iBaseCount, 3) < GetBulkBySpecifySequence(iOutCard, iOutCount, 3);
				return false;
			}
		}
		return false;
	}

	switch (iBaseCardShape)			//��������һ��
	{
		case UG_ONLY_ONE:  //����
		case UG_DOUBLE:    //����
		case UG_THREE:     //����
		{
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
		}
		case UG_BOMB: //4+�� ը��
		{
			if (iBaseCount > iOutCount) //�������ը����
				return FALSE;

			if (iBaseCount == iOutCount) //������ͬ,�ȵ���
				return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
			return TRUE;
		}

		case UG_FLUSH:			//ͬ��(��˳�ӣ��Ƚ�ͬ����������
		{
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 255) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 255);
		}
		case UG_STRAIGHT_FLUSH: //ͬ��˳
		case UG_STRAIGHT:		//˳��
		case UG_DOUBLE_SEC: //����
		case UG_THREE_SEQUENCE:  //����  
		case UG_FOUR_SEQUENCE:	//��˳
			if (iOutCount != iBaseCount)
				return FALSE;
			{
				return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
			}

		case UG_THREE_ONE:		//����һ
		case UG_THREE_TWO:		//������
		case UG_THREE_DOUBLE:	//������
		{
			return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 3);
		}
		case UG_FOUR_ONE:						//�Ĵ�һ
		case UG_FOUR_TWO:						//�Ĵ���
		case UG_FOUR_ONE_DOUBLE:				//�Ĵ�һ��
		case UG_FOUR_TWO_DOUBLE:				//�Ĵ�����
		{
			return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4);
		}

		case UG_THREE_ONE_SEQUENCE: //2+������һ
		case UG_THREE_TWO_SEQUENCE: //2+��������
		case UG_THREE_DOUBLE_SEC://������˳
		case UG_THREE_SEC_DOUBLE_SEC:		//��˳����˳(����)
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 3));
		}
		case UG_FOUR_ONE_SEC:					//��˳
		case UG_FOUR_TWO_SEC:
		case UG_FOUR_ONE_DOUBLE_SEC:
		case UG_FOUR_TWO_DOUBLE_SEC:
		{
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4));
		}
			//�������δ���
		case UG_VAR_STRAIGHT://��˳
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 1) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount, 1));

			break;
		}
		case UG_VAR_DOUBLE_SEC://��˳
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 2) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount, 2));
			break;
		}
		case UG_VAR_THREE_SEC://��˳
		case UG_VAR_THREE_ONE_SEC://����һ˳
		case UG_VAR_THREE_TWO_SEC://������˳
		case UG_VAR_THR_DOUBLE_SEC_1://������˳
		case UG_VAR_THR_DOUBLE_SEC_2://��˳����˳
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount, 3) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount, 3));
			break;
		}
		case UG_VAR_FOUR_SEC:		//������˳
		case UG_VAR_FOUR_ONE_SEC:	//�����Ĵ�һ˳
		case UG_VAR_FOUR_TWO_SEC:	//�����Ĵ���˳
		case UG_VAR_FOUR_ONE_DOUBLE_SEC://�����Ĵ�һ��˳
		case UG_VAR_FOUR_TWO_DOUBLE_SEC://�����Ĵ�����˳
		{
			if (iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount, 4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount, 4));

			return false;
		}
	}

	return FALSE;
}

//��ȡ����ը��Ϊ�ᷴ��˳,˫˳,��˳��׼��
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

//��ȡը��
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

//��ȡ��ը
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

//���������Ӽ���ϵ
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
//�Ƿ��ǱȽ�����
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
//��ȡѡ������
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
///�Ƿ���һ����
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

//���ͽзֲο���Ϣ
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

	//������
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(byCardList, byCardCount, byReslult, iReslultcount))
	{
		cardfacepoint += 8;
	}
	else//����С�����
	{
		if (temp[16]) cardfacepoint += 3;
		if (temp[17]) cardfacepoint += 4;
	}

	//ը�����
	for (int j = 0; j < 15; j++)
	{
		if (temp[j] == 4)
		{
			cardfacepoint += 6;
		}
	}

	//�ɻ����
	for (int j = 0; j < 13; j++)
	{
		if (temp[j] == 3 && temp[j+1] == 3)
		{
			temp[j+1] = 0;//���һ�����ţ������ظ����
			cardfacepoint += 2;
		}
	}

	//2�ĸ���
	cardfacepoint += (2 * GetCountBySpecifyBulk(byCardList, byCardCount, 15));

	return cardfacepoint;
}

//�������� NTposׯ��λ�� 0������  1���ϼ�  2���¼�
Analysis_Result CGameLogic::AnalyzeHandCard(BYTE byCardList[], BYTE byCardCount, bool bSplitBomb, bool bSaveResult)
{
	ClearTempAnalyseInfo();
	//�������飬����ʹ��
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

	//ת����ֵ
	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	//ȥ����С��
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
	
	//ȥ��˳��(������˳��˫˳���ɻ�����˳��),˳���п��ܲ�ը����bSplitBomb���������Ƿ��
	FindSequence(temp, SequenceListStart, SequenceCount, bSplitBomb);

	//ȥ��ը��
	for (int i = 1; i < 18; i++)
	{
		if (temp[i] == 4)
		{
			boom_info[boom_count++] = i;
			temp[i] = 0;
		}
	}

	//ȥ������
	for (int i = 1; i < 18; i++)
	{
		if (temp[i] == 3)
		{
			triple_info[triple_count++] = i;
			temp[i] = 0;
		}
	}

	//ȥ������
	for (int i = 1; i < 18; i++)
	{
		if (temp[i] == 2)
		{
			double_info[double_count++] = i;
			temp[i] = 0;
		}
	}

	//��ȡ����
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

	//ͳ������--------------------------------------------------------------------------------------------------------------------
	if (have_little_king != have_large_king)//����������С����һ���ƣ�˫����󲻼���
	{
		total_outhand_count++;
	}
	//ը�������Ŵ����߼�
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
			{//�ɻ����Դ���
				if (temp_4 >= count)temp_4 -= count;
				else if (temp_3 >= count)temp_3 -= count;
				else if (((temp_4 + temp_3) >= count) && (count == 2))
				{//���Բ���Ӳ����
					temp_3 = temp_3 - (count - temp_4);
					temp_4 = count - temp_4;
				}
			}
		}
	}
	
	while ((temp_1 + temp_2) > 0)
	{
		//ը�����ƣ����ȴ�����
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
		//�������ӣ��������ƣ�������һ
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
		//��ֵ
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
		m_single_count = single_count;//���е�����
		m_real_single_count = temp_4;//�����ĵ�����
		m_total_outhand_count = total_outhand_count;
	}
	
	Analysis_Result result;

	//��ֵ
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
	result.single_count = single_count;//���е�����
	result.real_single_count = temp_4;//�����ĵ�����
	result.total_outhand_count = total_outhand_count;

	return result;
}

//����˳��
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

	//ȥ��ը��
	for (int i = 0; i < 18; i++)
	{
		if (temp[i] == 4 && !bSplitBomb)
		{
			boom_info[boom_count++] = i;
			temp[i] = 0;
		}
	}

	//ȥ������
	for (int i = 0; i < 18; i++)
	{
		if (temp[i] == 3)
		{
			triple_info[triple_count++] = i;
			temp[i] = 0;
		}
	}

	//�Ȼ�ȡ������˳
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
			SequenceCount[num] = 0x10;//��˳��ʾ
			for (int j = 0; j < 5; j++)
			{
				temp[j + start_card]--;
				Sequence[num][j] = j + start_card;
				SequenceCount[num]++;
			}
		}
		num++;
	} while (num < 4);//����ĸ�˳��

	//��ȡʣ�������Ƿ�����ӵ���˳���е���
	int seq_count = 0;//˳�Ӹ���
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
			if ((temp[Sequence[i][j] + pos] == 1)//�н����ŵ�һ�ŵ���ֱ�����
				|| (temp[Sequence[i][j] + pos] == 2 && temp[Sequence[i][j] + pos + 1] == 1)//����һ�����ӣ����Ҷ����滹��һ�����ţ���ʱ�������˳��
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

	//�ϲ�˳�ӣ������ͷβ������ֱ�Ӻϲ�
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
		//�����ʼλ�û������ƣ���ԭ��һ��Ϊ����
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

	//��ȡ��˳
	for (int i = 0; i < (triple_count - 1); i++)
	{
		if (SequenceCount[seq_count] == 0x00)
		{
			//Q���ϵĶ���������˳
			if (triple_info[i] + 1 == triple_info[i + 1] && triple_info[i] < 12)
			{
				SequenceCount[seq_count] = 0x31;
				Sequence[seq_count][0] = triple_info[i];//��¼��˳
				//���������¼
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
					//���������¼
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

	//��ȡ˫˳,J���ϵ�˫˳������˫˳
	for (int i = 3; i < 11; i++)
	{
		if (SequenceCount[seq_count] == 0x00)
		{
			if (temp[i] == 2 && temp[i + 1] == 2 && temp[i + 2] == 2)
			{
				SequenceCount[seq_count] = 0x21;
				Sequence[seq_count][0] = i;//��¼˫˳
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

	//ʣ���������ӽ���
	for (int i = 0; i < triple_count; i++)
	{
		if (triple_info[i] != 0)
		{
			temp[triple_info[i]] += 3;//���������������
		}
	}

	//��ʣ��������������������˳�����������,�ٴ���ȡ˳��
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
			SequenceCount[seq_count] = 0x10;//��˳��ʾ
			for (int j = 0; j < 5; j++)
			{
				temp[j + start_card]--;
				Sequence[seq_count][j] = j + start_card;
				SequenceCount[seq_count]++;
			}
			seq_count++;
		}
		time++;
	} while (time < 4);//����ĸ�˳��

	//����ȡ����˳���ٺ�ʣ�������
	for (int i = seq_count - 1; i >= 0; i--)
	{
		BYTE type = SequenceCount[i] & 0xF0;
		if (type == 0x10)
		{

			int pos = 1;
			//��ǰѰ��
			while ((temp[Sequence[i][0] - pos] != 0) && ((Sequence[i][0] - pos) < 15))
			{
				if ((temp[Sequence[i][0] - pos] == 1)//�н����ŵ�һ�ŵ���ֱ�����
					|| (temp[Sequence[i][0] - pos] == 2 && temp[Sequence[i][0] - pos - 1] == 1)//����һ�����ӣ����Ҷ��Ӻ��滹��һ�����ţ���ʱ�������˳��
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
			//���Ѱ��
			BYTE j = (SequenceCount[i] & 0x0F) - 0x01;
			while ((temp[Sequence[i][j] + pos] != 0) && ((Sequence[i][j] + pos) < 15))
			{
				if ((temp[Sequence[i][j] + pos] == 1)//�н����ŵ�һ�ŵ���ֱ�����
					|| (temp[Sequence[i][j] + pos] == 2 && temp[Sequence[i][j] + pos + 1] == 1)//����һ�����ӣ����Ҷ��Ӻ��滹��һ�����ţ���ʱ�������˳��
					|| (temp[Sequence[i][j] + pos] == 3 && temp[Sequence[i][j] + pos + 1] == 1))
				{
					temp[Sequence[i][j] + pos]--;
					Sequence[i][j + pos] = Sequence[i][j] + pos;
					SequenceCount[i]++;

					if (temp[Sequence[i][0]] == 1)
					{
						BYTE decrease_Sequence[20] = { 0x00 };
						temp[Sequence[i][0]] ++;
						memcpy(decrease_Sequence, &Sequence[i][1], (sizeof(decrease_Sequence) - sizeof(BYTE)));//�����ʼλ�û������ƣ���ԭ��һ��Ϊ����
						memcpy(&Sequence[i][0], decrease_Sequence, sizeof(decrease_Sequence));
						SequenceCount[i]--;
					}
					j = (SequenceCount[i] & 0x0F) - 0x01;
				}
				else break;
			}
			//���5��˳����6��˳�в𿪺����������ϵ��������Ͳ�
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
			if ((last_single >= ((k + 1) / 2))//������������������+1�ٳ���1����˳��(��2�������ϸ�����)
				|| ((SequenceCount[i] & 0xF0) == 0x10 && (last_single > k - last_couple * 2 - last_single)))//�Ƚϲ𿪺󵥵�����
			{
				//˳�ӻ�ԭ��ȥ
				while (k > 0)
				{
					k--;
					temp[Sequence[i][k]]++;
				}
				//��˳���б����޳�
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
				if (temp[Sequence[i][j] + pos] == 2)//�н����ŵĶ���ֱ�����
				{
					temp[Sequence[i][j] + pos] -= 2;
					Sequence[i][j + pos] = Sequence[i][j] + pos;
					SequenceCount[i]++;

					if (temp[Sequence[i][0]] == 1)
					{
						BYTE decrease_Sequence[20] = { 0x00 };
						temp[Sequence[i][0]] += 2;
						memcpy(decrease_Sequence, &Sequence[i][1], (sizeof(decrease_Sequence) - sizeof(BYTE)));//�����ʼλ�û������ƣ���ԭ��һ��Ϊ����
						memcpy(&Sequence[i][0], decrease_Sequence, sizeof(decrease_Sequence));
						SequenceCount[i]--;
					}
					j = SequenceCount[i] & 0x0F - 0x01;
				}
				else break;
			}
		}
	}

	//�ϲ�˳�ӣ������ͷβ������ֱ�Ӻϲ�
	for (int i = 0; i < 4; i++)
	{
		if (Sequence[i][0] == 0x00)
		{
			seq_count = i;
			break;
		}
		//�����ʼλ�û������ƣ���ԭ��һ��Ϊ����
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

	//���������˳��������ͬ��ʼ���һ,����С˳�ӵ�ͷһ�ſ��Ի�ԭΪ����,��ʱ������˳�ӻ�ԭΪ˫˳������һ
	for (UINT i = 0; i < 4; i++)
	{
		if (SequenceCount[i] == 0x00)
		{//����˳��
			break;
		}
		if ((SequenceCount[i] & 0xF0) != 0x10)
		{//���ǵ�˳
			continue;
		}
		for (UINT j = i+1; j < 4; j++)
		{
			if (SequenceCount[j] == 0x00)
			{
				break;
			}
			if (((SequenceCount[i] & 0xF0) != 0x10) || (SequenceCount[j] != SequenceCount[i]))
			{//���ǵ�˳
				continue;
			}
			if (Sequence[j][0] + 1 == Sequence[i][0] && temp[Sequence[j][0]] == 2)
			{
				//ɾ��������˳,���һ��˫˳
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
				//ɾ��������˳,���һ��˫˳
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

	//��ȡ˫˳
	for (int i = 3; i < 14; i++)
	{
		if (temp[i] == 2 && temp[i + 1] == 2 && temp[i + 2] == 2 && i < 11)
		{
			SequenceCount[seq_count] = 0x23;
			Sequence[seq_count][0] = i;//��¼˫˳
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

	//ɾ������ȡ��������
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

//��ȡ�����еĶ�Ӧ��ֵ
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
//Ѱ��ƥ������
bool CGameLogic::GetMatchCards(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, BYTE iResultCard[], int& iResultCardCount, bool bFellowFriend)
{
	BYTE backup_cardlist[20] = { 0 }; //��������
	BYTE backup_CardCount = 0;	      //������Ŀ
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;
	int iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //�������Ƶ�����

	switch (iBaseShape)
	{
		case UG_ONLY_ONE: //����
		{
			BYTE byStartValue = GetCardBulk(iBaseCard[0]);
			GetSingleCard(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, !bFellowFriend);
			//���ֻʣһ�Ի���һ��,ʣ��ȫ��˳�Ӿ�ֱ�Ӹ���С��
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
		case UG_DOUBLE:   //����
		{
			BYTE byStartValue = GetCardBulk(iBaseCard[0]);
			GetDoubleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, !bFellowFriend);
			break;
		}
		case UG_THREE:    //����
		{
			BYTE byStartValue = GetBulkBySpecifyCardCount(iBaseCard, iBaseCardCount, 3);
			GetTripleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, false);
			break;
		}
		case UG_THREE_ONE: //����һ
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
		case UG_THREE_DOUBLE: //�����������ӣ�
		{
			BYTE byStartValue = GetBulkBySpecifyCardCount(iBaseCard, iBaseCardCount, 3);
			GetTripleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, byStartValue, false);
			GetDoubleCards(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, 0x00, true);
			break;
		}
		case UG_STRAIGHT:	//˳�� 5��+
		{
			if (!bFellowFriend)
			{//Ŀǰֻ�����ֵ�
				GetSequence(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount, false);
			}
			break;
		}
		case UG_DOUBLE_SEC: //˫˳
		{
			break;
		}
		case UG_BOMB:	//���� ը��
		{
			break;
		}
		case UG_FOUR_TWO:	//���� ը��
		{
			break;
		}
		case UG_FOUR_TWO_DOUBLE:	//���� ը��
		{
			break;
		}
		default:
		{
			iResultCardCount = 0;
			break;
		}
	}

	//����ܷ����
	if (!CanOutCard(iResultCard, iResultCardCount, iBaseCard, iBaseCardCount))
	{
		memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
		iResultCardCount = 0;
	}
	else return true;

	return false;
}

//�ж������Ƿ�ֻʣը����һ����
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

	//ȥ����С��
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
	
	//ȥ��ը��
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
	{//����һ����
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

//��������������
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
	memset(m_SequenceCount, 0, sizeof(m_SequenceCount));//0x15��ʾ��˳������0x24��ʾ˫˳4����0x23��ʾ�ɻ�2��
	 
	return;
}
//�����߼�����ɫ���ࣩ
bool CGameLogic::FellowCard(Card_Info card_info, BYTE iResultCard[], int& iResultCardCount, COperateRecord out_record)
{	
	CRecordTool Uplast_card;
	CRecordTool Downlast_card;
	Uplast_card.SetLastCard(card_info.UpUserCard, card_info.UpUserCardCount);
	Downlast_card.SetLastCard(card_info.DownUserCard, card_info.DownUserCardCount);

	bool bBasePosIsNext = false;
	if (card_info.DeskPos == 2)bBasePosIsNext = true;
	if (card_info.NTposition == 0)//����ׯ��
	{
		LandlordFellow(card_info.UserCard, card_info.UserCardCount, card_info.DeskCard, card_info.DeskCardCount,
							bBasePosIsNext, iResultCard, iResultCardCount, Uplast_card, Downlast_card, out_record);
	}
	else if (card_info.NTposition == 1)//�ϼ�ׯ��
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
	else   //�¼�ׯ��
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

//��������
void CGameLogic::LandlordFellow(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);

	BYTE backup_cardlist[20] = { 0 }; //��������
	BYTE backup_CardCount = 0;	      //������Ŀ
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;

	Analysis_Result sPreRS;		 //����ǰ�������
	Analysis_Result sPostRS;	 //���ƺ�������

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //�������Ƶ�����
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//ֻʣը��+��ը(����)+һ����
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

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
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
			//��ԭ
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//�������ֻʣ��ը��һ���ƣ�ֱ����ը
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
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
			//��ԭ
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//����������С��ը����������ֻʣ��ը����ը��ֱ�Ӹ�
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
		//�Լ�ֻʣһ��,��Ҫ���������
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
		//ƥ�����Ϳ��Ը���
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, false);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//�����Ƿ�����ĳ� 
			if (PostHandCount == 1) bOutBigSingle = true;//����¼�ֻʣһ�ţ������ĵ��ƿ�ʼ��
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
		}

		if (iResultCardCount == iHandCardCount)
		{//�ܳ���ֱ�ӳ�
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//�ܳ����ֱ�Ӹ�
			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Uplast_card.IsHaveBoom() < 2 && Downlast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//����
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
		{//����ը����,ը�����ƵĲ�����ǰ���Ѿ��ж�
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//����Լ�ֻʣһ��,����ը������ֱ�Ӹ�
		if (PreHandCount == 1 || PostHandCount == 1)
		{
			return;
		}

		//���Ʋ���
		//���ƶ���
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//��С����ѹ2 , ��������ѹС��
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

			//������Ҫ��˳��ʱ,���Ƕ���ֻʣһ����
			if (sPostRS.iSeqCount < sPreRS.iSeqCount && (PreHandCount > 1 || PostHandCount > 1))
			{
				return;
			}
			//����˳�ӵ�ǰ����,���β���,��ξ�Ҫ��
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && out_record.GetContinuePassCount(iBaseShape) >= 2)
			{
				return;
			}
			
			//�������ٻ������
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				if (GetCardBulk(iResultCard[0], false) <= 14)
				{
					return;
				}
				else
				{
					//ת����ֵ
					BYTE temp[18] = { 0 };
					for (int i = 0; i < backup_CardCount; i++)
					{
						temp[GetCardBulk(backup_cardlist[i], false)]++;
					}

					if (iBaseShape == UG_ONLY_ONE)
					{
						//����ǲ���ը,�Է����ƽ϶�ʱ�����Ȳ���
						if (iResultCard[0] == 0x4E && m_have_large_king && (PreHandCount > 6 && PostHandCount > 6))
						{
							memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
							iResultCardCount = 0;
							return;
						}
						//A���ϵĴ��Ƹ������ڵ��ڵ��Ÿ���
						int count_max = temp[15] + temp[16] + temp[17];
						if (count_max >= m_real_single_count)
						{
							return;
						}
					}
					if(iBaseShape == UG_DOUBLE)
					{
						//A���ϵĴ��Ƹ������ڵ��ڵ��Ÿ���
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
		
		//����(����)
		if (iBaseShape == UG_THREE || iBaseShape == UG_THREE_ONE || iBaseShape == UG_THREE_TWO || iBaseShape == UG_THREE_DOUBLE)
		{
			//������Ҫ��˳��ʱ,���Ƕ���ֻʣһ����
			if (sPostRS.iSeqCount < sPreRS.iSeqCount && (PreHandCount == 1 || PostHandCount == 1))
			{
				return;
			}

			//����˳�ӵ�ǰ����,����������Ȼ���ٻ������β���,��ξ�Ҫ��
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

			//����˳��,���е�С������,С��A,Ҫ��
			if (sPostRS.iSeqCount == sPreRS.iSeqCount
				&& m_real_single_count < 2 
				&& (GetCardBulk(iResultCard[0], false) < 14))
			{
				return;
			}

			//�������ٻ������
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

			//�����������
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		if (iBaseShape == UG_STRAIGHT || iBaseShape == UG_DOUBLE_SEC || iBaseShape == UG_THREE_SEQUENCE 
			|| iBaseShape == UG_THREE_ONE_SEQUENCE || iBaseShape == UG_THREE_DOUBLE_SEC || iBaseShape == UG_THREE_TWO_SEQUENCE)
		{
			//ֻʣһ����
			if ((PreHandCount == 1 || PostHandCount == 1))
			{
				return;
			}

			//�����������ٻ����
			if ((sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				return;
			}

			//�����������
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}
	}

	memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
	iResultCardCount = 0;
	return;
}

//ũ���(�ϼҵ���)
void CGameLogic::FamerFellowPreLD(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Downlast_card.ClearAllData();

	BYTE backup_cardlist[20] = { 0 }; //��������
	BYTE backup_CardCount = 0;	      //������Ŀ
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;

	Analysis_Result sPreRS;		 //����ǰ�������
	Analysis_Result sPostRS;	 //���ƺ�������

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //�������Ƶ�����
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//�������ֻʣ��ը��һ���ƣ�ֱ����ը
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Uplast_card.IsHaveBoom())
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//��ԭ
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//�¼�ֻʣһ��,��������ȥ��
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
			//ת����ֵ
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

	
	//����������С��ը����������ֻʣ��ը����ը��ֱ�Ӹ�
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
		//ƥ�����Ϳ��Ը���
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, false);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//�����Ƿ�����ĳ� 
			if (PostHandCount == 1) bOutBigSingle = true;//����¼�ֻʣһ�ţ������ĵ��ƿ�ʼ��
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
			//�������ը
			if (IsKingBomb(iResultCard, iResultCardCount))
			{
				BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
				int boomCardCount = 2;

				RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
				backup_CardCount -= 2;

				//�Ƿ�һ���ܳ���,����������û���κ�ը��
				if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
					&& 0 == Uplast_card.IsHaveBoom())
				{
					memcpy(iResultCard, byReslult, sizeof(byReslult));
					iResultCardCount = iReslultcount;
					return;
				}
				else
				{
					//��ԭ
					memcpy(backup_cardlist, iHandCard, iHandCardCount);
					backup_CardCount = iHandCardCount;
				}
			}
		}

		if (iResultCardCount == iHandCardCount)
		{//�ܳ���ֱ�ӳ�
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//�ܳ����ֱ�Ӹ�
			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Uplast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//����
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		//����Լ�ֻʣһ��,����ը������ֱ�Ӹ�
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
		{//����ը����,ը�����ƵĲ�����ǰ���Ѿ��ж�
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//���Ʋ���
		//���ƶ���
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//��С����ѹ2 , ��������ѹС��
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

			//������Ҫ��˳��ʱ,���Ƕ���ֻʣһ����,���߶���ֻʣһ����
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPreRS.iSeqCount == 0) && (PreHandCount < 3 || PostHandCount == 1))
			{
				return;
			}
			//����˳�ӵ�ǰ����,���β���,����������С�ڵ���6,��ξ�Ҫ��
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && out_record.GetContinuePassCount(iBaseShape) >= 2 && PreHandCount <= 6)
			{
				return;
			}
			
			//�������ٻ������
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

					//ת����ֵ
					BYTE temp[18] = { 0 };
					for (int i = 0; i < backup_CardCount; i++)
					{
						temp[GetCardBulk(backup_cardlist[i], false)]++;
					}

					if (iBaseShape == UG_ONLY_ONE)
					{
						//����ǲ���ը,�Է����ƽ϶�ʱ�����Ȳ���
						if (iResultCard[0] == 0x4E && m_have_large_king && PreHandCount > 6)
						{
							memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
							iResultCardCount = 0;
							return;
						}
						//A���ϵĴ��Ƹ������ڵ��ڵ��Ÿ���
						int count_max = temp[15] + temp[16] + temp[17]+ m_boom_count;
						if ((PreHandCount <= 6 || iHandCardCount <= 6) && count_max >= m_real_single_count)
						{
							return;
						}
						//�������ֻʣ��һ�ŵ���,���Ը�
						if (m_single_count == 1)
						{
							return;
						}
						//�������ֻʣ����,�ܸ��͸�
						if (m_single_count == (iHandCardCount - 1))
						{
							return;
						}
					}

					if (iBaseShape == UG_DOUBLE)
					{
						//A���ϵĴ��Ƹ������ڵ��ڵ��Ÿ���
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

		//����(����)
		if (iBaseShape == UG_THREE || iBaseShape == UG_THREE_ONE || iBaseShape == UG_THREE_TWO || iBaseShape == UG_THREE_DOUBLE)
		{
			//������Ҫ��˳��ʱ,���Ƕ���ֻʣһ����
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPostRS.total_outhand_count >= sPreRS.total_outhand_count) && (PreHandCount == 1))
			{
				return;
			}

			if (sPostRS.total_outhand_count <= sPreRS.total_outhand_count)
			{
				//�������ƴ���6��,�����ӻ��е�,����С���ӾͲ���
				if ((sPostRS.iAloneDouble + sPostRS.iAloneSingle > 1) && PreHandCount > 6 && (GetCardBulk(iResultCard[0], false) > 14))
				{
					memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
					iResultCardCount = 0;
					return;
				}
				if (PostHandCount < 6)
				{//������������6��Ҫ����
					return;
				}
			}
			if (out_record.GetContinuePassCount(iBaseShape) >= 1 && PostHandCount < 6)
			{
				return;
			}

			//����˳��,���е�С������,С��A,Ҫ��
			if (sPostRS.iSeqCount == sPreRS.iSeqCount
				&& m_real_single_count < 2
				&& (GetCardBulk(iResultCard[0], false) < 14))
			{
				return;
			}

			//�������ٻ������
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				return;
			}

			//�����������
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		if (iBaseShape == UG_STRAIGHT || iBaseShape == UG_DOUBLE_SEC || iBaseShape == UG_THREE_SEQUENCE
			|| iBaseShape == UG_THREE_ONE_SEQUENCE || iBaseShape == UG_THREE_DOUBLE_SEC || iBaseShape == UG_THREE_TWO_SEQUENCE)
		{
			//ֻʣһ����
			if (PreHandCount == 1 || PostHandCount == 1)
			{
				return;
			}

			//�����������ٻ����
			if ((sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				return;
			}

			//�����������
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}
	}

	memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
	iResultCardCount = 0;
	return;
}

//ũ���(�¼ҵ���)
void CGameLogic::FamerFellowPostLD(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Uplast_card.ClearAllData();

	//ͬ��������
	BYTE backup_cardlist[20] = { 0 }; //��������
	BYTE backup_CardCount = 0;	      //������Ŀ
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;

	Analysis_Result sPreRS;		 //����ǰ�������
	Analysis_Result sPostRS;	 //���ƺ�������

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //�������Ƶ�����
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//�������ֻʣ��ը��һ���ƣ�ֱ����ը
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Downlast_card.IsHaveBoom())
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//��ԭ
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//ֻʣը��+��ը(����)+һ����
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

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
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
			//��ԭ
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//����������С��ը����������ֻʣ��ը����ը��ֱ�Ӹ�
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
		//ƥ�����Ϳ��Ը���
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, false);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//�����Ƿ�����ĳ� 
			if (PostHandCount == 1) bOutBigSingle = true;//����¼�ֻʣһ�ţ������ĵ��ƿ�ʼ��
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
			//�������ը
			if (IsKingBomb(iResultCard, iResultCardCount))
			{
				BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
				int boomCardCount = 2;

				RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
				backup_CardCount -= 2;

				//�Ƿ�һ���ܳ���,����������û���κ�ը��
				if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
					&& 0 == Downlast_card.IsHaveBoom())
				{
					memcpy(iResultCard, byReslult, sizeof(byReslult));
					iResultCardCount = iReslultcount;
					return;
				}
				else
				{
					//��ԭ
					memcpy(backup_cardlist, iHandCard, iHandCardCount);
					backup_CardCount = iHandCardCount;
				}
			}
		}

		if (iResultCardCount == iHandCardCount)
		{//�ܳ���ֱ�ӳ�
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//�ܳ����ֱ�Ӹ�,�������ֻʣһ�ŵ�ʱ����Ҫ����(��ըҪ��)
			//ׯ�Ҵ���һ��,��ը
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
			{//��ճ�������
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		//����ֱ����ֱ�ӳ���ֱ�Ӹ�
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
			&& Downlast_card.IsHaveBoom() < 2)
		{
			return;
		}

		//����Լ�ֻʣһ��,����ը������ֱ�Ӹ�
		if (PostHandCount == 1)
		{
			return;
		}

		AnalyzeHandCard(backup_cardlist, backup_CardCount, false);
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPostRS);

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//����ը����,ը�����ƵĲ�����ǰ���Ѿ��ж�
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//���Ʋ���
		//���ƶ���
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//��С����ѹ2 , ��������ѹС��
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

			//������Ҫ��˳��ʱ,���Ƕ���ֻʣһ����
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPostRS.total_outhand_count >= sPreRS.total_outhand_count) && (PostHandCount == 1))
			{
				return;
			}
			//����˳�ӵ�ǰ����,���β���,��ξ�Ҫ��
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && out_record.GetContinuePassCount(iBaseShape) >= 2)
			{
				return;
			}
			//�������ٻ������
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount && (sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				if ((iResultCard[0] == 0x4E || iResultCard[0] == 0x4F) && (PostHandCount > 6 || m_single_count == 0))
				{
					if (sPostRS.iAloneSingle + sPostRS.iAloneDouble < 2 && m_triple_count == 0)
					{//�������С��֮������
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

		//����(����)
		if (iBaseShape == UG_THREE || iBaseShape == UG_THREE_ONE || iBaseShape == UG_THREE_TWO || iBaseShape == UG_THREE_DOUBLE)
		{
			//������Ҫ��˳��ʱ,���Ƕ���ֻʣһ����
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPostRS.total_outhand_count >= sPreRS.total_outhand_count) && (PostHandCount == 1))
			{
				return;
			}

			//����˳�ӵ�ǰ����,����������Ȼ���ٻ������β���,��ξ�Ҫ��
			if (sPostRS.iSeqCount >= sPreRS.iSeqCount)
			{
				if (sPostRS.total_outhand_count <= sPreRS.total_outhand_count)
				{
					//�������ƴ���6��,�����ӻ��е�,����С���ӾͲ���
					if ((sPostRS.iAloneDouble + sPostRS.iAloneSingle > 1) && PreHandCount > 6 && (GetCardBulk(iResultCard[0], false) > 14))
					{
						memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
						iResultCardCount = 0;
						return;
					}
					if (PostHandCount < 6)
					{//������������6��Ҫ����
						return;
					}
				}
				if (out_record.GetContinuePassCount(iBaseShape) >= 2 && PostHandCount < 6)
				{
					return;
				}
			}

			//����˳��,���е�С������,С��A,Ҫ��
			if (sPostRS.iSeqCount == sPreRS.iSeqCount
				&& m_real_single_count < 2
				&& (GetCardBulk(iResultCard[0], false) < 14))
			{
				return;
			}

			//�������ٻ������
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

			//�����������
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		if (iBaseShape == UG_STRAIGHT || iBaseShape == UG_DOUBLE_SEC || iBaseShape == UG_THREE_SEQUENCE
			|| iBaseShape == UG_THREE_ONE_SEQUENCE || iBaseShape == UG_THREE_DOUBLE_SEC || iBaseShape == UG_THREE_TWO_SEQUENCE)
		{
			//ֻʣһ����
			if (PostHandCount == 1)
			{
				return;
			}

			//�����������ٻ����
			if ((sPostRS.total_outhand_count - sPreRS.total_outhand_count) < 2)
			{
				return;
			}

			//�����������
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}
	}

	memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
	iResultCardCount = 0;
	return;
}

//ũ���(�ϼҶ���)
void CGameLogic::FamerFellowPreFM(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Uplast_card.ClearAllData();

	//�е�/���� �͸��ýϴ��ȥ��(A����),�Ƶ����Ĵ���
	//�������Ͳ���,�����Լ��ܳ���

	BYTE backup_cardlist[20] = { 0 }; //��������
	BYTE backup_CardCount = 0;	      //������Ŀ
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;
	Analysis_Result sPreRS;		 //����ǰ�������
	Analysis_Result sPostRS;	 //���ƺ�������

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //�������Ƶ�����
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//�������ֻʣ��ը��һ���ƣ�ֱ����ը
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Downlast_card.IsHaveBoom())
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//��ԭ
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//ֻʣը��+һ����
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

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
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
			//��ԭ
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//����������С��ը����������ֻʣ��ը����ը��ֱ�Ӹ�
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
		//�¼ҵ���ֻʣһ��,�����п��ܷ��ߵ���,��Ҫ���������
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

		//�¼ҵ���ֻʣһ��,�����п��ܷ��ߵ���,��Ҫ���������
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
		//ƥ�����Ϳ��Ը���
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, true);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//�����Ƿ�����ĳ� 
			if (PostHandCount == 1) bOutBigSingle = true;//����¼�ֻʣһ�ţ������ĵ��ƿ�ʼ��
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
			//�������ը
			if (IsKingBomb(iResultCard, iResultCardCount))
			{
				BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
				int boomCardCount = 2;

				RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
				backup_CardCount -= 2;

				//�Ƿ�һ���ܳ���,����������û���κ�ը��
				if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
					&& 0 == Downlast_card.IsHaveBoom())
				{
					memcpy(iResultCard, byReslult, sizeof(byReslult));
					iResultCardCount = iReslultcount;
					return;
				}
				else
				{
					//��ԭ
					memcpy(backup_cardlist, iHandCard, iHandCardCount);
					backup_CardCount = iHandCardCount;
				}
			}
		}

		if (iResultCardCount == iHandCardCount)
		{//�ܳ���ֱ�ӳ�
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//�ܳ����ֱ�Ӹ�
			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Downlast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//����
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		//����ֱ����ֱ�ӳ���ֱ�Ӹ�
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
			&& Downlast_card.IsHaveBoom() < 2)
		{
			return;
		}

		//�¼ҵ���ֻʣһ��,�����ϼҳ��ĵ���
		if (PostHandCount == 1 && iBaseShape == UG_ONLY_ONE)
		{
			//Ѱ�������е������,ը������
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
		{//����ը����,ը�����ƵĲ�����ǰ���Ѿ��ж�
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//���Ʋ���
		//���ƶ���
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//������
			if (iBaseShape == UG_ONLY_ONE)
			{
				for (INT i = 0; i < m_single_count; i++)
				{
					//�ϼҳ�����С��10,�Լ�̧һ��
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

			//���Ӵ���10������
			if (iBaseShape == UG_DOUBLE && (GetCardBulk(iResultCard[0], false) > 10))
			{
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}

			//������Ҫ��˳��ʱ,���Ƕ���ֻʣһ����
			if ((sPostRS.iSeqCount < sPreRS.iSeqCount || sPostRS.total_outhand_count >= sPreRS.total_outhand_count) && (PostHandCount == 1))
			{
				return;
			}

			//�������ٻ������
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

//ũ���(�¼Ҷ���)
void CGameLogic::FamerFellowPostFM(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bBaseisNext,
	BYTE iResultCard[], int& iResultCardCount, CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record)
{
	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PreHandCount = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	BYTE PostHandCount = Downlast_card.GetAllCardIndex(down_card_index);
	Downlast_card.ClearAllData();

	//����������Ҫ�͸�,������������5��ʲô��������,�����Լ��ܳ���

	BYTE backup_cardlist[20] = { 0 }; //��������
	BYTE backup_CardCount = 0;	      //������Ŀ
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;
	Analysis_Result sPreRS;		 //����ǰ�������
	Analysis_Result sPostRS;	 //���ƺ�������

	BYTE iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //�������Ƶ�����
	AnalyzeHandCard(iHandCard, iHandCardCount, false);
	SaveAnalysisResult(iHandCard, iHandCardCount, sPreRS);

	//�������ֻʣ��ը��һ���ƣ�ֱ����ը
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Uplast_card.IsHaveBoom())
		{
			memcpy(iResultCard, byReslult, sizeof(byReslult));
			iResultCardCount = iReslultcount;
			return;
		}
		else
		{
			//��ԭ
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//ֻʣը��+һ����
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

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
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
			//��ԭ
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			memcpy(backup_cardlist, iHandCard, iHandCardCount);
			backup_CardCount = iHandCardCount;
		}
	}

	//����������С��ը����������ֻʣ��ը����ը��ֱ�Ӹ�
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
		//ƥ�����Ϳ��Ը���
		bool bGetRes = GetMatchCards(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, true);

		if (!bGetRes)
		{
			bool bOutBigSingle = false;//�����Ƿ�����ĳ� 
			if (PostHandCount == 1) bOutBigSingle = true;//����¼�ֻʣһ�ţ������ĵ��ƿ�ʼ��
			TackOutCardMoreThanLast(backup_cardlist, backup_CardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, bOutBigSingle);
			//�������ը
			if (IsKingBomb(iResultCard, iResultCardCount))
			{
				BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
				int boomCardCount = 2;

				RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
				backup_CardCount -= 2;

				//�Ƿ�һ���ܳ���,����������û���κ�ը��
				if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, iResultCard, iResultCardCount, Uplast_card, Downlast_card)
					&& 0 == Uplast_card.IsHaveBoom())
				{
					memcpy(iResultCard, byReslult, sizeof(byReslult));
					iResultCardCount = iReslultcount;
					return;
				}
				else
				{
					//��ԭ
					memcpy(backup_cardlist, iHandCard, iHandCardCount);
					backup_CardCount = iHandCardCount;
				}
			}
		}

		if (iResultCardCount == iHandCardCount)
		{//�ܳ���ֱ�ӳ�
			return;
		}

		RemoveCard(iResultCard, iResultCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= iResultCardCount;

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//�ܳ����ֱ�Ӹ�
			if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
				&& Uplast_card.IsHaveBoom() < 2)
			{
				return;
			}
			else
			{//����
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}
		}

		//����ֱ����ֱ�ӳ���ֱ�Ӹ�
		if (CanOutAllCardOnce(backup_cardlist, backup_CardCount, byReslult, iReslultcount, Uplast_card, Downlast_card)
			&& Uplast_card.IsHaveBoom() < 3)
		{
			return;
		}

		//����ϼ�ׯ��ֻʣһ��,��ʱ���ܳ���Ͳ���
		if (PreHandCount == 1)
		{
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		AnalyzeHandCard(backup_cardlist, backup_CardCount, false);
		SaveAnalysisResult(backup_cardlist, backup_CardCount, sPostRS);

		if (GetCardShape(iResultCard, iResultCardCount) >= UG_BOMB)
		{//����ը����,ը�����ƵĲ�����ǰ���Ѿ��ж�
			memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
			iResultCardCount = 0;
			return;
		}

		//���Ʋ���
		//���ƶ���
		if (iBaseShape == UG_ONLY_ONE || iBaseShape == UG_DOUBLE)
		{
			//���Ӵ���10������
			if (iBaseShape == UG_DOUBLE && (GetCardBulk(iResultCard[0], false) > 10))
			{
				memset(iResultCard, 0, (MAX_COUNT*sizeof(BYTE)));
				iResultCardCount = 0;
				return;
			}

			//��������,����С��K�ĵ�����
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

//��ȡ˳�� bCanSplite:�Ƿ������
int CGameLogic::GetSequence(BYTE iHandCard[], BYTE &iHandCardCount, BYTE iResultCard[], int& iResultCardCount, BYTE iBaseCard[], int iBaseCardCount, bool bCanSplite)
{
	BYTE backup_cardlist[20] = { 0 }; //��������
	BYTE backup_CardCount = 0;	      //������Ŀ
	memcpy(backup_cardlist, iHandCard, iHandCardCount);
	backup_CardCount = iHandCardCount;
	//��ȡ�������͵���Ϣ 
	bool find_match = false;
	BYTE base_start = 0x0F;
	BYTE base_count = 0x10 + (BYTE)iBaseCardCount;

	for (int i = 0; i < iBaseCardCount; i++)
	{
		if (mGetCardBulk(iBaseCard[i], false) < base_start)
		{//�ҵ���ʼ�ƴ�С
			base_start = mGetCardBulk(iBaseCard[i], false);
		}
	}

	//���ҿ���ƥ�������
	for (int i = 0; i < 4; i++)
	{
		if (m_SequenceListStart[i] > base_start && m_SequenceCount[i] == base_count)
		{
			//�ҵ�ƥ��˳��
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
//�������� bCanSplite:�Ƿ������ 
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
			//ɾ��������Ϣ
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
//��ȡ���� bCanSplite:�Ƿ������ 
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
		//������
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
						//����������ʣ�µĵ����ܲ�����ӵ���֪��˳����
						if (m_triple_info[i] == (m_SequenceListStart[j] - 1))
						{
							byGetCard = m_triple_info[i];
							//���𿪵ĵ�����ӵ���˳����
							m_SequenceListStart[j] = m_triple_info[i];
							m_SequenceCount[j] += 1;
							//ɾ����������Ϣ
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
							//���𿪵ĵ�����ӵ�˳�ӵ���
							m_SequenceCount[j] += 1;
							//ɾ����������Ϣ
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
		//��˫˳�в���
		int iNum = 0;
		while (iNum < 4)
		{
			//��������������������һ���ܷ����
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
//������ bCanSplite:�Ƿ������ 
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
		//�����
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
						//���˶��ӣ�ʣ�µĵ����ܲ�����ӵ���֪��˳����
						if (m_double_info[i] == (m_SequenceListStart[j] - 1))
						{
							byGetCard = m_double_info[i];
							//���𿪵ĵ�����ӵ�˳�ӵ���
							m_SequenceListStart[j] = m_double_info[i];
							m_SequenceCount[j] += 1;
							//ɾ����������Ϣ
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
							//���𿪵ĵ�����ӵ�˳�ӵ���
							m_SequenceCount[j] += 1;
							//ɾ����������Ϣ
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
		//��˳�����ܲ����ó���һ��
		int iNum = 0;
		while (iNum < 4)
		{
			//�������ţ�����������һ���ܷ����
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
		//������
		for (int i = 0; i < m_triple_count; i++)
		{
			if (m_triple_info[i] > byStartValue)
			{
				int j = 0;
				while (j < 4)
				{
					//��˫˳
					if ((m_SequenceCount[j] & 0xF0) == 0x20)
					{
						BYTE byGetCard = 0x00;
						//����������ʣ�µĶ����ܲ�����ӵ���֪˫˳����
						if ((m_triple_info[i] == (m_SequenceListStart[j] - 1)))
						{
							//���𿪵Ķ�����ӵ�˫˳����
							m_SequenceListStart[j] = m_triple_info[i];
							m_SequenceCount[j] += 1;
							//ɾ����������Ϣ
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
							//���𿪵Ķ�����ӵ�˫˳����
							m_SequenceCount[j] += 1;
							//ɾ����������Ϣ
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

		//�����Ż��߶����е�A����2
		if (m_triple_count > 0)
		{
			for (int i = 0; i < m_triple_count; i++)
			{
				if (m_triple_info[i] > 13 && m_triple_info[i] > byStartValue)
				{//�Ӷ�A��ʼ��
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
				{//�Ӷ�A��ʼ��
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
	//����ͳ��
	int hand_count = 0;

	BYTE up_card_index[CARD_BULK_INDEX] = { 0 };
	UINT up_card_count = Uplast_card.GetAllCardIndex(up_card_index);
	BYTE down_card_index[CARD_BULK_INDEX] = { 0 };
	UINT down_card_count = Downlast_card.GetAllCardIndex(down_card_index);


	//����ͳ��
	UINT max_boom_count = 0;
	BYTE last_max_boom = 0x00;//����ը��
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
			{//ը��
				last_max_boom = i;
				last_boom_count++;
			}
		}
	}

	//���ֻʣ������һ����,����û��ը�������,�ȳ�����
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
		//��ԭ����
		memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
		byOutCardCount = 0;
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
	}
	//����С����һ����,û�д�����ը�������
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
		//��ԭ����
		memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
		byOutCardCount = 0;
		memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
		backup_CardCount = byCardCount;
	}

	//�������ֻʣ��ը��һ���ƣ�ֱ����ը
	BYTE byReslult[20] = { 0 };
	int  iReslultcount = 0;
	if (TackOutKingBomb(backup_cardlist, backup_CardCount, byReslult, iReslultcount))
	{
		BYTE boomCardList[2] = { 0x4e, 0x4f };	//Ҫ���������
		int boomCardCount = 2;

		RemoveCard(boomCardList, boomCardCount, backup_cardlist, backup_CardCount);
		backup_CardCount -= 2;

		//�Ƿ�һ���ܳ���,����������û���κ�ը��
		if ((GetCardShape(backup_cardlist, backup_CardCount) != UG_ERROR_KIND)
			&& 0 == Uplast_card.IsHaveBoom() && 0 == Downlast_card.IsHaveBoom())
		{
			memcpy(byOutCardList, byReslult, sizeof(byReslult));
			byOutCardCount = iReslultcount;
			return true;
		}
		else
		{
			//��ԭ����
			memset(byOutCardList, 0, MAX_COUNT * sizeof(BYTE));
			byOutCardCount = 0;
			memcpy(backup_cardlist, byCardList, sizeof(backup_cardlist));
			backup_CardCount = byCardCount;
			AnalyzeHandCard(byCardList, byCardCount, false);
		}
	}

	//���ֻʣһ����,��ֱ�ӳ���(�Ĵ������⴦��)
	if (GetCardShape(byCardList, byCardCount) != UG_ERROR_KIND)
	{
		if (GetCardShape(byCardList, byCardCount) == UG_FOUR_TWO
			|| GetCardShape(byCardList, byCardCount) == UG_FOUR_TWO_DOUBLE)
		{//�Ĵ�����(��)����ֱ�ӳ����ȳ�һ������(��)
			//�ж�ը���ǲ�������,����Ǿͷֿ���
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

	//���ֻʣը����һ����,����ը�������Ļ���ֻ��һ��ը�����Լ��Ĵ�(���Ҳ�����ը)
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
		//��ԭ����
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
	//��ԭ����
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

	//��Ҫ���������
	UINT Ssin_count = 0;//��˳����
	UINT Sdui_count = 0;//���Ը���
	UINT Stri_count = 0;//�ɻ�����
	UINT take_count[5] = { 0 };//�ɴ����������---(�ɴ�������/����) ����1�����5�� 4*5 = 20
	UINT countfortake = 0;		//take_count�ļ���

	//ͳ��˳������
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

		//Ѱ��ʣ�������Ƿ��д��
		if (up_card_count < seq_count_t && down_card_count < seq_count_t)
		{//���ж���������������Ƿ�����(������ը��)����ֱ��continue
			continue;
		}

		BYTE tt_count = 0;
		int tt_start_index = mGetCardBulk(m_SequenceListStart[i], false) + 1;
		bool find_big = false;
		//�����ϼ�
		while (tt_start_index + tt_count <= 14)
		{//���ڵ���2�Ͳ�����,�������˳��
			if (up_card_index[tt_start_index + tt_count] >= (t_type >> 4))
			{
				tt_count++;
				if (tt_count == t_count)
				{//�ҵ�����ѹס����
					hand_count++;
					find_big = true;
					break;
				}
			}
			else
			{//�ж�˳��,����һ��������
				tt_start_index = tt_start_index + tt_count + 1;
				tt_count = 0;
			}
		}
		//�����¼�
		while (tt_start_index + tt_count <= 14 && find_big)
		{//���ڵ���2�Ͳ�����,�������˳��
			if (down_card_index[tt_start_index + tt_count] >= (t_type >> 4))
			{
				tt_count++;
				if (tt_count == t_count)
				{//�ҵ�����ѹס����
					hand_count++;
					break;
				}
			}
			else
			{//�ж�˳��,����һ��������
				tt_start_index = tt_start_index + tt_count + 1;
				tt_count = 0;
			}
		}
	}

	UNIT triple_count = 0;
	//����
	for (int i = 0; i < m_triple_count; i++)
	{
		for (size_t j = m_triple_info[i]; j < 16; j++)
		{//������2������
			if (up_card_index[j + 1] >= 3 || down_card_index[j + 1] >= 3)
			{
				hand_count++;
				triple_count++;
				break;
			}
		}
		//���Դ�
		take_count[countfortake++] = 1;
	}

	UNIT double_count = 0;
	//����
	for (int i = 0; i < m_double_count; i++)
	{
		for (size_t j = m_double_info[i]; j < 16; j++)
		{//������2����
			if (up_card_index[j + 1] >= 2 || down_card_index[j + 1] >= 2)
			{
				hand_count++;
				double_count++;
				break;
			}
		}
	}

	UNIT single_count = 0;
	//��
	for (int i = 0; i < m_single_count; i++)
	{
		for (size_t j = m_single_info[i]; j < 18; j++)
		{//�����д���������
			if (j >= 17)//����������ը�����
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
		//���õ�����
		if (take_count[i] <= single_count && single_count > 0)
		{
			hand_count -= take_count[i];
			single_count -= take_count[i];
			continue;
		}
		//���ö�����
		if (take_count[i] <= double_count && double_count > 0)
		{
			hand_count -= take_count[i];
			double_count -= take_count[i];
			continue;
		}
	}

	//��ȡ������͸���,û��Ҫ��������Ͳ�������,ʣ���������,���б���Ҫ�����ը��Ҫ��һ��
	if (m_have_large_king && m_have_little_king)
	{
		hand_count--;
		if (m_SequenceCount[0] == 0 && m_triple_count == 0 && m_double_count == 0)
		{//���ֻʣ�����͵��������,��ը���Բ����������
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
		//С��
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
	{//��������1,����һ�ֳ���
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
	//���ʣ��һ��,�޳���һ�����һ�ֳ�
	if (hand_count == 1)
	{
		if (double_count == 1 && m_double_count > 0)
		{
			//������������Ŀ��Թ�,�����е���,���ȳ�����(�����¼ұ�����ʱ�����)
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


	//Ѱ�ҳ���
	//��ȡ�����϶�����ͳ���--------------------------------------------------------------------------------
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
			seq_count_t += count * 0x01;//�ɻ����Դ�
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
		if (m_single_count >= count)//����
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
		else if (m_double_count >= count)//��˫
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
	else if (type == 0x02 || type == 0x01)//��˫˳
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

	//����,m_total_outhand_count == 2 ��ʾֻʣ��������һ����
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

		//�жϴ������Ǵ�����,˭С��˭
		bool three_take_single = true;
		if (m_single_count > 0 && gameconfig.bTakePair && m_double_count > 0)
		{
			//��������ڶ���,������
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
			//����������Щ��,ֻʣһ�Ի���һ�ŵ��Ļ��ͳ�
			if ((backup_CardCount == 1) || (backup_CardCount == 2 && backup_cardlist[0] == backup_cardlist[1]))
			{
				return true;
			}
			if (backup_CardCount == 2 && m_have_large_king && m_have_little_king)
			{
				return true;
			}
			//��ԭ����
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

	//���ֻʣ������һ��,ֱ�ӳ�����
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

//Ѱ�����еĴ���(�����¼ҵ���һ��,���ϼҳ���һ�ŵ������)
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
		{//ը��
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

//�������Ʒ�������
bool CGameLogic::SaveAnalysisResult(BYTE byCardList[], BYTE byCardCount, Analysis_Result &result)
{
	//ת����ֵ
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

//Ѱ�����ֳ�������
BYTE CGameLogic::FindOutCardType(Analysis_Result result, BYTE PreHandCount, BYTE PostHandCount, COperateRecord out_record, 
									shun_info &shun_type, CRecordTool Uplast_card, CRecordTool Downlast_card)
{
	BYTE out_type = UG_ERROR_KIND;

	/*BYTE lastcard_index[CARD_BULK_INDEX] = { 0 };
	UINT lastcard_count = last_card.GetAllCardIndex(lastcard_index);*/

	//��ȡ�����϶��˳������
	int seq_count = 0;// ˳�ӵĸ���
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
				seq_count_t += t_count * 0x01;//�ɻ����Դ������߶���
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

	//˳��ȡ��������ȳ�
	if (shun_type.seq_count > 0)
	{
		if (shun_type.type == 0x03 || shun_type.type == 0x02)
		{//˫˳���߷ɻ���ֱ�ӳ�
			out_type = UG_STRAIGHT;
			return out_type;
		}
		else
		{//��˳
			if (shun_type.seq_end < 0x0A)//С��10��˳��
			{	
				//�����˳������������,����˫˳,��˳��ֱ�ӳ�
				if ((m_SequenceCount[0] & 0xF0) == 0x30 
					|| (m_SequenceCount[0] & 0xF0) == 0x20 
					|| ((m_SequenceCount[0] & 0xF0) == 0x10
						&& (m_SequenceCount[0] & 0x0F) > 0x06))
				{
					out_type = UG_STRAIGHT;
					return out_type;
				}

				//���ֻʣ˳��ֻ����һ��,��ֱ�ӳ�
				if (m_SequenceCount[1] == 0x00)
				{
					out_type = UG_STRAIGHT;
					return out_type;
				}
				//�ж���û�еĹ�
				for (int i = 0; i < 4; i++)
				{
					BYTE other_type = (m_SequenceCount[i] & 0xF0) >> 4;
					BYTE other_count = m_SequenceCount[i] & 0x0F;
					if (other_type == shun_type.type //��ͬ����
						&& (other_count == shun_type.count || other_count == shun_type.count + 1)//������ͬ���߶�һ�ŵĶ���
						&& (m_SequenceListStart[i] + other_count - 0x01) >(shun_type.seq_end + 0x01)) // ��������ű����˳�Ӵ�����
					{//�����˳��
						out_type = UG_STRAIGHT;
						return out_type;
					}
				}
				//�ж���������û�йܵ��ϵ�˳��
				if ((!Uplast_card.IsHaveBigShun((shun_type.seq_end - shun_type.count + 1), shun_type.count))
					&& (!Downlast_card.IsHaveBigShun((shun_type.seq_end - shun_type.count + 1), shun_type.count)))
				{
					out_type = UG_STRAIGHT;
					return out_type;
				}
				//�������˳��û�е�������,���ǳ�˳
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

	//����
	if (m_triple_count > 0)
	{
		//�������������ֻʣ��һ�Ի���һ��,Ҳ����ֱ�ӳ�
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
			//����С��10 �����д�������ܿ��Գ�
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
			{//����ֱ�ӳ�
				out_type = UG_THREE;
				shun_type.seq_count = 4;
			}
		}

		if (m_real_single_count == 0 && m_triple_count >= m_double_count + m_single_count)
		{
			out_type = UG_THREE;
			shun_type.seq_count = 4;
		}

		//�����ĵ�һ��С�ڶ��ӻ��ߵ��ŵĵڶ��ž�ֱ�ӳ�
		if ((m_triple_info[0] < m_single_info[1] && m_single_count > 1) || (m_triple_info[0] < m_double_info[1] && m_double_count > 1))
		{
			out_type = UG_THREE;
			shun_type.seq_count = 4;
		}
	}

	return out_type;
}

//Ѱ�������������е������(�����������),ֻ���ҵ��Ͷ���
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

//Ѱ��Ӧ�ó�����
bool CGameLogic::FindOutCard(BYTE byCardList[], BYTE byCardCount, BYTE byOutCardList[], int &byOutCardCount,
								CRecordTool Uplast_card, CRecordTool Downlast_card, COperateRecord out_record, BYTE NTpos)
{
	//����
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

	//�ж����¼ұ�־λ
	bool Up_flag = (NTpos != 2 ? true : false);
	bool Down_flag = (NTpos != 1 ? true : false);

	//6����ʼ�ж�
	if ((t_UpUserCardCount >= 6 && Up_flag) || (t_DownUserCardCount >= 6 && Down_flag))
	{
		return false;
	}

	//�����������
	Analysis_Result UpAnalyResult;
	UpAnalyResult = AnalyzeHandCard(t_UpUserCard, t_UpUserCardCount, false, false);
	
	Analysis_Result DownAnalyResult;
	DownAnalyResult = AnalyzeHandCard(t_DownUserCard, t_DownUserCardCount, false, false);

	//Ѱ�����û�е����ͻ������͵�����С�������ȳ�
	BYTE out_type = UG_ERROR_KIND;

	//ѭ�����ĸ�˳��
	for (UINT i = 0; i < 4; i++)
	{
		if (m_SequenceCount[i] != 0x00)
		{
			//6�����ϵ�˳�Ӳ��Ҷ���û��˳��ֱ�ӳ�(6�ŵ�˳,˫˳�ͷɻ�)
			if ((((m_SequenceCount[i] & 0x0F) > 5
				|| (m_SequenceCount[i] & 0xF0) > 0x10))
				&& (Up_flag && UpAnalyResult.SequenceCount[0] == 0x00
					|| Down_flag && DownAnalyResult.SequenceCount[0] == 0x00))
			{//�Լ���˳��,����û��,ֱ�ӳ�˳��
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
					{//�ᱻѹ��,���ܳ�
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
					{//�ᱻѹ��,���ܳ�
						out_type = UG_ERROR_KIND;
						break;
					}
				}
			}
			//���Գ�˳�Ӿ�ֱ�ӳ�˳��
			if (out_type == UG_STRAIGHT)
			{
				//---------------------------------------------------------------------------------
			}
		}
	}

	if (m_triple_count > 0 && out_type == UG_ERROR_KIND && m_triple_info[0] < 14)
	{
		out_type = UG_THREE;
		//�ж϶����ܲ���Ҫ����
		if ((Up_flag && UpAnalyResult.triple_count > 0)
			|| (Down_flag && DownAnalyResult.triple_count > 0))
		{
			if (Up_flag && m_triple_info[0] < UpAnalyResult.triple_info[0])
			{//������Ҫ����,
				//�ж��Ƿ���˾ͳ�����
				out_type = UG_THREE_ONE;
				if (m_single_count > 0 && t_UpUserCardCount == 4)
				{
					out_type = UG_ERROR_KIND;
				}
				//�ж��Լ��ܲ���ѹ
				if (m_triple_info[m_triple_count - 1] <= UpAnalyResult.triple_info[0])
				{
					out_type = UG_ERROR_KIND;
				}
				if (m_single_count == 0 && m_double_count > 0)
				{
					out_type = UG_THREE_DOUBLE;
					//�ж��Լ��ܲ���ѹ
					if (m_triple_info[m_triple_count - 1] <= UpAnalyResult.triple_info[0] && UpAnalyResult.double_count == 1)
					{
						out_type = UG_ERROR_KIND;
					}
				}
			}
			if (Down_flag && m_triple_info[0] < DownAnalyResult.triple_info[0])
			{//������Ҫ����,
				//�ж��Ƿ���˾ͳ�����
				out_type = UG_THREE_ONE;
				if (m_single_count > 0 && t_DownUserCardCount == 4)
				{
					out_type = UG_ERROR_KIND;
				}
				//�ж��Լ��ܲ���ѹ
				if (m_triple_info[m_triple_count - 1] <= DownAnalyResult.triple_info[0])
				{
					out_type = UG_ERROR_KIND;
				}
				if (m_single_count == 0 && m_double_count > 0)
				{
					out_type = UG_THREE_DOUBLE;
					//�ж��Լ��ܲ���ѹ
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
		//����(�ö��Ӹ�,��������)
		if (Up_flag && UpAnalyResult.double_count > 0)
		{
			//���ֵĶ��Ӵ����Լ��Ķ���
			if (m_double_info[0] < UpAnalyResult.double_info[0])
			{
				for (int i = 1; i < m_double_count; i++)
				{
					//�Լ��Ķ��Ӵ��ڶ��ֵĶ���
					if (m_double_info[i] > UpAnalyResult.double_info[0])
					{
						out_type = UG_DOUBLE;
						//�ж϶��ֻ���û�ж��ӿ��Ը�
						if (m_double_info[i] < UpAnalyResult.double_info[1])
						{
							for (int j = i + 1; j < m_double_count; j++)
							{
								if (m_double_info[j] < UpAnalyResult.double_info[1])
								{//Ҫ����
									out_type = UG_ERROR_KIND;
								}
								else
								{//Ҫ����
									out_type = UG_DOUBLE;
									break;
								}
							}
						}
						//�ж϶��ֲ��������Ը�
						if (UpAnalyResult.triple_count > 0)
						{
							if (m_double_info[i] < UpAnalyResult.triple_info[0])
							{//���ֵĶ��Ӵ�
								//�Լ��Ļ��ܲ����ö��Ӹ�
								if (m_double_info[m_double_count - 1] > UpAnalyResult.triple_info[0])
								{
									out_type = UG_DOUBLE;
									break;
								}
								else
								{//�Լ��Ķ��ӹܲ�����,���Լ�������
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
						//�����ǲ��Լ�����
					}
				}
			}
		}
		if (Down_flag && DownAnalyResult.double_count > 0)
		{
			//���ֵĶ��Ӵ����Լ��Ķ���
			if (m_double_info[0] < DownAnalyResult.double_info[0])
			{
				for (int i = 1; i < m_double_count; i++)
				{
					//�Լ��Ķ��Ӵ��ڶ��ֵĶ���
					if (m_double_info[i] > DownAnalyResult.double_info[0])
					{
						out_type = UG_DOUBLE;
						//�ж϶��ֻ���û�ж��ӿ��Ը�
						if (m_double_info[i] < DownAnalyResult.double_info[1])
						{
							for (int j = i + 1; j < m_double_count; j++)
							{
								if (m_double_info[j] < DownAnalyResult.double_info[1])
								{//Ҫ����
									out_type = UG_ERROR_KIND;
								}
								else
								{//Ҫ����
									out_type = UG_DOUBLE;
									break;
								}
							}
						}
						//�ж϶��ֲ��������Ը�
						if (DownAnalyResult.triple_count > 0)
						{
							if (m_double_info[i] < DownAnalyResult.triple_info[0])
							{//���ֵĶ��Ӵ�
								//�Լ��Ļ��ܲ����ö��Ӹ�
								if (m_double_info[m_double_count - 1] > DownAnalyResult.triple_info[0])
								{
									out_type = UG_DOUBLE;
									break;
								}
								else
								{//�Լ��Ķ��ӹܲ�����,���Լ�������
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
						//�����ǲ��Լ�����
					}
				}
			}
		}
	}

	if (m_single_count > 0 && out_type == UG_ERROR_KIND)
	{
		out_type = UG_ONLY_ONE;
		//����(�õ��Ƹ�,������,�����)
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
			///---------����˫-----------------------------
			if (shun_count == 0x03)
			{//��������
				//����С��������(���ŵ�,���ŵ�,һ��һ��)
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
								//û�е�ֱ���ö���
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
			{//������˳
				shun_count = 0x02;//�����ǳ�����˳�Ĳ��
				if (m_single_count >= shun_count)//����
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
				else if (m_double_count >= shun_count)//��˫
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
		else if (shun_type == 0x02 || shun_type == 0x01)//��˫˳
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

//----------------------------���Ʋ���------------------------------------------


//���ֳ���
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
		//�����Ƿ���һ���Գ���
		if (!CanOutAllCardOnce(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card))
		{
			LandlordFirst(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record);
		}		
	}
	else if (1 == card_info.NTposition)
	{
		Uplast_card.SetLastCard(card_info.UpUserCard, card_info.UpUserCardCount);
		Downlast_card.SetLastCard(card_info.DownUserCard, card_info.DownUserCardCount);
		//�����Ƿ���һ���Գ���
		if (!CanOutAllCardOnce(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card))
		{
			FamerFirstBefFM(card_info.UserCard, card_info.UserCardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record);
		}
	}
	else if (2 == card_info.NTposition)
	{
		Uplast_card.SetLastCard(card_info.UpUserCard, card_info.UpUserCardCount);
		Downlast_card.SetLastCard(card_info.DownUserCard, card_info.DownUserCardCount);
		//�����Ƿ���һ���Գ���
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

//��������
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

	//Ѱ���Ƿ�ᵼ�¶���һ�γ���()
	bool already_find = false;
	//already_find = m_DeskPositon(backup_cardlist, backup_CardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record, 0);

	if (already_find)
	{
		//Ѱ�ҵ�����,ֱ�ӳ�
		return;
	}

	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	//��ȡ���ڵ���2��������
	int temp_king_hand_count = 0;//������������
	if (m_have_little_king != m_have_large_king) temp_king_hand_count = 1;


	int bigcardcount = temp[15] + temp[16] + temp[17];
	if ((m_real_single_count > 0 && bigcardcount > 3)
		&& (PreHandCount > 1 && PostHandCount > 1))
	{
		if (m_single_info[0] < 14)//����A�Ȳ���
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

	//��ȡ�����϶��˳������
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
			//�����һ��A���2���ҵ��ƴ���2��,���ȳ�����
		if ((PreHandCount != 1 && PostHandCount != 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				out_type = UG_ONLY_ONE;
			}
		else if ((PreHandCount == 1 || PostHandCount == 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				inverted = true;//��ʱҪ�������
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
		//������������ٲ��Ҷ�����С��10�ľ��ȳ�����
		if (m_real_single_count <= 2 && m_double_count > 0 && m_double_info[0] < 10)
		{
			out_type = UG_DOUBLE;
		}
	}

	BYTE last_out_type = out_record.GetLastOutType();
	//����ϴ�������Ҷ���Ҫ�����ͻ��оͼ�������������
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
			{//����С��A
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

	//�������û��
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

	//���ݳ������ͳ���
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
			if (m_single_count >= shun_data.count)//����
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
			else if (m_double_count >= shun_data.count && (m_triple_info[0] < 15 || ((m_total_outhand_count - m_boom_count - temp_king_hand_count) == 1)))//��˫
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
		else if (shun_data.type == 0x02 || shun_data.type == 0x01)//��˫˳
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
		//����
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

		//�жϴ������Ǵ�����
		bool three_take_single = true;
		if (m_single_count > 0 && gameconfig.bTakePair && m_double_count > 0)
		{
			//������Ӵ��ڵ�,����
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
		{//����ֻʣһ����ʱ
			t_single_info = m_single_info[m_single_count - 1];				//��ʱ���Ŵ����ĳ�
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

//ũ������(�¼ҵ���)
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

	//Ѱ���Ƿ�ᵼ�¶���һ�γ���()
	bool already_find = false;
	//already_find = FindOutCard(backup_cardlist, backup_CardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record, 0);

	if (already_find)
	{
		//Ѱ�ҵ�����,ֱ�ӳ�
		return;
	}

	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	//��ȡ���ڵ���2��������
	int temp_king_hand_count = 0;//������������
	if (m_have_little_king != m_have_large_king) temp_king_hand_count = 1;

	//������Ѽ�ֻ��һ�ž����¼���,���ҵ�����ֹһ��
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
		if (m_single_info[0] < 14)//����A�Ȳ���
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

	//��ȡ�����϶��˳������
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
			//�����һ��A���2���ҵ��ƴ���2��,���ȳ�����
			if ((PostHandCount != 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				out_type = UG_ONLY_ONE;
			}
			else if ((PostHandCount == 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				inverted = true;//��ʱҪ�������
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

		//������������ٲ��Ҷ�����С��10�ľ��ȳ�����
		if (m_real_single_count <= 2 && m_double_count > 0 && m_double_info[0] < 10)
		{
			out_type = UG_DOUBLE;
		}

		//����������Ѿ��������Ƹ���2�����Ͼ��ȳ���������
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
			//����������Ͷ��Ǵ���K�����;ͼ�������
		}

		if (PostHandCount == 2 && m_single_count > 0)
		{//�������ֻʣ����,�ȳ���
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
	//����ϴ�������Ҷ���Ҫ�����ͻ��оͼ�������������
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
			{//����С��A
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

	//����ϼҶ���ֻʣһ����,����������С��2�ŵ����
	bool bSpliteCard = false;//�Ƿ����
	if (PreHandCount == 2)
	{
		BYTE last_card[HAND_CARD_MAX] = { 0 };
		BYTE LastHandCount = backup_last_card.GetAllCard(last_card);
		
		if (last_card[0] != last_card[1])
		{//���ŵ�
			if (backup_last_card.GetLastMaxCard() >= Downlast_card.GetLastMaxCard())
			{//�����л������,������Ƴ�
				out_type = UG_ONLY_ONE;
				bSpliteCard = true;
			}
		}
		else
		{//����
			if (m_double_count > 0 && m_double_info[m_double_count - 1] < last_card[0])
			{
				out_type = UG_DOUBLE;
			}
			else if (m_triple_count > 0 && m_triple_info[m_triple_count - 1] < last_card[0])
			{//�����л������,������Ƴ�
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
		{//�����л������,������Ƴ�
			out_type = UG_ONLY_ONE;
			bSpliteCard = true;
		}
	}


	//�������û��
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


	//���ݳ������ͳ���
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
			if (m_single_count >= shun_data.count)//����
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
			else if (m_double_count >= shun_data.count && (m_triple_info[0] < 15 || ((m_total_outhand_count - m_boom_count - temp_king_hand_count) == 1)))//��˫
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
		else if (shun_data.type == 0x02 || shun_data.type == 0x01)//��˫˳
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
		//����
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
		//�жϴ������Ǵ�����
		bool three_take_single = true;
		if (m_single_count > 0 && gameconfig.bTakePair && m_double_count > 0)
		{
			//������Ӵ��ڵ�,����
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
		{//�������ֻʣһ��,�����ĳ�
			t_single_info = m_single_info[m_single_count - 1];				//��ʱ���Ŵ����ĳ�
		}

		if (bSpliteCard)
		{//����
			BYTE card_val[HAND_CARD_MAX] = { 0 };
			BYTE card_count = 0;
			if (UG_ONLY_ONE == out_record.GetLastOutType())
			{
				bool re = out_record.GetLastOutCardbyType(UG_ONLY_ONE, card_val, card_count);
				//���ұ�������С�������
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
			{//�ӵڶ����ƿ�ʼ��
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

//ũ������(�¼Ҷ���)
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

	//Ѱ���Ƿ�ᵼ�¶���һ�γ���()
	bool already_find = false;
	//already_find = FindOutCard(backup_cardlist, backup_CardCount, byOutCardList, byOutCardCount, Uplast_card, Downlast_card, out_record, 0);

	if (already_find)
	{
		//Ѱ�ҵ�����,ֱ�ӳ�
		return;
	}

	BYTE temp[18] = { 0 };
	for (int i = 0; i < backup_CardCount; i++)
	{
		temp[GetCardBulk(backup_cardlist[i], false)]++;
	}

	//����¼�ֻ��һ�ž����¼���
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


	//��ȡ���ڵ���2��������
	int temp_king_hand_count = 0;//������������
	if (m_have_little_king != m_have_large_king) temp_king_hand_count = 1;


	int bigcardcount = temp[15] + temp[16] + temp[17];
	if ((m_real_single_count > 0 && bigcardcount > 3)
		&& (PreHandCount > 1 && PostHandCount > 1))
	{
		if (m_single_info[0] < 14)//����A�Ȳ���
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

	//��ȡ�����϶��˳������
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
			//�����һ��A���2���ҵ��ƴ���2��,���ȳ�����
			if ((PreHandCount != 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				out_type = UG_ONLY_ONE;
			}
			else if ((PostHandCount == 1)
				&& (m_double_info[m_double_count - 1] > 13 && (m_real_single_count > 1 || (m_boom_count > 0 && m_single_count > 1))))
			{
				inverted = true;//��ʱҪ�������
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

		//������������ٲ��Ҷ�����С��10�ľ��ȳ�����
		if (m_real_single_count <= 2 && m_double_count > 0 && m_double_info[0] < 10)
		{
			out_type = UG_DOUBLE;
		}

		//����������Ѿ��������Ƹ���2�����Ͼ��ȳ���������
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
			//����������Ͷ��Ǵ���K�����;ͼ�������
		}

		if (PreHandCount == 2 && m_single_count > 0)
		{//�������ֻʣ����,�ȳ���
			out_type = UG_ONLY_ONE;
		}
	}

	BYTE last_out_type = out_record.GetLastOutType();
	//����ϴ�������Ҷ���Ҫ�����ͻ��оͼ�������������
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
			{//����С��A
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

	//�������û��
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

	//���ݳ������ͳ���
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
			if (m_single_count >= shun_data.count)//����
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
			else if (m_double_count >= shun_data.count && (m_triple_info[0] < 15 || ((m_total_outhand_count - m_boom_count - temp_king_hand_count) == 1)))//��˫
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
		else if (shun_data.type == 0x02 || shun_data.type == 0x01)//��˫˳
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
		//����
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
		//�жϴ������Ǵ�����
		bool three_take_single = true;
		if (m_single_count > 0 && gameconfig.bTakePair && m_double_count > 0)
		{
			//������Ӵ��ڵ�,����
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
		if (m_double_info[0] < 14 || m_single_count <= 1)//��2����A�Ȳ���������ֻʣһ��ʱ�ٳ�����Щ�����
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
//���캯��
CRecordTool::CRecordTool(void)
{
	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;
}
//���캯��
CRecordTool::~CRecordTool()
{
	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;
}
//��ʼ��������
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

//���ü�������ֵ
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
//���ü�������ֵ
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
//ɾ����������
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

//�Ƿ���ĳ����
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
////�Ƿ���ը�� 1;ֻ����ͨը�� 2:����ը 3:����ը����ͨը��
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

//����
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

//��ȡ������ֵ
UNIT CRecordTool::GetAllCard(BYTE byCardList[])
{
	if (m_LastCount > HAND_CARD_MAX)
	{
		return 0;
	}

	memcpy(byCardList, m_LastCard, HAND_CARD_MAX * sizeof(BYTE));

	return m_LastCount;
}

//��ȡ����������ֵindex
UNIT CRecordTool::GetAllCardIndex(BYTE byIndex[])
{
	//ת����ֵ
	BYTE temp[CARD_BULK_INDEX] = { 0 };
	for (UINT i = 0; i < m_LastCount; i++)
	{
		temp[mGetCardBulk(m_LastCard[i], false)]++;
	}

	memcpy(byIndex, temp, sizeof(temp));

	return m_LastCount;
}

//��ȡʣ���������
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

//�Ƿ��д�Ĺ���˳��
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

//��������
bool CRecordTool::ClearAllData()
{
	memset(m_LastCard, 0, sizeof(m_LastCard));
	m_LastCount = 0;
	
	return true;
}

//---------------------------------------------------------------------------------------------------------

//���캯��
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
//���캯��
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

//���һ�������Ϣ
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
//���һ�������Ϣ byFriend 0x00 ����  0x01����
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
	m_pass_info[m_pass_count].byExInfo = byFriend;//��������
	m_pass_count++;

	//����һ�ε�����һ��,���һ��ǲ���
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
//��ȡ�������ϴγ�����ֵ
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
//��ȡ�������ϴγ�������
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
//��ȡ�������ϴβ�������ֵ
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
//��ȡ�����ͳ��ƵĴ���
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
//��ȡ�����Ͳ����Ĵ���
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
//��ȡ����ͬ���Ͳ�������
unsigned int COperateRecord::GetContinuePassCount(BYTE iBaseShape)
{
	if (m_pass_info[m_pass_count].byCardType == iBaseShape)
	{
		return m_continue_pass_count;
	}
	return 0;
}
//�����������
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
