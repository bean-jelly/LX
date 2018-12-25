#include <assert.h>
#include <memory>
#include "Windows.h"
#include "robot_ai.h"

const BYTE G_AllCardData[FULL_COUNT] = 
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//���� A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,	//÷�� A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	//���� A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,	//���� A - K
	0x4E, 0x4F,
};

template <typename T>
std::shared_ptr<T> make_shared_array(size_t size)
{
	//default_delete��STL�е�Ĭ��ɾ����
	return std::shared_ptr<T>(new T[size], std::default_delete<T[]>());
}

robotAI::robotAI()
{
	init();
}

robotAI::~robotAI()
{

}

void robotAI::init()
{
	memset(m_UserCardData, 0, sizeof(m_UserCardData));
	memset(m_AllCardData, 0, sizeof(m_AllCardData));
	memset(m_LandScoreCardData, 0, sizeof(m_LandScoreCardData));
	memset(m_UserCardCount, 0, sizeof(m_UserCardCount));
	m_BankerUser = 255;
	m_LanScoreCurrent = 0;
	m_AllCardCount = FULL_COUNT;

	CopyMemory(m_AllCardData, G_AllCardData, FULL_COUNT);
}

BYTE robotAI::GetCardType(const BYTE CardData[], BYTE CardCount)
{
	switch (CardCount)
	{
	case 0:
	{
		return CT_ERROR;
	}
	case 1:
	{
		return CT_SINGLE;
	}
	case 2:
	{
		if (CardData[0] == 0x4F && CardData[1] == 0x4E) return CT_MISSILE_CARD;
		if (GetCardLogicValue(CardData[0]) == GetCardLogicValue(CardData[1])) return CT_DOUBLE;
		return CT_ERROR;
	}
	default:
		break;
	}

	tagAnalyseResult AnalyseResult;
	AnalysebCardData(CardData, CardCount, AnalyseResult);

	if (AnalyseResult.BlockCount[3] > 0)
	{
		if ((AnalyseResult.BlockCount[3] == 1) && (CardCount == 4)) return CT_BOMB_CARD;
		if ((AnalyseResult.BlockCount[3] == 1) && (CardCount == 6)) return CT_FOUR_TAKE_ONE;
		if ((AnalyseResult.BlockCount[3] == 1) && (CardCount == 8) && (AnalyseResult.BlockCount[1] == 2)) return CT_FOUR_TAKE_TWO;
		return CT_ERROR;
	}

	//�����ж�
	if (AnalyseResult.BlockCount[2] > 0)
	{
		//�����ж�
		if (AnalyseResult.BlockCount[2] > 1)
		{
			//��������
			BYTE cbCardData = AnalyseResult.CardData[2][0];
			BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

			//�������
			if (cbFirstLogicValue >= 15) return CT_ERROR;

			//�����ж�
			for (BYTE i = 1; i < AnalyseResult.BlockCount[2]; i++)
			{
				BYTE cbCardData = AnalyseResult.CardData[2][i * 3];
				if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
			}
		}
		else if (CardCount == 3) return CT_THREE;

		//�����ж�
		if (AnalyseResult.BlockCount[2] * 3 == CardCount) return CT_THREE_LINE;
		if (AnalyseResult.BlockCount[2] * 4 == CardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.BlockCount[2] * 5 == CardCount) && (AnalyseResult.BlockCount[1] == AnalyseResult.BlockCount[2])) return CT_THREE_TAKE_TWO;

		return CT_ERROR;
	}

	//��������
	if (AnalyseResult.BlockCount[1] >= 3)
	{
		//��������
		BYTE cbCardData = AnalyseResult.CardData[1][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue >= 15) return CT_ERROR;

		//�����ж�
		for (BYTE i = 1; i < AnalyseResult.BlockCount[1]; i++)
		{
			BYTE cbCardData = AnalyseResult.CardData[1][i * 2];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
		}

		//�����ж�
		if ((AnalyseResult.BlockCount[1] * 2) == CardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//�����ж�
	if ((AnalyseResult.BlockCount[0] >= 5) && (AnalyseResult.BlockCount[0] == CardCount))
	{
		//��������
		BYTE cbCardData = AnalyseResult.CardData[0][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//�������
		if (cbFirstLogicValue >= 15) return CT_ERROR;

		//�����ж�
		for (BYTE i = 1; i < AnalyseResult.BlockCount[0]; i++)
		{
			BYTE cbCardData = AnalyseResult.CardData[0][i];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
		}

		return CT_SINGLE_LINE;
	}

	return CT_ERROR;
}

void robotAI::SortCardList(BYTE CardData[], BYTE CardCount, BYTE SortType)
{
	if (CardCount == 0) return;
	if (SortType == ST_CUSTOM) return;

	BYTE SortValue[MAX_COUNT];
	for (BYTE i = 0; i < CardCount; i++) SortValue[i] = GetCardLogicValue(CardData[i]);

	bool bSorted = true;
	BYTE SwitchData = 0, cbLast = CardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < cbLast; i++)
		{
			if ((SortValue[i] < SortValue[i + 1]) ||
				((SortValue[i] == SortValue[i + 1]) && (CardData[i] < CardData[i + 1])))
			{
				bSorted = false;
				SwitchData = CardData[i];
				CardData[i] = CardData[i + 1];
				CardData[i + 1] = SwitchData;

				SwitchData = SortValue[i];
				SortValue[i] = SortValue[i + 1];
				SortValue[i + 1] = SwitchData;
			}
		}
		cbLast--;
	} while (bSorted == false);

	//��Ŀ����
	if (SortType == ST_COUNT)
	{
		//��������
		BYTE CardIndex = 0;

		//�����˿�
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&CardData[CardIndex], CardCount - CardIndex, AnalyseResult);

		//��ȡ�˿�
		for (BYTE i = 0; i < CountArray(AnalyseResult.BlockCount); i++)
		{
			//�����˿�
			BYTE cbIndex = CountArray(AnalyseResult.BlockCount) - i - 1;
			CopyMemory(&CardData[CardIndex], AnalyseResult.CardData[cbIndex], AnalyseResult.BlockCount[cbIndex] * (cbIndex + 1)*sizeof(BYTE));

			//��������
			CardIndex += AnalyseResult.BlockCount[cbIndex] * (cbIndex + 1)*sizeof(BYTE);
		}
	}

	return;
}

bool robotAI::RemoveCards(const BYTE RemoveCard[], BYTE RemoveCount, BYTE CardData[], BYTE CardCount)
{
	//��������
	if (RemoveCount > CardCount)
	{
		return false;
	}

	//�������
	BYTE DeleteCount = 0, TempCardData[MAX_COUNT];
	if (CardCount > CountArray(TempCardData)) return false;
	BYTE TempRemoveCard[MAX_COUNT];
	CopyMemory(TempCardData, CardData, CardCount*sizeof(CardData[0]));
	CopyMemory(TempRemoveCard, RemoveCard, RemoveCount);

	//�����˿�
	for (BYTE i = 0; i < RemoveCount; i++)
	{
		for (BYTE j = 0; j < CardCount; j++)
		{
			if (TempRemoveCard[i] == TempCardData[j])
			{
				DeleteCount++;
				TempCardData[j] = 0;
				break;
			}
		}
	}
	if (DeleteCount != RemoveCount) return false;

	memset(CardData, 0, CardCount);

	//�����˿�
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i < CardCount; i++)
	{
		if (TempCardData[i] != 0) CardData[cbCardPos++] = TempCardData[i];
	}

	return true;
}

bool robotAI::IsValidCard(BYTE CardData)
{
	BYTE CardColor = GetCardColor(CardData);
	BYTE CardValue = GetCardValue(CardData);

	if ((CardData == 0x4E) || (CardData == 0x4F)) return true;
	if ((CardColor <= 0x30) && (CardValue >= 0x01) && (CardValue <= 0x0D)) return true;

	return false;
}

BYTE robotAI::GetCardLogicValue(BYTE CardData)
{
	if(CardData < 0x01 || CardData > 0x4F)
		return 0x00;
	//�˿�����
	BYTE CardColor = GetCardColor(CardData);
	BYTE CardValue = GetCardValue(CardData);

	//ת����ֵ
	if (CardColor == 0x40) return CardValue + 2;
	return (CardValue <= 2) ? (CardValue + 13) : CardValue;
}

//�Ա��˿�
bool robotAI::CompareCard(const BYTE FirstCard[], const BYTE NextCard[], BYTE FirstCount, BYTE NextCount)
{
	BYTE NextType = GetCardType(NextCard, NextCount);
	BYTE FirstType = GetCardType(FirstCard, FirstCount);

	if (NextType == CT_ERROR)
	{
		return false;
	}
	if (NextType == CT_MISSILE_CARD)
	{
		return true;
	}

	//ը���ж�
	if ((FirstType != CT_BOMB_CARD) && (NextType == CT_BOMB_CARD)) return true;
	if ((FirstType == CT_BOMB_CARD) && (NextType != CT_BOMB_CARD)) return false;

	if ((FirstType != CT_BOMB_CARD) && (NextType == CT_BOMB_CARD)) return true;
	if ((FirstType == CT_BOMB_CARD) && (NextType != CT_BOMB_CARD)) return false;

	if ((FirstType != NextType) || (FirstCount != NextCount)) return false;

	switch (NextType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
	{
		BYTE NextLogicValue = GetCardLogicValue(NextCard[0]);
		BYTE FirstLogicValue = GetCardLogicValue(FirstCard[0]);

		return NextLogicValue > FirstLogicValue;
	}
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
	{
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		AnalysebCardData(NextCard, NextCount, NextResult);
		AnalysebCardData(FirstCard, FirstCount, FirstResult);

		//tagAnalyseResult.CardData[2] is three card list
		BYTE NextLogicValue = GetCardLogicValue(NextResult.CardData[2][0]);
		BYTE FirstLogicValue = GetCardLogicValue(FirstResult.CardData[2][0]);

		return NextLogicValue > FirstLogicValue;
	}
	case CT_FOUR_TAKE_ONE:
	case CT_FOUR_TAKE_TWO:
	{
		tagAnalyseResult NextResult;
		tagAnalyseResult FirstResult;
		AnalysebCardData(NextCard, NextCount, NextResult);
		AnalysebCardData(FirstCard, FirstCount, FirstResult);

		BYTE NextLogicValue = GetCardLogicValue(NextResult.CardData[3][0]);
		BYTE FirstLogicValue = GetCardLogicValue(FirstResult.CardData[3][0]);

		return NextLogicValue > FirstLogicValue;
	}
	default:
		break;
	}

	return false;
}

//��������
bool robotAI::SearchOutCard(
	const BYTE HandCardData[],
	BYTE HandCardCount,
	const BYTE TurnCardData[],
	BYTE TurnCardCount,
	tagOutCardResult & OutCardResult)
{
	memset(&OutCardResult, 0, sizeof(OutCardResult));

	BYTE CardData[MAX_COUNT];
	BYTE CardCount = HandCardCount;
	CopyMemory(CardData, HandCardData, sizeof(BYTE)*HandCardCount);

	SortCardList(CardData, CardCount, ST_ORDER);

	BYTE TurnOutType = GetCardType(TurnCardData, TurnCardCount);

	switch (TurnOutType)
	{
	case CT_ERROR:
	{
		//get value
		BYTE LogicValue = GetCardLogicValue(CardData[CardCount - 1]);
		BYTE SameCount = 1;
		for (BYTE i = 1; i < CardCount; i++)
		{
			if (GetCardLogicValue(CardData[CardCount - i - 1]) == LogicValue) SameCount++;
			else break;
		}

		if (SameCount > 1)
		{
			OutCardResult.CardCount = SameCount;
			for (BYTE j = 0; j < SameCount; j++) OutCardResult.ResultCard[j] = CardData[CardCount - 1 - j];
			return true;
		}

		OutCardResult.CardCount = 1;
		OutCardResult.ResultCard[0] = CardData[CardCount - 1];
		return true;
	}
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_THREE:
	{
		BYTE LogicValue = GetCardLogicValue(TurnCardData[0]);

		tagAnalyseResult AnalyseResult;
		AnalysebCardData(CardData, CardCount, AnalyseResult);

		if (TurnCardCount <= 1)
		{
			for (BYTE i = 0; i < AnalyseResult.BlockCount[0]; i++)
			{
				BYTE Index = AnalyseResult.BlockCount[0] - i - 1;
				if (GetCardLogicValue(AnalyseResult.CardData[0][Index]) > LogicValue)
				{
					OutCardResult.CardCount = TurnCardCount;
					CopyMemory(OutCardResult.ResultCard, &AnalyseResult.CardData[0][Index], sizeof(BYTE)*TurnCardCount);
					return true;
				}
			}
		}
		if (TurnCardCount <= 2)
		{
			for (BYTE i = 0; i < AnalyseResult.BlockCount[1]; i++)
			{
				BYTE Index = (AnalyseResult.BlockCount[1] - i - 1) * 2;
				if (GetCardLogicValue(AnalyseResult.CardData[1][Index]) > LogicValue)
				{
					//���ý��
					OutCardResult.CardCount = TurnCardCount;
					CopyMemory(OutCardResult.ResultCard, &AnalyseResult.CardData[1][Index], sizeof(BYTE)*TurnCardCount);
					return true;
				}
			}
		}
		if (TurnCardCount <= 3)
		{
			for (BYTE i = 0; i < AnalyseResult.BlockCount[2]; i++)
			{
				BYTE Index = (AnalyseResult.BlockCount[2] - i - 1) * 3;
				if (GetCardLogicValue(AnalyseResult.CardData[2][Index]) > LogicValue)
				{
					OutCardResult.CardCount = TurnCardCount;
					CopyMemory(OutCardResult.ResultCard, &AnalyseResult.CardData[2][Index], sizeof(BYTE)*TurnCardCount);
					return true;
				}
			}
		}
		break;
	}
	case CT_SINGLE_LINE:
	{
		if (CardCount < TurnCardCount) break;

		BYTE LogicValue = GetCardLogicValue(TurnCardData[0]);

		for (BYTE i = TurnCardCount - 1; i < CardCount; i++)
		{
			BYTE HandLogicValue = GetCardLogicValue(CardData[CardCount - i - 1]);

			if (HandLogicValue >= 15) break;
			if (HandLogicValue <= LogicValue) continue;

			BYTE LineCount = 0;

			for (BYTE j = CardCount - i - 1; j < CardCount; j++)
			{
				if ((GetCardLogicValue(CardData[j]) + LineCount) == HandLogicValue)
				{
					OutCardResult.ResultCard[LineCount++] = CardData[j];
					if (LineCount == TurnCardCount)
					{
						OutCardResult.CardCount = TurnCardCount;
						return true;
					}
				}
			}
		}
		break;
	}
	case CT_DOUBLE_LINE:
	{
		//�����ж�
		if (CardCount < TurnCardCount) break;

		//��ȡ��ֵ
		BYTE LogicValue = GetCardLogicValue(TurnCardData[0]);

		//��������
		for (BYTE i = (TurnCardCount - 1); i < CardCount; i++)
		{
			//��ȡ��ֵ
			BYTE HandLogicValue = GetCardLogicValue(CardData[CardCount - i - 1]);

			//�����ж�
			if (HandLogicValue <= LogicValue) continue;
			if ((TurnCardCount > 1) && (HandLogicValue >= 15)) break;

			//��������
			BYTE LineCount = 0;
			for (BYTE j = (CardCount - i - 1); j < (CardCount - 1); j++)
			{
				if (((GetCardLogicValue(CardData[j]) + LineCount) == HandLogicValue)
					&& ((GetCardLogicValue(CardData[j + 1]) + LineCount) == HandLogicValue))
				{
					//��������
					OutCardResult.ResultCard[LineCount * 2] = CardData[j];
					OutCardResult.ResultCard[(LineCount++) * 2 + 1] = CardData[j + 1];

					//����ж�
					if (LineCount * 2 == TurnCardCount)
					{
						OutCardResult.CardCount = TurnCardCount;
						return true;
					}
				}
			}
		}
		break;
	}
	case CT_THREE_LINE:
	case CT_THREE_TAKE_ONE:
	case CT_THREE_TAKE_TWO:
	{
		//�����ж�
		if (CardCount < TurnCardCount) break;

		//��ȡ��ֵ
		BYTE LogicValue = 0;
		for (BYTE i = 0; i < TurnCardCount - 2; i++)
		{
			LogicValue = GetCardLogicValue(TurnCardData[i]);
			if (GetCardLogicValue(TurnCardData[i + 1]) != LogicValue) continue;
			if (GetCardLogicValue(TurnCardData[i + 2]) != LogicValue) continue;
			break;
		}

		//������ֵ
		BYTE TurnLineCount = 0;
		if (TurnOutType == CT_THREE_TAKE_ONE) TurnLineCount = TurnCardCount / 4;
		else if (TurnOutType == CT_THREE_TAKE_TWO) TurnLineCount = TurnCardCount / 5;
		else TurnLineCount = TurnCardCount / 3;

		//��������
		for (BYTE i = TurnLineCount * 3 - 1; i < CardCount; i++)
		{
			//��ȡ��ֵ
			BYTE HandLogicValue = GetCardLogicValue(CardData[CardCount - i - 1]);

			//�����ж�
			if (HandLogicValue <= LogicValue) continue;
			if ((TurnLineCount > 1) && (HandLogicValue >= 15)) break;

			//��������
			BYTE LineCount = 0;
			for (BYTE j = (CardCount - i - 1); j < (CardCount - 2); j++)
			{
				//���ñ���
				OutCardResult.CardCount = 0;

				//�����ж�
				if ((GetCardLogicValue(CardData[j]) + LineCount) != HandLogicValue) continue;
				if ((GetCardLogicValue(CardData[j + 1]) + LineCount) != HandLogicValue) continue;
				if ((GetCardLogicValue(CardData[j + 2]) + LineCount) != HandLogicValue) continue;

				//��������
				OutCardResult.ResultCard[LineCount * 3] = CardData[j];
				OutCardResult.ResultCard[LineCount * 3 + 1] = CardData[j + 1];
				OutCardResult.ResultCard[(LineCount++) * 3 + 2] = CardData[j + 2];

				//����ж�
				if (LineCount == TurnLineCount)
				{
					//��������
					OutCardResult.CardCount = LineCount * 3;

					//�����˿�
					BYTE LeftCardData[MAX_COUNT];
					BYTE LeftCount = CardCount - OutCardResult.CardCount;
					CopyMemory(LeftCardData, CardData, sizeof(BYTE)*CardCount);
					RemoveCards(OutCardResult.ResultCard, OutCardResult.CardCount, LeftCardData, CardCount);

					//�����˿�
					tagAnalyseResult AnalyseResultLeft;
					AnalysebCardData(LeftCardData, LeftCount, AnalyseResultLeft);

					//���ƴ���
					if (TurnOutType == CT_THREE_TAKE_ONE)
					{
						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[0]; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.CardCount == TurnCardCount) break;

							//�����˿�
							BYTE Index = AnalyseResultLeft.BlockCount[0] - k - 1;
							BYTE SignedCard = AnalyseResultLeft.CardData[0][Index];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = SignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[1] * 2; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.CardCount == TurnCardCount) break;

							//�����˿�
							BYTE Index = (AnalyseResultLeft.BlockCount[1] * 2 - k - 1);
							BYTE SignedCard = AnalyseResultLeft.CardData[1][Index];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = SignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[2] * 3; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.CardCount == TurnCardCount) break;

							//�����˿�
							BYTE Index = (AnalyseResultLeft.BlockCount[2] * 3 - k - 1);
							BYTE SignedCard = AnalyseResultLeft.CardData[2][Index];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = SignedCard;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[3] * 4; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.CardCount == TurnCardCount) break;

							//�����˿�
							BYTE cbIndex = (AnalyseResultLeft.BlockCount[3] * 4 - k - 1);
							BYTE cbSignedCard = AnalyseResultLeft.CardData[3][cbIndex];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = cbSignedCard;
						}
					}

					//���ƴ���
					if (TurnOutType == CT_THREE_TAKE_TWO)
					{
						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[1]; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.CardCount == TurnCardCount) break;

							//�����˿�
							BYTE Index = (AnalyseResultLeft.BlockCount[1] - k - 1) * 2;
							BYTE CardData1 = AnalyseResultLeft.CardData[1][Index];
							BYTE CardData2 = AnalyseResultLeft.CardData[1][Index + 1];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData1;
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData2;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[2]; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.CardCount == TurnCardCount) break;

							//�����˿�
							BYTE Index = (AnalyseResultLeft.BlockCount[2] - k - 1) * 3;
							BYTE CardData1 = AnalyseResultLeft.CardData[2][Index];
							BYTE CardData2 = AnalyseResultLeft.CardData[2][Index + 1];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData1;
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData2;
						}

						//��ȡ����
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[3]; k++)
						{
							//��ֹ�ж�
							if (OutCardResult.CardCount == TurnCardCount) break;

							//�����˿�
							BYTE Index = (AnalyseResultLeft.BlockCount[3] - k - 1) * 4;
							BYTE CardData1 = AnalyseResultLeft.CardData[3][Index];
							BYTE CardData2 = AnalyseResultLeft.CardData[3][Index + 1];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData1;
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData2;
						}
					}

					//����ж�
					if (OutCardResult.CardCount == TurnCardCount) return true;
				}
			}
		}
		break;
	}

	default:
		break;
	}
	if ((CardCount >= 4) && (TurnOutType != CT_MISSILE_CARD))
	{
		BYTE LogicValue = 0;
		if (TurnOutType == CT_BOMB_CARD) LogicValue = GetCardLogicValue(TurnCardData[0]);

		for (BYTE i = 3; i < CardCount; i++)
		{
			BYTE HandLogicValue = GetCardLogicValue(CardData[CardCount - i - 1]);

			if (HandLogicValue <= LogicValue) continue;

			BYTE TempLogicValue = GetCardLogicValue(CardData[CardCount - i - 1]);
			for (BYTE j = 1; j < 4; j++)
			{
				if (GetCardLogicValue(CardData[CardCount+j-i-1]) != TempLogicValue) break;
				if (j != 4) continue;
			}

			OutCardResult.CardCount = 4;
			OutCardResult.ResultCard[0] = CardData[CardCount - i - 1];
			OutCardResult.ResultCard[1] = CardData[CardCount - i];
			OutCardResult.ResultCard[2] = CardData[CardCount - i + 1];
			OutCardResult.ResultCard[3] = CardData[CardCount - i + 2];

			return true;
		}
	}
	if ((CardCount >= 2) && (CardData[0] == 0x4F) && (CardData[0] == 0x4E))
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = CardData[0];
		OutCardResult.ResultCard[1] = CardData[1];
		return true;
	}

	return false;
}

void robotAI::AnalysebCardData(const BYTE CardData[], BYTE CardCount, tagAnalyseResult &AnalyseResult)
{
	memset(&AnalyseResult, 0, sizeof(tagAnalyseResult));

	for (BYTE i = 0; i < CardCount; i++)
	{
		BYTE SameCount = 1, CardValueTemp = 0;
		BYTE LogicValue = GetCardLogicValue(CardData[i]);

		for (BYTE j = i + 1; j < CardCount; j++)
		{
			if (GetCardLogicValue(CardData[j]) != LogicValue)
			{
				break;
			}
			SameCount++;
		}
		BYTE Index = AnalyseResult.BlockCount[SameCount - 1]++;
		for (BYTE j = 0; j < SameCount; j++)
		{
			AnalyseResult.CardData[SameCount - 1][Index*SameCount + j] = CardData[i + j];
		}

		i += SameCount - 1;
	}
}

void robotAI::AnalysebDistributing(const BYTE CardData[], BYTE CardCount, tagDistributing &Distributing)
{
	memset(&Distributing, 0, sizeof(Distributing));

	for (BYTE i = 0; i < CardCount; i++)
	{
		if (CardData[i] == 0) continue;

		BYTE CardColor = GetCardColor(CardData[i]);
		BYTE CardValue = GetCardValue(CardData[i]);

		Distributing.CardCount++;
		Distributing.Distributing[CardValue - 1][5]++;
		Distributing.Distributing[CardValue - 1][CardColor >> 4]++;
	}
}

bool robotAI::SearchOutCard(const BYTE HandCardData[], BYTE HandCardCount, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, BYTE MeChairID, tagOutCardResult &OutCardResult)
{
	BYTE UndersideOfBanker = (m_BankerUser + 1) % GAME_PLAYER;
	BYTE UpsideOfBanker = (UndersideOfBanker + 1) % GAME_PLAYER;

	memset(&OutCardResult, 0, sizeof(OutCardResult));

	//�ȳ���
	if (TurnCardCount == 0)
	{
		//��������
		if (MeChairID == m_BankerUser)
		{
			BankerOutCard(MeChairID, OutCardResult);
		}
		//�����¼ҳ���
		else if (MeChairID == UndersideOfBanker)
		{
			UndersideOfBankerOutCard(MeChairID, OutCardResult);
		}
		//�����ϼҳ���
		else if (MeChairID == UpsideOfBanker)
		{
			UpsideOfBankerOutCard(MeChairID, OutCardResult);
		}
		else
		{
			return false;
		}
	}
	//����
	else
	{
		//��������
		if (MeChairID == m_BankerUser) 
		{ 
			BankerOutCard(MeChairID, TurnCardData, TurnCardCount, OutCardUser, OutCardResult);
		}
		//�����¼Ҹ���
		else if (MeChairID == UndersideOfBanker) 
		{ 
			UndersideOfBankerOutCard(MeChairID, TurnCardData, TurnCardCount, OutCardUser, OutCardResult);
		}
		//�����ϼҸ���
		else if (MeChairID == UpsideOfBanker) 
		{ 
			UpsideOfBankerOutCard(MeChairID, TurnCardData, TurnCardCount, OutCardUser, OutCardResult);
		}
		else
		{
			return false;
		}
	}

	return true;
}

void robotAI::GetAllBombCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE BombCardData[], BYTE &BombCardCount)
{
	BYTE TmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(TmpCardData, HandCardData, HandCardCount);

	//��С����
	SortCardList(TmpCardData, HandCardCount, ST_ORDER);

	BombCardCount = 0;

	if (HandCardCount < 2) return;

	//˫��ը��
	if (0x4F == TmpCardData[0] && 0x4E == TmpCardData[1])
	{
		BombCardData[BombCardCount++] = TmpCardData[0];
		BombCardData[BombCardCount++] = TmpCardData[1];
	}

	//�˿˷���
	for (BYTE i = 0; i < HandCardCount; i++)
	{
		//��������
		BYTE SameCount = 1;
		BYTE LogicValue = GetCardLogicValue(TmpCardData[i]);

		//����ͬ��
		for (BYTE j = i + 1; j < HandCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(TmpCardData[j]) != LogicValue) break;

			//���ñ���
			SameCount++;
		}

		if (4 == SameCount)
		{
			BombCardData[BombCardCount++] = TmpCardData[i];
			BombCardData[BombCardCount++] = TmpCardData[i + 1];
			BombCardData[BombCardCount++] = TmpCardData[i + 2];
			BombCardData[BombCardCount++] = TmpCardData[i + 3];
		}

		//��������
		i += SameCount - 1;
	}
}

//����˳��
void robotAI::GetAllLineCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE LineCardData[], BYTE &LineCardCount)
{
	BYTE TmpCard[MAX_COUNT] = { 0 };
	CopyMemory(TmpCard, HandCardData, HandCardCount);
	//��С����
	SortCardList(TmpCard, HandCardCount, ST_ORDER);

	LineCardCount = 0;

	//����У��
	if (HandCardCount < 5) return;

	BYTE FirstCard = 0;
	//ȥ��2����
	for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCard[i]) < 15)	{ FirstCard = i; break; }

	BYTE SingleLineCard[12];
	BYTE SingleLineCount = 0;
	BYTE LeftCardCount = HandCardCount;
	bool bFindSingleLine = true;

	//�����ж�
	while (LeftCardCount >= 5 && bFindSingleLine)
	{
		SingleLineCount = 1;
		bFindSingleLine = false;
		BYTE LastCard = TmpCard[FirstCard];
		SingleLineCard[SingleLineCount - 1] = TmpCard[FirstCard];
		for (BYTE i = FirstCard + 1; i < LeftCardCount; i++)
		{
			BYTE CardData = TmpCard[i];

			//�����ж�
			if (1 != (GetCardLogicValue(LastCard) - GetCardLogicValue(CardData)) && GetCardValue(LastCard) != GetCardValue(CardData))
			{
				LastCard = TmpCard[i];
				if (SingleLineCount < 5)
				{
					SingleLineCount = 1;
					SingleLineCard[SingleLineCount - 1] = TmpCard[i];
					continue;
				}
				else break;
			}
			//ͬ���ж�
			else if (GetCardValue(LastCard) != GetCardValue(CardData))
			{
				LastCard = CardData;
				SingleLineCard[SingleLineCount] = CardData;
				++SingleLineCount;
			}
		}

		//��������
		if (SingleLineCount >= 5)
		{
			RemoveCards(SingleLineCard, SingleLineCount, TmpCard, LeftCardCount);
			memcpy(LineCardData + LineCardCount, SingleLineCard, sizeof(BYTE)*SingleLineCount);
			LineCardCount += SingleLineCount;
			LeftCardCount -= SingleLineCount;
			bFindSingleLine = true;
		}
	}
}

//��������
void robotAI::GetAllThreeCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE ThreeCardData[], BYTE &ThreeCardCount)
{
	BYTE TmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(TmpCardData, HandCardData, HandCardCount);

	//��С����
	SortCardList(TmpCardData, HandCardCount, ST_ORDER);

	ThreeCardCount = 0;

	//�˿˷���
	for (BYTE i = 0; i < HandCardCount; i++)
	{
		//��������
		BYTE SameCount = 1;
		BYTE LogicValue = GetCardLogicValue(TmpCardData[i]);

		//����ͬ��
		for (BYTE j = i + 1; j < HandCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(TmpCardData[j]) != LogicValue) break;

			//���ñ���
			SameCount++;
		}

		if (SameCount >= 3)
		{
			ThreeCardData[ThreeCardCount++] = TmpCardData[i];
			ThreeCardData[ThreeCardCount++] = TmpCardData[i + 1];
			ThreeCardData[ThreeCardCount++] = TmpCardData[i + 2];
		}

		//��������
		i += SameCount - 1;
	}
}

//��������
void robotAI::GetAllDoubleCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE DoubleCardData[], BYTE &DoubleCardCount)
{
	BYTE TmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(TmpCardData, HandCardData, HandCardCount);

	//��С����
	SortCardList(TmpCardData, HandCardCount, ST_ORDER);

	DoubleCardCount = 0;

	//�˿˷���
	for (BYTE i = 0; i < HandCardCount; i++)
	{
		//��������
		BYTE SameCount = 1;
		BYTE LogicValue = GetCardLogicValue(TmpCardData[i]);

		//����ͬ��
		for (BYTE j = i + 1; j < HandCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(TmpCardData[j]) != LogicValue) break;

			//���ñ���
			SameCount++;
		}

		if (SameCount >= 2 && SameCount < 4)
		{
			DoubleCardData[DoubleCardCount++] = TmpCardData[i];
			DoubleCardData[DoubleCardCount++] = TmpCardData[i + 1];
		}

		//��������
		i += SameCount - 1;
	}
}

//��������
void robotAI::GetAllSingleCard(BYTE const HandCardData[], BYTE const HandCardCount, BYTE SingleCardData[], BYTE &SingleCardCount)
{
	SingleCardCount = 0;

	BYTE TmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(TmpCardData, HandCardData, HandCardCount);

	//��С����
	SortCardList(TmpCardData, HandCardCount, ST_ORDER);

	//�˿˷���
	for (BYTE i = 0; i < HandCardCount; i++)
	{
		//��������
		BYTE SameCount = 1;
		BYTE LogicValue = GetCardLogicValue(TmpCardData[i]);

		//����ͬ��
		for (BYTE j = i + 1; j < HandCardCount; j++)
		{
			//��ȡ�˿�
			if (GetCardLogicValue(TmpCardData[j]) != LogicValue) break;

			//���ñ���
			SameCount++;
		}

		if (SameCount == 1)
		{
			SingleCardData[SingleCardCount++] = TmpCardData[i];
		}

		//��������
		i += SameCount - 1;
	}
}

void robotAI::AnalyseOutCardType(BYTE const HandCardData[], BYTE const HandCardCount, tagOutCardTypeResult CardTypeResult[CT_NUM])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*(CT_NUM));
	BYTE TmpCardData[MAX_COUNT] = { 0 };

	//�����˿ˣ���ֹ����ʱ�ı��˿�
	BYTE ReserveCardData[MAX_COUNT] = { 0 };
	CopyMemory(ReserveCardData, HandCardData, HandCardCount);
	SortCardList(ReserveCardData, HandCardCount, ST_ORDER);
	CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

	//��������
	for (BYTE i = 0; i < HandCardCount; ++i)
	{
		BYTE Index = CardTypeResult[CT_SINGLE].CardTypeCount;
		CardTypeResult[CT_SINGLE].CardType = CT_SINGLE;
		CardTypeResult[CT_SINGLE].CardData[Index][0] = TmpCardData[i];
		CardTypeResult[CT_SINGLE].EachHandCardCount[Index] = 1;
		CardTypeResult[CT_SINGLE].CardTypeCount++;

		assert(CardTypeResult[CT_SINGLE].CardTypeCount < MAX_TYPE_COUNT);
	}

	//��������
	{
		BYTE DoubleCardData[MAX_COUNT] = { 0 };
		BYTE DoubleCardcount = 0;
		GetAllDoubleCard(TmpCardData, HandCardCount, DoubleCardData, DoubleCardcount);
		for (BYTE i = 0; i < DoubleCardcount; i += 2)
		{
			BYTE Index = CardTypeResult[CT_DOUBLE].CardTypeCount;
			CardTypeResult[CT_DOUBLE].CardType = CT_DOUBLE;
			CardTypeResult[CT_DOUBLE].CardData[Index][0] = DoubleCardData[i];
			CardTypeResult[CT_DOUBLE].CardData[Index][1] = DoubleCardData[i + 1];
			CardTypeResult[CT_DOUBLE].EachHandCardCount[Index] = 2;
			CardTypeResult[CT_DOUBLE].CardTypeCount++;

			assert(CardTypeResult[CT_DOUBLE].CardTypeCount < MAX_TYPE_COUNT);
		}
	}

	//��������
	{
		BYTE ThreeCardData[MAX_COUNT] = { 0 };
		BYTE ThreeCardCount = 0;
		GetAllThreeCard(TmpCardData, HandCardCount, ThreeCardData, ThreeCardCount);
		for (BYTE i = 0; i < ThreeCardCount; i += 3)
		{
			BYTE Index = CardTypeResult[CT_THREE].CardTypeCount;
			CardTypeResult[CT_THREE].CardType = CT_THREE;
			CardTypeResult[CT_THREE].CardData[Index][0] = ThreeCardData[i];
			CardTypeResult[CT_THREE].CardData[Index][1] = ThreeCardData[i + 1];
			CardTypeResult[CT_THREE].CardData[Index][2] = ThreeCardData[i + 2];
			CardTypeResult[CT_THREE].EachHandCardCount[Index] = 3;
			CardTypeResult[CT_THREE].CardTypeCount++;

			assert(CardTypeResult[CT_THREE].CardTypeCount < MAX_TYPE_COUNT);
		}
	}

	//ը������
	{
		BYTE FourCardData[MAX_COUNT] = { 0 };
		BYTE FourCardCount = 0;
		if (HandCardCount >= 2 && 0x4F == TmpCardData[0] && 0x4E == TmpCardData[1])
		{
			BYTE Index = CardTypeResult[CT_BOMB_CARD].CardTypeCount;
			CardTypeResult[CT_BOMB_CARD].CardType = CT_BOMB_CARD;
			CardTypeResult[CT_BOMB_CARD].CardData[Index][0] = TmpCardData[0];
			CardTypeResult[CT_BOMB_CARD].CardData[Index][1] = TmpCardData[1];
			CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] = 2;
			CardTypeResult[CT_BOMB_CARD].CardTypeCount++;
			GetAllBombCard(TmpCardData + 2, HandCardCount - 2, FourCardData, FourCardCount);
		}
		else GetAllBombCard(TmpCardData, HandCardCount, FourCardData, FourCardCount);
		for (BYTE i = 0; i < FourCardCount; i += 4)
		{
			BYTE Index = CardTypeResult[CT_BOMB_CARD].CardTypeCount;
			CardTypeResult[CT_BOMB_CARD].CardType = CT_BOMB_CARD;
			CardTypeResult[CT_BOMB_CARD].CardData[Index][0] = FourCardData[i];
			CardTypeResult[CT_BOMB_CARD].CardData[Index][1] = FourCardData[i + 1];
			CardTypeResult[CT_BOMB_CARD].CardData[Index][2] = FourCardData[i + 2];
			CardTypeResult[CT_BOMB_CARD].CardData[Index][3] = FourCardData[i + 3];
			CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] = 4;
			CardTypeResult[CT_BOMB_CARD].CardTypeCount++;

			assert(CardTypeResult[CT_BOMB_CARD].CardTypeCount < MAX_TYPE_COUNT);
		}
	}

	//��������
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		ZeroMemory(TmpCardData, HandCardCount);
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		BYTE FirstCard = 0;
		//ȥ��2����
		for (BYTE i = 0; i < HandCardCount; ++i)
		{
			if (GetCardLogicValue(TmpCardData[i]) < 15)
			{
				FirstCard = i;
				break;
			}
		}

		BYTE SingleLineCard[12];
		BYTE SingleLineCount = 1;
		BYTE LeftCardCount = HandCardCount;
		bool bFindSingleLine = true;

		//�����ж�
		while (LeftCardCount >= 5 && bFindSingleLine)
		{
			SingleLineCount = 1;
			bFindSingleLine = false;
			BYTE LastCard = TmpCardData[FirstCard];
			SingleLineCard[SingleLineCount - 1] = TmpCardData[FirstCard];
			for (BYTE i = FirstCard + 1; i < LeftCardCount; i++)
			{
				BYTE CardData = TmpCardData[i];

				//�����ж�
				if (1 != (GetCardLogicValue(LastCard) - GetCardLogicValue(CardData)) && GetCardValue(LastCard) != GetCardValue(CardData))
				{
					LastCard = TmpCardData[i];
					//�Ƿ�Ϸ�
					if (SingleLineCount < 5)
					{
						SingleLineCount = 1;
						SingleLineCard[SingleLineCount - 1] = TmpCardData[i];
						continue;
					}
					else break;
				}
				//ͬ���ж�
				else if (GetCardValue(LastCard) != GetCardValue(CardData))
				{
					LastCard = CardData;
					SingleLineCard[SingleLineCount] = CardData;
					++SingleLineCount;
				}
			}

			//��������
			if (SingleLineCount >= 5)
			{
				BYTE Index;
				//��������
				BYTE Start = 0;
				//�Ӵ�С
				while (SingleLineCount - Start >= 5)
				{
					Index = CardTypeResult[CT_SINGLE_LINE].CardTypeCount;
					BYTE ThisLineCount = SingleLineCount - Start;
					CardTypeResult[CT_SINGLE_LINE].CardType = CT_SINGLE_LINE;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].CardData[Index], SingleLineCard + Start, sizeof(BYTE)*(ThisLineCount));
					CardTypeResult[CT_SINGLE_LINE].EachHandCardCount[Index] = ThisLineCount;
					CardTypeResult[CT_SINGLE_LINE].CardTypeCount++;

					assert(CardTypeResult[CT_SINGLE_LINE].CardTypeCount < MAX_TYPE_COUNT);
					Start++;
				}
				//��С����
				Start = 1;
				while (SingleLineCount - Start >= 5)
				{
					Index = CardTypeResult[CT_SINGLE_LINE].CardTypeCount;
					BYTE ThisLineCount = SingleLineCount - Start;
					CardTypeResult[CT_SINGLE_LINE].CardType = CT_SINGLE_LINE;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].CardData[Index], SingleLineCard, sizeof(BYTE)*(ThisLineCount));
					CardTypeResult[CT_SINGLE_LINE].EachHandCardCount[Index] = ThisLineCount;
					CardTypeResult[CT_SINGLE_LINE].CardTypeCount++;

					assert(CardTypeResult[CT_SINGLE_LINE].CardTypeCount < MAX_TYPE_COUNT);
					Start++;
				}

				RemoveCards(SingleLineCard, SingleLineCount, TmpCardData, LeftCardCount);
				LeftCardCount -= SingleLineCount;
				bFindSingleLine = true;
			}
		}

	}

	//��������
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		//�����ж�
		BYTE FirstCard = 0;
		//ȥ��2����
		for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCardData[i]) < 15)	{ FirstCard = i; break; }

		BYTE LeftCardCount = HandCardCount - FirstCard;
		bool bFindDoubleLine = true;
		BYTE DoubleLineCount = 0;
		BYTE DoubleLineCard[24];
		//��ʼ�ж�
		while (LeftCardCount >= 6 && bFindDoubleLine)
		{
			BYTE LastCard = TmpCardData[FirstCard];
			BYTE SameCount = 1;
			DoubleLineCount = 0;
			bFindDoubleLine = false;
			for (BYTE i = FirstCard + 1; i < LeftCardCount + FirstCard; ++i)
			{
				//����ͬ��
				while (GetCardLogicValue(LastCard) == GetCardLogicValue(TmpCardData[i]) && i < LeftCardCount + FirstCard)
				{
					++SameCount;
					++i;
				}

				BYTE LastDoubleCardValue;
				if (DoubleLineCount > 0) LastDoubleCardValue = GetCardLogicValue(DoubleLineCard[DoubleLineCount - 1]);
				//���¿�ʼ
				if ((SameCount < 2 || (DoubleLineCount>0 && (LastDoubleCardValue - GetCardLogicValue(LastCard)) != 1)) && i <= LeftCardCount + FirstCard)
				{
					if (DoubleLineCount >= 6) break;
					//����
					if (SameCount >= 2) i -= SameCount;
					LastCard = TmpCardData[i];
					DoubleLineCount = 0;
				}
				//��������
				else if (SameCount >= 2)
				{
					DoubleLineCard[DoubleLineCount] = TmpCardData[i - SameCount];
					DoubleLineCard[DoubleLineCount + 1] = TmpCardData[i - SameCount + 1];
					DoubleLineCount += 2;

					//��β�ж�
					if (i == (LeftCardCount + FirstCard - 2))
						if ((GetCardLogicValue(LastCard) - GetCardLogicValue(TmpCardData[i])) == 1 && (GetCardLogicValue(TmpCardData[i]) == GetCardLogicValue(TmpCardData[i + 1])))
						{
							DoubleLineCard[DoubleLineCount] = TmpCardData[i];
							DoubleLineCard[DoubleLineCount + 1] = TmpCardData[i + 1];
							DoubleLineCount += 2;
							break;
						}

				}

				LastCard = TmpCardData[i];
				SameCount = 1;
			}

			//��������
			if (DoubleLineCount >= 6)
			{
				BYTE Index;

				Index = CardTypeResult[CT_DOUBLE_LINE].CardTypeCount;
				CardTypeResult[CT_DOUBLE_LINE].CardType = CT_DOUBLE_LINE;
				CopyMemory(CardTypeResult[CT_DOUBLE_LINE].CardData[Index], DoubleLineCard, sizeof(BYTE)*DoubleLineCount);
				CardTypeResult[CT_DOUBLE_LINE].EachHandCardCount[Index] = DoubleLineCount;
				CardTypeResult[CT_DOUBLE_LINE].CardTypeCount++;

				assert(CardTypeResult[CT_DOUBLE_LINE].CardTypeCount < MAX_TYPE_COUNT);

				RemoveCards(DoubleLineCard, DoubleLineCount, TmpCardData, FirstCard + LeftCardCount);
				bFindDoubleLine = true;
				LeftCardCount -= DoubleLineCount;
			}
		}
	}

	//��������
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		//�����ж�
		BYTE FirstCard = 0;
		//ȥ��2����
		for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCardData[i]) < 15)	{ FirstCard = i; break; }

		BYTE LeftCardCount = HandCardCount - FirstCard;
		bool bFindThreeLine = true;
		BYTE ThreeLineCount = 0;
		BYTE ThreeLineCard[20];
		//��ʼ�ж�
		while (LeftCardCount >= 6 && bFindThreeLine)
		{
			BYTE LastCard = TmpCardData[FirstCard];
			BYTE SameCount = 1;
			ThreeLineCount = 0;
			bFindThreeLine = false;
			for (BYTE i = FirstCard + 1; i < LeftCardCount + FirstCard; ++i)
			{
				//����ͬ��
				while (GetCardLogicValue(LastCard) == GetCardLogicValue(TmpCardData[i]) && i < LeftCardCount + FirstCard)
				{
					++SameCount;
					++i;
				}

				BYTE LastThreeCardValue;
				if (ThreeLineCount > 0) LastThreeCardValue = GetCardLogicValue(ThreeLineCard[ThreeLineCount - 1]);

				//���¿�ʼ
				if ((SameCount < 3 || (ThreeLineCount>0 && (LastThreeCardValue - GetCardLogicValue(LastCard)) != 1)) && i <= LeftCardCount + FirstCard)
				{
					if (ThreeLineCount >= 6) break;

					if (SameCount >= 3) i -= SameCount;
					LastCard = TmpCardData[i];
					ThreeLineCount = 0;
				}
				//��������
				else if (SameCount >= 3)
				{
					ThreeLineCard[ThreeLineCount] = TmpCardData[i - SameCount];
					ThreeLineCard[ThreeLineCount + 1] = TmpCardData[i - SameCount + 1];
					ThreeLineCard[ThreeLineCount + 2] = TmpCardData[i - SameCount + 2];
					ThreeLineCount += 3;

					//��β�ж�
					if (i == (LeftCardCount + FirstCard - 3))
						if ((GetCardLogicValue(LastCard) - GetCardLogicValue(TmpCardData[i])) == 1 && (GetCardLogicValue(TmpCardData[i]) == GetCardLogicValue(TmpCardData[i + 1])) && (GetCardLogicValue(TmpCardData[i]) == GetCardLogicValue(TmpCardData[i + 2])))
						{
							ThreeLineCard[ThreeLineCount] = TmpCardData[i];
							ThreeLineCard[ThreeLineCount + 1] = TmpCardData[i + 1];
							ThreeLineCard[ThreeLineCount + 2] = TmpCardData[i + 2];
							ThreeLineCount += 3;
							break;
						}

				}

				LastCard = TmpCardData[i];
				SameCount = 1;
			}

			//��������
			if (ThreeLineCount >= 6)
			{
				BYTE Index;

				Index = CardTypeResult[CT_THREE_LINE].CardTypeCount;
				CardTypeResult[CT_THREE_LINE].CardType = CT_THREE_LINE;
				CopyMemory(CardTypeResult[CT_THREE_LINE].CardData[Index], ThreeLineCard, sizeof(BYTE)*ThreeLineCount);
				CardTypeResult[CT_THREE_LINE].EachHandCardCount[Index] = ThreeLineCount;
				CardTypeResult[CT_THREE_LINE].CardTypeCount++;

				assert(CardTypeResult[CT_THREE_LINE].CardTypeCount < MAX_TYPE_COUNT);

				RemoveCards(ThreeLineCard, ThreeLineCount, TmpCardData, FirstCard + LeftCardCount);
				bFindThreeLine = true;
				LeftCardCount -= ThreeLineCount;
			}
		}

	}
	//����һ��
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		BYTE HandThreeCard[MAX_COUNT] = { 0 };
		BYTE HandThreeCount = 0;

		//�Ƴ�ը��
		BYTE AllBomCardData[MAX_COUNT] = { 0 };
		BYTE AllBomCardCount = 0;
		GetAllBombCard(TmpCardData, HandCardCount, AllBomCardData, AllBomCardCount);
		RemoveCards(AllBomCardData, AllBomCardCount, TmpCardData, HandCardCount);

		GetAllThreeCard(TmpCardData, HandCardCount - AllBomCardCount, HandThreeCard, HandThreeCount);

		{
			BYTE Index;
			//ȥ������
			BYTE RemainCardData[MAX_COUNT] = { 0 };
			CopyMemory(RemainCardData, TmpCardData, HandCardCount - AllBomCardCount);
			BYTE RemainCardCount = HandCardCount - AllBomCardCount - HandThreeCount;
			RemoveCards(HandThreeCard, HandThreeCount, RemainCardData, HandCardCount - AllBomCardCount);
			//������һ��
			for (BYTE i = 0; i < HandThreeCount; i += 3)
			{
				//������һ��
				for (BYTE j = 0; j < RemainCardCount; ++j)
				{
					Index = CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount;
					CardTypeResult[CT_THREE_TAKE_ONE].CardType = CT_THREE_TAKE_ONE;
					CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index][0] = HandThreeCard[i];
					CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index][1] = HandThreeCard[i + 1];
					CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index][2] = HandThreeCard[i + 2];
					CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index][3] = RemainCardData[j];
					CardTypeResult[CT_THREE_TAKE_ONE].EachHandCardCount[Index] = 4;
					CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount++;
				}
			}
		}

		//��������
		BYTE LeftThreeCardCount = HandThreeCount;
		bool bFindThreeLine = true;
		BYTE LastIndex = 0;
		if (GetCardLogicValue(HandThreeCard[0]) == 15) LastIndex = 3;
		while (LeftThreeCardCount >= 6 && bFindThreeLine)
		{
			BYTE LastLogicCard = GetCardLogicValue(HandThreeCard[LastIndex]);
			BYTE ThreeLineCard[MAX_COUNT] = { 0 };
			BYTE ThreeLineCardCount = 3;
			ThreeLineCard[0] = HandThreeCard[LastIndex];
			ThreeLineCard[1] = HandThreeCard[LastIndex + 1];
			ThreeLineCard[2] = HandThreeCard[LastIndex + 2];

			bFindThreeLine = false;
			for (BYTE j = 3 + LastIndex; j < LeftThreeCardCount; j += 3)
			{
				//�����ж�
				if (1 != (LastLogicCard - (GetCardLogicValue(HandThreeCard[j]))))
				{
					LastIndex = j;
					if (LeftThreeCardCount - j >= 6) bFindThreeLine = true;

					break;
				}

				LastLogicCard = GetCardLogicValue(HandThreeCard[j]);
				ThreeLineCard[ThreeLineCardCount] = HandThreeCard[j];
				ThreeLineCard[ThreeLineCardCount + 1] = HandThreeCard[j + 1];
				ThreeLineCard[ThreeLineCardCount + 2] = HandThreeCard[j + 2];
				ThreeLineCardCount += 3;
			}
			if (ThreeLineCardCount > 3)
			{
				BYTE Index;

				BYTE RemainCard[MAX_COUNT] = { 0 };
				BYTE RemainCardCount = HandCardCount - AllBomCardCount - HandThreeCount;


				//�Ƴ���������Ӧ���Ƴ�ը�����ȣ�
				CopyMemory(RemainCard, TmpCardData, (HandCardCount - AllBomCardCount)*sizeof(BYTE));
				RemoveCards(HandThreeCard, HandThreeCount, RemainCard, HandCardCount - AllBomCardCount);

				for (BYTE start = 0; start < ThreeLineCardCount - 3; start += 3)
				{
					//��˳��Ŀ
					BYTE ThisTreeLineCardCount = ThreeLineCardCount - start;
					//���Ƹ���
					BYTE SingleCardCount = (ThisTreeLineCardCount) / 3;

					//���Ʋ���
					if (RemainCardCount < SingleCardCount) continue;

					//�������
					BYTE ComCard[5] = { 0 };
					BYTE ComResCard[MAX_TYPE_COUNT][5] = { 0 };
					BYTE ComResLen = 0;

					Combination(ComCard, 0, ComResCard, ComResLen, RemainCard, RemainCardCount, SingleCardCount, SingleCardCount);
					for (BYTE i = 0; i < ComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount;
						CardTypeResult[CT_THREE_TAKE_ONE].CardType = CT_THREE_TAKE_ONE;
						//��������
						CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index], ThreeLineCard + start, sizeof(BYTE)*ThisTreeLineCardCount);
						//���浥��
						CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index] + ThisTreeLineCardCount, ComResCard[i], SingleCardCount);


						CardTypeResult[CT_THREE_TAKE_ONE].EachHandCardCount[Index] = ThisTreeLineCardCount + SingleCardCount;
						CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount++;

						assert(CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount < MAX_TYPE_COUNT);
					}

				}

				//�Ƴ�����
				bFindThreeLine = true;
				RemoveCards(ThreeLineCard, ThreeLineCardCount, HandThreeCard, LeftThreeCardCount);
				LeftThreeCardCount -= ThreeLineCardCount;
			}
		}
	}

	//����һ��
	{
		//�ָ��˿ˣ���ֹ����ʱ�ı��˿�
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		BYTE HandThreeCard[MAX_COUNT] = { 0 };
		BYTE HandThreeCount = 0;
		BYTE RemainCarData[MAX_COUNT] = { 0 };
		BYTE RemainCardCount = 0;

		//��ȡ����
		GetAllThreeCard(TmpCardData, HandCardCount, HandThreeCard, HandThreeCount);

		//�Ƴ���������Ӧ���Ƴ�ը�����ȣ�
		CopyMemory(RemainCarData, TmpCardData, HandCardCount);
		RemoveCards(HandThreeCard, HandThreeCount, RemainCarData, HandCardCount);
		RemainCardCount = HandCardCount - HandThreeCount;

		//��ȡ����
		BYTE AllDoubleCardData[MAX_COUNT] = { 0 };
		BYTE AllDoubleCardCount = 0;
		GetAllDoubleCard(RemainCarData, RemainCardCount, AllDoubleCardData, AllDoubleCardCount);

		//������һ��
		for (BYTE i = 0; i < HandThreeCount; i += 3)
		{
			BYTE Index;

			//������һ��
			for (BYTE j = 0; j < AllDoubleCardCount; j += 2)
			{
				Index = CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount;
				CardTypeResult[CT_THREE_TAKE_TWO].CardType = CT_THREE_TAKE_TWO;
				CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][0] = HandThreeCard[i];
				CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][1] = HandThreeCard[i + 1];
				CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][2] = HandThreeCard[i + 2];
				CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][3] = AllDoubleCardData[j];
				CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][4] = AllDoubleCardData[j + 1];
				CardTypeResult[CT_THREE_TAKE_TWO].EachHandCardCount[Index] = 5;
				CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount++;
			}
		}

		//��������
		BYTE LeftThreeCardCount = HandThreeCount;
		bool bFindThreeLine = true;
		BYTE LastIndex = 0;
		if (GetCardLogicValue(HandThreeCard[0]) == 15) LastIndex = 3;
		while (LeftThreeCardCount >= 6 && bFindThreeLine)
		{
			BYTE LastLogicCard = GetCardLogicValue(HandThreeCard[LastIndex]);
			BYTE ThreeLineCard[MAX_COUNT];
			BYTE ThreeLineCardCount = 3;
			ThreeLineCard[0] = HandThreeCard[LastIndex];
			ThreeLineCard[1] = HandThreeCard[LastIndex + 1];
			ThreeLineCard[2] = HandThreeCard[LastIndex + 2];

			bFindThreeLine = false;
			for (BYTE j = 3 + LastIndex; j < LeftThreeCardCount; j += 3)
			{
				//�����ж�
				if (1 != (LastLogicCard - (GetCardLogicValue(HandThreeCard[j]))))
				{
					LastIndex = j;
					if (LeftThreeCardCount - j >= 6) bFindThreeLine = true;

					break;
				}

				LastLogicCard = GetCardLogicValue(HandThreeCard[j]);
				ThreeLineCard[ThreeLineCardCount] = HandThreeCard[j];
				ThreeLineCard[ThreeLineCardCount + 1] = HandThreeCard[j + 1];
				ThreeLineCard[ThreeLineCardCount + 2] = HandThreeCard[j + 2];
				ThreeLineCardCount += 3;
			}
			if (ThreeLineCardCount > 3)
			{
				BYTE Index;

				for (BYTE start = 0; start < ThreeLineCardCount - 3; start += 3)
				{
					//��˳��Ŀ
					BYTE ThisTreeLineCardCount = ThreeLineCardCount - start;
					//��������
					BYTE DoubleCardCount = ((ThisTreeLineCardCount) / 3);

					//���Ʋ���
					if (RemainCardCount < DoubleCardCount) continue;

					BYTE DoubleCardIndex[10]; //�����±�
					for (BYTE i = 0, j = 0; i < AllDoubleCardCount; i += 2, ++j)
						DoubleCardIndex[j] = i;

					//�������
					BYTE ComCard[5];
					BYTE ComResCard[254][5];
					BYTE ComResLen = 0;

					//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
					Combination(ComCard, 0, ComResCard, ComResLen, DoubleCardIndex, AllDoubleCardCount / 2, DoubleCardCount, DoubleCardCount);

					assert(ComResLen <= 254);

					for (BYTE i = 0; i < ComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount;
						CardTypeResult[CT_THREE_TAKE_TWO].CardType = CT_THREE_TAKE_TWO;
						//��������
						CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index], ThreeLineCard + start, sizeof(BYTE)*ThisTreeLineCardCount);
						//�������
						for (BYTE j = 0, k = 0; j < DoubleCardCount; ++j, k += 2)
						{
							CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][ThisTreeLineCardCount + k] = AllDoubleCardData[ComResCard[i][j]];
							CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][ThisTreeLineCardCount + k + 1] = AllDoubleCardData[ComResCard[i][j] + 1];
						}

						CardTypeResult[CT_THREE_TAKE_TWO].EachHandCardCount[Index] = ThisTreeLineCardCount + 2 * DoubleCardCount;
						CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount++;

						assert(CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount < MAX_TYPE_COUNT);
					}

				}
				//�Ƴ�����
				bFindThreeLine = true;
				RemoveCards(ThreeLineCard, ThreeLineCardCount, HandThreeCard, LeftThreeCardCount);
				LeftThreeCardCount -= ThreeLineCardCount;
			}
		}
	}
	return;
}

//��������
void robotAI::AnalyseOutCardType(BYTE const HandCardData[], BYTE const HandCardCount, BYTE const TurnCardData[], BYTE const TurnCardCount, tagOutCardTypeResult CardTypeResult[CT_NUM])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0]) * 12);

	BYTE TmpCard[MAX_COUNT];
	CopyMemory(TmpCard, HandCardData, HandCardCount);
	SortCardList(TmpCard, HandCardCount, ST_ORDER);

	BYTE TurnCardType = GetCardType(TurnCardData, TurnCardCount);
	assert(TurnCardType != CT_ERROR);
	if (TurnCardType == CT_ERROR)
		return;

	if (TurnCardType != CT_MISSILE_CARD && TurnCardType != CT_BOMB_CARD)
	{
		//˫��ը��
		if (HandCardCount >= 2 && 0x4F == TmpCard[0] && 0x4E == TmpCard[1])
		{
			BYTE Index = CardTypeResult[CT_BOMB_CARD].CardTypeCount;
			CardTypeResult[CT_BOMB_CARD].CardType = CT_BOMB_CARD;
			CardTypeResult[CT_BOMB_CARD].CardData[Index][0] = TmpCard[0];
			CardTypeResult[CT_BOMB_CARD].CardData[Index][1] = TmpCard[1];
			CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] = 2;
			CardTypeResult[CT_BOMB_CARD].CardTypeCount++;

			BYTE BomCardData[MAX_COUNT];
			BYTE BomCardCount = 0;
			GetAllBombCard(TmpCard + 2, HandCardCount - 2, BomCardData, BomCardCount);
			for (BYTE i = 0; i < BomCardCount / 4; ++i)
			{
				Index = CardTypeResult[CT_BOMB_CARD].CardTypeCount;
				CardTypeResult[CT_BOMB_CARD].CardType = CT_BOMB_CARD;
				CopyMemory(CardTypeResult[CT_BOMB_CARD].CardData[Index], BomCardData + 4 * i, 4);
				CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] = 4;
				CardTypeResult[CT_BOMB_CARD].CardTypeCount++;

				assert(CardTypeResult[CT_BOMB_CARD].CardTypeCount <= MAX_TYPE_COUNT);
			}
		}
		//ը������
		else
		{
			BYTE BomCardData[MAX_COUNT];
			BYTE BomCardCount = 0;
			GetAllBombCard(TmpCard, HandCardCount, BomCardData, BomCardCount);
			for (BYTE i = 0; i < BomCardCount / 4; ++i)
			{
				BYTE Index = CardTypeResult[CT_BOMB_CARD].CardTypeCount;
				CardTypeResult[CT_BOMB_CARD].CardType = CT_BOMB_CARD;
				CopyMemory(CardTypeResult[CT_BOMB_CARD].CardData[Index], BomCardData + 4 * i, 4);
				CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] = 4;
				CardTypeResult[CT_BOMB_CARD].CardTypeCount++;

				assert(CardTypeResult[CT_BOMB_CARD].CardTypeCount <= MAX_TYPE_COUNT);
			}
		}
	}

	switch (TurnCardType)
	{
	case CT_SINGLE:				//��������
	{
		for (BYTE i = 0; i<HandCardCount; ++i)
			if (GetCardLogicValue(TmpCard[i])>GetCardLogicValue(TurnCardData[0]))
			{
				BYTE Index = CardTypeResult[CT_SINGLE].CardTypeCount;
				CardTypeResult[CT_SINGLE].CardType = CT_SINGLE;
				CardTypeResult[CT_SINGLE].CardData[Index][0] = TmpCard[i];
				CardTypeResult[CT_SINGLE].EachHandCardCount[Index] = 1;
				CardTypeResult[CT_SINGLE].CardTypeCount++;

				assert(CardTypeResult[CT_SINGLE].CardTypeCount <= MAX_TYPE_COUNT);
			}
		break;
	}
	case CT_DOUBLE:				//��������
	{
		//�˿˷���
		for (BYTE i = 0; i < HandCardCount; i++)
		{
			//��������
			BYTE SameCount = 1;
			BYTE LogicValue = GetCardLogicValue(TmpCard[i]);

			//����ͬ��
			for (BYTE j = i + 1; j<HandCardCount; j++)
			{
				//��ȡ�˿�
				if (GetCardLogicValue(TmpCard[j]) != LogicValue) break;

				//���ñ���
				SameCount++;
			}

			if (SameCount >= 2 && GetCardLogicValue(TmpCard[i])>GetCardLogicValue(TurnCardData[0]))
			{
				BYTE Index = CardTypeResult[CT_DOUBLE].CardTypeCount;
				CardTypeResult[CT_DOUBLE].CardType = CT_DOUBLE;
				CardTypeResult[CT_DOUBLE].CardData[Index][0] = TmpCard[i];
				CardTypeResult[CT_DOUBLE].CardData[Index][1] = TmpCard[i + 1];
				CardTypeResult[CT_DOUBLE].EachHandCardCount[Index] = 2;
				CardTypeResult[CT_DOUBLE].CardTypeCount++;

				assert(CardTypeResult[CT_DOUBLE].CardTypeCount <= MAX_TYPE_COUNT);
			}
			//��������
			i += SameCount - 1;
		}
		break;
	}
	case CT_THREE:				//��������
	{
		//�˿˷���
		for (BYTE i = 0; i < HandCardCount; i++)
		{
			//��������
			BYTE SameCount = 1;
			BYTE LogicValue = GetCardLogicValue(TmpCard[i]);

			//����ͬ��
			for (BYTE j = i + 1; j<HandCardCount; j++)
			{
				//��ȡ�˿�
				if (GetCardLogicValue(TmpCard[j]) != LogicValue) break;

				//���ñ���
				SameCount++;
			}

			if (SameCount >= 3 && GetCardLogicValue(TmpCard[i])>GetCardLogicValue(TurnCardData[0]))
			{
				BYTE Index = CardTypeResult[CT_THREE].CardTypeCount;
				CardTypeResult[CT_THREE].CardType = CT_THREE;
				CardTypeResult[CT_THREE].CardData[Index][0] = TmpCard[i];
				CardTypeResult[CT_THREE].CardData[Index][1] = TmpCard[i + 1];
				CardTypeResult[CT_THREE].CardData[Index][2] = TmpCard[i + 2];
				CardTypeResult[CT_THREE].EachHandCardCount[Index] = 3;
				CardTypeResult[CT_THREE].CardTypeCount++;

				assert(CardTypeResult[CT_THREE].CardTypeCount <= MAX_TYPE_COUNT);
			}
			//��������
			i += SameCount - 1;
		}
		break;
	}
	case CT_SINGLE_LINE:		//��������
	{
		BYTE FirstCard = 0;
		//ȥ��2����
		for (BYTE i = 0; i < HandCardCount; ++i)
		{
			if (GetCardLogicValue(TmpCard[i]) < 15)	
			{ 
				FirstCard = i; 
				break; 
			}
		}

		BYTE SingleLineCard[12];
		BYTE SingleLineCount = 1;
		BYTE LeftCardCount = HandCardCount;
		bool bFindSingleLine = true;

		//�����ж�
		while (LeftCardCount >= TurnCardCount && bFindSingleLine)
		{
			SingleLineCount = 1;
			bFindSingleLine = false;
			BYTE LastCard = TmpCard[FirstCard];
			SingleLineCard[SingleLineCount - 1] = TmpCard[FirstCard];
			for (BYTE i = FirstCard + 1; i < LeftCardCount; i++)
			{
				BYTE CardData = TmpCard[i];

				//�����ж�
				if (1 != (GetCardLogicValue(LastCard) - GetCardLogicValue(CardData)) && GetCardValue(LastCard) != GetCardValue(CardData))
				{
					LastCard = TmpCard[i];
					//�Ƿ�Ϸ�
					if (SingleLineCount<TurnCardCount)
					{
						SingleLineCount = 1;
						SingleLineCard[SingleLineCount - 1] = TmpCard[i];
						continue;
					}
					else break;
				}
				//ͬ���ж�
				else if (GetCardValue(LastCard) != GetCardValue(CardData))
				{
					LastCard = CardData;
					SingleLineCard[SingleLineCount] = CardData;
					++SingleLineCount;
				}
			}

			//��������
			if (SingleLineCount >= TurnCardCount && 
				GetCardLogicValue(SingleLineCard[0])>GetCardLogicValue(TurnCardData[0]))
			{
				BYTE Index;
				BYTE Start = 0;
				//��������
				while (GetCardLogicValue(SingleLineCard[Start]) > GetCardLogicValue(TurnCardData[0]) && ((SingleLineCount - Start) >= TurnCardCount))
				{
					Index = CardTypeResult[CT_SINGLE_LINE].CardTypeCount;
					CardTypeResult[CT_SINGLE_LINE].CardType = CT_SINGLE_LINE;
					CopyMemory(CardTypeResult[CT_SINGLE_LINE].CardData[Index], SingleLineCard + Start, sizeof(BYTE)*TurnCardCount);
					CardTypeResult[CT_SINGLE_LINE].EachHandCardCount[Index] = TurnCardCount;
					CardTypeResult[CT_SINGLE_LINE].CardTypeCount++;
					Start++;

					assert(CardTypeResult[CT_SINGLE_LINE].CardTypeCount <= MAX_TYPE_COUNT);
				}

				RemoveCards(SingleLineCard, SingleLineCount, TmpCard, LeftCardCount);
				LeftCardCount -= SingleLineCount;
				bFindSingleLine = true;
			}
		}

		break;
	}
	case CT_DOUBLE_LINE:		//��������
	{
		//�����ж�
		BYTE FirstCard = 0;
		//ȥ��2����
		for (BYTE i = 0; i < HandCardCount; ++i)
		{
			if (GetCardLogicValue(TmpCard[i]) < 15)	
			{ 
				FirstCard = i; 
				break; 
			}
		}

		BYTE LeftCardCount = HandCardCount - FirstCard;
		bool bFindDoubleLine = true;
		BYTE DoubleLineCount = 0;
		BYTE DoubleLineCard[24];
		//��ʼ�ж�
		while (LeftCardCount >= TurnCardCount && bFindDoubleLine)
		{
			BYTE LastCard = TmpCard[FirstCard];
			BYTE SameCount = 1;
			DoubleLineCount = 0;
			bFindDoubleLine = false;
			for (BYTE i = FirstCard + 1; i < LeftCardCount + FirstCard; ++i)
			{
				//����ͬ��
				while (GetCardValue(LastCard) == GetCardValue(TmpCard[i]) && i < LeftCardCount + FirstCard)
				{
					++SameCount;
					++i;
				}

				BYTE LastDoubleCardValue;
				if (DoubleLineCount > 0) LastDoubleCardValue = GetCardLogicValue(DoubleLineCard[DoubleLineCount - 1]);
				//���¿�ʼ
				if ((SameCount < 2 || (DoubleLineCount>0 && (LastDoubleCardValue - GetCardLogicValue(LastCard)) != 1)) && i <= LeftCardCount + FirstCard)
				{
					if (DoubleLineCount >= TurnCardCount) break;

					if (SameCount >= 2) i -= SameCount;

					LastCard = TmpCard[i];
					DoubleLineCount = 0;
				}
				//��������
				else if (SameCount >= 2)
				{
					DoubleLineCard[DoubleLineCount] = TmpCard[i - SameCount];
					DoubleLineCard[DoubleLineCount + 1] = TmpCard[i - SameCount + 1];
					DoubleLineCount += 2;

					//��β�ж�
					if (i == (LeftCardCount + FirstCard - 2))
					{
						if ((GetCardLogicValue(LastCard) - GetCardLogicValue(TmpCard[i])) == 1 && (GetCardLogicValue(TmpCard[i]) == GetCardLogicValue(TmpCard[i + 1])))
						{
							DoubleLineCard[DoubleLineCount] = TmpCard[i];
							DoubleLineCard[DoubleLineCount + 1] = TmpCard[i + 1];
							DoubleLineCount += 2;
							break;
						}
					}
				}

				LastCard = TmpCard[i];
				SameCount = 1;
			}

			//��������
			if (DoubleLineCount >= TurnCardCount)
			{
				BYTE Index;
				BYTE Start = 0;
				//��������
				while (GetCardLogicValue(DoubleLineCard[Start]) > GetCardLogicValue(TurnCardData[0]) && ((DoubleLineCount - Start) >= TurnCardCount))
				{
					Index = CardTypeResult[CT_DOUBLE_LINE].CardTypeCount;
					CardTypeResult[CT_DOUBLE_LINE].CardType = CT_DOUBLE_LINE;
					CopyMemory(CardTypeResult[CT_DOUBLE_LINE].CardData[Index], DoubleLineCard + Start, sizeof(BYTE)*TurnCardCount);
					CardTypeResult[CT_DOUBLE_LINE].EachHandCardCount[Index] = TurnCardCount;
					CardTypeResult[CT_DOUBLE_LINE].CardTypeCount++;
					Start += 2;

					assert(CardTypeResult[CT_DOUBLE_LINE].CardTypeCount <= MAX_TYPE_COUNT);
				}
				RemoveCards(DoubleLineCard, DoubleLineCount, TmpCard, FirstCard + LeftCardCount);
				bFindDoubleLine = true;
				LeftCardCount -= DoubleLineCount;
			}
		}

		break;
	}
	case CT_THREE_LINE:			//��������
	{
		//�����ж�
		BYTE FirstCard = 0;
		//ȥ��2����
		for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCard[i]) < 15)	{ FirstCard = i; break; }

		BYTE LeftCardCount = HandCardCount - FirstCard;
		bool bFindThreeLine = true;
		BYTE ThreeLineCount = 0;
		BYTE ThreeLineCard[20];
		//��ʼ�ж�
		while (LeftCardCount >= TurnCardCount && bFindThreeLine)
		{
			BYTE LastCard = TmpCard[FirstCard];
			BYTE SameCount = 1;
			ThreeLineCount = 0;
			bFindThreeLine = false;
			for (BYTE i = FirstCard + 1; i < LeftCardCount + FirstCard; ++i)
			{
				//����ͬ��
				while (GetCardValue(LastCard) == GetCardValue(TmpCard[i]) && i < LeftCardCount + FirstCard)
				{
					++SameCount;
					++i;
				}

				BYTE LastThreeCardValue;
				if (ThreeLineCount > 0) LastThreeCardValue = GetCardLogicValue(ThreeLineCard[ThreeLineCount - 1]);

				//���¿�ʼ
				if ((SameCount < 3 || (ThreeLineCount>0 && (LastThreeCardValue - GetCardLogicValue(LastCard)) != 1)) && i <= LeftCardCount + FirstCard)
				{
					if (ThreeLineCount >= TurnCardCount) break;

					if (SameCount >= 3) i -= 3;
					LastCard = TmpCard[i];
					ThreeLineCount = 0;
				}
				//��������
				else if (SameCount >= 3)
				{
					ThreeLineCard[ThreeLineCount] = TmpCard[i - SameCount];
					ThreeLineCard[ThreeLineCount + 1] = TmpCard[i - SameCount + 1];
					ThreeLineCard[ThreeLineCount + 2] = TmpCard[i - SameCount + 2];
					ThreeLineCount += 3;

					//��β�ж�
					if (i == (LeftCardCount + FirstCard - 3))
						if ((GetCardLogicValue(LastCard) - GetCardLogicValue(TmpCard[i])) == 1 && (GetCardLogicValue(TmpCard[i]) == GetCardLogicValue(TmpCard[i + 1])) && (GetCardLogicValue(TmpCard[i]) == GetCardLogicValue(TmpCard[i + 2])))
						{
							ThreeLineCard[ThreeLineCount] = TmpCard[i];
							ThreeLineCard[ThreeLineCount + 1] = TmpCard[i + 1];
							ThreeLineCard[ThreeLineCount + 2] = TmpCard[i + 2];
							ThreeLineCount += 3;
							break;
						}

				}

				LastCard = TmpCard[i];
				SameCount = 1;
			}

			//��������
			if (ThreeLineCount >= TurnCardCount)
			{
				BYTE Index;
				BYTE Start = 0;
				//��������
				while (GetCardLogicValue(ThreeLineCard[Start]) > GetCardLogicValue(TurnCardData[0]) && ((ThreeLineCount - Start) >= TurnCardCount))
				{
					Index = CardTypeResult[CT_THREE_LINE].CardTypeCount;
					CardTypeResult[CT_THREE_LINE].CardType = CT_THREE_LINE;
					CopyMemory(CardTypeResult[CT_THREE_LINE].CardData[Index], ThreeLineCard + Start, sizeof(BYTE)*TurnCardCount);
					CardTypeResult[CT_THREE_LINE].EachHandCardCount[Index] = TurnCardCount;
					CardTypeResult[CT_THREE_LINE].CardTypeCount++;
					Start += 3;

					assert(CardTypeResult[CT_THREE_LINE].CardTypeCount <= MAX_TYPE_COUNT);
				}

				RemoveCards(ThreeLineCard, ThreeLineCount, TmpCard, FirstCard + LeftCardCount);
				bFindThreeLine = true;
				LeftCardCount -= ThreeLineCount;
			}
		}

		break;
	}
	case CT_THREE_TAKE_ONE://����һ��
	{
		BYTE TurnThreeCard[MAX_COUNT];
		BYTE TurnThreeCount = 0;
		BYTE HandThreeCard[MAX_COUNT];
		BYTE HandThreeCount = 0;
		BYTE SingleCardCount = TurnCardCount / 4;

		//�Ƴ�ը��
		BYTE AllBomCardData[MAX_COUNT];
		BYTE AllBomCardCount = 0;
		GetAllBombCard(TmpCard, HandCardCount, AllBomCardData, AllBomCardCount);
		RemoveCards(AllBomCardData, AllBomCardCount, TmpCard, HandCardCount);

		//�����˿�
		GetAllThreeCard(TurnCardData, TurnCardCount, TurnThreeCard, TurnThreeCount);

		BYTE FirstCard = 0;

		//ȥ��2����
		if (TurnThreeCount > 3)
			for (BYTE i = 0; i < HandCardCount - AllBomCardCount; ++i)
				if (GetCardLogicValue(TmpCard[i]) < 15)
				{
					FirstCard = i;
					break;
				}

		GetAllThreeCard(TmpCard + FirstCard, HandCardCount - FirstCard - AllBomCardCount, HandThreeCard, HandThreeCount);

		if (HandThreeCount < TurnThreeCount || (HandThreeCount>0 && GetCardLogicValue(HandThreeCard[0]) < GetCardLogicValue(TurnThreeCard[0]))) return;

		for (BYTE i = 0; i < HandThreeCount; i += 3)
		{
			BYTE LastLogicCard = GetCardLogicValue(HandThreeCard[i]);
			BYTE ThreeLineCard[MAX_COUNT];
			BYTE ThreeLineCardCount = 3;
			ThreeLineCard[0] = HandThreeCard[i];
			ThreeLineCard[1] = HandThreeCard[i + 1];
			ThreeLineCard[2] = HandThreeCard[i + 2];
			for (BYTE j = i + 3; j<HandThreeCount; j += 3)
			{
				//�����ж�
				if (1 != (LastLogicCard - (GetCardLogicValue(HandThreeCard[j]))) || ThreeLineCardCount == TurnThreeCount) break;

				LastLogicCard = GetCardLogicValue(HandThreeCard[j]);
				ThreeLineCard[ThreeLineCardCount] = HandThreeCard[j];
				ThreeLineCard[ThreeLineCardCount + 1] = HandThreeCard[j + 1];
				ThreeLineCard[ThreeLineCardCount + 2] = HandThreeCard[j + 2];
				ThreeLineCardCount += 3;
			}
			if (ThreeLineCardCount == TurnThreeCount && GetCardLogicValue(ThreeLineCard[0])>GetCardLogicValue(TurnThreeCard[0]))
			{
				BYTE Index;

				BYTE RemainCard[MAX_COUNT];
				CopyMemory(RemainCard, TmpCard, (HandCardCount - AllBomCardCount)*sizeof(BYTE));
				RemoveCards(ThreeLineCard, TurnThreeCount, RemainCard, (HandCardCount - AllBomCardCount));

				//�������
				BYTE ComCard[5];
				BYTE ComResCard[254][5];
				BYTE ComResLen = 0;
				Combination(ComCard, 0, ComResCard, ComResLen, RemainCard, (HandCardCount - AllBomCardCount) - TurnThreeCount, SingleCardCount, SingleCardCount);
				for (BYTE i = 0; i < ComResLen; ++i)
				{
					Index = CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount;
					CardTypeResult[CT_THREE_TAKE_ONE].CardType = CT_THREE_TAKE_ONE;
					//��������
					CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index], ThreeLineCard, sizeof(BYTE)*TurnThreeCount);
					//���浥��
					CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index] + TurnThreeCount, ComResCard[i], SingleCardCount);

					assert(TurnThreeCount + SingleCardCount == TurnCardCount);
					CardTypeResult[CT_THREE_TAKE_ONE].EachHandCardCount[Index] = TurnCardCount;
					CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount++;

					assert(CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount <= MAX_TYPE_COUNT);
				}

			}
		}

		break;
	}
	case CT_THREE_TAKE_TWO://����һ��
	{
		BYTE TurnThreeCard[MAX_COUNT];
		BYTE TurnThreeCount = 0;
		BYTE HandThreeCard[MAX_COUNT];
		BYTE HandThreeCount = 0;
		BYTE DoubleCardCount = TurnCardCount / 5;

		//�����˿�
		GetAllThreeCard(TurnCardData, TurnCardCount, TurnThreeCard, TurnThreeCount);

		BYTE FirstCard = 0;

		//ȥ��2����
		if (TurnThreeCount > 3)
			for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCard[i]) < 15)	{ FirstCard = i; break; }

		GetAllThreeCard(TmpCard + FirstCard, HandCardCount - FirstCard, HandThreeCard, HandThreeCount);

		if (HandThreeCount < TurnThreeCount || (HandThreeCount>0 && GetCardLogicValue(HandThreeCard[0]) < GetCardLogicValue(TurnThreeCard[0]))) return;

		for (BYTE i = 0; i < HandThreeCount; i += 3)
		{
			BYTE LastLogicCard = GetCardLogicValue(HandThreeCard[i]);
			BYTE ThreeLineCard[MAX_COUNT];
			BYTE ThreeLineCardCount = 3;
			ThreeLineCard[0] = HandThreeCard[i];
			ThreeLineCard[1] = HandThreeCard[i + 1];
			ThreeLineCard[2] = HandThreeCard[i + 2];
			for (BYTE j = i + 3; j<HandThreeCount; j += 3)
			{
				//�����ж�
				if (1 != (LastLogicCard - (GetCardLogicValue(HandThreeCard[j]))) || ThreeLineCardCount == TurnThreeCount) break;

				LastLogicCard = GetCardLogicValue(HandThreeCard[j]);
				ThreeLineCard[ThreeLineCardCount] = HandThreeCard[j];
				ThreeLineCard[ThreeLineCardCount + 1] = HandThreeCard[j + 1];
				ThreeLineCard[ThreeLineCardCount + 2] = HandThreeCard[j + 2];
				ThreeLineCardCount += 3;
			}
			if (ThreeLineCardCount == TurnThreeCount && GetCardLogicValue(ThreeLineCard[0])>GetCardLogicValue(TurnThreeCard[0]))
			{
				BYTE Index;

				BYTE RemainCard[MAX_COUNT];
				CopyMemory(RemainCard, TmpCard, HandCardCount*sizeof(BYTE));
				RemoveCards(ThreeLineCard, TurnThreeCount, RemainCard, HandCardCount);

				BYTE AllDoubleCardData[MAX_COUNT];
				BYTE AllDoubleCardCount = 0;
				GetAllDoubleCard(RemainCard, HandCardCount - TurnThreeCount, AllDoubleCardData, AllDoubleCardCount);


				BYTE DoubleCardIndex[10]; //�����±�
				for (BYTE i = 0, j = 0; i < AllDoubleCardCount; i += 2, ++j)
					DoubleCardIndex[j] = i;

				//�������
				BYTE ComCard[5];
				BYTE ComResCard[254][5];
				BYTE ComResLen = 0;

				//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
				Combination(ComCard, 0, ComResCard, ComResLen, DoubleCardIndex, AllDoubleCardCount / 2, DoubleCardCount, DoubleCardCount);
				for (BYTE i = 0; i < ComResLen; ++i)
				{
					Index = CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount;
					CardTypeResult[CT_THREE_TAKE_TWO].CardType = CT_THREE_TAKE_TWO;
					//��������
					CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index], ThreeLineCard, sizeof(BYTE)*TurnThreeCount);
					//�������
					for (BYTE j = 0, k = 0; j < DoubleCardCount; ++j, k += 2)
					{
						CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][TurnThreeCount + k] = AllDoubleCardData[ComResCard[i][j]];
						CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index][TurnThreeCount + k + 1] = AllDoubleCardData[ComResCard[i][j] + 1];
					}

					assert(TurnThreeCount + DoubleCardCount * 2 == TurnCardCount);
					CardTypeResult[CT_THREE_TAKE_TWO].EachHandCardCount[Index] = TurnCardCount;

					CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount++;

					assert(CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount <= MAX_TYPE_COUNT);
				}

			}
		}

		break;
	}
	case CT_FOUR_TAKE_ONE://�Ĵ�����
	{
		BYTE FirstCard = 0;
		//ȥ������
		for (BYTE i = 0; i < HandCardCount; ++i)
		{
			if (GetCardColor(TmpCard[i]) != 0x40)	
			{ 
				FirstCard = i; 
				break; 
			}
		}

		BYTE HandAllFourCardData[MAX_COUNT];
		BYTE HandAllFourCardCount = 0;
		BYTE TurnAllFourCardData[MAX_COUNT];
		BYTE TurnAllFourCardCount = 0;
		//��ȡ����
		GetAllBombCard(TmpCard + FirstCard, HandCardCount - FirstCard, HandAllFourCardData, HandAllFourCardCount);
		GetAllBombCard(TurnCardData, TurnCardCount, TurnAllFourCardData, TurnAllFourCardCount);

		if (HandAllFourCardCount > 0 && GetCardLogicValue(HandAllFourCardData[0]) < GetCardLogicValue(TurnAllFourCardData[0])) return;

		BYTE CanOutFourCardData[MAX_COUNT];
		BYTE CanOutFourCardCount = 0;
		//�ɳ�����
		for (BYTE i = 0; i<HandAllFourCardCount; i += 4)
		{
			if (GetCardLogicValue(HandAllFourCardData[i])>GetCardLogicValue(TurnAllFourCardData[0]))
			{
				CanOutFourCardData[CanOutFourCardCount] = HandAllFourCardData[i];
				CanOutFourCardData[CanOutFourCardCount + 1] = HandAllFourCardData[i + 1];
				CanOutFourCardData[CanOutFourCardCount + 2] = HandAllFourCardData[i + 2];
				CanOutFourCardData[CanOutFourCardCount + 3] = HandAllFourCardData[i + 3];
				CanOutFourCardCount += 4;
			}
		}

		if ((HandCardCount - CanOutFourCardCount) < (TurnCardCount - TurnAllFourCardCount)) return;

		BYTE RemainCard[MAX_COUNT];
		CopyMemory(RemainCard, TmpCard, HandCardCount*sizeof(BYTE));
		RemoveCards(CanOutFourCardData, CanOutFourCardCount, RemainCard, HandCardCount);
		for (BYTE Start = 0; Start < CanOutFourCardCount; Start += 4)
		{
			BYTE Index;
			//�������
			BYTE ComCard[5];
			BYTE ComResCard[254][5];
			BYTE ComResLen = 0;
			//�������
			Combination(ComCard, 0, ComResCard, ComResLen, RemainCard, HandCardCount - CanOutFourCardCount, 2, 2);
			for (BYTE i = 0; i < ComResLen; ++i)
			{
				//���ܴ���
				if (GetCardValue(ComResCard[i][0]) == GetCardValue(ComResCard[i][1])) continue;

				Index = CardTypeResult[CT_FOUR_TAKE_ONE].CardTypeCount;
				CardTypeResult[CT_FOUR_TAKE_ONE].CardType = CT_FOUR_TAKE_ONE;
				CopyMemory(CardTypeResult[CT_FOUR_TAKE_ONE].CardData[Index], CanOutFourCardData + Start, 4);
				CardTypeResult[CT_FOUR_TAKE_ONE].CardData[Index][4] = ComResCard[i][0];
				CardTypeResult[CT_FOUR_TAKE_ONE].CardData[Index][4 + 1] = ComResCard[i][1];
				CardTypeResult[CT_FOUR_TAKE_ONE].EachHandCardCount[Index] = 6;
				CardTypeResult[CT_FOUR_TAKE_ONE].CardTypeCount++;

				assert(CardTypeResult[CT_FOUR_TAKE_ONE].CardTypeCount <= MAX_TYPE_COUNT);
			}
		}
		break;
	}
	case CT_FOUR_TAKE_TWO://�Ĵ�����
	{
		BYTE FirstCard = 0;
		//ȥ������
		for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardColor(TmpCard[i]) != 0x40)	{ FirstCard = i; break; }

		BYTE HandAllFourCardData[MAX_COUNT];
		BYTE HandAllFourCardCount = 0;
		BYTE TurnAllFourCardData[MAX_COUNT];
		BYTE TurnAllFourCardCount = 0;
		//��ȡ����
		GetAllBombCard(TmpCard + FirstCard, HandCardCount - FirstCard, HandAllFourCardData, HandAllFourCardCount);
		GetAllBombCard(TurnCardData, TurnCardCount, TurnAllFourCardData, TurnAllFourCardCount);

		if (HandAllFourCardCount > 0 && GetCardLogicValue(HandAllFourCardData[0]) < GetCardLogicValue(TurnAllFourCardData[0])) 
			return;

		BYTE CanOutFourCardData[MAX_COUNT];
		BYTE CanOutFourCardCount = 0;
		//�ɳ�����
		for (BYTE i = 0; i<HandAllFourCardCount; i += 4)
		{
			if (GetCardLogicValue(HandAllFourCardData[i])>GetCardLogicValue(TurnAllFourCardData[0]))
			{
				CanOutFourCardData[CanOutFourCardCount] = HandAllFourCardData[i];
				CanOutFourCardData[CanOutFourCardCount + 1] = HandAllFourCardData[i + 1];
				CanOutFourCardData[CanOutFourCardCount + 2] = HandAllFourCardData[i + 2];
				CanOutFourCardData[CanOutFourCardCount + 3] = HandAllFourCardData[i + 3];
				CanOutFourCardCount += 4;
			}
		}

		if ((HandCardCount - CanOutFourCardCount) < (TurnCardCount - TurnAllFourCardCount)) return;

		BYTE RemainCard[MAX_COUNT];
		CopyMemory(RemainCard, TmpCard, HandCardCount*sizeof(BYTE));
		RemoveCards(CanOutFourCardData, CanOutFourCardCount, RemainCard, HandCardCount);
		for (BYTE Start = 0; Start < CanOutFourCardCount; Start += 4)
		{
			BYTE AllDoubleCardData[MAX_COUNT];
			BYTE AllDoubleCardCount = 0;
			GetAllDoubleCard(RemainCard, HandCardCount - CanOutFourCardCount, AllDoubleCardData, AllDoubleCardCount);


			BYTE DoubleCardIndex[10]; //�����±�
			for (BYTE i = 0, j = 0; i < AllDoubleCardCount; i += 2, ++j)
				DoubleCardIndex[j] = i;

			//�������
			BYTE ComCard[5];
			BYTE ComResCard[254][5];
			BYTE ComResLen = 0;

			//���ö��Ƶ��±�����ϣ��ٸ����±���ȡ������
			Combination(ComCard, 0, ComResCard, ComResLen, DoubleCardIndex, AllDoubleCardCount / 2, 2, 2);
			for (BYTE i = 0; i < ComResLen; ++i)
			{
				BYTE Index = CardTypeResult[CT_FOUR_TAKE_TWO].CardTypeCount;
				CardTypeResult[CT_FOUR_TAKE_TWO].CardType = CT_FOUR_TAKE_TWO;
				CopyMemory(CardTypeResult[CT_FOUR_TAKE_TWO].CardData[Index], CanOutFourCardData + Start, 4);

				//�������
				for (BYTE j = 0, k = 0; j < 4; ++j, k += 2)
				{
					CardTypeResult[CT_FOUR_TAKE_TWO].CardData[Index][4 + k] = AllDoubleCardData[ComResCard[i][j]];
					CardTypeResult[CT_FOUR_TAKE_TWO].CardData[Index][4 + k + 1] = AllDoubleCardData[ComResCard[i][j] + 1];
				}

				CardTypeResult[CT_FOUR_TAKE_TWO].EachHandCardCount[Index] = 8;
				CardTypeResult[CT_FOUR_TAKE_TWO].CardTypeCount++;

				assert(CardTypeResult[CT_FOUR_TAKE_TWO].CardTypeCount <= MAX_TYPE_COUNT);
			}
		}
		break;
	}
	case CT_BOMB_CARD:			//ը������
	{
		BYTE AllBomCardData[MAX_COUNT];
		BYTE AllBomCardCount = 0;
		GetAllBombCard(TmpCard, HandCardCount, AllBomCardData, AllBomCardCount);
		BYTE FirstBom = 0;
		BYTE Index;
		if (AllBomCardCount > 0 && AllBomCardData[0] == 0x4F)
		{
			Index = CardTypeResult[CT_BOMB_CARD].CardTypeCount;
			CardTypeResult[CT_BOMB_CARD].CardType = CT_BOMB_CARD;
			CardTypeResult[CT_BOMB_CARD].CardData[Index][0] = 0x4F;
			CardTypeResult[CT_BOMB_CARD].CardData[Index][1] = 0x4E;
			CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] = 2;
			CardTypeResult[CT_BOMB_CARD].CardTypeCount++;

			assert(CardTypeResult[CT_BOMB_CARD].CardTypeCount <= MAX_TYPE_COUNT);
			FirstBom = 2;
		}
		for (BYTE i = FirstBom; i<AllBomCardCount; i += 4)
		{
			if (GetCardLogicValue(AllBomCardData[i])>GetCardLogicValue(TurnCardData[0]))
			{
				Index = CardTypeResult[CT_BOMB_CARD].CardTypeCount;
				CardTypeResult[CT_BOMB_CARD].CardType = CT_BOMB_CARD;
				CopyMemory(CardTypeResult[CT_BOMB_CARD].CardData[Index], AllBomCardData + i, 4);
				CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] = 4;
				CardTypeResult[CT_BOMB_CARD].CardTypeCount++;

				assert(CardTypeResult[CT_BOMB_CARD].CardTypeCount <= MAX_TYPE_COUNT);
			}
		}
		break;
	}
	case CT_MISSILE_CARD:		//�������
	{
		//û�бȻ�����������
		break;
	}
	default:
		assert(false);
		break;
	}
	return;
}

/********************************************************************
��������Combination
������
CombineCardData���洢��������Ͻ��
ResComLen���ѵõ�����ϳ��ȣ���ʼ����ʱ�˲���Ϊ0
ResultCardData���洢���е���Ͻ��
ResCardLen��ResultCardData�ĵ�һ�±�ĳ��ȣ���Ͻ���ĸ���
SrcCardData����Ҫ����ϵ�����
SrcLen��cbSrcCardData��������Ŀ
CombineLen2��cbCombineLen1����ϵĳ��ȣ���ʼ����ʱ������ȡ�
*********************************************************************/
void robotAI::Combination(BYTE CombineCardData[], BYTE ResComLen, BYTE ResultCardData[MAX_TYPE_COUNT][5], BYTE &ResCardLen, BYTE SrcCardData[], BYTE SrcLen, BYTE CombineLen1, BYTE CombineLen2)
{
	if (ResComLen == CombineLen2)
	{
		CopyMemory(&ResultCardData[ResCardLen], CombineCardData, ResComLen);
		++ResCardLen;
		assert(ResCardLen < 255);
	}
	else
	{
		if (CombineLen1 >= 1 && SrcLen > 0 && (SrcLen + 1) >= 0)
		{
			CombineCardData[CombineLen2 - CombineLen1] = SrcCardData[0];
			++ResComLen;
			//ȥ��SrcCardData�еĵ�һ������
			Combination(CombineCardData, ResComLen, ResultCardData, ResCardLen, SrcCardData + 1, SrcLen - 1, CombineLen1 - 1, CombineLen2);
			--ResComLen;
			Combination(CombineCardData, ResComLen, ResultCardData, ResCardLen, SrcCardData + 1, SrcLen - 1, CombineLen1, CombineLen2);
		}
	}
}

void robotAI::Permutation(BYTE *list, int m, int n, BYTE result[][4], BYTE &len)
{
	int j, temp;
	if (m == n)
	{
		for (j = 0; j < n; j++)
		{
			result[len][j] = list[j];
		}
		len++;
	}
	else
	{
		for (j = m; j < n; j++)
		{
			temp = list[m];
			list[m] = list[j];
			list[j] = temp;
			Permutation(list, m + 1, n, result, len);
			temp = list[m];
			list[m] = list[j];
			list[j] = temp;
		}
	}
	return;
}

BYTE robotAI::AnalyseSingleCardCount(const BYTE HandCardData[], const BYTE HandCardCount, const BYTE WantOutCardData[], const BYTE WantOutCardCount, BYTE SingleCardData[])
{
	BYTE RemainCard[MAX_COUNT] = { 0 };
	BYTE RemainCardCount = 0;
	CopyMemory(RemainCard, HandCardData, HandCardCount);
	SortCardList(RemainCard, HandCardCount, ST_ORDER);

	if (WantOutCardCount != 0)
	{
		RemoveCards(WantOutCardData, WantOutCardCount, RemainCard, HandCardCount);
	}
	RemainCardCount = HandCardCount - WantOutCardCount;

	typedef void (robotAI::*pGetAllCardFun)(const BYTE[], const BYTE, BYTE[], BYTE &);

	pGetAllCardFun GetAllCardFunArray[4];
	GetAllCardFunArray[0] = &robotAI::GetAllBombCard;
	GetAllCardFunArray[1] = &robotAI::GetAllLineCard;
	GetAllCardFunArray[2] = &robotAI::GetAllThreeCard;
	GetAllCardFunArray[3] = &robotAI::GetAllDoubleCard;

	BYTE IndexArray[4] = { 0, 1, 2, 3 };
	BYTE PermutationRes[24][4];
	BYTE Len = 0;
	BYTE SingleCard[MAX_COUNT];
	ZeroMemory(SingleCard, sizeof(SingleCard));

	Permutation(IndexArray, 0, 4, PermutationRes, Len);
	if (Len != 24) return MAX_COUNT;

	BYTE MinSingleCardCount = MAX_COUNT;
	for (BYTE i = 0; i < 24; ++i)
	{
		BYTE TmpCardData[MAX_COUNT] = { 0 };
		BYTE TmpCardCount = RemainCardCount;
		CopyMemory(TmpCardData, RemainCard, RemainCardCount);

		for (BYTE j = 0; j < 4; j++)
		{
			BYTE Index = PermutationRes[i][j];
			assert(Index >= 0 && Index < 4);
			if (Index < 0 || Index > 4) return MAX_COUNT;

			pGetAllCardFun pTmpGetAllCardFun = GetAllCardFunArray[Index];

			BYTE GetCardData[MAX_COUNT] = { 0 };
			BYTE GetCardCount = 0;

			robotAI ra;
			(ra.*pTmpGetAllCardFun)(TmpCardData, TmpCardCount, GetCardData, GetCardCount);

			if (GetCardCount != 0)
			{
				if (RemoveCards(GetCardData, GetCardCount, TmpCardData, TmpCardCount))
				{
					TmpCardCount -= GetCardCount;
				}
				else
				{
					continue;
				}
			}
		}
		BYTE tmpSingleCard[MAX_COUNT];
		BYTE tmpSingleCardCount = 0;
		GetAllSingleCard(TmpCardData, TmpCardCount, tmpSingleCard, tmpSingleCardCount);
		if (MinSingleCardCount > tmpSingleCardCount)
		{
			MinSingleCardCount = tmpSingleCardCount;
			memset(SingleCard, 0, sizeof(SingleCard));
			memcpy(SingleCard, tmpSingleCard, MinSingleCardCount);
		}
	}
	if (MinSingleCardCount != MAX_COUNT && SingleCardData != NULL)
	{
		memcpy(SingleCardData, SingleCard, MinSingleCardCount);
	}

	return MinSingleCardCount;
}

BYTE robotAI::AnalyseSingleCardCountS(const BYTE HandCardData[], const BYTE HandCardCount, const BYTE WantOutCardData[], const BYTE WantOutCardCount)
{
	BYTE RemainCard[MAX_COUNT] = { 0 };
	BYTE RemainCardCount = 0;
	CopyMemory(RemainCard, HandCardData, HandCardCount);
	SortCardList(RemainCard, HandCardCount, ST_ORDER);

	if (WantOutCardCount != 0)
	{
		RemoveCards(WantOutCardData, WantOutCardCount, RemainCard, HandCardCount);
	}
	RemainCardCount = HandCardCount - WantOutCardCount;

	typedef void (robotAI::*pGetAllCardFun)(const BYTE[], const BYTE, BYTE[], BYTE &);

	pGetAllCardFun GetAllCardFunArray[4];
	GetAllCardFunArray[0] = &robotAI::GetAllBombCard;
	GetAllCardFunArray[1] = &robotAI::GetAllLineCard;
	GetAllCardFunArray[2] = &robotAI::GetAllThreeCard;
	GetAllCardFunArray[3] = &robotAI::GetAllDoubleCard;

	BYTE IndexArray[4] = { 0, 1, 2, 3 };
	BYTE PermutationRes[24][4];
	BYTE Len = 0;
	BYTE SingleCard[MAX_COUNT];
	ZeroMemory(SingleCard, sizeof(SingleCard));

	Permutation(IndexArray, 0, 4, PermutationRes, Len);
	if (Len != 24) return MAX_COUNT;

	BYTE MinSingleCardCount = MAX_COUNT;
	for (BYTE i = 0; i < 24; ++i)
	{
		BYTE TmpCardData[MAX_COUNT] = { 0 };
		BYTE TmpCardCount = RemainCardCount;
		CopyMemory(TmpCardData, RemainCard, RemainCardCount);

		for (BYTE j = 0; j < 4; j++)
		{
			BYTE Index = PermutationRes[i][j];
			assert(Index >= 0 && Index < 4);
			if (Index < 0 || Index > 4) return MAX_COUNT;

			pGetAllCardFun pTmpGetAllCardFun = GetAllCardFunArray[Index];

			BYTE GetCardData[MAX_COUNT] = { 0 };
			BYTE GetCardCount = 0;

			robotAI ra;
			(ra.*pTmpGetAllCardFun)(TmpCardData, TmpCardCount, GetCardData, GetCardCount);

			if (GetCardCount != 0)
			{
				if (RemoveCards(GetCardData, GetCardCount, TmpCardData, TmpCardCount))
				{
					TmpCardCount -= GetCardCount;
				}
				else
				{
					continue;
				}
			}
		}
		BYTE tmpSingleCard[MAX_COUNT];
		BYTE tmpSingleCardCount = 0;
		BYTE tmpSingleCardCountEx = 0;
		GetAllSingleCard(TmpCardData, TmpCardCount, tmpSingleCard, tmpSingleCardCount);
		for (BYTE k = 0; k < tmpSingleCardCount; k++)
		{
			if (GetCardLogicValue(tmpSingleCard[k]) <= 14)
			{
				tmpSingleCardCountEx++;
			}
		}
		if (MinSingleCardCount > tmpSingleCardCountEx)
		{
			MinSingleCardCount = tmpSingleCardCountEx;
			memset(SingleCard, 0, sizeof(SingleCard));
			memcpy(SingleCard, tmpSingleCard + (tmpSingleCardCount - tmpSingleCardCountEx), MinSingleCardCount);
		}
	}

	return MinSingleCardCount;
}

void robotAI::SetUserCard(BYTE ChairID, BYTE CardData[], BYTE CardCount)
{
	CopyMemory(m_UserCardData[ChairID], CardData, CardCount);
	m_UserCardCount[ChairID] = CardCount;

	SortCardList(m_UserCardData[ChairID], CardCount, ST_ORDER);
}

void robotAI::SetBackCard(BYTE ChairID, BYTE BackCardData[], BYTE CardCount)
{
	BYTE TmpCount = m_UserCardCount[ChairID];
	CopyMemory(m_UserCardData[ChairID] + TmpCount, BackCardData, CardCount);
	m_UserCardCount[ChairID] += CardCount;

	SortCardList(m_UserCardData[ChairID], m_UserCardCount[ChairID], ST_ORDER);
}

void robotAI::SetBanker(BYTE Banker)
{
	m_BankerUser = Banker;
}

void robotAI::RemoveUserCardData(BYTE ChairID, BYTE RemoveCardData[], BYTE RemoveCardCount)
{
	bool Success = RemoveCards(RemoveCardData, RemoveCardCount, m_UserCardData[ChairID], m_UserCardCount[ChairID]);
	assert(Success);
	m_UserCardCount[ChairID] -= RemoveCardCount;
}

//�������ȳ��ƣ�
void robotAI::BankerOutCard(BYTE MeChairID, tagOutCardResult &OutCardResult)
{
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	memset(CardTypeResult, 0, sizeof(CardTypeResult));
	memset(&OutCardResult, 0, sizeof(OutCardResult));

	BYTE LineCard[MAX_COUNT] = { 0 };
	BYTE ThreeLineCard[MAX_COUNT] = { 0 };
	BYTE DoubleLineCard[MAX_COUNT] = { 0 };
	BYTE LineCardCount = 0;
	BYTE ThreeLineCardCount = 0;
	BYTE DoubleLineCount = 0;

	GetAllLineCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], LineCard, LineCardCount);
	GetAllThreeCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], ThreeLineCard, ThreeLineCardCount);
	GetAllDoubleCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], DoubleLineCard, DoubleLineCount);

	BYTE UndersideOfBanker = (m_BankerUser + 1) % GAME_PLAYER;		//�����¼�
	BYTE UpsideOfBanker = (UndersideOfBanker + 1) % GAME_PLAYER;	//�����ϼ�

	//���ֻʣ˳�ƺ͵�ֻ(�����)�����ȳ�˳��
	{
		BYTE TmpHandCard[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(LineCard, LineCardCount, TmpHandCard, TmpHandCardLen);
		TmpHandCardLen -= LineCardCount;

		//�����˳�Ƽӵ��ƣ��ȳ�˳��
		if (LineCardCount + 1 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount))
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}
		//�����˳�ƼӶ��ƣ��ȳ�˳��
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount) && GetCardType(TmpHandCard, TmpHandCardLen) != CT_ERROR)
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}

		BYTE TmpHandCard_1[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen_1 = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard_1, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(ThreeLineCard, ThreeLineCardCount, TmpHandCard_1, TmpHandCardLen_1);
		TmpHandCardLen_1 -= ThreeLineCardCount;

		//�����˳�Ƽӵ��ƣ��ȳ�˳��
		if (ThreeLineCardCount + 1 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount))
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}
		//�����˳�ƼӶ��ƣ��ȳ�˳��
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount) && GetCardType(TmpHandCard_1, TmpHandCardLen_1) != CT_ERROR)
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}

		BYTE TmpHandCard_2[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen_2 = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard_2, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(DoubleLineCard, DoubleLineCount, TmpHandCard_2, TmpHandCardLen_2);
		TmpHandCardLen_2 -= DoubleLineCount;

		//���˫˳�ӵ��ƣ��ȳ�˳��
		if (DoubleLineCount + 1 == DoubleLineCount && CT_DOUBLE_LINE == GetCardType(DoubleLineCard, DoubleLineCount))
		{
			OutCardResult.CardCount = DoubleLineCount;
			CopyMemory(OutCardResult.ResultCard, DoubleLineCard, DoubleLineCount);
		}
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_DOUBLE_LINE == GetCardType(DoubleLineCard, DoubleLineCount) && GetCardType(TmpHandCard_2, TmpHandCardLen_2) != CT_ERROR)
		{
			OutCardResult.CardCount = DoubleLineCount;
			CopyMemory(OutCardResult.ResultCard, DoubleLineCard, DoubleLineCount);
		}

		//˫��ը����һ��
		if (m_UserCardCount[MeChairID] > 2 && m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E && CT_ERROR != GetCardType(m_UserCardData[MeChairID] + 2, m_UserCardCount[MeChairID] - 2))
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
		}

		if (OutCardResult.CardCount > 0)
		{
			return;
		}
	}

	//������һֻ
	if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4f;
		OutCardResult.ResultCard[1] = 0x4e;
		return;
	}
	//����
	else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4f;
		OutCardResult.ResultCard[1] = 0x4e;
		return;
	}
	//ֻʣһ����
	else if (CT_ERROR != GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID]))
	{
		OutCardResult.CardCount = m_UserCardCount[MeChairID];
		CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		return;
	}

	//ֻʣһ�ź�һ��
	if (m_UserCardCount[MeChairID] >= 2)
	{
		//�ϼ��˿�
		tagOutCardTypeResult UpsideCanOutCardType1[13];
		ZeroMemory(UpsideCanOutCardType1, sizeof(UpsideCanOutCardType1));
		tagOutCardTypeResult UpsideCanOutCardType2[13];
		ZeroMemory(UpsideCanOutCardType2, sizeof(UpsideCanOutCardType2));

		//�¼��˿�
		tagOutCardTypeResult UndersideCanOutCardType1[13];
		ZeroMemory(UndersideCanOutCardType1, sizeof(UndersideCanOutCardType1));
		tagOutCardTypeResult UndersideCanOutCardType2[13];
		ZeroMemory(UndersideCanOutCardType2, sizeof(UndersideCanOutCardType2));

		//ֻʣһ�ź�һ��
		BYTE FirstHandCardType = GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
		BYTE SecondHandCardType = GetCardType(m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);

		if (CT_ERROR != FirstHandCardType && FirstHandCardType != CT_FOUR_TAKE_ONE && FirstHandCardType != CT_FOUR_TAKE_TWO)
		{
			AnalyseOutCardType(m_UserCardData[UpsideOfBanker], m_UserCardCount[UpsideOfBanker], m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1, UpsideCanOutCardType1);
			AnalyseOutCardType(m_UserCardData[UndersideOfBanker], m_UserCardCount[UndersideOfBanker], m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1, UndersideCanOutCardType1);
		}
		if (CT_ERROR != SecondHandCardType && SecondHandCardType != CT_FOUR_TAKE_ONE && SecondHandCardType != CT_FOUR_TAKE_TWO)
		{
			AnalyseOutCardType(m_UserCardData[UpsideOfBanker], m_UserCardCount[UpsideOfBanker], m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1, UpsideCanOutCardType2);
			AnalyseOutCardType(m_UserCardData[UndersideOfBanker], m_UserCardCount[UndersideOfBanker], m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1, UndersideCanOutCardType2);
		}

		if (SecondHandCardType != CT_ERROR && SecondHandCardType != CT_FOUR_TAKE_ONE && SecondHandCardType != CT_FOUR_TAKE_TWO &&
			UpsideCanOutCardType2[SecondHandCardType].CardTypeCount == 0 && UndersideCanOutCardType2[SecondHandCardType].CardTypeCount == 0 &&
			UpsideCanOutCardType2[CT_BOMB_CARD].CardTypeCount == 0 && UndersideCanOutCardType2[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);
			return;
		}

		if (FirstHandCardType != CT_ERROR && FirstHandCardType != CT_FOUR_TAKE_ONE && FirstHandCardType != CT_FOUR_TAKE_TWO &&
			UpsideCanOutCardType1[FirstHandCardType].CardTypeCount == 0 && UndersideCanOutCardType1[FirstHandCardType].CardTypeCount == 0 &&
			UpsideCanOutCardType1[CT_BOMB_CARD].CardTypeCount == 0 && UndersideCanOutCardType1[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
			return;
		}

		if (GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[UpsideOfBanker][0]) &&
			GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[UndersideOfBanker][0]) &&
			CT_ERROR != SecondHandCardType && SecondHandCardType != CT_FOUR_TAKE_ONE && SecondHandCardType != CT_FOUR_TAKE_TWO &&
			UpsideCanOutCardType2[CT_BOMB_CARD].CardTypeCount == 0 && UndersideCanOutCardType2[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = 1;
			OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
			return;
		}

		if (CT_ERROR != SecondHandCardType && SecondHandCardType != CT_FOUR_TAKE_ONE && SecondHandCardType != CT_FOUR_TAKE_TWO &&
			UpsideCanOutCardType2[CT_BOMB_CARD].CardTypeCount == 0 && UndersideCanOutCardType2[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);
			return;
		}
	}

	{
		tagOutCardTypeResult MeOutCardTypeResult[CT_NUM];
		memset(MeOutCardTypeResult, 0, sizeof(MeOutCardTypeResult));
		AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeOutCardTypeResult);

		BYTE MinSingleCardCount[4];
		MinSingleCardCount[0] = MAX_COUNT;
		MinSingleCardCount[1] = MAX_COUNT;
		MinSingleCardCount[2] = MAX_COUNT;
		MinSingleCardCount[3] = MAX_COUNT;

		BYTE NumIndex[4] = { 0 };
		BYTE OutcardType[4] = { CT_ERROR };
		BYTE MinValue = MAX_COUNT;
		BYTE MinSingleCountInFour = MAX_COUNT;
		BYTE MinCardType = CT_ERROR;
		BYTE MinIndex = 0;

		for (int CardType = CT_DOUBLE; CardType < CT_BOMB_CARD; CardType++)
		{
			tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[CardType];

			BYTE ThisHandCardCount = MAX_COUNT;

			//�ϼ��˿�
			tagOutCardTypeResult UpsideOutCardTypeResult[CT_NUM];
			ZeroMemory(UpsideOutCardTypeResult, sizeof(UpsideOutCardTypeResult));

			//�¼��˿�
			tagOutCardTypeResult UndersideOutCardTypeResult[CT_NUM];
			ZeroMemory(UndersideOutCardTypeResult, sizeof(UndersideOutCardTypeResult));

			for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
			{
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(tmpCardResult.CardData[i][0])) == 4)
				{
					continue;
				}

				//ȥ�������ƺ�ĵ���
				BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

				if (tmpCardResult.EachHandCardCount[i] != ThisHandCardCount)
				{
					ThisHandCardCount = tmpCardResult.EachHandCardCount[i];

					AnalyseOutCardType(m_UserCardData[UndersideOfBanker], m_UserCardCount[UpsideOfBanker], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], UpsideOutCardTypeResult);
					AnalyseOutCardType(m_UserCardData[UndersideOfBanker], m_UserCardCount[UndersideOfBanker], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], UndersideOutCardTypeResult);
				}

				if (CardType == CT_DOUBLE &&
					((m_UserCardCount[UpsideOfBanker] == 2 && UpsideOutCardTypeResult[CardType].CardTypeCount > 0 && GetCardLogicValue(m_UserCardData[UpsideOfBanker][0]) > GetCardLogicValue(tmpCardResult.CardData[i][0])) ||
					(m_UserCardCount[UndersideOfBanker] == 2 && UndersideOutCardTypeResult[CardType].CardTypeCount > 0 && GetCardLogicValue(m_UserCardData[UndersideOfBanker][0]) > GetCardLogicValue(tmpCardResult.CardData[i][0])))
			       )
				{
					continue;
				}

				if (IsCanOneHand(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], MeChairID))
				{
					OutCardResult.CardCount = tmpCardResult.EachHandCardCount[i];
					memcpy(OutCardResult.ResultCard, tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);
					return;
				}

				if (tmpCardResult.EachHandCardCount[i] != m_UserCardCount[MeChairID])
				{
					bool bHaveLargeCard = false;
					for (BYTE j = 0; j < tmpCardResult.EachHandCardCount[i]; ++j)
					{
						if (GetCardLogicValue(tmpCardResult.CardData[i][j]) >= 13)
						{
							if (AnalyseSingleCardCountS(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]) <= 1)
							{
								continue;
							}
							bHaveLargeCard = true;
						}
						if (CardType != CT_SINGLE_LINE && CardType != CT_DOUBLE_LINE && GetCardLogicValue(tmpCardResult.CardData[i][0]) == 14)
						{
							bHaveLargeCard = true;
						}
					}

					if (bHaveLargeCard)
						continue;
				}

				for (BYTE j = 0; j < 4; j++)
				{
					if (MinSingleCardCount[j] >= TmpCount)
					{
						MinSingleCardCount[j] = TmpCount;
						NumIndex[j] = i;
						OutcardType[j] = CardType;
						break;
					}
				}

				if (MinSingleCountInFour >= TmpCount)
				{
					MinCardType = CardType;
					MinSingleCountInFour = TmpCount;
					MinIndex = i;
				}
			}
		}

		if (MinSingleCountInFour >= AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], NULL, 0) + 3 &&
			(m_UserCardCount[UndersideOfBanker] >= 4 && m_UserCardCount[UpsideOfBanker] >= 4))
		{
			MinSingleCountInFour = MAX_COUNT;
		}

		if (MinSingleCountInFour != MAX_COUNT)
		{
			BYTE Index = MinIndex;

			//ѡ����С��
			for (BYTE i = 0; i < 4; ++i)
			{
				if (OutcardType[i] == MinCardType &&
					MinSingleCardCount[i] <= MinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[Index][0]))
					Index = NumIndex[i];
			}

			//������һֻ
			if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4f;
				OutCardResult.ResultCard[1] = 0x4e;
				return;
			}
			//����
			else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4f;
				OutCardResult.ResultCard[1] = 0x4e;
				return;
			}
			else
			{
				//���ñ���
				OutCardResult.CardCount = MeOutCardTypeResult[MinCardType].EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, MeOutCardTypeResult[MinCardType].CardData[Index], MeOutCardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
				return;
			}

			assert(OutCardResult.CardCount > 0);
			return;
		}


		if (OutCardResult.CardCount <= 0 && (m_UserCardCount[UpsideOfBanker] >= 4 || m_UserCardCount[UndersideOfBanker] >= 4))
		{
			if (m_UserCardCount[m_BankerUser] == 1 && MeOutCardTypeResult[CT_SINGLE].CardTypeCount > 0)
			{
				BYTE Index = MAX_COUNT;
				for (BYTE i = 0; i < MeOutCardTypeResult[CT_SINGLE].CardTypeCount; i++)
				{
					if (GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].CardData[i][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
					{
						Index = i;
					}
					else break;
				}

				if (MAX_COUNT != Index)
				{
					OutCardResult.CardCount = MeOutCardTypeResult[CT_SINGLE].EachHandCardCount[Index];
					CopyMemory(OutCardResult.ResultCard, MeOutCardTypeResult[CT_SINGLE].CardData[Index], OutCardResult.CardCount);
					return;
				}
			}
		}
	}

	//���ж�ƫ���Ƿ�����ֻʣһ�����ˣ�����ǣ�������ĵ��ƣ��������ѹ�������ը��
	if (m_UserCardCount[UndersideOfBanker] == 1 || m_UserCardCount[UpsideOfBanker] == 1)
	{
		if (m_UserCardCount[UndersideOfBanker] == 1 && GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[UndersideOfBanker][0]))
		{
			OutCardResult.CardCount = 1;
			OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
			return;
		}
		if (m_UserCardCount[UpsideOfBanker] == 1 && GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[UpsideOfBanker][0]))
		{
			OutCardResult.CardCount = 1;
			OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
			return;
		}
		if (m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		BYTE BombList[MAX_COUNT] = { 0 };
		BYTE BombLen = 0;
		GetAllBombCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombList, BombLen);
		if (BombLen >= 4)
		{
			OutCardResult.CardCount = 4;
			OutCardResult.ResultCard[0] = BombList[BombLen - 4];
			OutCardResult.ResultCard[1] = BombList[BombLen - 3];
			OutCardResult.ResultCard[2] = BombList[BombLen - 2];
			OutCardResult.ResultCard[3] = BombList[BombLen - 1];
			return;
		}
	}

	//����������
	BYTE FirstCard = 0;
	//������
	for (BYTE i = 0; i < m_UserCardCount[MeChairID]; ++i)
		if (GetCardLogicValue(m_UserCardData[MeChairID][i]) < 16)
		{
			FirstCard = i;
			break;
		}

	if (FirstCard < m_UserCardCount[MeChairID] - 1)
		AnalyseOutCardType(m_UserCardData[MeChairID] + FirstCard, m_UserCardCount[MeChairID] - FirstCard, CardTypeResult);
	else
		AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CardTypeResult);

	//���㵥��
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE OutcardType[4] = { CT_ERROR };
	BYTE MinValue = MAX_COUNT;
	BYTE MinSingleCountInFour = MAX_COUNT;
	BYTE MinCardType = CT_ERROR;
	BYTE MinIndex = 0;

	//��ը�������
	for (BYTE CardType = CT_SINGLE; CardType < CT_BOMB_CARD; ++CardType)
	{
		tagOutCardTypeResult const &tmpCardResult = CardTypeResult[CardType];
		for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
		{
			BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

			BYTE MaxValue = 0;
			BYTE Index = 0;

			//����MinSingleCardCount[4]�����ֵ
			for (BYTE j = 0; j < 4; ++j)
			{
				if (MinSingleCardCount[j] >= TmpCount)
				{
					MinSingleCardCount[j] = TmpCount;
					NumIndex[j] = i;
					OutcardType[j] = CardType;
					break;
				}
			}

			//������Сֵ
			if (MinSingleCountInFour >= TmpCount)
			{
				//��С����
				MinCardType = CardType;
				//��С�����е���С����
				MinSingleCountInFour = TmpCount;
				//��С�����е���С��
				MinIndex = i;
			}
		}
	}

	if (MinSingleCountInFour != MAX_COUNT)
	{
		BYTE Index = MinIndex;

		//ѡ����С��
		for (BYTE i = 0; i < 4; ++i)
		{
			if (OutcardType[i] == MinCardType &&
				MinSingleCardCount[i] <= MinSingleCountInFour &&
				GetCardLogicValue(CardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(CardTypeResult[MinCardType].CardData[Index][0]))
			{
				Index = NumIndex[i];
			}
		}

		//������һֻ
		if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		//����
		else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		else
		{
			//���ñ���
			OutCardResult.CardCount = CardTypeResult[MinCardType].EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[MinCardType].CardData[Index], CardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
			return;
		}

		assert(OutCardResult.CardCount > 0);

		return;
	}
	//���ֻʣը��
	else
	{
		BYTE Index = 0;
		BYTE LogicCardValue = GetCardLogicValue(0x4F) + 1;
		//��Сը��
		for (BYTE i = 0; i < CardTypeResult[CT_BOMB_CARD].CardTypeCount; ++i)
			if (LogicCardValue>GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]))
			{
				LogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]);
				Index = i;
			}

		//���ñ���
		OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index];
		CopyMemory(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData[Index], CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] * sizeof(BYTE));

		return;
	}

	//�����û���������ͳ���С��һ��
	OutCardResult.CardCount = 1;
	OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];

	return;
}

//����������ƣ�
void robotAI::BankerOutCard(BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult)
{
	memset(&OutCardResult, 0, sizeof(OutCardResult));

	tagOutCardTypeResult CardTypeResult[CT_NUM];
	memset(CardTypeResult, 0, sizeof(CardTypeResult));

	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);
	AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], TurnCardData, TurnCardCount, CardTypeResult);

	BYTE UndersideUser = (m_BankerUser + 1) % GAME_PLAYER;
	BYTE UpsideUser = (UndersideUser + 1) % GAME_PLAYER;

	if (IsCanOneHandFollow(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], m_BankerUser, TurnCardData, TurnCardCount, OutCardUser, OutCardResult))
	{
		return;
	}

	{
		//����ǵ��ƣ����ж�ƫ���Ƿ�����ֻʣһ�����ˣ�����ǣ�������ĵ��ƣ��������ѹ�������ը��
		if (OutCardType == CT_SINGLE && (m_UserCardCount[UndersideUser] == 1 || m_UserCardCount[UpsideUser] == 1))
		{
			if (m_UserCardCount[UndersideUser] == 1 && GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[UndersideUser][0]))
			{
				OutCardResult.CardCount = 1;
				OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
				return;
			}
			if (m_UserCardCount[UpsideUser] == 1 && GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[UpsideUser][0]))
			{
				OutCardResult.CardCount = 1;
				OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
				return;
			}
			if (m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			BYTE BombList[MAX_COUNT] = { 0 };
			BYTE BombLen = 0;
			GetAllBombCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombList, BombLen);
			if (BombLen >= 4)
			{
				OutCardResult.CardCount = 4;
				OutCardResult.ResultCard[0] = BombList[BombLen - 4];
				OutCardResult.ResultCard[1] = BombList[BombLen - 3];
				OutCardResult.ResultCard[2] = BombList[BombLen - 2];
				OutCardResult.ResultCard[3] = BombList[BombLen - 1];
				return;
			}
		}

		//����Ƕ��ƣ����ж�ƫ���Ƿ�����ֻʣ�����ˣ�����ǣ�������Ķ��ƣ��������ѹ�������ը��
		if (OutCardType == CT_DOUBLE && (m_UserCardCount[UndersideUser] == 2 || m_UserCardCount[UpsideUser] == 2))
		{
			if (m_UserCardCount[UndersideUser] == 2 &&
				GetCardLogicValue(m_UserCardData[UndersideUser][0]) == GetCardLogicValue(m_UserCardData[UndersideUser][1]))
			{
				if (CardTypeResult[CT_DOUBLE].CardTypeCount > 0 && GetCardLogicValue(CardTypeResult[CT_DOUBLE].CardData[0][0]) >= GetCardLogicValue(m_UserCardData[UndersideUser][0]))
				{
					OutCardResult.CardCount = 2;
					memcpy(OutCardResult.ResultCard, CardTypeResult[CT_DOUBLE].CardData[0], CardTypeResult[CT_DOUBLE].EachHandCardCount[0]);
					OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];

					return;
				}
			}
			if (m_UserCardCount[UpsideUser] == 2 &&
				GetCardLogicValue(m_UserCardData[UpsideUser][0]) == GetCardLogicValue(m_UserCardData[UpsideUser][1]))
			{
				if (CardTypeResult[CT_DOUBLE].CardTypeCount > 0 && GetCardLogicValue(CardTypeResult[CT_DOUBLE].CardData[0][0]) >= GetCardLogicValue(m_UserCardData[UpsideUser][0]))
				{
					OutCardResult.CardCount = 2;
					memcpy(OutCardResult.ResultCard, CardTypeResult[CT_DOUBLE].CardData[0], CardTypeResult[CT_DOUBLE].EachHandCardCount[0]);
					OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];

					return;
				}
			}
			if (m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			BYTE BombList[MAX_COUNT] = { 0 };
			BYTE BombLen = 0;
			GetAllBombCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombList, BombLen);
			if (BombLen >= 4)
			{
				OutCardResult.CardCount = 4;
				OutCardResult.ResultCard[0] = BombList[BombLen - 4];
				OutCardResult.ResultCard[1] = BombList[BombLen - 3];
				OutCardResult.ResultCard[2] = BombList[BombLen - 2];
				OutCardResult.ResultCard[3] = BombList[BombLen - 1];
				return;
			}
		}
	}

	//ȡ���ĸ���С�ĵ���
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE MinSingleCountInFour = MAX_COUNT;

	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
	const tagOutCardTypeResult &CanOutCard = CardTypeResult[OutCardType];
	
	for (BYTE i = 0; i < CanOutCard.CardTypeCount; ++i)
	{
		//����ը��
		if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[i][0])) == 4)
		{
			continue;
		}

		//��С����
		BYTE TmpCount = AnalyseSingleCardCount(
			m_UserCardData[MeChairID],
			m_UserCardCount[MeChairID],
			CanOutCard.CardData[i],
			CanOutCard.EachHandCardCount[i]);
		BYTE Maxvalue = 0;
		BYTE Index = 0;

		//����MinSingleCardCount[4]�����ֵ
		for (BYTE j = 0; j < 4; j++)
		{
			if (MinSingleCardCount[j] >= TmpCount)
			{
				MinSingleCardCount[j] = TmpCount;
				NumIndex[j] = i;
				break;
			}
		}
	}
	for (BYTE i = 0; i < 4; ++i)
	{
		if (MinSingleCountInFour > MinSingleCardCount[i])
		{
			MinSingleCountInFour = MinSingleCardCount[i];
		}
	}
	
	//ԭʼ������
	BYTE OriginSingleCardCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], NULL, 0);

	if (CanOutCard.CardTypeCount > 0)
	{
		BYTE MinLogicCardValue = GetCardLogicValue(0x4F) + 1;
		bool FindCard = false;
		BYTE CanOutIndex = 0;
		for (BYTE i = 0; i < 4; ++i)
		{
			BYTE Index = NumIndex[i];
			if ((MinSingleCardCount[i] < OriginSingleCardCount + 3) && 
				MinSingleCardCount[i] <= MinSingleCountInFour &&
				MinLogicCardValue > GetCardLogicValue(CanOutCard.CardData[Index][0]))
			{
				//��Դ���
				bool NoLargeCard = true;

				//������������ϵ�����������4�����ҳ�����С��K���ƶ��Ҳ��ǳ������ϵ������ʱ�����ܳ�2ȥ��
				if (m_UserCardCount[OutCardUser] >= 5 && 
					m_UserCardCount[MeChairID] >= 5 &&
					GetCardLogicValue(CanOutCard.CardData[Index][0]) >= 15 &&
					GetCardLogicValue(TurnCardData[0]) <= 14)
				{
					NoLargeCard = false;
				}

				//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
				for (BYTE k = 3; k < CanOutCard.EachHandCardCount[Index]; ++k)
				{
					if (GetCardLogicValue(CanOutCard.CardData[Index][k]) >= 15 &&
						CanOutCard.EachHandCardCount[Index] != m_UserCardCount[MeChairID])
					{
						NoLargeCard = false;
					}
				}
				if (NoLargeCard)
				{
					FindCard = true;
					CanOutIndex = Index;
					MinLogicCardValue = GetCardLogicValue(CanOutCard.CardData[Index][0]);
				}
			}
		}
		if (FindCard)
		{
			//�����
			BYTE LargesLogicCard;
			if (OutCardUser == UndersideUser) LargesLogicCard = GetCardLogicValue(m_UserCardData[UndersideUser][0]);
			else if (OutCardUser == UpsideUser) LargesLogicCard = GetCardLogicValue(m_UserCardData[UpsideUser][0]);
			bool CanOut = true;

			//��ֻѹ��
			if (GetCardLogicValue(TurnCardData[0]) < LargesLogicCard)
			{
				if (GetCardColor(CanOutCard.CardData[CanOutIndex][0]) == 0x40 &&
					GetCardLogicValue(TurnCardData[0]) <= 14 &&
					m_UserCardCount[MeChairID] > 5)
				{
					CanOut = false;
				}
			}
			if (CanOut)
			{
				OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
				CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));
				return;
			}
		}
		if (OutCardType == CT_SINGLE)
		{
			//�мҵ������
			BYTE LargestLogicCard;
			if (OutCardUser == UndersideUser)
			{
				LargestLogicCard = GetCardLogicValue(m_UserCardData[UndersideUser][0]);
			}
			else if (OutCardUser == UpsideUser)
			{
				LargestLogicCard = GetCardLogicValue(m_UserCardData[UpsideUser][0]);
			}

			if (GetCardLogicValue(TurnCardData[0]) == 14 ||
				GetCardLogicValue(TurnCardData[0]) >= LargestLogicCard ||
				GetCardLogicValue(TurnCardData[0]) < LargestLogicCard-1 ||
				(
					(OutCardUser == UndersideUser && m_UserCardCount[UndersideUser] <= 5) || 
					(OutCardUser == UpsideUser && m_UserCardCount[UpsideUser] <= 5)
				)
				)
			{
				//ȡһ�Ŵ��ڵ���2����Ҫ���мҳ����ƴ����
				BYTE cbIndex = MAX_COUNT;
				for (int i = m_UserCardCount[MeChairID] - 1; i >= 0; i--)
				{
					if (m_UserCardData[MeChairID][i] == 0x4F || m_UserCardData[MeChairID][i] == 0x4E)
					{
						if (m_UserCardCount[MeChairID] > 5)
						{
							continue;
						}
					}
					if (GetCardLogicValue(m_UserCardData[MeChairID][i]) >= GetCardLogicValue(TurnCardData[0]) && 
						GetCardLogicValue(m_UserCardData[MeChairID][i]) >= 15)
					{
						cbIndex = i;
					}
					if (cbIndex != MAX_COUNT)
					{
						OutCardResult.CardCount = 1;
						OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][cbIndex];
						return;
					}
				}
			}

			//������ʱ�����ܹ�������������
			if (GetCardLogicValue(CanOutCard.CardData[CanOutCard.CardTypeCount - 1][0]) >= 16 && m_UserCardCount[MeChairID] > 5)
			{
				return;
			}
		}

		BYTE MinSingleCount = MAX_COUNT;
		BYTE Index = 0;
		bool outflag = false;
		for (BYTE i = 0; i < CardTypeResult[OutCardType].CardTypeCount; ++i)
		{
			BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CardTypeResult[OutCardType].CardData[i], CardTypeResult[OutCardType].EachHandCardCount[i]);
			if (MinSingleCount >= TmpCount &&
				FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[i][0])) < 4)
			{
				MinSingleCount = TmpCount;
				Index = i;
				outflag = true;
			}
		}
		if (outflag)
		{
			OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[Index], OutCardResult.CardCount);
			return;
		}
	}

	//�����ˣ�Ҫ����ը��
	if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0)
	{
		const tagOutCardTypeResult &BombCard = CardTypeResult[CT_BOMB_CARD];
		BYTE MinLogicValue = GetCardLogicValue(BombCard.CardData[0][0]);
		BYTE Index = 0;
		for (BYTE i = 0; i < BombCard.CardTypeCount; ++i)
		{
			if (MinLogicValue > GetCardLogicValue(BombCard.CardData[i][0]))
			{
				MinLogicValue = GetCardLogicValue(BombCard.CardData[i][0]);
				Index = i;
			}
		}

		if (m_UserCardCount[OutCardUser] < 5 ||
			GetCardType(m_UserCardData[OutCardUser], m_UserCardCount[OutCardUser]) != CT_ERROR)
		{
			OutCardResult.CardCount = BombCard.EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, BombCard.CardData[Index], BombCard.EachHandCardCount[Index] * sizeof(BYTE));
			return;
		}

		//�жϳ���ը����ĵ�����
		BYTE SingleCardCount = AnalyseSingleCardCountS(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombCard.CardData[Index], BombCard.EachHandCardCount[Index]);
		if (m_UserCardCount[MeChairID] > 7 || SingleCardCount > 2 || (OutCardType == CT_SINGLE && GetCardLogicValue(TurnCardData[0]) < 15))
		{
			return;
		}

		OutCardResult.CardCount = BombCard.EachHandCardCount[Index];
		CopyMemory(OutCardResult.ResultCard, BombCard.CardData[Index], BombCard.EachHandCardCount[Index] * sizeof(BYTE));
		return;
	}
	return;
}

//�����ϼң��ȳ��ƣ�
void robotAI::UpsideOfBankerOutCard(BYTE MeChairID, tagOutCardResult &OutCardResult)
{
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	memset(CardTypeResult, 0, sizeof(CardTypeResult));
	memset(&OutCardResult, 0, sizeof(OutCardResult));

	BYTE FriendID;
	for (BYTE chairID = 0; chairID < GAME_PLAYER; ++chairID)
	{
		if (chairID != m_BankerUser && chairID != MeChairID) FriendID = chairID;
	}

	BYTE LineCard[MAX_COUNT] = { 0 };
	BYTE ThreeLineCard[MAX_COUNT] = { 0 };
	BYTE DoubleLineCard[MAX_COUNT] = { 0 };
	BYTE LineCardCount = 0;
	BYTE ThreeLineCardCount = 0;
	BYTE DoubleLineCount = 0;
	GetAllLineCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], LineCard, LineCardCount);
	GetAllThreeCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], ThreeLineCard, ThreeLineCardCount);
	GetAllDoubleCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], DoubleLineCard, DoubleLineCount);

	//������
	tagOutCardTypeResult MeOutCardTypeResult[CT_NUM];
	memset(MeOutCardTypeResult, 0, sizeof(MeOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeOutCardTypeResult);

	tagOutCardTypeResult FriendOutCardTypeResult[CT_NUM];
	ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID], FriendOutCardTypeResult);

	tagOutCardTypeResult BankerOutCardTypeResult[CT_NUM];
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerOutCardTypeResult);

	//�����˳�ƺ͵�ֻ��һ�ԣ����ҵ�ֻ��Աȵ�����С�����ȳ�˳
	{
		BYTE TmpHandCard[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(LineCard, LineCardCount, TmpHandCard, TmpHandCardLen);
		TmpHandCardLen -= LineCardCount;

		//�����˳�Ƽӵ��ƣ��ȳ�˳��
		if (LineCardCount + 1 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount))
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}
		//�����˳�ƼӶ��ƣ��ȳ�˳��
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount) && GetCardType(TmpHandCard, TmpHandCardLen) != CT_ERROR)
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}

		BYTE TmpHandCard_1[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen_1 = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard_1, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(ThreeLineCard, ThreeLineCardCount, TmpHandCard_1, TmpHandCardLen_1);
		TmpHandCardLen_1 -= ThreeLineCardCount;

		//�����˳�Ƽӵ��ƣ��ȳ�˳��
		if (ThreeLineCardCount + 1 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount))
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}
		//�����˳�ƼӶ��ƣ��ȳ�˳��
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount) && GetCardType(TmpHandCard_1, TmpHandCardLen_1) != CT_ERROR)
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}

		BYTE TmpHandCard_2[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen_2 = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard_2, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(DoubleLineCard, DoubleLineCount, TmpHandCard_2, TmpHandCardLen_2);
		TmpHandCardLen_2 -= DoubleLineCount;

		//���˫˳�ӵ��ƣ��ȳ�˳��
		if (DoubleLineCount + 1 == DoubleLineCount && CT_DOUBLE_LINE == GetCardType(DoubleLineCard, DoubleLineCount))
		{
			OutCardResult.CardCount = DoubleLineCount;
			CopyMemory(OutCardResult.ResultCard, DoubleLineCard, DoubleLineCount);
		}
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_DOUBLE_LINE == GetCardType(DoubleLineCard, DoubleLineCount) && GetCardType(TmpHandCard_2, TmpHandCardLen_2) != CT_ERROR)
		{
			OutCardResult.CardCount = DoubleLineCount;
			CopyMemory(OutCardResult.ResultCard, DoubleLineCard, DoubleLineCount);
		}


		//˫��ը����һ��
		if (m_UserCardCount[MeChairID] > 2 && m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E && CT_ERROR != GetCardType(m_UserCardData[MeChairID] + 2, m_UserCardCount[MeChairID] - 2))
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
		}

		if (OutCardResult.CardCount > 0)
		{
			return;
		}
	}
	//������һֻ
	if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4F;
		OutCardResult.ResultCard[1] = 0x4E;
		return;
	}
	//����
	else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4F;
		OutCardResult.ResultCard[1] = 0x4E;
		return;
	}
	//ֻʣһ����
	else if (CT_ERROR != GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID]))
	{
		OutCardResult.CardCount = m_UserCardCount[MeChairID];
		CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		return;
	}

	//ֻʣһ�ź�һ��
	if (m_UserCardCount[MeChairID] >= 2)
	{
		//�����˿�
		tagOutCardTypeResult BankerCanOutCardType1[CT_NUM];
		ZeroMemory(BankerCanOutCardType1, sizeof(BankerCanOutCardType1));
		tagOutCardTypeResult BankerCanOutCardType2[CT_NUM];
		ZeroMemory(BankerCanOutCardType2, sizeof(BankerCanOutCardType2));

		BYTE FirstHandCardType = GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
		BYTE SecondHandCardType = GetCardType(m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);

		//�������Գ�����
		if (FirstHandCardType != CT_ERROR)
			AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1, BankerCanOutCardType1);
		if (SecondHandCardType != CT_ERROR)
			AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1, BankerCanOutCardType2);

		if (FirstHandCardType != CT_ERROR && FirstHandCardType != CT_FOUR_TAKE_ONE && FirstHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType1[FirstHandCardType].CardTypeCount == 0 && BankerCanOutCardType1[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
			return;
		}

		if (SecondHandCardType != CT_ERROR && SecondHandCardType != CT_FOUR_TAKE_ONE && SecondHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType2[SecondHandCardType].CardTypeCount == 0 && BankerCanOutCardType2[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);
			return;
		}

		if (GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]) &&
			CT_ERROR != SecondHandCardType && SecondHandCardType != CT_FOUR_TAKE_ONE && SecondHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType2[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = 1;
			OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
			return;
		}

		if (FirstHandCardType != CT_ERROR && FirstHandCardType != CT_FOUR_TAKE_ONE && FirstHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType1[FirstHandCardType].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
			return;
		}
	}

	//��϶����߼�
	{
		//�õ���û��ը������
		if (BankerOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			//��������б�ׯ�Ҹ���ĵ��ƺ���һ�֣���/��/�飩��
			for (BYTE i = 0; i < FriendOutCardTypeResult[CT_SINGLE].CardTypeCount; i++)
			{
				if (GetCardLogicValue(FriendOutCardTypeResult[CT_SINGLE].CardData[i][0]) >= GetCardLogicValue(BankerOutCardTypeResult[CT_SINGLE].CardData[0][0]))
				{
					BYTE TmpExceptSingleFriendCard[MAX_COUNT] = { 0 };
					BYTE TmpExceptSingleLen = m_UserCardCount[FriendID];
					CopyMemory(TmpExceptSingleFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
					RemoveCards(FriendOutCardTypeResult[CT_SINGLE].CardData[i], FriendOutCardTypeResult[CT_SINGLE].EachHandCardCount[i], TmpExceptSingleFriendCard, TmpExceptSingleLen);
					TmpExceptSingleLen -= FriendOutCardTypeResult[CT_SINGLE].EachHandCardCount[i];
					if (GetCardType(TmpExceptSingleFriendCard, TmpExceptSingleLen) != CT_ERROR)
					{
						if (m_UserCardCount[m_BankerUser] != 1)
						{
							OutCardResult.CardCount = 1;
							OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];
							return;
						}
						else
						{
							for (BYTE k = 0; k < MeOutCardTypeResult[CT_SINGLE].CardTypeCount; k++)
							{
								if (GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].CardData[k][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
								{
									OutCardResult.CardCount = 1;
									OutCardResult.ResultCard[0] = MeOutCardTypeResult[CT_SINGLE].CardData[k][0];
									return;
								}
							}
						}
					}
				}
			}

			BYTE MeDoubleCard[MAX_COUNT];
			BYTE MeDoubleCount = 0;
			GetAllDoubleCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeDoubleCard, MeDoubleCount);

			//��������б�ׯ�Ҹ���Ķ��ƺ���һ�֣���/��/�飩��
			for (BYTE i = 0; i < FriendOutCardTypeResult[CT_DOUBLE].CardTypeCount; i++)
			{
				if (GetCardLogicValue(FriendOutCardTypeResult[CT_DOUBLE].CardData[i][0]) >= GetCardLogicValue(BankerOutCardTypeResult[CT_DOUBLE].CardData[0][0]))
				{
					BYTE TmpExceptDoubleFriendCard[MAX_COUNT] = { 0 };
					BYTE TmpExceptDoubleLen = m_UserCardCount[FriendID];
					CopyMemory(TmpExceptDoubleFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
					RemoveCards(FriendOutCardTypeResult[CT_DOUBLE].CardData[i], FriendOutCardTypeResult[CT_DOUBLE].EachHandCardCount[i], TmpExceptDoubleFriendCard, TmpExceptDoubleLen);
					TmpExceptDoubleLen -= FriendOutCardTypeResult[CT_DOUBLE].EachHandCardCount[i];
					if (GetCardType(TmpExceptDoubleFriendCard, TmpExceptDoubleLen) != CT_ERROR && MeDoubleCount != 0)
					{
						BYTE j = MeDoubleCount - 1;
						while (j >= 0)
						{
							if (GetCardLogicValue(FriendOutCardTypeResult[CT_DOUBLE].CardData[i][0]) > GetCardLogicValue(MeDoubleCard[j]))
							{
								OutCardResult.CardCount = 2;
								OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
								OutCardResult.ResultCard[0] = MeDoubleCard[j];
								OutCardResult.ResultCard[1] = MeDoubleCard[j + 1];
								return;
							}
							j -= 2;
						}
					}
				}
			}
		}
	}

	//��������
	{
		BYTE MinSingleCardCount[4];
		MinSingleCardCount[0] = MAX_COUNT;
		MinSingleCardCount[1] = MAX_COUNT;
		MinSingleCardCount[2] = MAX_COUNT;
		MinSingleCardCount[3] = MAX_COUNT;
		BYTE NumIndex[4] = { 0 };
		BYTE OutCardType[4] = { CT_ERROR };
		BYTE MinValue = MAX_COUNT;
		BYTE MinSingleCountInFour = MAX_COUNT;
		BYTE MinCardType = CT_ERROR;
		BYTE MinIndex = 0;

		for (int CardType = CT_DOUBLE; CardType < CT_BOMB_CARD; CardType++)
		{
			tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[CardType];

			if (tmpCardResult.CardTypeCount > 0 &&
				GetCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser]) == CardType &&
				GetCardLogicValue(m_UserCardData[m_BankerUser][0]) > GetCardLogicValue(tmpCardResult.CardData[0][0]))
			{
				continue;
			}

			//��ͬ���͡���ͬ���ȣ���˳��˫˳����ͬ���͵ĳ��ȿ��ܲ�һ��
			BYTE ThisHandCardCount = MAX_COUNT;
			//�����˿�
			tagOutCardTypeResult BankerCanOutCard[CT_NUM];
			memset(BankerCanOutCard, 0, sizeof(BankerCanOutCard));
			tagOutCardTypeResult FriendOutCardTypeResult[CT_NUM];
			memset(FriendOutCardTypeResult, 0, sizeof(FriendOutCardTypeResult));

			for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
			{
				//����ը��
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(tmpCardResult.CardData[i][0])) == 4)
				{
					continue;
				}

				bool flag = IsCanOneHand(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], MeChairID);
				if (flag)
				{
					OutCardResult.CardCount = tmpCardResult.EachHandCardCount[i];
					memcpy(OutCardResult.ResultCard, tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

					return;
				}

				BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

				//���·���
				if (tmpCardResult.EachHandCardCount[i] != ThisHandCardCount)
				{
					AnalyseOutCardType(m_UserCardData[m_BankerUser],
						m_UserCardCount[m_BankerUser],
						tmpCardResult.CardData[i],
						tmpCardResult.EachHandCardCount[i],
						BankerCanOutCard);

					AnalyseOutCardType(m_UserCardData[FriendID],
						m_UserCardCount[FriendID],
						tmpCardResult.CardData[i],
						tmpCardResult.EachHandCardCount[i],
						FriendOutCardTypeResult);
				}

				//�Ƿ��д���
				if (tmpCardResult.EachHandCardCount[i] != m_UserCardCount[MeChairID])
				{
					bool bHaveLargeCard = false;
					for (BYTE j = 0; j < tmpCardResult.EachHandCardCount[i]; ++j)
					{
						if (GetCardLogicValue(tmpCardResult.CardData[i][j]) >= 15)
						{
							if (AnalyseSingleCardCountS(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]) <= 1)
							{
								continue;
							}
							bHaveLargeCard = true;
						}
					}
					if (CardType != CT_SINGLE_LINE && CardType != CT_DOUBLE_LINE  && GetCardLogicValue(tmpCardResult.CardData[i][0]) == 14)
					{
						bHaveLargeCard = true;
					}

					if (bHaveLargeCard)
					{
						continue;
					}
				}

				//�����Ƿ�����ߵ������ﶼû�п���ը��
				if (tmpCardResult.EachHandCardCount[i] == m_UserCardCount[m_BankerUser] &&
					GetCardLogicValue(m_UserCardData[m_BankerUser][0])>GetCardLogicValue(tmpCardResult.CardData[i][0])) continue;

				//����MinSingleCardCount[4]�����ֵ
				for (BYTE j = 0; j < 4; ++j)
				{
					if (MinSingleCardCount[j] >= TmpCount)
					{
						MinSingleCardCount[j] = TmpCount;
						NumIndex[j] = i;
						OutCardType[j] = CardType;
						break;
					}
				}

				//������Сֵ
				if (MinSingleCountInFour >= TmpCount)
				{
					//��С����
					MinCardType = CardType;
					//��С�����е���С����
					MinSingleCountInFour = TmpCount;
					//��С�����е���С��
					MinIndex = i;
				}
			}
		}
		if (MinSingleCountInFour >= AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], NULL, 0) + 3 &&
			m_UserCardCount[m_BankerUser] > 4)
		{
			MinSingleCountInFour = MAX_COUNT;
		}
		if (MinSingleCountInFour != MAX_COUNT)
		{
			BYTE Index = MinIndex;

			//ѡ����С��
			for (BYTE i = 0; i < 4; ++i)
			{
				if (OutCardType[i] == MinCardType && MinSingleCardCount[i] <= MinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[Index][0]))
				{
					Index = NumIndex[i];
				}
			}

			//������һֻ
			if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			//����
			else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4f;
				OutCardResult.ResultCard[1] = 0x4e;
				return;
			}
			else
			{
				//���ñ���
				OutCardResult.CardCount = MeOutCardTypeResult[MinCardType].EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, MeOutCardTypeResult[MinCardType].CardData[Index], MeOutCardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
			}
			return;
		}

		//��������˿�����5����û���ҵ��ʺϵ�����Ӵ����С
		if (OutCardResult.CardCount <= 0 && m_UserCardCount[m_BankerUser] <= 5)
		{
			//ֻ��һ����ʱ���ܷŵ�����
			if (m_UserCardCount[m_BankerUser] == 1 && MeOutCardTypeResult[CT_SINGLE].CardTypeCount > 0)
			{
				BYTE Index = MAX_COUNT;
				for (BYTE i = 0; i < MeOutCardTypeResult[CT_SINGLE].CardTypeCount; ++i)
				{
					if (GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].CardData[i][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
					{
						Index = i;
					}
					else break;
				}

				if (MAX_COUNT != Index)
				{
					OutCardResult.CardCount = MeOutCardTypeResult[CT_SINGLE].EachHandCardCount[Index];
					CopyMemory(OutCardResult.ResultCard, MeOutCardTypeResult[CT_SINGLE].CardData[Index], OutCardResult.CardCount);
					return;
				}
			}
		}
	}

	BYTE FirstCard = 0;
	//������
	for (BYTE i = 0; i < m_UserCardCount[MeChairID]; ++i)
		if (GetCardLogicValue(m_UserCardData[MeChairID][i]) < 16)
		{
			FirstCard = i;
			break;
		}

	if (FirstCard < m_UserCardCount[MeChairID] - 1)
		AnalyseOutCardType(m_UserCardData[MeChairID] + FirstCard, m_UserCardCount[MeChairID] - FirstCard, CardTypeResult);
	else
		AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CardTypeResult);

	//���㵥��
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE OutcardType[4] = { CT_ERROR };
	BYTE MinValue = MAX_COUNT;
	BYTE MinSingleCountInFour = MAX_COUNT;
	BYTE MinCardType = CT_ERROR;
	BYTE MinIndex = 0;

	//������������
	BYTE BankerSingleCardData[MAX_COUNT];
	BYTE BankerSingleCardCount = AnalyseSingleCardCount(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], NULL, 0, BankerSingleCardData);
	BYTE BankerSingleCardLogic = 0;

	if (BankerSingleCardCount > 2 && GetCardLogicValue(BankerSingleCardData[1]) <= 10)
	{
		BankerSingleCardLogic = GetCardLogicValue(BankerSingleCardData[1]);
	}
	else if (BankerSingleCardCount > 0 && GetCardLogicValue(BankerSingleCardData[0]) <= 10)
	{
		BankerSingleCardLogic = GetCardLogicValue(BankerSingleCardData[0]);
	}

	//��ը�������
	for (BYTE CardType = CT_SINGLE; CardType < CT_BOMB_CARD; ++CardType)
	{
		tagOutCardTypeResult const &tmpCardResult = CardTypeResult[CardType];

		if (tmpCardResult.CardTypeCount > 0 &&
			GetCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser]) == CardType &&
			GetCardLogicValue(m_UserCardData[m_BankerUser][0]) > GetCardLogicValue(tmpCardResult.CardData[0][0]))
		{
			continue;
		}

		if (CardType == CT_SINGLE &&
			m_UserCardCount[m_BankerUser] == 1)
		{
			continue;
		}

		for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
		{
			//����ը��
			if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(tmpCardResult.CardData[i][0])) == 4)
			{
				continue;
			}

			//���ܷ��ߵ���С��
			if (CardType == CT_SINGLE &&
				GetCardLogicValue(tmpCardResult.CardData[i][0]) < BankerSingleCardLogic)
			{
				continue;
			}

			if (CardType == CT_SINGLE &&
				m_UserCardCount[m_BankerUser] == 1 &&
				GetCardLogicValue(tmpCardResult.CardData[i][0]) < GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
			{
				continue;
			}

			if (IsCanOneHand(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], MeChairID))
			{
				OutCardResult.CardCount = tmpCardResult.EachHandCardCount[i];
				memcpy(OutCardResult.ResultCard, tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);
				return;
			}

			BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

			BYTE MaxValue = 0;
			BYTE Index = 0;

			//����MinSingleCardCount[4]�����ֵ
			for (BYTE j = 0; j < 4; ++j)
			{
				if (MinSingleCardCount[j] >= TmpCount)
				{
					MinSingleCardCount[j] = TmpCount;
					NumIndex[j] = i;
					OutcardType[j] = CardType;
					break;
				}
			}

			//������Сֵ
			if (MinSingleCountInFour >= TmpCount)
			{
				//��С����
				MinCardType = CardType;
				//��С�����е���С����
				MinSingleCountInFour = TmpCount;
				//��С�����е���С��
				MinIndex = i;
			}
		}
	}

	if (MinSingleCountInFour != MAX_COUNT)
	{
		BYTE Index = MinIndex;

		//ѡ����С��
		for (BYTE i = 0; i < 4; ++i)
		{
			if (OutcardType[i] == MinCardType && MinSingleCardCount[i] <= MinSingleCountInFour &&
				GetCardLogicValue(CardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(CardTypeResult[MinCardType].CardData[Index][0]))
				Index = NumIndex[i];
		}

		//������һֻ
		if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4f;
			OutCardResult.ResultCard[1] = 0x4e;
			return;
		}
		//����
		else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4f;
			OutCardResult.ResultCard[1] = 0x4e;
			return;
		}
		else
		{
			//���ñ���
			OutCardResult.CardCount = CardTypeResult[MinCardType].EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[MinCardType].CardData[Index], CardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
			return;
		}

		return;
	}
	//���ֻʣը��
	else if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0)
	{
		BYTE Index = 0;
		BYTE LogicCardValue = GetCardLogicValue(0x4F) + 1;
		//��Сը��
		for (BYTE i = 0; i < CardTypeResult[CT_BOMB_CARD].CardTypeCount; ++i)
		{
			if (LogicCardValue > GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]))
			{
				LogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]);
				Index = i;
			}
		}
		//���ñ���
		OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index];
		CopyMemory(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData[Index], CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] * sizeof(BYTE));

		return;
	}
	else
	{
		if (m_UserCardCount[m_BankerUser] == 1)
		{
			OutCardResult.CardCount = 1;
			OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
			return;
		}

		//�����û���������ͳ���С��һ��
		OutCardResult.CardCount = 1;
		OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];
	}
	
	return;
}

//�����ϼң�����ƣ�
void robotAI::UpsideOfBankerOutCard(BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult)
{
	//���±�û��
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//��������
	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);

	//�����ɳ���
	tagOutCardTypeResult BankerOutCardTypeResult[CT_NUM];
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult));

	AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerOutCardTypeResult);
	AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], TurnCardData, TurnCardCount, CardTypeResult);

	if (IsCanOneHandFollow(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeChairID, TurnCardData, TurnCardCount, OutCardUser, OutCardResult))
	{
		return;
	}

	{
		//����ǵ��ƣ����ж�ƫ���Ƿ�����ֻʣһ�����ˣ�����ǣ�������ĵ��ƣ��������ѹ�������ը��
		if (OutCardType == CT_SINGLE && m_UserCardCount[m_BankerUser] == 1 &&
			(OutCardUser == m_BankerUser || (OutCardUser != m_BankerUser && GetCardLogicValue(m_UserCardData[m_BankerUser][0]) >= GetCardLogicValue(TurnCardData[0]))))
		{
			if (m_UserCardCount[m_BankerUser] == 1 && GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
			{
				OutCardResult.CardCount = 1;
				OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
				return;
			}
			if (m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			BYTE BombList[MAX_COUNT] = { 0 };
			BYTE BombLen = 0;
			GetAllBombCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombList, BombLen);
			if (BombLen >= 4)
			{
				OutCardResult.CardCount = 4;
				OutCardResult.ResultCard[0] = BombList[BombLen - 4];
				OutCardResult.ResultCard[1] = BombList[BombLen - 3];
				OutCardResult.ResultCard[2] = BombList[BombLen - 2];
				OutCardResult.ResultCard[3] = BombList[BombLen - 1];
				return;
			}
		}

		//����Ƕ��ƣ����ж�ƫ���Ƿ�����ֻʣ�����ˣ�����ǣ�������Ķ��ƣ��������ѹ�������ը��
		if (OutCardType == CT_DOUBLE && m_UserCardCount[m_BankerUser] == 2)
		{
			if (m_UserCardCount[m_BankerUser] == 2 &&
				GetCardLogicValue(m_UserCardData[m_BankerUser][0]) == GetCardLogicValue(m_UserCardData[m_BankerUser][1]))
			{
				if (CardTypeResult[CT_DOUBLE].CardTypeCount > 0 && GetCardLogicValue(CardTypeResult[CT_DOUBLE].CardData[0][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
				{
					OutCardResult.CardCount = 2;
					memcpy(OutCardResult.ResultCard, CardTypeResult[CT_DOUBLE].CardData[0], CardTypeResult[CT_DOUBLE].EachHandCardCount[0]);
					OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];

					return;
				}
			}
			if (m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			BYTE BombList[MAX_COUNT] = { 0 };
			BYTE BombLen = 0;
			GetAllBombCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombList, BombLen);
			if (BombLen >= 4)
			{
				OutCardResult.CardCount = 4;
				OutCardResult.ResultCard[0] = BombList[BombLen - 4];
				OutCardResult.ResultCard[1] = BombList[BombLen - 3];
				OutCardResult.ResultCard[2] = BombList[BombLen - 2];
				OutCardResult.ResultCard[3] = BombList[BombLen - 1];
				return;
			}
		}
	}

	//��϶��Ѵ�
	{
		tagOutCardTypeResult FriendCardTypeResult[CT_NUM];
		ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult));
		tagOutCardTypeResult BankerCardTypeResult[CT_NUM];
		ZeroMemory(BankerCardTypeResult, sizeof(BankerCardTypeResult));

		BYTE FriendID = (m_BankerUser + 1) % GAME_PLAYER;
		AnalyseOutCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID], FriendCardTypeResult);
		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerCardTypeResult);

		bool CanRun = false;

		//��������б�ׯ�Ҹ���ĵ��ƺ���һ�֣���/��/�飩��
		for (BYTE i = 0; i < FriendCardTypeResult[CT_SINGLE].CardTypeCount; i++)
		{
			if (GetCardLogicValue(FriendCardTypeResult[CT_SINGLE].CardData[i][0]) >= GetCardLogicValue(BankerCardTypeResult[CT_SINGLE].CardData[0][0]))
			{
				BYTE TmpExceptSingleFriendCard[MAX_COUNT] = { 0 };
				BYTE TmpExceptSingleLen = m_UserCardCount[FriendID];
				CopyMemory(TmpExceptSingleFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
				RemoveCards(FriendCardTypeResult[CT_SINGLE].CardData[i], FriendCardTypeResult[CT_SINGLE].EachHandCardCount[i], TmpExceptSingleFriendCard, TmpExceptSingleLen);
				TmpExceptSingleLen -= FriendCardTypeResult[CT_SINGLE].EachHandCardCount[i];
				if (GetCardType(TmpExceptSingleFriendCard, TmpExceptSingleLen) != CT_ERROR)
				{
					CanRun = true;
					break;
				}
			}
		}

		BYTE MeDoubleCard[MAX_COUNT];
		BYTE MeDoubleCount = 0;
		GetAllDoubleCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeDoubleCard, MeDoubleCount);

		//��������б�ׯ�Ҹ���Ķ��ƺ���һ�֣���/��/�飩��
		for (BYTE i = 0; i < FriendCardTypeResult[CT_DOUBLE].CardTypeCount; i++)
		{
			if (GetCardLogicValue(FriendCardTypeResult[CT_DOUBLE].CardData[i][0]) >= GetCardLogicValue(BankerCardTypeResult[CT_DOUBLE].CardData[0][0]))
			{
				BYTE TmpExceptDoubleFriendCard[MAX_COUNT] = { 0 };
				BYTE TmpExceptDoubleLen = m_UserCardCount[FriendID];
				CopyMemory(TmpExceptDoubleFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
				RemoveCards(FriendCardTypeResult[CT_DOUBLE].CardData[i], FriendCardTypeResult[CT_DOUBLE].EachHandCardCount[i], TmpExceptDoubleFriendCard, TmpExceptDoubleLen);
				TmpExceptDoubleLen -= FriendCardTypeResult[CT_DOUBLE].EachHandCardCount[i];
				if (GetCardType(TmpExceptDoubleFriendCard, TmpExceptDoubleLen) != CT_ERROR && MeDoubleCount != 0)
				{
					BYTE j = MeDoubleCount - 2;
					while (j >= 0)
					{
						if (GetCardLogicValue(FriendCardTypeResult[CT_DOUBLE].CardData[i][0]) > GetCardLogicValue(MeDoubleCard[j]))
						{
							CanRun = true;
							break;
						}
						j -= 2;
					}
				}
			}
		}

		//���ѿ��ܣ�ׯ��û��ը��
		if (OutCardUser == m_BankerUser &&
			(OutCardType == CT_SINGLE || OutCardType == CT_DOUBLE) &&
			CanRun && 
			BankerCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			//��ǰҪ�������ͣ��Լ��ܳ��ıȵ�������ʱ��������
			if (CardTypeResult[OutCardType].CardTypeCount > 0 &&
				BankerOutCardTypeResult[OutCardType].CardTypeCount > 0 &&
				GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))
			{
				memcpy(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0]);
				OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[0];
				return;
			}
			//����ѹ���ˣ����Լ�Ҳ���ó�
			if (BankerOutCardTypeResult[OutCardType].CardTypeCount == 0)
			{
				return;
			}
			//��������ѹ�����Լ���ը�����ը��
			if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0 &&
				BankerOutCardTypeResult[OutCardType].CardTypeCount >= 0)
			{
				OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0];
				memcpy(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData[0], CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0]);
				return;
			}
		}
	}

	//�������û�д���������ѹ�Լ���
	if (OutCardUser != m_BankerUser &&																						//���Ƶ����Ƕ���
		(BankerOutCardTypeResult[OutCardType].CardTypeCount == 0 ||															//����û�д�������
		GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]) <= GetCardLogicValue(TurnCardData[0])) &&	//��������ѹ����
		CardTypeResult[OutCardType].EachHandCardCount[0] != m_UserCardCount[MeChairID])													//����һ�γ���
	{
		//��������
		return;
	}

	/*//�¼�Ϊ���������ҵ����˿�����5��
	if (m_UserCardCount[m_BankerUser] <= 5 &&																									 //����������С�ڵ���5��
		CardTypeResult[OutCardType].CardTypeCount > 0 &&																						 //���ҿ��Գ�
		OutCardType != CT_BOMB_CARD &&																											 //�ҳ����Ʋ���ը��
		(
			(GetCardLogicValue(TurnCardData[0]) < 12 && OutCardUser != m_BankerUser && BankerOutCardTypeResult[OutCardType].CardTypeCount > 0) ||//���ѳ��ƽ�С�ҵ�������ѹס
			(OutCardUser == m_BankerUser)																										 //��������
		))
	{
		BYTE Index = 0;
		//Ѱ�ҿ���ѹס��������Сһ����
		BYTE ThisOutTypeMinSingleCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0]);
		BYTE BestIndex = 255;
		for (BYTE i = 0; i < CardTypeResult[OutCardType].CardTypeCount; ++i)
		{
			BYTE TmpSingleCardCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CardTypeResult[OutCardType].CardData[i], CardTypeResult[OutCardType].EachHandCardCount[i]);
			if (((	
					BankerOutCardTypeResult[OutCardType].CardTypeCount > 0 &&
					GetCardLogicValue(CardTypeResult[OutCardType].CardData[i][0]) >= GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))||
					BankerOutCardTypeResult[OutCardType].CardTypeCount == 0
				) &&
				TmpSingleCardCount <= ThisOutTypeMinSingleCount)
			{
				BestIndex = i;
				ThisOutTypeMinSingleCount = TmpSingleCardCount;
			}

			if ((BankerOutCardTypeResult[OutCardType].CardTypeCount>0 &&
				GetCardLogicValue(CardTypeResult[OutCardType].CardData[i][0]) >= GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]) ||
				BankerOutCardTypeResult[OutCardType].CardTypeCount == 0))
				Index = i;
			else break;
		}

		if (BestIndex != 255)
		{
			OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[BestIndex];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[BestIndex], OutCardResult.CardCount);
		}
		else
		{
			OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[Index], OutCardResult.CardCount);
		}

		return;
	}//�¼�Ϊ���������ҵ����˿�����5��*/

	//ȡ���ĸ���С����
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE MinSingleCountInFour = MAX_COUNT;

	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
	tagOutCardTypeResult const &CanOutCard = CardTypeResult[OutCardType];

	for (BYTE i = 0; i<CanOutCard.CardTypeCount; ++i)
	{
		//����ը��
		if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[i][0])) == 4)
		{
			continue;
		}

		//��С����
		BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CanOutCard.CardData[i], CanOutCard.EachHandCardCount[i]);
		BYTE MaxValue = 0;
		BYTE Index = 0;

		//����MinSingleCardCount[4]�����ֵ
		for (BYTE j = 0; j < 4; ++j)
		{
			if (MinSingleCardCount[j] >= TmpCount)
			{
				MinSingleCardCount[j] = TmpCount;
				NumIndex[j] = i;
				break;
			}
		}
	}

	for (BYTE i = 0; i < 4; ++i)
	{
		if (MinSingleCountInFour > MinSingleCardCount[i])
		{
			MinSingleCountInFour = MinSingleCardCount[i];
		}
	}

	//ԭʼ������
	BYTE OriginSingleCardCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], NULL, 0);

	//���ѳ���
	bool bFriendOut = m_BankerUser != OutCardUser;
	if (bFriendOut)
	{
		if (CanOutCard.CardTypeCount > 0)
		{
			//������������
			BYTE BankerSingleCardData[MAX_COUNT];
			ZeroMemory(BankerSingleCardData, sizeof(BankerSingleCardData));
			BYTE BankerSingleCardCount = AnalyseSingleCardCount(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], NULL, 0, BankerSingleCardData);
			BYTE BankerSingleCardLogic = 0;

			//�����ж�����ƣ���С�ĵ��ƶ�
			if (BankerSingleCardCount > 3 && GetCardLogicValue(BankerSingleCardData[2]) <= 12)
			{
				BankerSingleCardLogic = GetCardLogicValue(BankerSingleCardData[3]);
			}
			else if (BankerSingleCardCount > 2 && GetCardLogicValue(BankerSingleCardData[1]) <= 10)
			{
				BankerSingleCardLogic = GetCardLogicValue(BankerSingleCardData[1]);
			}
			else if (BankerSingleCardCount > 0 && GetCardLogicValue(BankerSingleCardData[0]) <= 10)
			{
				BankerSingleCardLogic = GetCardLogicValue(BankerSingleCardData[0]);
			}

			BYTE MinLogicCardValue = GetCardLogicValue(0x4F) + 1;
			bool bFindCard = false;
			BYTE CanOutIndex = 0;
			for (BYTE i = 0; i<4; ++i)
			{
				BYTE Index = NumIndex[i];

				//����ը��
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[Index][0])) == 4)
				{
					continue;
				}

				bool bCanOut = false;
				if (OutCardType == CT_SINGLE &&											//�����ǵ��� 
					GetCardLogicValue(TurnCardData[0]) < BankerSingleCardLogic &&		//��������ѹ����
					GetCardLogicValue(CanOutCard.CardData[Index][0]) <= 14 && 
					MinSingleCardCount[i] < OriginSingleCardCount + 2)
				{
					bCanOut = true;
				}

				/*if (OutCardType != CT_SINGLE &&
					GetCardLogicValue(CanOutCard.CardData[Index][0]) >= 14 &&			//����ֵ���ڵ���A
					BankerOutCardTypeResult[OutCardType].CardTypeCount > 1 &&			//ׯ�ҿ�ѹ���ʹ���1
					GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(TurnCardData[0]) &&
					CardTypeResult[OutCardType].CardTypeCount > 0 &&
					GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))
				{
					continue;
				}*/

				//С��K���ƣ�����С��K������ɢ��
				if (bCanOut ||
					(
						(
						 MinSingleCardCount[i] < OriginSingleCardCount + 4 &&
						 MinSingleCardCount[i] <= MinSingleCountInFour &&
							(GetCardLogicValue(CanOutCard.CardData[Index][0]) <= 11 ||
								(MinSingleCardCount[i] < OriginSingleCardCount) &&
								GetCardLogicValue(CanOutCard.CardData[Index][0]) <= 13
							)
						) &&
						MinLogicCardValue > GetCardLogicValue(CanOutCard.CardData[Index][0]) && 
						m_UserCardCount[MeChairID] > 5
					))
				{
					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					bool bNoLargeCard = true;
					for (BYTE k = 3; k < CanOutCard.EachHandCardCount[Index]; ++k)
					{
						//�д��ƶ��Ҳ���һ�γ���
						if (GetCardLogicValue(CanOutCard.CardData[Index][k]) >= 15 &&
							CanOutCard.EachHandCardCount[Index] != m_UserCardCount[MeChairID]) bNoLargeCard = false;
					}
					if (bNoLargeCard)
					{
						bFindCard = true;
						CanOutIndex = Index;
						MinLogicCardValue = GetCardLogicValue(CanOutCard.CardData[Index][0]);
					}
				}
				else if (m_UserCardCount[MeChairID]<5 && MinSingleCardCount[i]<OriginSingleCardCount + 4 && MinSingleCardCount[i] <= MinSingleCountInFour &&
					MinLogicCardValue>GetCardLogicValue(CanOutCard.CardData[Index][0]))
				{
					bFindCard = true;
					CanOutIndex = Index;
					MinLogicCardValue = GetCardLogicValue(CanOutCard.CardData[Index][0]);
				}
			}

			if (bFindCard)
			{

				//���ñ���
				OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
				CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

				return;
			}
			//��������������
			else if (m_UserCardCount[MeChairID] <= 5)
			{
				BYTE MinLogicCard = GetCardLogicValue(0x4f) + 1;
				BYTE CanOutIndex = 0;
				for (BYTE i = 0; i < 4; ++i)
					if (MinSingleCardCount[i] < MAX_COUNT &&
						MinSingleCardCount[i] <= MinSingleCountInFour &&
						MinLogicCard > GetCardLogicValue(CanOutCard.CardData[NumIndex[i]][0]) &&
						GetCardLogicValue(CanOutCard.CardData[NumIndex[i]][0]) <= 14)
					{
						MinLogicCard = GetCardLogicValue(CanOutCard.CardData[NumIndex[i]][0]);
						CanOutIndex = NumIndex[i];
					}

				if (MinLogicCard != (GetCardLogicValue(0x4F) + 1))
				{
					//���ñ���
					OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
					CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

					return;
				}
			}

			return;
		}
		else
		{
			return;
		}
	}
	//��������
	else
	{
		if (CanOutCard.CardTypeCount>0)
		{
			BYTE MinLogicCardValue = GetCardLogicValue(0x4F) + 1;
			bool bFindCard = false;
			BYTE CanOutIndex = 0;
			for (BYTE i = 0; i<4; ++i)
			{
				BYTE Index = NumIndex[i];

				//����ը��
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[Index][0])) == 4)
				{
					continue;
				}

				if ((MinSingleCardCount[i]<OriginSingleCardCount + 3) && MinSingleCardCount[i] <= MinSingleCountInFour &&
					MinLogicCardValue>GetCardLogicValue(CanOutCard.CardData[Index][0]))
				{
					//��Դ���
					bool bNoLargeCard = true;

					//�������Ŀ�ѹ������������1�����������ֵ���Լ�С��ʱ���Լ���ѹ�����ں�������
					if (OutCardType != CT_SINGLE &&
						GetNumOfType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], OutCardType) > 1 &&
						GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(TurnCardData[0]) &&
						CardTypeResult[OutCardType].CardTypeCount > 0 &&
						GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))
					{
						bNoLargeCard = false;
					}

					//������������������10���Լ�������������5�����ҵ���������С��K���ƶ��Ҳ��ǵ������ϵ������ʱ�����ܳ�2ȥ��
					if (m_UserCardCount[m_BankerUser] >= 5 && m_UserCardCount[MeChairID] > 5 && CanOutCard.EachHandCardCount[Index] >= 2 &&
						GetCardLogicValue(CanOutCard.CardData[Index][0]) >= 15 &&
						GetCardLogicValue(TurnCardData[0]) < 13 &&
						GetCardLogicValue(TurnCardData[0]) < GetCardLogicValue(m_UserCardData[m_BankerUser][0]) &&
						CanOutCard.EachHandCardCount[Index] != m_UserCardCount[MeChairID])
					{
						bNoLargeCard = false;
					}

					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					for (BYTE k = 3; k<CanOutCard.EachHandCardCount[Index]; ++k)
					{
						if (GetCardLogicValue(CanOutCard.CardData[Index][k]) >= 15 &&
							CanOutCard.EachHandCardCount[Index] != m_UserCardCount[MeChairID])
							bNoLargeCard = false;
					}
					if (bNoLargeCard)
					{
						bFindCard = true;
						CanOutIndex = Index;
						MinLogicCardValue = GetCardLogicValue(CanOutCard.CardData[Index][0]);
					}
				}
			}

			if (bFindCard)
			{
				//�����������
				BYTE LargestLogicCard = GetCardLogicValue(m_UserCardData[m_BankerUser][0]);
				bool bCanOut = true;

				//��ֻѹ2
				if (GetCardLogicValue(TurnCardData[0])<LargestLogicCard)
				{
					if (GetCardColor(CanOutCard.CardData[CanOutIndex][0]) == 0x40 && GetCardLogicValue(TurnCardData[0]) <= 14 && m_UserCardCount[MeChairID]>5)
					{
						bCanOut = false;
					}
				}

				if (bCanOut)
				{
					//���ñ���
					OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
					CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

					return;
				}
			}

			if (OutCardType == CT_SINGLE)
			{
				//�����������
				BYTE LargestLogicCard = GetCardLogicValue(m_UserCardData[m_BankerUser][0]);

				if (GetCardLogicValue(TurnCardData[0]) == 14 ||
					GetCardLogicValue(TurnCardData[0]) >= LargestLogicCard ||
					(GetCardLogicValue(TurnCardData[0])<LargestLogicCard - 1) ||
					m_UserCardCount[m_BankerUser] <= 5)
				{
					//ȡһ�Ŵ��ڵ���2����Ҫ�ȵ��������ƴ���ƣ�
					BYTE Index = MAX_COUNT;
					for (int i = m_UserCardCount[MeChairID] - 1; i >= 0; i--)
					{
						if (m_UserCardData[MeChairID][i] == 0x4F || m_UserCardData[MeChairID][i] == 0x4E)
						{
							if (m_UserCardCount[MeChairID] > 5)
							{
								continue;
							}
						}
						if (GetCardLogicValue(m_UserCardData[MeChairID][i]) >= GetCardLogicValue(TurnCardData[0]) && GetCardLogicValue(m_UserCardData[MeChairID][i]) >= 15)
						{
							Index = i;
						}
					}
					if (Index != MAX_COUNT)
					{
						//���ñ���
						OutCardResult.CardCount = 1;
						OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][Index];

						return;
					}
				}
			}
			//������ʱ�����ܹ�������������
			if (GetCardLogicValue(CanOutCard.CardData[CanOutCard.CardTypeCount - 1][0]) >= 16 && m_UserCardCount[MeChairID] > 5)
			{
				return;
			}
		}

		//��Ҫ����ը��
		if (CardTypeResult[CT_BOMB_CARD].CardTypeCount>0)
		{
			tagOutCardTypeResult const &BombCard = CardTypeResult[CT_BOMB_CARD];
			BYTE MinLogicValue = GetCardLogicValue(BombCard.CardData[0][0]);
			BYTE Index = 0;
			for (BYTE i = 0; i < BombCard.CardTypeCount; ++i)
			{
				if (MinLogicValue > GetCardLogicValue(BombCard.CardData[i][0]))
				{
					MinLogicValue = GetCardLogicValue(BombCard.CardData[i][0]);
					Index = i;
				}
			}

			if (m_UserCardCount[m_BankerUser] < 5 ||
				GetCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser]) != CT_ERROR)
			{
				OutCardResult.CardCount = BombCard.EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, BombCard.CardData[Index], BombCard.EachHandCardCount[Index] * sizeof(BYTE));
				return;
			}

			//�жϳ���ը����ĵ�����
			BYTE SingleCardList[MAX_COUNT] = { 0 };
			BYTE SingleCardCount = AnalyseSingleCardCountS(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombCard.CardData[Index], BombCard.EachHandCardCount[Index]);
			if (m_UserCardCount[MeChairID] > 7 || SingleCardCount >= 3 || (OutCardType == CT_SINGLE && GetCardLogicValue(TurnCardData[0]) < 14))
			{
				return;
			}

			//���ñ���
			OutCardResult.CardCount = BombCard.EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, BombCard.CardData[Index], BombCard.EachHandCardCount[Index] * sizeof(BYTE));

			return;
		}
		return;
	}
	return;
}

//�����¼ң��ȳ��ƣ�
void robotAI::UndersideOfBankerOutCard(BYTE MeChairID, tagOutCardResult &OutCardResult)
{
	//���±�û��
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	BYTE FriendID = (MeChairID + 1) % GAME_PLAYER;

	//���������˵���
	tagOutCardTypeResult MineOutCardTypeResult[CT_NUM];
	ZeroMemory(MineOutCardTypeResult, sizeof(MineOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MineOutCardTypeResult);

	tagOutCardTypeResult FriendOutCardTypeResult[CT_NUM];
	ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID], FriendOutCardTypeResult);

	tagOutCardTypeResult BankerOutCardTypeResult[CT_NUM];
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerOutCardTypeResult);

	{
		//����ֻʣ��һ����
		BYTE FriendCardType = GetCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID]);
		if (FriendCardType != CT_ERROR &&
			MineOutCardTypeResult[FriendCardType].CardTypeCount > 0 &&
			GetCardLogicValue(MineOutCardTypeResult[FriendCardType].CardData[MineOutCardTypeResult[FriendCardType].CardTypeCount-1][0]) < GetCardLogicValue(m_UserCardData[FriendCardType][0])
			)
		{
			OutCardResult.CardCount = MineOutCardTypeResult[FriendCardType].EachHandCardCount[0];
			memcpy(OutCardResult.ResultCard, MineOutCardTypeResult[FriendCardType].CardData[MineOutCardTypeResult[FriendCardType].CardTypeCount - 1], OutCardResult.CardCount);
			return;
		}

		//�õ���û��ը������
		if (BankerOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			//�������������ը���Լ�һ���ƣ��ҵ���������������1�����Լ��������ĵ���
			for (BYTE i = 0; i < FriendOutCardTypeResult[CT_BOMB_CARD].CardTypeCount; i++)
			{
				BYTE TmpExceptBombFriendCard[MAX_COUNT] = { 0 };
				BYTE TmpExceptBombLen = m_UserCardCount[FriendID];
				CopyMemory(TmpExceptBombFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
				RemoveCards(FriendOutCardTypeResult[CT_BOMB_CARD].CardData[i], FriendOutCardTypeResult[CT_BOMB_CARD].EachHandCardCount[i], TmpExceptBombFriendCard, TmpExceptBombLen);
				TmpExceptBombLen -= FriendOutCardTypeResult[CT_BOMB_CARD].EachHandCardCount[i];
				if (GetCardType(TmpExceptBombFriendCard, TmpExceptBombLen) != CT_ERROR)
				{
					if (m_UserCardCount[m_BankerUser] > 1)
					{
						OutCardResult.CardCount = 1;
						OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];
						return;
					}
					else if (m_UserCardCount[m_BankerUser] == 1)
					{
						for (BYTE type = CT_FOUR_TAKE_TWO; type > CT_SINGLE; type--)
						{
							if (MineOutCardTypeResult[type].CardTypeCount > 0)
							{
								BYTE index = MineOutCardTypeResult[type].CardTypeCount - 1;
								OutCardResult.CardCount = MineOutCardTypeResult[type].EachHandCardCount[index];
								memcpy(OutCardResult.ResultCard, MineOutCardTypeResult[type].CardData[index], MineOutCardTypeResult[type].EachHandCardCount[index]);
								return;
							}
						}
					}
				}
			}

			//��������б�ׯ�Ҹ���ĵ��ƺ���һ�֣���/��/�飩��
			for (BYTE i = 0; i < FriendOutCardTypeResult[CT_SINGLE].CardTypeCount; i++)
			{
				if (m_UserCardCount[FriendID] == 1)
				{
					for (int k = MineOutCardTypeResult[CT_SINGLE].CardTypeCount - 1; k >= 0; k--)
					{
						if (GetCardLogicValue(MineOutCardTypeResult[CT_SINGLE].CardData[k][0]) < GetCardLogicValue(m_UserCardData[FriendID][0]))
						{
							OutCardResult.CardCount = 1;
							OutCardResult.ResultCard[0] = MineOutCardTypeResult[CT_SINGLE].CardData[k][0];
							return;
						}
					}
				}

				if (GetCardLogicValue(FriendOutCardTypeResult[CT_SINGLE].CardData[i][0]) >= GetCardLogicValue(BankerOutCardTypeResult[CT_SINGLE].CardData[0][0]))
				{
					BYTE TmpExceptSingleFriendCard[MAX_COUNT] = { 0 };
					BYTE TmpExceptSingleLen = m_UserCardCount[FriendID];
					CopyMemory(TmpExceptSingleFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
					RemoveCards(FriendOutCardTypeResult[CT_SINGLE].CardData[i], FriendOutCardTypeResult[CT_SINGLE].EachHandCardCount[i], TmpExceptSingleFriendCard, TmpExceptSingleLen);
					TmpExceptSingleLen -= FriendOutCardTypeResult[CT_SINGLE].EachHandCardCount[i];
					if (GetCardType(TmpExceptSingleFriendCard, TmpExceptSingleLen) != CT_ERROR)
					{
						OutCardResult.CardCount = 1;
						OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];
						return;
					}
				}
			}

			BYTE MeDoubleCard[MAX_COUNT];
			BYTE MeDoubleCount = 0;
			GetAllDoubleCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeDoubleCard, MeDoubleCount);

			//��������б�ׯ�Ҹ���Ķ��ƺ���һ�֣���/��/�飩��
			for (BYTE i = 0; i < FriendOutCardTypeResult[CT_DOUBLE].CardTypeCount; i++)
			{
				if (m_UserCardCount[FriendID] == 2 && GetCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID]) == CT_DOUBLE)
				{
					for (int k = MineOutCardTypeResult[CT_DOUBLE].CardTypeCount - 1; k >= 0; k--)
					{
						if (GetCardLogicValue(MineOutCardTypeResult[CT_SINGLE].CardData[k][0]) < GetCardLogicValue(m_UserCardData[FriendID][0]))
						{
							OutCardResult.CardCount = 2;
							OutCardResult.ResultCard[0] = MineOutCardTypeResult[CT_SINGLE].CardData[k][0];
							return;
						}
					}
				}

				if (GetCardLogicValue(FriendOutCardTypeResult[CT_DOUBLE].CardData[i][0]) >= GetCardLogicValue(BankerOutCardTypeResult[CT_DOUBLE].CardData[0][0]))
				{
					BYTE TmpExceptDoubleFriendCard[MAX_COUNT] = { 0 };
					BYTE TmpExceptDoubleLen = m_UserCardCount[FriendID];
					CopyMemory(TmpExceptDoubleFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
					RemoveCards(FriendOutCardTypeResult[CT_DOUBLE].CardData[i], FriendOutCardTypeResult[CT_DOUBLE].EachHandCardCount[i], TmpExceptDoubleFriendCard, TmpExceptDoubleLen);
					TmpExceptDoubleLen -= FriendOutCardTypeResult[CT_DOUBLE].EachHandCardCount[i];
					if (GetCardType(TmpExceptDoubleFriendCard, TmpExceptDoubleLen) != CT_ERROR && MeDoubleCount != 0)
					{
						BYTE j = MeDoubleCount - 2;
						while (j >= 0)
						{
							if (GetCardLogicValue(FriendOutCardTypeResult[CT_DOUBLE].CardData[i][0]) > GetCardLogicValue(MeDoubleCard[j]))
							{
								OutCardResult.CardCount = 2;
								OutCardResult.ResultCard[0] = MeDoubleCard[j];
								OutCardResult.ResultCard[1] = MeDoubleCard[j + 1];
								return;
							}
							j -= 2;
						}
					}
				}
			}
		}
	}

	//�����˳�ƺ͵�ֻ��һ�ԣ����ҵ�ֻ��Աȵ�����С�����ȳ�˳
	{
		BYTE LineCard[MAX_COUNT];
		BYTE ThreeLineCard[MAX_COUNT];
		BYTE DoubleLineCard[MAX_COUNT];
		BYTE LineCardCount;
		BYTE ThreeLineCardCount;
		BYTE DoubleLineCount;
		GetAllLineCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], LineCard, LineCardCount);
		GetAllThreeCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], ThreeLineCard, ThreeLineCardCount);
		GetAllDoubleCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], DoubleLineCard, DoubleLineCount);

		BYTE TmpHandCard[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(LineCard, LineCardCount, TmpHandCard, TmpHandCardLen);
		TmpHandCardLen -= LineCardCount;

		//�����˳�Ƽӵ��ƣ��ȳ�˳��
		if (LineCardCount + 1 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount))
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}
		//�����˳�ƼӶ��ƣ��ȳ�˳��
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount) && GetCardType(TmpHandCard, TmpHandCardLen) != CT_ERROR)
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}

		BYTE TmpHandCard_1[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen_1 = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard_1, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(ThreeLineCard, ThreeLineCardCount, TmpHandCard_1, TmpHandCardLen_1);
		TmpHandCardLen_1 -= ThreeLineCardCount;

		//�����˳�Ƽӵ��ƣ��ȳ�˳��
		if (ThreeLineCardCount + 1 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount))
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}
		//�����˳�ƼӶ��ƣ��ȳ�˳��
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount) && GetCardType(TmpHandCard_1, TmpHandCardLen_1) != CT_ERROR)
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}

		BYTE TmpHandCard_2[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen_2 = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard_2, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(DoubleLineCard, DoubleLineCount, TmpHandCard_2, TmpHandCardLen_2);
		TmpHandCardLen_2 -= DoubleLineCount;

		//���˫˳�ӵ��ƣ��ȳ�˳��
		if (DoubleLineCount + 1 == DoubleLineCount && CT_DOUBLE_LINE == GetCardType(DoubleLineCard, DoubleLineCount))
		{
			OutCardResult.CardCount = DoubleLineCount;
			CopyMemory(OutCardResult.ResultCard, DoubleLineCard, DoubleLineCount);
		}
		else if (LineCardCount + 2 == m_UserCardCount[MeChairID] && CT_DOUBLE_LINE == GetCardType(DoubleLineCard, DoubleLineCount) && GetCardType(TmpHandCard_2, TmpHandCardLen_2) != CT_ERROR)
		{
			OutCardResult.CardCount = DoubleLineCount;
			CopyMemory(OutCardResult.ResultCard, DoubleLineCard, DoubleLineCount);
		}

		//˫��ը����һ��
		if (m_UserCardCount[MeChairID] > 2 &&
			m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E &&
			CT_ERROR != GetCardType(m_UserCardData[MeChairID] + 2, m_UserCardCount[MeChairID] - 2))
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
		}

		if (OutCardResult.CardCount > 0)
		{
			return;
		}
	}

	//������һֻ
	if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4F;
		OutCardResult.ResultCard[1] = 0x4E;
		return;
	}
	//����
	else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4F;
		OutCardResult.ResultCard[1] = 0x4E;
		return;
	}
	//ֻʣһ����
	else if (CT_ERROR != GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID]))
	{
		OutCardResult.CardCount = m_UserCardCount[MeChairID];
		CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		return;
	}

	//ֻʣ��һ�Ż�һ�ԣ���һ��
	if (m_UserCardCount[MeChairID] >= 2)
	{
		//�����˿�
		tagOutCardTypeResult BankerCanOutCardType_ES[CT_NUM];
		ZeroMemory(BankerCanOutCardType_ES, sizeof(BankerCanOutCardType_ES));
		tagOutCardTypeResult BankerCanOutCardType_EB[CT_NUM];
		ZeroMemory(BankerCanOutCardType_EB, sizeof(BankerCanOutCardType_EB));

		//����ȥ����С���ƺ�����������
		BYTE ExceptSmallHandCardType = GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
		//����ȥ�������ƺ�����������
		BYTE ExceptBigHandCardType = GetCardType(m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);

		//�������Գ�����
		if (ExceptSmallHandCardType != CT_ERROR)
		{
			AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1, BankerCanOutCardType_ES);
		}
		if (ExceptBigHandCardType != CT_ERROR)
		{
			AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1, BankerCanOutCardType_EB);
		}

		//��ȥ��С���ƺ��һ���ƣ��������4��1���ߣ�4��2���ҵ���û����ͬ���͵Ĵ��ƺ�ըʱ�ų�
		if (ExceptSmallHandCardType != CT_ERROR &&
			ExceptSmallHandCardType != CT_FOUR_TAKE_ONE &&
			ExceptSmallHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType_ES[ExceptSmallHandCardType].CardTypeCount == 0 &&
			BankerCanOutCardType_ES[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
			return;
		}

		//ȥ������ƺ��һ���ƣ��������4��1����4��2���ҵ���û����ͬ���͵Ĵ���ʱ����������
		if (ExceptBigHandCardType != CT_ERROR &&
			ExceptBigHandCardType != CT_FOUR_TAKE_ONE &&
			ExceptBigHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType_EB[ExceptBigHandCardType].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);
			return;
		}

		//�ҷ����Ʊȵ���Ҫ�󣬳����
		if (GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]) &&
			ExceptBigHandCardType != CT_ERROR &&
			ExceptBigHandCardType != CT_FOUR_TAKE_ONE &&
			ExceptBigHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType_EB[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = 1;
			OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
			return;
		}

		if (ExceptBigHandCardType != CT_ERROR &&
			ExceptBigHandCardType != CT_FOUR_TAKE_ONE &&
			ExceptBigHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType_EB[CT_BOMB_CARD].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);
			return;
		}
	}

	{
		//�����˿�
		tagOutCardTypeResult MeOutCardTypeResult[CT_NUM];
		ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult));
		AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeOutCardTypeResult);

		//�Լ��˿�
		WORD wFriendID;
		for (WORD wChairID = 0; wChairID<GAME_PLAYER; ++wChairID)
			if (wChairID != m_BankerUser && MeChairID != wChairID) wFriendID = wChairID;

		//���㵥��
		BYTE MinSingleCardCount[4];
		MinSingleCardCount[0] = MAX_COUNT;
		MinSingleCardCount[1] = MAX_COUNT;
		MinSingleCardCount[2] = MAX_COUNT;
		MinSingleCardCount[3] = MAX_COUNT;
		BYTE NumIndex[4] = { 0 };
		BYTE OutcardType[4] = { CT_ERROR };
		BYTE MinValue = MAX_COUNT;
		BYTE MinSingleCountInFour = MAX_COUNT;
		BYTE MinCardType = CT_ERROR;
		BYTE MinIndex = 0;

		//��ը�������
		for (int CardType = CT_DOUBLE; CardType < CT_BOMB_CARD; CardType++)
		{
			tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[CardType];

			//��ͬ���ͣ���ͬ���ȣ���������������ͬ���Ϳ��ܳ��Ȳ�һ��
			BYTE ThisHandCardCount = MAX_COUNT;

			//�����˿�
			tagOutCardTypeResult BankerCanOutCard[CT_NUM];
			ZeroMemory(BankerCanOutCard, sizeof(BankerCanOutCard));

			tagOutCardTypeResult FriendOutCardTypeResult[CT_NUM];
			ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult));

			for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
			{
				//����ը��
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(tmpCardResult.CardData[i][0])) == 4)
				{
					continue;
				}

				BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

				//���·���
				if (tmpCardResult.EachHandCardCount[i] != ThisHandCardCount)
				{
					AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser],
						tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], BankerCanOutCard);
					AnalyseOutCardType(m_UserCardData[wFriendID], m_UserCardCount[wFriendID],
						tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], FriendOutCardTypeResult);
				}

				//�����Ƿ�����ߵ������ﶼû�п���ը��
				if (tmpCardResult.EachHandCardCount[i] == m_UserCardCount[m_BankerUser] &&
					GetCardLogicValue(m_UserCardData[m_BankerUser][0]) > GetCardLogicValue(tmpCardResult.CardData[i][0]))
				{
					continue;
				}

				if (IsCanOneHand(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], MeChairID))
				{
					OutCardResult.CardCount = tmpCardResult.EachHandCardCount[i];
					memcpy(OutCardResult.ResultCard, tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);
					return;
				}

				//Ҫ�������������Ƿ�����˴���
				if (tmpCardResult.EachHandCardCount[i] != m_UserCardCount[MeChairID])
				{
					bool bHaveLargeCard = false;
					for (BYTE j = 0; j < tmpCardResult.EachHandCardCount[i]; ++j)
						if (GetCardLogicValue(tmpCardResult.CardData[i][j]) >= 15)
						{
							if (AnalyseSingleCardCountS(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]) <= 1)
							{
								continue;
							}
							bHaveLargeCard = true;
						}

					if (CardType != CT_SINGLE_LINE && CardType != CT_DOUBLE_LINE && GetCardLogicValue(tmpCardResult.CardData[i][0]) == 14) bHaveLargeCard = true;

					if (bHaveLargeCard) continue;
				}

				//����MinSingleCardCount[4]�����ֵ
				for (BYTE j = 0; j<4; ++j)
				{
					if (MinSingleCardCount[j] >= TmpCount)
					{
						MinSingleCardCount[j] = TmpCount;
						NumIndex[j] = i;
						OutcardType[j] = CardType;
						break;
					}
				}

				//������Сֵ
				if (MinSingleCountInFour >= TmpCount)
				{
					//��С����
					MinCardType = CardType;
					//��С�����е���С����
					MinSingleCountInFour = TmpCount;
					//��С�����е���С��
					MinIndex = i;
				}
			}
		}

		if (MinSingleCountInFour >= AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], NULL, 0) + 2 && m_UserCardCount[m_BankerUser] > 4)
		{
			MinSingleCountInFour = MAX_COUNT;
		}

		if (MinSingleCountInFour != MAX_COUNT)
		{
			BYTE Index = MinIndex;

			//ѡ����С��
			for (BYTE i = 0; i < 4; ++i)
			{
				if (OutcardType[i] == MinCardType &&
					MinSingleCardCount[i] <= MinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[NumIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[Index][0]))
				{
					Index = NumIndex[i];
				}
			}

			//������һֻ
			if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			//����
			else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			else
			{
				//���ñ���
				OutCardResult.CardCount = MeOutCardTypeResult[MinCardType].EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, MeOutCardTypeResult[MinCardType].CardData[Index], MeOutCardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
				return;
			}

			return;
		}

		//��������˿�����5����û���ҵ��ʺϵ�����Ӵ����С
		if (OutCardResult.CardCount <= 0 && m_UserCardCount[m_BankerUser] <= 5)
		{
			//ֻ��һ����ʱ���ܷŵ�����
			if (m_UserCardCount[m_BankerUser] == 1 && MeOutCardTypeResult[CT_SINGLE].CardTypeCount>0)
			{
				BYTE Index = MAX_COUNT;
				for (BYTE i = 0; i<MeOutCardTypeResult[CT_SINGLE].CardTypeCount; ++i)
				{
					if (GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].CardData[i][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
					{
						Index = i;
					}
					else break;
				}

				if (MAX_COUNT != Index)
				{
					OutCardResult.CardCount = MeOutCardTypeResult[CT_SINGLE].EachHandCardCount[Index];
					CopyMemory(OutCardResult.ResultCard, MeOutCardTypeResult[CT_SINGLE].CardData[Index], OutCardResult.CardCount);
					return;
				}
			}
		}
	}

	BYTE FirstCardIndex = 0;
	//��������2
	for (BYTE i = 0; i < m_UserCardCount[MeChairID]; ++i)
	{
		if (GetCardLogicValue(m_UserCardData[MeChairID][i]) < 15)
		{
			FirstCardIndex = i;
			break;
		}
	}

	if (FirstCardIndex < m_UserCardCount[MeChairID] - 1)
	{
		AnalyseOutCardType(m_UserCardData[MeChairID] + FirstCardIndex, m_UserCardCount[MeChairID] - FirstCardIndex, CardTypeResult);
	}
	else
	{
		AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CardTypeResult);
	}

	//���㵥��
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE OutcardType[4] = { CT_ERROR };
	BYTE MinValue = MAX_COUNT;
	BYTE MinSingleCountInFour = MAX_COUNT;
	BYTE MinCardType = CT_ERROR;
	BYTE MinIndex = 0;

	//��ը�������
	for (BYTE CardType = CT_SINGLE; CardType < CT_BOMB_CARD; ++CardType)
	{
		tagOutCardTypeResult const &tmpCardResult = CardTypeResult[CardType];

		if (CardType == CT_SINGLE && m_UserCardCount[m_BankerUser] == 1)
		{
			continue;
		}
		for (BYTE i = 0; i<tmpCardResult.CardTypeCount; ++i)
		{
			BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

			BYTE MaxValue = 0;
			BYTE Index = 0;

			//����MinSingleCardCount[4]�����ֵ
			for (BYTE j = 0; j < 4; ++j)
			{
				if (MinSingleCardCount[j] >= TmpCount)
				{
					MinSingleCardCount[j] = TmpCount;
					NumIndex[j] = i;
					OutcardType[j] = CardType;
					break;
				}
			}

			//������Сֵ
			if (MinSingleCountInFour >= TmpCount)
			{
				//��С����
				MinCardType = CardType;
				//��С�����е���С����
				MinSingleCountInFour = TmpCount;
				//��С�����е���С��
				MinIndex = i;
			}
		}
	}

	if (MinSingleCountInFour != MAX_COUNT)
	{
		BYTE Index = MinIndex;

		//ѡ����С��
		for (BYTE i = 0; i<4; ++i)
		{
			if (OutcardType[i] == MinCardType && 
				MinSingleCardCount[i] <= MinSingleCountInFour &&
				GetCardLogicValue(CardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(CardTypeResult[MinCardType].CardData[Index][0]))
			{
				Index = NumIndex[i];
			}
		}

		//������һֻ
		if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		//����
		else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		else
		{
			//���ñ���
			OutCardResult.CardCount = CardTypeResult[MinCardType].EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[MinCardType].CardData[Index], CardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
			return;
		}

		return;
	}
	//���ֻʣը��
	else if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0)
	{
		BYTE Index = 0;
		BYTE LogicCardValue = GetCardLogicValue(0x4F) + 1;
		//��Сը��
		for (BYTE i = 0; i < CardTypeResult[CT_BOMB_CARD].CardTypeCount; ++i)
		{
			if (LogicCardValue > GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]))
			{
				LogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]);
				Index = i;
			}
		}
		//���ñ���
		OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index];
		CopyMemory(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData[Index], CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] * sizeof(BYTE));

		return;
	}
	else
	{
		if (m_UserCardCount[m_BankerUser] == 1)
		{
			OutCardResult.CardCount = 1;
			OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
			return;
		}

		//�����û���������ͳ���С��һ��
		OutCardResult.CardCount = 1;
		OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];
	}
	
	return;
}

//�����¼ң�����ƣ�
void robotAI::UndersideOfBankerOutCard(BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult)
{
	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//���±�û��
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	//��������
	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);

	//�����ɳ���
	tagOutCardTypeResult BankerOutCardTypeResult[CT_NUM];
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult));

	AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerOutCardTypeResult);
	AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], TurnCardData, TurnCardCount, CardTypeResult);

	BYTE FriendID = (MeChairID + 1) % GAME_PLAYER;

	if (IsCanOneHandFollow(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeChairID, TurnCardData, TurnCardCount, OutCardUser, OutCardResult))
	{
		return;
	}

	{
		//����ǵ��ƣ����ж�ƫ���Ƿ�����ֻʣһ�����ˣ�����ǣ�������ĵ��ƣ��������ѹ�������ը��
		if (OutCardType == CT_SINGLE && m_UserCardCount[m_BankerUser] == 1 &&
			(m_BankerUser == OutCardUser || (m_BankerUser != OutCardUser && GetCardLogicValue(m_UserCardData[m_BankerUser][0]) > GetCardLogicValue(m_UserCardData[FriendID][0]))))
		{
			if (m_UserCardCount[m_BankerUser] == 1 && GetCardLogicValue(m_UserCardData[MeChairID][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
			{
				OutCardResult.CardCount = 1;
				OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];
				return;
			}
			if (m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			BYTE BombList[MAX_COUNT] = { 0 };
			BYTE BombLen = 0;
			GetAllBombCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombList, BombLen);
			if (BombLen >= 4)
			{
				OutCardResult.CardCount = 4;
				OutCardResult.ResultCard[0] = BombList[BombLen - 4];
				OutCardResult.ResultCard[1] = BombList[BombLen - 3];
				OutCardResult.ResultCard[2] = BombList[BombLen - 2];
				OutCardResult.ResultCard[3] = BombList[BombLen - 1];
				return;
			}
		}

		//����Ƕ��ƣ����ж�ƫ���Ƿ�����ֻʣ�����ˣ�����ǣ�������Ķ��ƣ��������ѹ�������ը��
		if (OutCardType == CT_DOUBLE && m_UserCardCount[m_BankerUser] == 2)
		{
			if (m_UserCardCount[m_BankerUser] == 2 &&
				GetCardLogicValue(m_UserCardData[m_BankerUser][0]) == GetCardLogicValue(m_UserCardData[m_BankerUser][1]))
			{
				if (CardTypeResult[CT_DOUBLE].CardTypeCount > 0 && GetCardLogicValue(CardTypeResult[CT_DOUBLE].CardData[0][0]) >= GetCardLogicValue(m_UserCardData[m_BankerUser][0]))
				{
					OutCardResult.CardCount = 2;
					memcpy(OutCardResult.ResultCard, CardTypeResult[CT_DOUBLE].CardData[0], CardTypeResult[CT_DOUBLE].EachHandCardCount[0]);
					OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][0];

					return;
				}
			}
			if (m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			BYTE BombList[MAX_COUNT] = { 0 };
			BYTE BombLen = 0;
			GetAllBombCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombList, BombLen);
			if (BombLen >= 4)
			{
				OutCardResult.CardCount = 4;
				OutCardResult.ResultCard[0] = BombList[BombLen - 4];
				OutCardResult.ResultCard[1] = BombList[BombLen - 3];
				OutCardResult.ResultCard[2] = BombList[BombLen - 2];
				OutCardResult.ResultCard[3] = BombList[BombLen - 1];
				return;
			}
		}
	}

	//��϶��Ѵ�
	{
		tagOutCardTypeResult FriendCardTypeResult[CT_NUM];
		ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult));
		tagOutCardTypeResult BankerCardTypeResult[CT_NUM];
		ZeroMemory(BankerCardTypeResult, sizeof(BankerCardTypeResult));

		AnalyseOutCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID], FriendCardTypeResult);
		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerCardTypeResult);

		bool CanRun = false;

		//��������б�ׯ�Ҹ���ĵ��ƺ���һ�֣���/��/�飩��
		for (BYTE i = 0; i < FriendCardTypeResult[CT_SINGLE].CardTypeCount; i++)
		{
			if (GetCardLogicValue(FriendCardTypeResult[CT_SINGLE].CardData[i][0]) >= GetCardLogicValue(BankerCardTypeResult[CT_SINGLE].CardData[0][0]))
			{
				BYTE TmpExceptSingleFriendCard[MAX_COUNT] = { 0 };
				BYTE TmpExceptSingleLen = m_UserCardCount[FriendID];
				CopyMemory(TmpExceptSingleFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
				RemoveCards(FriendCardTypeResult[CT_SINGLE].CardData[i], FriendCardTypeResult[CT_SINGLE].EachHandCardCount[i], TmpExceptSingleFriendCard, TmpExceptSingleLen);
				TmpExceptSingleLen -= FriendCardTypeResult[CT_SINGLE].EachHandCardCount[i];
				if (GetCardType(TmpExceptSingleFriendCard, TmpExceptSingleLen) != CT_ERROR)
				{
					CanRun = true;
					break;
				}
			}
		}

		BYTE MeDoubleCard[MAX_COUNT];
		BYTE MeDoubleCount = 0;
		GetAllDoubleCard(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeDoubleCard, MeDoubleCount);

		//��������б�ׯ�Ҹ���Ķ��ƺ���һ�֣���/��/�飩��
		for (BYTE i = 0; i < FriendCardTypeResult[CT_DOUBLE].CardTypeCount; i++)
		{
			if (GetCardLogicValue(FriendCardTypeResult[CT_DOUBLE].CardData[i][0]) >= GetCardLogicValue(BankerCardTypeResult[CT_DOUBLE].CardData[0][0]))
			{
				BYTE TmpExceptDoubleFriendCard[MAX_COUNT] = { 0 };
				BYTE TmpExceptDoubleLen = m_UserCardCount[FriendID];
				CopyMemory(TmpExceptDoubleFriendCard, m_UserCardData[FriendID], m_UserCardCount[FriendID]);
				RemoveCards(FriendCardTypeResult[CT_DOUBLE].CardData[i], FriendCardTypeResult[CT_DOUBLE].EachHandCardCount[i], TmpExceptDoubleFriendCard, TmpExceptDoubleLen);
				TmpExceptDoubleLen -= FriendCardTypeResult[CT_DOUBLE].EachHandCardCount[i];
				if (GetCardType(TmpExceptDoubleFriendCard, TmpExceptDoubleLen) != CT_ERROR && MeDoubleCount != 0)
				{
					BYTE j = MeDoubleCount - 1;
					while (j >= 0)
					{
						if (GetCardLogicValue(FriendCardTypeResult[CT_DOUBLE].CardData[i][0]) > GetCardLogicValue(MeDoubleCard[j]))
						{
							CanRun = true;
							break;
						}
						j -= 2;
					}
				}
			}
		}

		//���ѿ��ܣ�ׯ��û��ը��
		if (OutCardUser == m_BankerUser &&
			(OutCardType == CT_SINGLE || OutCardType == CT_DOUBLE) &&
			CanRun &&
			BankerCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			//��ǰҪ�������ͣ��Լ��ܳ��ıȵ�������ʱ��������
			if (CardTypeResult[OutCardType].CardTypeCount > 0 &&
				BankerOutCardTypeResult[OutCardType].CardTypeCount > 0 &&
				GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))
			{
				memcpy(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0]);
				OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[0];
				return;
			}
			//����ѹ���ˣ����Լ�Ҳ���ó�
			if (BankerOutCardTypeResult[OutCardType].CardTypeCount == 0)
			{
				return;
			}
			//��������ѹ�����Լ���ը�����ը��
			if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0 &&
				BankerOutCardTypeResult[OutCardType].CardTypeCount >= 0)
			{
				OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0];
				memcpy(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData[0], CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0]);
				return;
			}
		}
	}

	//ȡ���ĸ���С����
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE MinSingleCountInFour = MAX_COUNT;

	//�ɳ��˿ˣ������Ѿ����˵�ը���ˣ�
	tagOutCardTypeResult const &CanOutCard = CardTypeResult[OutCardType];

	for (BYTE i = 0; i < CanOutCard.CardTypeCount; ++i)
	{
		//����ը��
		if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[i][0])) == 4)
		{
			continue;
		}

		//��С����
		BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CanOutCard.CardData[i], CanOutCard.EachHandCardCount[i]);
		BYTE MaxValue = 0;
		BYTE Index = 0;

		//����MinSingleCardCount[4]�����ֵ
		for (BYTE j = 0; j < 4; ++j)
		{
			if (MinSingleCardCount[j] >= TmpCount)
			{
				MinSingleCardCount[j] = TmpCount;
				NumIndex[j] = i;
				break;
			}
		}
	}

	for (BYTE i = 0; i < 4; ++i)
	{
		if (MinSingleCountInFour > MinSingleCardCount[i])
		{
			MinSingleCountInFour = MinSingleCardCount[i];
		}
	}
	
	//ԭʼ������
	BYTE OriginSingleCardCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], NULL, 0);

	//���ѳ���
	bool bFriendOut = (m_BankerUser != OutCardUser);
	if (bFriendOut)
	{
		if (CanOutCard.CardTypeCount > 0)
		{
			BYTE MinLogicCardValue = GetCardLogicValue(0x4F) + 1;
			bool bFindCard = false;
			BYTE CanOutIndex = 0;
			for (BYTE i = 0; i < 4; ++i)
			{
				BYTE Index = NumIndex[i];
				
				//����ը��
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[Index][0])) == 4)
				{
					continue;
				}

				//С��J���ƣ�����С��A������ɢ��(�߼�������ȼ����߼���Ҫ��)
				if ((
							MinSingleCardCount[i] < OriginSingleCardCount + 2 &&
							MinSingleCardCount[i] <= MinSingleCountInFour &&
							(
								GetCardLogicValue(CanOutCard.CardData[Index][0]) <= 11 ||
								(MinSingleCardCount[i] < OriginSingleCardCount) && GetCardLogicValue(CanOutCard.CardData[Index][0]) <= 14
							)
					) &&
					MinLogicCardValue > GetCardLogicValue(CanOutCard.CardData[Index][0]) &&
					m_UserCardCount[MeChairID] > 5)
				{
					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					bool bNoLargeCard = true;
					for (BYTE k = 3; k < CanOutCard.EachHandCardCount[Index]; ++k)
					{
						//�д��ƶ��Ҳ���һ�γ���
						if (GetCardLogicValue(CanOutCard.CardData[Index][k]) >= 14 &&
							CanOutCard.EachHandCardCount[Index] != m_UserCardCount[MeChairID]) 
						{
							bNoLargeCard = false;
						}
					}
					if (bNoLargeCard)
					{
						bFindCard = true;
						CanOutIndex = Index;
						MinLogicCardValue = GetCardLogicValue(CanOutCard.CardData[Index][0]);
					}
				}
				else if (m_UserCardCount[MeChairID] < 5 &&
					MinSingleCardCount[i] < OriginSingleCardCount + 4 &&
					MinSingleCardCount[i] <= MinSingleCountInFour &&
					MinLogicCardValue > GetCardLogicValue(CanOutCard.CardData[Index][0]))
				{
					bFindCard = true;
					CanOutIndex = Index;
					MinLogicCardValue = GetCardLogicValue(CanOutCard.CardData[Index][0]);
				}
			}

			if (bFindCard)
			{
				//���ñ���
				OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
				CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

				return;
			}
			//��������������
			else if (m_UserCardCount[MeChairID] <= 5)
			{
				BYTE MinLogicCard = GetCardLogicValue(0x4F) + 1;
				BYTE CanOutIndex = 0;
				for (BYTE i = 0; i < 4; ++i)
				{
					if (MinSingleCardCount[i] < MAX_COUNT &&
						MinSingleCardCount[i] <= MinSingleCountInFour &&
						MinLogicCard > GetCardLogicValue(CanOutCard.CardData[NumIndex[i]][0]) &&
						GetCardLogicValue(CanOutCard.CardData[NumIndex[i]][0]) <= 14)
					{
						MinLogicCard = GetCardLogicValue(CanOutCard.CardData[NumIndex[i]][0]);
						CanOutIndex = NumIndex[i];
					}
				}

				if (MinLogicCard != (GetCardLogicValue(0x4F) + 1))
				{
					//���ñ���
					OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
					CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

					return;
				}
			}
			return;
		}
		else
		{
			return;
		}
	}
	//��������
	else
	{
		if (CanOutCard.CardTypeCount > 0)
		{
			BYTE MinLogicCardValue = GetCardLogicValue(0x4F) + 1;
			bool bFindCard = false;
			BYTE CanOutIndex = 0;
			for (BYTE i = 0; i < 4; ++i)
			{
				BYTE Index = NumIndex[i];

				//����ը��
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[Index][0])) == 4)
				{
					continue;
				}

				if (MinSingleCardCount[i] < OriginSingleCardCount + 3 &&
					MinSingleCardCount[i] <= MinSingleCountInFour &&
					MinLogicCardValue > GetCardLogicValue(CanOutCard.CardData[Index][0]))
				{
					//��Դ���
					bool bNoLargeCard = true;

					//������������������4�����ҵ���������С��K���ƶ��Ҳ��ǵ������ϵ������ʱ�����ܳ�2ȥ��
					if (m_UserCardCount[m_BankerUser] >= 10 &&
						m_UserCardCount[MeChairID] >= 10 &&
						CanOutCard.EachHandCardCount[Index] >= 2 &&
						GetCardLogicValue(CanOutCard.CardData[Index][0]) >= 15 &&
						GetCardLogicValue(TurnCardData[0]) < 13 &&
						GetCardLogicValue(TurnCardData[0]) < GetCardLogicValue(m_UserCardData[m_BankerUser][0]) &&
						CanOutCard.EachHandCardCount[Index] != m_UserCardCount[MeChairID])
					{
						bNoLargeCard = false;
					}

					//������û�д��ƣ���Էɻ���������Ĵ��ƣ�
					for (BYTE k = 3; k < CanOutCard.EachHandCardCount[Index]; ++k)
					{
						if (GetCardLogicValue(CanOutCard.CardData[Index][k]) >= 15 &&
							CanOutCard.EachHandCardCount[Index] != m_UserCardCount[MeChairID])
						{
							bNoLargeCard = false;
						}	
					}
					if (bNoLargeCard)
					{
						bFindCard = true;
						CanOutIndex = Index;
						MinLogicCardValue = GetCardLogicValue(CanOutCard.CardData[Index][0]);
					}
				}
			}

			if (bFindCard)
			{
				//�����������
				BYTE LargestLogicCard = GetCardLogicValue(m_UserCardData[m_BankerUser][0]);
				bool bCanOut = true;

				//��ֻѹ2
				if (GetCardLogicValue(TurnCardData[0]) < LargestLogicCard)
				{
					if (GetCardColor(CanOutCard.CardData[CanOutIndex][0]) == 0x40 && GetCardLogicValue(TurnCardData[0]) <= 14 && m_UserCardCount[MeChairID] > 5)
					{
						bCanOut = false;
					}
				}

				if (bCanOut)
				{
					//���ñ���
					OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
					CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

					return;
				}
			}

			if (OutCardType == CT_SINGLE)
			{
				//�����������
				BYTE LargestLogicCard = GetCardLogicValue(m_UserCardData[m_BankerUser][0]);

				if (GetCardLogicValue(TurnCardData[0]) == 14 ||
					GetCardLogicValue(TurnCardData[0]) >= LargestLogicCard ||
					(GetCardLogicValue(TurnCardData[0])<LargestLogicCard - 1) || 
					m_UserCardCount[m_BankerUser] <= 5)
				{
					//ȡһ�Ŵ��ڵ���2����Ҫ�ȵ��������ƴ���ƣ�
					BYTE Index = MAX_COUNT;
					for (int i = m_UserCardCount[MeChairID] - 1; i >= 0; i--)
					{
						if (m_UserCardData[MeChairID][i] == 0x4F || m_UserCardData[MeChairID][i] == 0x4E)
						{
							if (m_UserCardCount[MeChairID] > 5)
							{
								continue;
							}
						}
						if (GetCardLogicValue(m_UserCardData[MeChairID][i]) >= GetCardLogicValue(TurnCardData[0]) &&
							GetCardLogicValue(m_UserCardData[MeChairID][i]) >= 15)
						{
							Index = i;
						}
					}
					if (Index != MAX_COUNT)
					{
						//���ñ���
						OutCardResult.CardCount = 1;
						OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][Index];

						return;
					}
				}
				//������ʱ�����ܹ�������������
				if (GetCardLogicValue(CanOutCard.CardData[CanOutCard.CardTypeCount - 1][0]) >= 16 && m_UserCardCount[MeChairID] > 5)
				{
					return;
				}
			}

			//�����Ѳ������ص���ʱ
			WORD wMeChairID = (m_BankerUser + 1) % GAME_PLAYER;
			WORD wFriendID = (wMeChairID + 1) % GAME_PLAYER;

			tagOutCardTypeResult FriendCardTypeResult[13];
			ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult));
			AnalyseOutCardType(m_UserCardData[wFriendID], m_UserCardCount[wFriendID], TurnCardData, TurnCardCount, FriendCardTypeResult);

			//�����Ѳ������ص���ʱ
			if (m_UserCardCount[m_BankerUser] <= 4 && FriendCardTypeResult[OutCardType].CardTypeCount == 0 && CardTypeResult[OutCardType].CardTypeCount>0)
			{
				BYTE MinSingleCount = MAX_COUNT;
				BYTE Index = 0;
				for (BYTE i = 0; i<CardTypeResult[OutCardType].CardTypeCount; ++i)
				{
					BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CardTypeResult[OutCardType].CardData[i], CardTypeResult[OutCardType].EachHandCardCount[i]);
					if (MinSingleCount >= TmpCount)
					{
						MinSingleCount = TmpCount;
						Index = i;
					}
				}
				//���ñ���
				OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[Index], OutCardResult.CardCount);

				return;
			}
		}

		//��Ҫ����ը��
		if (CardTypeResult[CT_BOMB_CARD].CardTypeCount>0)
		{
			tagOutCardTypeResult const &BombCard = CardTypeResult[CT_BOMB_CARD];
			BYTE MinLogicValue = GetCardLogicValue(BombCard.CardData[0][0]);
			BYTE Index = 0;
			for (BYTE i = 0; i<BombCard.CardTypeCount; ++i)
			{
				if (MinLogicValue>GetCardLogicValue(BombCard.CardData[i][0]))
				{
					MinLogicValue = GetCardLogicValue(BombCard.CardData[i][0]);
					Index = i;
				}
			}

			if (m_UserCardCount[m_BankerUser] < 5 ||
				GetCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser]) != CT_ERROR)
			{
				OutCardResult.CardCount = BombCard.EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, BombCard.CardData[Index], BombCard.EachHandCardCount[Index] * sizeof(BYTE));
				return;
			}

			//�жϳ���ը����ĵ�����
			BYTE SingleCardCount = AnalyseSingleCardCountS(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombCard.CardData[Index], BombCard.EachHandCardCount[Index]);
			if (m_UserCardCount[MeChairID] > 7 || SingleCardCount >= 3 || (OutCardType == CT_SINGLE && GetCardLogicValue(TurnCardData[0]) < 15))
			{
				return;
			}

			//���ñ���
			OutCardResult.CardCount = BombCard.EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, BombCard.CardData[Index], BombCard.EachHandCardCount[Index] * sizeof(BYTE));

			return;
		}
		return;
	}
	return;
}

/*
�������ܣ��ж��Ƿ����һ�ֳ����ѹ�Ƴ���
HandCardData:����������е���
HandCardCount:����������е�������
OutCardUser:������ҵ�ID���������Լ���Ҳ�����Ǳ���)
TurnCardData:����������
TurnCardCount:����������
tagOutCardResult:���������Ҫ������
*/
bool robotAI::IsCanOneHandFollow(const BYTE HandCardData[], BYTE HandCardCount, BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult)
{
	if (HandCardCount == 0)
	{
		return true;
	}

	//��ʼ����
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//���±�û��
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	//��������
	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);

	//������������,��������ũ��,�Լ��ǵ���
	if (OutCardUser != m_BankerUser && MeChairID == m_BankerUser)
	{
		BYTE UndersideUser = (m_BankerUser + 1) % GAME_PLAYER;
		BYTE UpsideUser = (UndersideUser + 1) % GAME_PLAYER;

		//�����ɳ���
		tagOutCardTypeResult UndersideOutCardTypeResult[CT_NUM];
		tagOutCardTypeResult UpsideOutCardTypeResult[CT_NUM];
		ZeroMemory(UndersideOutCardTypeResult, sizeof(UndersideOutCardTypeResult));
		ZeroMemory(UpsideOutCardTypeResult, sizeof(UpsideOutCardTypeResult));

		AnalyseOutCardType(m_UserCardData[UndersideUser], m_UserCardCount[UndersideUser], UndersideOutCardTypeResult);
		AnalyseOutCardType(m_UserCardData[UpsideUser], m_UserCardCount[UpsideUser], UpsideOutCardTypeResult);
		AnalyseOutCardType(HandCardData, HandCardCount, TurnCardData, TurnCardCount, CardTypeResult);

		//ֻʣը��
		if (HandCardCount == CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0])
		{
			OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData, OutCardResult.CardCount);
			return true;
		}
		//ֻʣ��ը
		if (HandCardCount == 2 && HandCardData[0] == 0x4F && HandCardData[1] == 0x4E)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return true;
		}
		//˫��ը����һ��
		else if (HandCardCount > 2 && HandCardData[0] == 0x4F && HandCardData[1] == 0x4E && GetCardType(HandCardData + 2, HandCardCount - 2) != CT_ERROR)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return true;
		}
		//������������ƫ��û��ը�����Ҹ����ֻ��һ���ƣ����
		else if (CardTypeResult[OutCardType].CardTypeCount != 0)
		{
			if (GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) >= GetCardLogicValue(UndersideOutCardTypeResult[OutCardType].CardData[0][0]) &&
				GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) >= GetCardLogicValue(UpsideOutCardTypeResult[OutCardType].CardData[0][0]) &&
				UndersideOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0 &&
				UpsideOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0
				)
			{
				BYTE tmpList[MAX_COUNT] = { 0 };
				BYTE tmpListLen = HandCardCount;
				memcpy(tmpList, HandCardData, HandCardCount * sizeof(BYTE));
				RemoveCards(CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0], tmpList, tmpListLen);
				if (GetCardType(tmpList, tmpListLen - CardTypeResult[OutCardType].EachHandCardCount[0]) != CT_ERROR)
				{
					OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[0];
					memcpy(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0] * sizeof(BYTE));
					return true;
				}
			}
		}
	}
	//������������,�������ǵ������Լ���ũ��
	else
	{
		//�����ɳ���
		tagOutCardTypeResult BankerOutCardTypeResult[CT_NUM];
		ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult));

		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerOutCardTypeResult);
		AnalyseOutCardType(HandCardData, HandCardCount, TurnCardData, TurnCardCount, CardTypeResult);

		BYTE FriendID = (m_BankerUser + 1) % GAME_PLAYER;

		//ֻʣը��
		if (HandCardCount == CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0])
		{
			OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData, OutCardResult.CardCount);

			return true;
		}
		//ֻʣ��ը
		if (HandCardCount == 2 && HandCardData[0] == 0x4F && HandCardData[1] == 0x4E)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;

			return true;
		}
		//˫��ը����һ��
		else if (HandCardCount > 2 && HandCardData[0] == 0x4F && HandCardData[1] == 0x4E && CT_ERROR != GetCardType(HandCardData + 2, HandCardCount - 2))
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return true;
		}
		//������������ƫ��û��ը�����Ҹ����ֻ��һ���ƣ����
		else if (CardTypeResult[OutCardType].CardTypeCount != 0)
		{
			if ((BankerOutCardTypeResult[OutCardType].CardTypeCount == 0 || GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) >= GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0])) &&
				BankerOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
			{
				BYTE tmpList[MAX_COUNT] = { 0 };
				BYTE tmpListLen = HandCardCount;
				memcpy(tmpList, HandCardData, HandCardCount * sizeof(BYTE));
				RemoveCards(CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0], tmpList, tmpListLen);
				if (GetCardType(tmpList, tmpListLen - CardTypeResult[OutCardType].EachHandCardCount[0]) != CT_ERROR)
				{
					OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[0];
					memcpy(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0] * sizeof(BYTE));
					return true;
				}
			}
		}
	}

	return false;
}

//�Լ����ƺ��жϣ����Ǹ��Ƶ�
bool robotAI::IsCanOneHand(const BYTE HandCardData[], BYTE HandCardCount, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser)
{
	//���±�û��
	BYTE TmpHandData[MAX_COUNT] = { 0 };
	BYTE TmpHandLen = HandCardCount;
	memcpy(TmpHandData, HandCardData, HandCardCount);
	RemoveCards(TurnCardData, TurnCardCount, TmpHandData, TmpHandLen);
	TmpHandLen -= TurnCardCount;

	//��������
	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);

	//�Լ���ũ��
	if (OutCardUser != m_BankerUser)
	{
		//�����ɳ���
		tagOutCardTypeResult *CardTypeResult = new tagOutCardTypeResult[CT_NUM];
		tagOutCardTypeResult *BankerOutCardTypeResult = new tagOutCardTypeResult[CT_NUM];

		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], TurnCardData, TurnCardCount, BankerOutCardTypeResult);
		AnalyseOutCardType(TmpHandData, TmpHandLen, TurnCardData, TurnCardCount, CardTypeResult);

		BYTE FriendID = (m_BankerUser + 1) % GAME_PLAYER;

		//ֻʣը��
		if (TmpHandLen == CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0] && GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[CT_BOMB_CARD].CardData[0][0]))
		{
			delete[] CardTypeResult;
			delete[] BankerOutCardTypeResult;
			return true;
		}
		//ֻʣ��ը
		if (TmpHandLen == 2 && TmpHandData[0] == 0x4F && TmpHandData[1] == 0x4E)
		{
			delete[] CardTypeResult;
			delete[] BankerOutCardTypeResult;
			return true;
		}
		//˫��ը����һ��
		if (TmpHandLen > 2 && TmpHandData[0] == 0x4F && TmpHandData[1] == 0x4E && CT_ERROR != GetCardType(TmpHandData + 2, TmpHandLen - 2))
		{
			delete[] CardTypeResult;
			delete[] BankerOutCardTypeResult;
			return true;
		}

		//������Լ����������Լ��ܻأ�ƫ��û��ը�����һ����ֻ��һ���ƣ�����һ���־��Դ���ƴ�һ��С�ƣ�
		if (GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) >= GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]) &&
			BankerOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			BYTE TmpList[MAX_COUNT] = { 0 };
			BYTE TmpListLen = TmpHandLen;
			memcpy(TmpList, TmpHandData, TmpHandLen * sizeof(BYTE));
			RemoveCards(CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0], TmpList, TmpListLen);
			TmpListLen -= CardTypeResult[OutCardType].EachHandCardCount[0];
			if (GetCardType(TmpList, TmpListLen) != CT_ERROR)
			{
				delete[] CardTypeResult;
				delete[] BankerOutCardTypeResult;
				return true;
			}
			else
			{
				tagOutCardTypeResult *TToutType = new tagOutCardTypeResult[CT_NUM];
				ZeroMemory(TToutType, sizeof(TToutType));
				AnalyseOutCardType(TmpList, TmpListLen, TToutType);
				for (BYTE type = CT_SINGLE; type <= CT_BOMB_CARD; type++)
				{
					tagOutCardTypeResult TToutCard = TToutType[type];
					for (int i = 0; i < TToutCard.CardTypeCount; i++)
					{
						tagOutCardTypeResult *BankerTToutType = new tagOutCardTypeResult[CT_NUM];
						AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], TToutCard.CardData[i], TToutCard.EachHandCardCount[i], BankerTToutType);
						if (type == CT_BOMB_CARD || GetCardLogicValue(TToutCard.CardData[i][0]) > GetCardLogicValue(BankerTToutType[type].CardData[0][0]))
						{
							BYTE TTmpList[MAX_COUNT] = { 0 };
							memcpy(TTmpList, TmpList, TmpListLen);
							RemoveCards(TToutCard.CardData[i], TToutCard.EachHandCardCount[i], TTmpList, TmpListLen);
							if (GetCardType(TTmpList, TmpListLen - TToutCard.EachHandCardCount[i]) != CT_ERROR)
							{
								delete[] CardTypeResult;
								delete[] BankerOutCardTypeResult;
								delete[] TToutType;
								delete[] BankerTToutType;
								return true;
							}
						}
						delete[] BankerTToutType;
					}
				}
				delete[] CardTypeResult;
				delete[] BankerOutCardTypeResult;
				delete[] TToutType;
				return false;
			}
		}
	}
	//�Լ��ǵ���
	else
	{
		BYTE UndersideUser = (m_BankerUser + 1) % GAME_PLAYER;
		BYTE UpsideUser = (UndersideUser + 1) % GAME_PLAYER;

		//�����ɳ���
		tagOutCardTypeResult *CardTypeResult = new tagOutCardTypeResult[CT_NUM];
		tagOutCardTypeResult *UndersideOutCardTypeResult = new tagOutCardTypeResult[CT_NUM];
		tagOutCardTypeResult *UpsideOutCardTypeResult = new tagOutCardTypeResult[CT_NUM];
		ZeroMemory(UndersideOutCardTypeResult, sizeof(UndersideOutCardTypeResult));
		ZeroMemory(UpsideOutCardTypeResult, sizeof(UpsideOutCardTypeResult));
		ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

		AnalyseOutCardType(m_UserCardData[UndersideUser], m_UserCardCount[UndersideUser], TurnCardData, TurnCardCount, UndersideOutCardTypeResult);
		AnalyseOutCardType(m_UserCardData[UpsideUser], m_UserCardCount[UpsideUser], TurnCardData, TurnCardCount, UpsideOutCardTypeResult);
		AnalyseOutCardType(TmpHandData, TmpHandLen, TurnCardData, TurnCardCount, CardTypeResult);

		//ֻʣը��
		if (TmpHandLen == CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0])
		{
			delete[] CardTypeResult;
			delete[] UndersideOutCardTypeResult;
			delete[] UpsideOutCardTypeResult;
			return true;
		}
		//ֻʣ��ը
		if (TmpHandLen == 2 && TmpHandData[0] == 0x4F && TmpHandData[1] == 0x4E)
		{
			delete[] CardTypeResult;
			delete[] UndersideOutCardTypeResult;
			delete[] UpsideOutCardTypeResult;
			return true;
		}
		//˫��ը����һ��
		if (TmpHandLen > 2 && TmpHandData[0] == 0x4F && TmpHandData[1] == 0x4E && GetCardType(TmpHandData + 2, TmpHandLen - 2) != CT_ERROR)
		{
			delete[] CardTypeResult;
			delete[] UndersideOutCardTypeResult;
			delete[] UpsideOutCardTypeResult;
			return true;
		}
		//������Լ����������Լ��ܻأ�ƫ��û��ը�����һ����ֻ��һ���ƣ�����һ���־��Դ���ƴ�һ��С�ƣ�
		if (GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) >= GetCardLogicValue(UndersideOutCardTypeResult[OutCardType].CardData[0][0]) &&
			GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) >= GetCardLogicValue(UpsideOutCardTypeResult[OutCardType].CardData[0][0]) &&
			UndersideOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0 &&
			UpsideOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			BYTE TmpList[MAX_COUNT] = { 0 };
			BYTE TmpListLen = TmpHandLen;
			memcpy(TmpList, TmpHandData, TmpHandLen * sizeof(BYTE));
			RemoveCards(CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0], TmpList, TmpListLen);
			TmpListLen -= CardTypeResult[OutCardType].EachHandCardCount[0];
			if (GetCardType(TmpList, TmpListLen) != CT_ERROR)
			{
				delete[] CardTypeResult;
				delete[] UndersideOutCardTypeResult;
				delete[] UpsideOutCardTypeResult;
				return true;
			}
			else
			{
				tagOutCardTypeResult *TToutType = new tagOutCardTypeResult[CT_NUM];
				ZeroMemory(TToutType, sizeof(TToutType));
				AnalyseOutCardType(TmpList, TmpListLen, TToutType);
				for (BYTE type = CT_SINGLE; type <= CT_BOMB_CARD; type++)
				{
					tagOutCardTypeResult TToutCard = TToutType[type];
					for (int i = 0; i < TToutCard.CardTypeCount; i++)
					{
						tagOutCardTypeResult *UpsideTToutType = new tagOutCardTypeResult[CT_NUM];
						tagOutCardTypeResult *UndersideTToutType = new tagOutCardTypeResult[CT_NUM];
						AnalyseOutCardType(m_UserCardData[UpsideUser], m_UserCardCount[UpsideUser], TToutCard.CardData[i], TToutCard.EachHandCardCount[i], UpsideTToutType);
						AnalyseOutCardType(m_UserCardData[UndersideUser], m_UserCardCount[UndersideUser], TToutCard.CardData[i], TToutCard.EachHandCardCount[i], UndersideTToutType);
						if (type == CT_BOMB_CARD || 
							(GetCardLogicValue(TToutCard.CardData[i][0]) > GetCardLogicValue(UpsideTToutType[type].CardData[0][0]) &&
							GetCardLogicValue(TToutCard.CardData[i][0]) > GetCardLogicValue(UndersideTToutType[type].CardData[0][0]))
							)
						{
							BYTE TTmpList[MAX_COUNT] = { 0 };
							memcpy(TTmpList, TmpList, TmpListLen);
							RemoveCards(TToutCard.CardData[i], TToutCard.EachHandCardCount[i], TTmpList, TmpListLen);
							if (GetCardType(TTmpList, TmpListLen - TToutCard.EachHandCardCount[i]) != CT_ERROR)
							{
								delete[] CardTypeResult;
								delete[] UndersideOutCardTypeResult;
								delete[] UpsideOutCardTypeResult;
								delete[] TToutType;
								delete[] UpsideTToutType;
								delete[] UndersideTToutType;
								return true;
							}
						}
						delete[] UpsideTToutType;
						delete[] UndersideTToutType;
					}
				}
				delete[] CardTypeResult;
				delete[] UndersideOutCardTypeResult;
				delete[] UpsideOutCardTypeResult;
				delete[] TToutType;
				return false;
			}
		}
	}
	return false;
}

//���ƺ��Ƿ���Ի���
bool robotAI::IsCanPressCard(BYTE ChairID, const BYTE TurnCardData[], BYTE TurnCardCount)
{
	if (TurnCardData == NULL || TurnCardCount == 0)
	{
		return false;
	}

	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);
	if (OutCardType == CT_ERROR)
	{
		return false;
	}

	//��������ȥ��Ҫ��������
	BYTE HandCard[MAX_COUNT] = { 0 };
	BYTE HandCardLen = m_UserCardCount[ChairID];
	memcpy(HandCard, m_UserCardData[ChairID], m_UserCardCount[ChairID]);
	RemoveCards(TurnCardData, TurnCardCount, HandCard, HandCardLen);
	HandCardLen -= TurnCardCount;

	//����������ǵ���
	if (ChairID == m_BankerUser)
	{
		BYTE Farmer1 = (m_BankerUser + 1) % GAME_PLAYER;
		BYTE Farmer2 = (m_BankerUser + 2) % GAME_PLAYER;

		tagOutCardTypeResult FarmerCardType1[CT_NUM];
		tagOutCardTypeResult FarmerCardType2[CT_NUM];
		tagOutCardTypeResult BankerCardType[CT_NUM];
		ZeroMemory(FarmerCardType1, sizeof(FarmerCardType1));
		ZeroMemory(FarmerCardType2, sizeof(FarmerCardType2));
		ZeroMemory(BankerCardType, sizeof(BankerCardType));

		AnalyseOutCardType(m_UserCardData[Farmer1], m_UserCardCount[Farmer1], TurnCardData, TurnCardCount, FarmerCardType1);
		AnalyseOutCardType(m_UserCardData[Farmer2], m_UserCardCount[Farmer2], TurnCardData, TurnCardCount, FarmerCardType2);
		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], TurnCardData, TurnCardCount, BankerCardType);

		//ũ���ܸ���
		if (FarmerCardType1[OutCardType].CardTypeCount > 0 || FarmerCardType2[OutCardType].CardTypeCount > 0 ||
			FarmerCardType1[CT_BOMB_CARD].CardTypeCount > 0 || FarmerCardType2[CT_BOMB_CARD].CardTypeCount > 0)
		{
			//����û����Ӧ����
			if (BankerCardType[OutCardType].CardTypeCount == 0)
			{
				//������ը����ѹ��
				if (BankerCardType[CT_BOMB_CARD].CardTypeCount > 0 && 
					(GetCardLogicValue(BankerCardType[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(FarmerCardType1[CT_BOMB_CARD].CardData[0][0]) &&
					GetCardLogicValue(BankerCardType[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(FarmerCardType2[CT_BOMB_CARD].CardData[0][0])
					))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			//��������Ӧ������
			else
			{
				//����û��ը��
				if (BankerCardType[CT_BOMB_CARD].CardTypeCount == 0)
				{
					//ũ��û��ը���ҵ������Ʊ����Ƕ���ʱ����ѹ��
					if (FarmerCardType1[CT_BOMB_CARD].CardTypeCount == 0 && FarmerCardType2[CT_BOMB_CARD].CardTypeCount &&
						GetCardLogicValue(BankerCardType[OutCardType].CardData[0][0]) > GetCardLogicValue(FarmerCardType1[OutCardType].CardData[0][0]) &&
						GetCardLogicValue(BankerCardType[OutCardType].CardData[0][0]) > GetCardLogicValue(FarmerCardType2[OutCardType].CardData[0][0]))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					//������ը�������Ƕ���,��ѹ��
					if (GetCardLogicValue(BankerCardType[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(FarmerCardType1[CT_BOMB_CARD].CardData[0][0]) &&
						GetCardLogicValue(BankerCardType[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(FarmerCardType2[CT_BOMB_CARD].CardData[0][0]))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
		else
		{
			//ũ��Ҫ����
			return true;
		}
	}
	//�����������ũ��
	else
	{
		tagOutCardTypeResult MeFarmerCardType[CT_NUM];
		tagOutCardTypeResult BankerCardType[CT_NUM];
		ZeroMemory(MeFarmerCardType, sizeof(MeFarmerCardType));
		ZeroMemory(BankerCardType, sizeof(BankerCardType));

		AnalyseOutCardType(m_UserCardData[ChairID], m_UserCardCount[ChairID], TurnCardData, TurnCardCount, MeFarmerCardType);
		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], TurnCardData, TurnCardCount, BankerCardType);

		//����������Ը���
		if (BankerCardType[OutCardType].CardTypeCount > 0 || BankerCardType[CT_BOMB_CARD].CardTypeCount > 0)
		{
			//���ũ��û����Ӧ������
			if (MeFarmerCardType[OutCardType].CardTypeCount == 0)
			{
				//���ũ���ը����ѹ��
				if (GetCardLogicValue(MeFarmerCardType[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(BankerCardType[CT_BOMB_CARD].CardData[0][0]))
				{
					return true;
				}
				//ũ��û����Ӧ������Ҳû��ը��
				else
				{
					return false;
				}
			}
			//ũ������Ӧ������
			else
			{
				//ũ��û��ը��
				if (MeFarmerCardType[CT_BOMB_CARD].CardTypeCount == 0)
				{
					//����û��ը�����Լ����Ʊ�������ʱ����ѹ��
					if (BankerCardType[CT_BOMB_CARD].CardTypeCount == 0 &&
						GetCardLogicValue(MeFarmerCardType[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerCardType[OutCardType].CardData[0][0]))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					//ũ���ը���ȵ�����,��ѹ��
					if (GetCardLogicValue(MeFarmerCardType[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(BankerCardType[CT_BOMB_CARD].CardData[0][0]))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
		//�����������ƣ���ѹ��
		else
		{
			return true;
		}
	}
	return false;
}

BYTE robotAI::FindNumSV(const BYTE HandCardData[], BYTE HandCardCount, BYTE CardVaue)
{
	if (CardVaue < 3 || CardVaue > 17)
	{
		return 0;
	}
	if (HandCardCount <= 0)
	{
		return 0;
	}
	int num = 0;
	for (int i = 0; i < HandCardCount; i++)
	{
		if (GetCardLogicValue(HandCardData[i]) == CardVaue)
		{
			num++;
		}
	}
	return num;
}

BYTE robotAI::LandScore(BYTE MeChairID)
{
	int Point = 0;

	//�����˿�
	tagOutCardTypeResult MeOutCardTypeResult[13];
	ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeOutCardTypeResult);

	if (m_UserCardData[MeChairID][0] == 0x4F && m_UserCardData[MeChairID][1] == 0x4E)
	{
		Point += 8;
	}
	else
	{
		for (BYTE i = 0; i < MeOutCardTypeResult[CT_SINGLE].CardTypeCount; i++)
		{
			if (GetCardLogicValue(MeOutCardTypeResult[CT_SINGLE].CardData[i][0]) < 15)
			{
				break;
			}
			if (MeOutCardTypeResult[CT_SINGLE].CardData[i][0] == 0x4F)
			{
				Point += 4;
			}
			if (MeOutCardTypeResult[CT_SINGLE].CardData[i][0] == 0x4E)
			{
				Point += 3;
			}
			if (GetCardValue(MeOutCardTypeResult[CT_SINGLE].CardData[i][0]) == 0x02)
			{
				Point += 2;
			}
		}
	}

	Point += (MeOutCardTypeResult[CT_BOMB_CARD].CardTypeCount * 6);

	return Point;
}

BYTE robotAI::GetNumOfType(const BYTE HandCardData[], const BYTE HandCardCount, BYTE CardType)
{
	if (CardType <= CT_ERROR || CardType >= CT_NUM)
	{
		return 0;
	}

	tagOutCardTypeResult tagCardType[CT_NUM];
	ZeroMemory(tagCardType, sizeof(tagCardType));
	AnalyseOutCardType(HandCardData, HandCardCount, tagCardType);

	switch (CardType)
	{
	case CT_SINGLE:
	case CT_DOUBLE:
	case CT_SINGLE_LINE:
	case CT_DOUBLE_LINE:
	case CT_THREE_LINE:
	case CT_BOMB_CARD:
	{
		return tagCardType[CardType].CardTypeCount;
	}
	case CT_THREE_TAKE_ONE:
	{
		if (HandCardCount < 4)
		{
			return 0;
		}
		int ThreeNum = tagCardType[CT_THREE].CardTypeCount;
		int Num = 0;
		for (Num = 0; Num < ThreeNum; Num++)
		{
			if (HandCardCount >= (Num + 1) * 4) continue;
			else break;
		}
		if (Num > 0) return Num;

		break;
	}
	case CT_THREE_TAKE_TWO:
	{
		if (HandCardCount < 5)
		{
			return 0;
		}
		int ThreeNum = tagCardType[CT_THREE].CardTypeCount;
		int Num = 0;
		for (Num = 0; Num < ThreeNum; Num++)
		{
			if (HandCardCount >= (Num + 1) * 5) continue;
			else break;
		}
		if (Num > 0) return Num;

		break;
	}
	case CT_FOUR_TAKE_ONE:
	{
		if (HandCardCount < 4)
		{
			return 0;
		}
		int FourNum = tagCardType[CT_BOMB_CARD].CardTypeCount;
		if (GetCardColor(tagCardType[CT_BOMB_CARD].CardData[0][0]) == 0x40)
		{
			FourNum -= 1;
		}
		int Num = 0;
		for (Num = 0; Num < FourNum; Num++)
		{
			if (HandCardCount >= (Num + 1) * 4) continue;
			else break;
		}
		if (Num > 0) return Num;

		break;
	}
	case CT_FOUR_TAKE_TWO:
	{
		if (HandCardCount < 5)
		{
			return 0;
		}
		int FourNum = tagCardType[CT_BOMB_CARD].CardTypeCount;
		if (GetCardColor(tagCardType[CT_BOMB_CARD].CardData[0][0]) == 0x40)
		{
			FourNum -= 1;
		}
		int Num = 0;
		for (Num = 0; Num < FourNum; Num++)
		{
			if (HandCardCount >= (Num + 1) * 5) continue;
			else break;
		}
		if (Num > 0) return Num;

		break;
	}
	default:
		break;
	}
	return 0;
}