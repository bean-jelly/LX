#include <assert.h>
#include <memory>
#include "Windows.h"
#include "robot_ai.h"

const BYTE G_AllCardData[FULL_COUNT] = 
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,	//方块 A - K
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,	//梅花 A - K
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	//红桃 A - K
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,	//黑桃 A - K
	0x4E, 0x4F,
};

template <typename T>
std::shared_ptr<T> make_shared_array(size_t size)
{
	//default_delete是STL中的默认删除器
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

	//三牌判断
	if (AnalyseResult.BlockCount[2] > 0)
	{
		//连牌判断
		if (AnalyseResult.BlockCount[2] > 1)
		{
			//变量定义
			BYTE cbCardData = AnalyseResult.CardData[2][0];
			BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

			//错误过虑
			if (cbFirstLogicValue >= 15) return CT_ERROR;

			//连牌判断
			for (BYTE i = 1; i < AnalyseResult.BlockCount[2]; i++)
			{
				BYTE cbCardData = AnalyseResult.CardData[2][i * 3];
				if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
			}
		}
		else if (CardCount == 3) return CT_THREE;

		//牌形判断
		if (AnalyseResult.BlockCount[2] * 3 == CardCount) return CT_THREE_LINE;
		if (AnalyseResult.BlockCount[2] * 4 == CardCount) return CT_THREE_TAKE_ONE;
		if ((AnalyseResult.BlockCount[2] * 5 == CardCount) && (AnalyseResult.BlockCount[1] == AnalyseResult.BlockCount[2])) return CT_THREE_TAKE_TWO;

		return CT_ERROR;
	}

	//两张类型
	if (AnalyseResult.BlockCount[1] >= 3)
	{
		//变量定义
		BYTE cbCardData = AnalyseResult.CardData[1][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue >= 15) return CT_ERROR;

		//连牌判断
		for (BYTE i = 1; i < AnalyseResult.BlockCount[1]; i++)
		{
			BYTE cbCardData = AnalyseResult.CardData[1][i * 2];
			if (cbFirstLogicValue != (GetCardLogicValue(cbCardData) + i)) return CT_ERROR;
		}

		//二连判断
		if ((AnalyseResult.BlockCount[1] * 2) == CardCount) return CT_DOUBLE_LINE;

		return CT_ERROR;
	}

	//单张判断
	if ((AnalyseResult.BlockCount[0] >= 5) && (AnalyseResult.BlockCount[0] == CardCount))
	{
		//变量定义
		BYTE cbCardData = AnalyseResult.CardData[0][0];
		BYTE cbFirstLogicValue = GetCardLogicValue(cbCardData);

		//错误过虑
		if (cbFirstLogicValue >= 15) return CT_ERROR;

		//连牌判断
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

	//数目排序
	if (SortType == ST_COUNT)
	{
		//变量定义
		BYTE CardIndex = 0;

		//分析扑克
		tagAnalyseResult AnalyseResult;
		AnalysebCardData(&CardData[CardIndex], CardCount - CardIndex, AnalyseResult);

		//提取扑克
		for (BYTE i = 0; i < CountArray(AnalyseResult.BlockCount); i++)
		{
			//拷贝扑克
			BYTE cbIndex = CountArray(AnalyseResult.BlockCount) - i - 1;
			CopyMemory(&CardData[CardIndex], AnalyseResult.CardData[cbIndex], AnalyseResult.BlockCount[cbIndex] * (cbIndex + 1)*sizeof(BYTE));

			//设置索引
			CardIndex += AnalyseResult.BlockCount[cbIndex] * (cbIndex + 1)*sizeof(BYTE);
		}
	}

	return;
}

bool robotAI::RemoveCards(const BYTE RemoveCard[], BYTE RemoveCount, BYTE CardData[], BYTE CardCount)
{
	//检验数据
	if (RemoveCount > CardCount)
	{
		return false;
	}

	//定义变量
	BYTE DeleteCount = 0, TempCardData[MAX_COUNT];
	if (CardCount > CountArray(TempCardData)) return false;
	BYTE TempRemoveCard[MAX_COUNT];
	CopyMemory(TempCardData, CardData, CardCount*sizeof(CardData[0]));
	CopyMemory(TempRemoveCard, RemoveCard, RemoveCount);

	//置零扑克
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

	//清理扑克
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
	//扑克属性
	BYTE CardColor = GetCardColor(CardData);
	BYTE CardValue = GetCardValue(CardData);

	//转换数值
	if (CardColor == 0x40) return CardValue + 2;
	return (CardValue <= 2) ? (CardValue + 13) : CardValue;
}

//对比扑克
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

	//炸弹判断
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

//出牌搜索
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
					//设置结果
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
		//长度判断
		if (CardCount < TurnCardCount) break;

		//获取数值
		BYTE LogicValue = GetCardLogicValue(TurnCardData[0]);

		//搜索连牌
		for (BYTE i = (TurnCardCount - 1); i < CardCount; i++)
		{
			//获取数值
			BYTE HandLogicValue = GetCardLogicValue(CardData[CardCount - i - 1]);

			//构造判断
			if (HandLogicValue <= LogicValue) continue;
			if ((TurnCardCount > 1) && (HandLogicValue >= 15)) break;

			//搜索连牌
			BYTE LineCount = 0;
			for (BYTE j = (CardCount - i - 1); j < (CardCount - 1); j++)
			{
				if (((GetCardLogicValue(CardData[j]) + LineCount) == HandLogicValue)
					&& ((GetCardLogicValue(CardData[j + 1]) + LineCount) == HandLogicValue))
				{
					//增加连数
					OutCardResult.ResultCard[LineCount * 2] = CardData[j];
					OutCardResult.ResultCard[(LineCount++) * 2 + 1] = CardData[j + 1];

					//完成判断
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
		//长度判断
		if (CardCount < TurnCardCount) break;

		//获取数值
		BYTE LogicValue = 0;
		for (BYTE i = 0; i < TurnCardCount - 2; i++)
		{
			LogicValue = GetCardLogicValue(TurnCardData[i]);
			if (GetCardLogicValue(TurnCardData[i + 1]) != LogicValue) continue;
			if (GetCardLogicValue(TurnCardData[i + 2]) != LogicValue) continue;
			break;
		}

		//属性数值
		BYTE TurnLineCount = 0;
		if (TurnOutType == CT_THREE_TAKE_ONE) TurnLineCount = TurnCardCount / 4;
		else if (TurnOutType == CT_THREE_TAKE_TWO) TurnLineCount = TurnCardCount / 5;
		else TurnLineCount = TurnCardCount / 3;

		//搜索连牌
		for (BYTE i = TurnLineCount * 3 - 1; i < CardCount; i++)
		{
			//获取数值
			BYTE HandLogicValue = GetCardLogicValue(CardData[CardCount - i - 1]);

			//构造判断
			if (HandLogicValue <= LogicValue) continue;
			if ((TurnLineCount > 1) && (HandLogicValue >= 15)) break;

			//搜索连牌
			BYTE LineCount = 0;
			for (BYTE j = (CardCount - i - 1); j < (CardCount - 2); j++)
			{
				//设置变量
				OutCardResult.CardCount = 0;

				//三牌判断
				if ((GetCardLogicValue(CardData[j]) + LineCount) != HandLogicValue) continue;
				if ((GetCardLogicValue(CardData[j + 1]) + LineCount) != HandLogicValue) continue;
				if ((GetCardLogicValue(CardData[j + 2]) + LineCount) != HandLogicValue) continue;

				//增加连数
				OutCardResult.ResultCard[LineCount * 3] = CardData[j];
				OutCardResult.ResultCard[LineCount * 3 + 1] = CardData[j + 1];
				OutCardResult.ResultCard[(LineCount++) * 3 + 2] = CardData[j + 2];

				//完成判断
				if (LineCount == TurnLineCount)
				{
					//连牌设置
					OutCardResult.CardCount = LineCount * 3;

					//构造扑克
					BYTE LeftCardData[MAX_COUNT];
					BYTE LeftCount = CardCount - OutCardResult.CardCount;
					CopyMemory(LeftCardData, CardData, sizeof(BYTE)*CardCount);
					RemoveCards(OutCardResult.ResultCard, OutCardResult.CardCount, LeftCardData, CardCount);

					//分析扑克
					tagAnalyseResult AnalyseResultLeft;
					AnalysebCardData(LeftCardData, LeftCount, AnalyseResultLeft);

					//单牌处理
					if (TurnOutType == CT_THREE_TAKE_ONE)
					{
						//提取单牌
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[0]; k++)
						{
							//中止判断
							if (OutCardResult.CardCount == TurnCardCount) break;

							//设置扑克
							BYTE Index = AnalyseResultLeft.BlockCount[0] - k - 1;
							BYTE SignedCard = AnalyseResultLeft.CardData[0][Index];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = SignedCard;
						}

						//提取对牌
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[1] * 2; k++)
						{
							//中止判断
							if (OutCardResult.CardCount == TurnCardCount) break;

							//设置扑克
							BYTE Index = (AnalyseResultLeft.BlockCount[1] * 2 - k - 1);
							BYTE SignedCard = AnalyseResultLeft.CardData[1][Index];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = SignedCard;
						}

						//提取三牌
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[2] * 3; k++)
						{
							//中止判断
							if (OutCardResult.CardCount == TurnCardCount) break;

							//设置扑克
							BYTE Index = (AnalyseResultLeft.BlockCount[2] * 3 - k - 1);
							BYTE SignedCard = AnalyseResultLeft.CardData[2][Index];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = SignedCard;
						}

						//提取四牌
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[3] * 4; k++)
						{
							//中止判断
							if (OutCardResult.CardCount == TurnCardCount) break;

							//设置扑克
							BYTE cbIndex = (AnalyseResultLeft.BlockCount[3] * 4 - k - 1);
							BYTE cbSignedCard = AnalyseResultLeft.CardData[3][cbIndex];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = cbSignedCard;
						}
					}

					//对牌处理
					if (TurnOutType == CT_THREE_TAKE_TWO)
					{
						//提取对牌
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[1]; k++)
						{
							//中止判断
							if (OutCardResult.CardCount == TurnCardCount) break;

							//设置扑克
							BYTE Index = (AnalyseResultLeft.BlockCount[1] - k - 1) * 2;
							BYTE CardData1 = AnalyseResultLeft.CardData[1][Index];
							BYTE CardData2 = AnalyseResultLeft.CardData[1][Index + 1];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData1;
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData2;
						}

						//提取三牌
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[2]; k++)
						{
							//中止判断
							if (OutCardResult.CardCount == TurnCardCount) break;

							//设置扑克
							BYTE Index = (AnalyseResultLeft.BlockCount[2] - k - 1) * 3;
							BYTE CardData1 = AnalyseResultLeft.CardData[2][Index];
							BYTE CardData2 = AnalyseResultLeft.CardData[2][Index + 1];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData1;
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData2;
						}

						//提取四牌
						for (BYTE k = 0; k < AnalyseResultLeft.BlockCount[3]; k++)
						{
							//中止判断
							if (OutCardResult.CardCount == TurnCardCount) break;

							//设置扑克
							BYTE Index = (AnalyseResultLeft.BlockCount[3] - k - 1) * 4;
							BYTE CardData1 = AnalyseResultLeft.CardData[3][Index];
							BYTE CardData2 = AnalyseResultLeft.CardData[3][Index + 1];
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData1;
							OutCardResult.ResultCard[OutCardResult.CardCount++] = CardData2;
						}
					}

					//完成判断
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

	//先出牌
	if (TurnCardCount == 0)
	{
		//地主出牌
		if (MeChairID == m_BankerUser)
		{
			BankerOutCard(MeChairID, OutCardResult);
		}
		//地主下家出牌
		else if (MeChairID == UndersideOfBanker)
		{
			UndersideOfBankerOutCard(MeChairID, OutCardResult);
		}
		//地主上家出牌
		else if (MeChairID == UpsideOfBanker)
		{
			UpsideOfBankerOutCard(MeChairID, OutCardResult);
		}
		else
		{
			return false;
		}
	}
	//跟牌
	else
	{
		//地主跟牌
		if (MeChairID == m_BankerUser) 
		{ 
			BankerOutCard(MeChairID, TurnCardData, TurnCardCount, OutCardUser, OutCardResult);
		}
		//地主下家跟牌
		else if (MeChairID == UndersideOfBanker) 
		{ 
			UndersideOfBankerOutCard(MeChairID, TurnCardData, TurnCardCount, OutCardUser, OutCardResult);
		}
		//地主上家跟牌
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

	//大小排序
	SortCardList(TmpCardData, HandCardCount, ST_ORDER);

	BombCardCount = 0;

	if (HandCardCount < 2) return;

	//双王炸弹
	if (0x4F == TmpCardData[0] && 0x4E == TmpCardData[1])
	{
		BombCardData[BombCardCount++] = TmpCardData[0];
		BombCardData[BombCardCount++] = TmpCardData[1];
	}

	//扑克分析
	for (BYTE i = 0; i < HandCardCount; i++)
	{
		//变量定义
		BYTE SameCount = 1;
		BYTE LogicValue = GetCardLogicValue(TmpCardData[i]);

		//搜索同牌
		for (BYTE j = i + 1; j < HandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(TmpCardData[j]) != LogicValue) break;

			//设置变量
			SameCount++;
		}

		if (4 == SameCount)
		{
			BombCardData[BombCardCount++] = TmpCardData[i];
			BombCardData[BombCardCount++] = TmpCardData[i + 1];
			BombCardData[BombCardCount++] = TmpCardData[i + 2];
			BombCardData[BombCardCount++] = TmpCardData[i + 3];
		}

		//设置索引
		i += SameCount - 1;
	}
}

//分析顺子
void robotAI::GetAllLineCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE LineCardData[], BYTE &LineCardCount)
{
	BYTE TmpCard[MAX_COUNT] = { 0 };
	CopyMemory(TmpCard, HandCardData, HandCardCount);
	//大小排序
	SortCardList(TmpCard, HandCardCount, ST_ORDER);

	LineCardCount = 0;

	//数据校验
	if (HandCardCount < 5) return;

	BYTE FirstCard = 0;
	//去除2和王
	for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCard[i]) < 15)	{ FirstCard = i; break; }

	BYTE SingleLineCard[12];
	BYTE SingleLineCount = 0;
	BYTE LeftCardCount = HandCardCount;
	bool bFindSingleLine = true;

	//连牌判断
	while (LeftCardCount >= 5 && bFindSingleLine)
	{
		SingleLineCount = 1;
		bFindSingleLine = false;
		BYTE LastCard = TmpCard[FirstCard];
		SingleLineCard[SingleLineCount - 1] = TmpCard[FirstCard];
		for (BYTE i = FirstCard + 1; i < LeftCardCount; i++)
		{
			BYTE CardData = TmpCard[i];

			//连续判断
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
			//同牌判断
			else if (GetCardValue(LastCard) != GetCardValue(CardData))
			{
				LastCard = CardData;
				SingleLineCard[SingleLineCount] = CardData;
				++SingleLineCount;
			}
		}

		//保存数据
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

//分析三条
void robotAI::GetAllThreeCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE ThreeCardData[], BYTE &ThreeCardCount)
{
	BYTE TmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(TmpCardData, HandCardData, HandCardCount);

	//大小排序
	SortCardList(TmpCardData, HandCardCount, ST_ORDER);

	ThreeCardCount = 0;

	//扑克分析
	for (BYTE i = 0; i < HandCardCount; i++)
	{
		//变量定义
		BYTE SameCount = 1;
		BYTE LogicValue = GetCardLogicValue(TmpCardData[i]);

		//搜索同牌
		for (BYTE j = i + 1; j < HandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(TmpCardData[j]) != LogicValue) break;

			//设置变量
			SameCount++;
		}

		if (SameCount >= 3)
		{
			ThreeCardData[ThreeCardCount++] = TmpCardData[i];
			ThreeCardData[ThreeCardCount++] = TmpCardData[i + 1];
			ThreeCardData[ThreeCardCount++] = TmpCardData[i + 2];
		}

		//设置索引
		i += SameCount - 1;
	}
}

//分析对子
void robotAI::GetAllDoubleCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE DoubleCardData[], BYTE &DoubleCardCount)
{
	BYTE TmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(TmpCardData, HandCardData, HandCardCount);

	//大小排序
	SortCardList(TmpCardData, HandCardCount, ST_ORDER);

	DoubleCardCount = 0;

	//扑克分析
	for (BYTE i = 0; i < HandCardCount; i++)
	{
		//变量定义
		BYTE SameCount = 1;
		BYTE LogicValue = GetCardLogicValue(TmpCardData[i]);

		//搜索同牌
		for (BYTE j = i + 1; j < HandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(TmpCardData[j]) != LogicValue) break;

			//设置变量
			SameCount++;
		}

		if (SameCount >= 2 && SameCount < 4)
		{
			DoubleCardData[DoubleCardCount++] = TmpCardData[i];
			DoubleCardData[DoubleCardCount++] = TmpCardData[i + 1];
		}

		//设置索引
		i += SameCount - 1;
	}
}

//分析单牌
void robotAI::GetAllSingleCard(BYTE const HandCardData[], BYTE const HandCardCount, BYTE SingleCardData[], BYTE &SingleCardCount)
{
	SingleCardCount = 0;

	BYTE TmpCardData[MAX_COUNT] = { 0 };
	CopyMemory(TmpCardData, HandCardData, HandCardCount);

	//大小排序
	SortCardList(TmpCardData, HandCardCount, ST_ORDER);

	//扑克分析
	for (BYTE i = 0; i < HandCardCount; i++)
	{
		//变量定义
		BYTE SameCount = 1;
		BYTE LogicValue = GetCardLogicValue(TmpCardData[i]);

		//搜索同牌
		for (BYTE j = i + 1; j < HandCardCount; j++)
		{
			//获取扑克
			if (GetCardLogicValue(TmpCardData[j]) != LogicValue) break;

			//设置变量
			SameCount++;
		}

		if (SameCount == 1)
		{
			SingleCardData[SingleCardCount++] = TmpCardData[i];
		}

		//设置索引
		i += SameCount - 1;
	}
}

void robotAI::AnalyseOutCardType(BYTE const HandCardData[], BYTE const HandCardCount, tagOutCardTypeResult CardTypeResult[CT_NUM])
{
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult[0])*(CT_NUM));
	BYTE TmpCardData[MAX_COUNT] = { 0 };

	//保留扑克，防止分析时改变扑克
	BYTE ReserveCardData[MAX_COUNT] = { 0 };
	CopyMemory(ReserveCardData, HandCardData, HandCardCount);
	SortCardList(ReserveCardData, HandCardCount, ST_ORDER);
	CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

	//单牌类型
	for (BYTE i = 0; i < HandCardCount; ++i)
	{
		BYTE Index = CardTypeResult[CT_SINGLE].CardTypeCount;
		CardTypeResult[CT_SINGLE].CardType = CT_SINGLE;
		CardTypeResult[CT_SINGLE].CardData[Index][0] = TmpCardData[i];
		CardTypeResult[CT_SINGLE].EachHandCardCount[Index] = 1;
		CardTypeResult[CT_SINGLE].CardTypeCount++;

		assert(CardTypeResult[CT_SINGLE].CardTypeCount < MAX_TYPE_COUNT);
	}

	//对牌类型
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

	//三条类型
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

	//炸弹类型
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

	//单连类型
	{
		//恢复扑克，防止分析时改变扑克
		ZeroMemory(TmpCardData, HandCardCount);
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		BYTE FirstCard = 0;
		//去除2和王
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

		//连牌判断
		while (LeftCardCount >= 5 && bFindSingleLine)
		{
			SingleLineCount = 1;
			bFindSingleLine = false;
			BYTE LastCard = TmpCardData[FirstCard];
			SingleLineCard[SingleLineCount - 1] = TmpCardData[FirstCard];
			for (BYTE i = FirstCard + 1; i < LeftCardCount; i++)
			{
				BYTE CardData = TmpCardData[i];

				//连续判断
				if (1 != (GetCardLogicValue(LastCard) - GetCardLogicValue(CardData)) && GetCardValue(LastCard) != GetCardValue(CardData))
				{
					LastCard = TmpCardData[i];
					//是否合法
					if (SingleLineCount < 5)
					{
						SingleLineCount = 1;
						SingleLineCard[SingleLineCount - 1] = TmpCardData[i];
						continue;
					}
					else break;
				}
				//同牌判断
				else if (GetCardValue(LastCard) != GetCardValue(CardData))
				{
					LastCard = CardData;
					SingleLineCard[SingleLineCount] = CardData;
					++SingleLineCount;
				}
			}

			//保存数据
			if (SingleLineCount >= 5)
			{
				BYTE Index;
				//所有连牌
				BYTE Start = 0;
				//从大到小
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
				//从小到大
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

	//对连类型
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		//连牌判断
		BYTE FirstCard = 0;
		//去除2和王
		for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCardData[i]) < 15)	{ FirstCard = i; break; }

		BYTE LeftCardCount = HandCardCount - FirstCard;
		bool bFindDoubleLine = true;
		BYTE DoubleLineCount = 0;
		BYTE DoubleLineCard[24];
		//开始判断
		while (LeftCardCount >= 6 && bFindDoubleLine)
		{
			BYTE LastCard = TmpCardData[FirstCard];
			BYTE SameCount = 1;
			DoubleLineCount = 0;
			bFindDoubleLine = false;
			for (BYTE i = FirstCard + 1; i < LeftCardCount + FirstCard; ++i)
			{
				//搜索同牌
				while (GetCardLogicValue(LastCard) == GetCardLogicValue(TmpCardData[i]) && i < LeftCardCount + FirstCard)
				{
					++SameCount;
					++i;
				}

				BYTE LastDoubleCardValue;
				if (DoubleLineCount > 0) LastDoubleCardValue = GetCardLogicValue(DoubleLineCard[DoubleLineCount - 1]);
				//重新开始
				if ((SameCount < 2 || (DoubleLineCount>0 && (LastDoubleCardValue - GetCardLogicValue(LastCard)) != 1)) && i <= LeftCardCount + FirstCard)
				{
					if (DoubleLineCount >= 6) break;
					//回退
					if (SameCount >= 2) i -= SameCount;
					LastCard = TmpCardData[i];
					DoubleLineCount = 0;
				}
				//保存数据
				else if (SameCount >= 2)
				{
					DoubleLineCard[DoubleLineCount] = TmpCardData[i - SameCount];
					DoubleLineCard[DoubleLineCount + 1] = TmpCardData[i - SameCount + 1];
					DoubleLineCount += 2;

					//结尾判断
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

			//保存数据
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

	//三连类型
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		//连牌判断
		BYTE FirstCard = 0;
		//去除2和王
		for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCardData[i]) < 15)	{ FirstCard = i; break; }

		BYTE LeftCardCount = HandCardCount - FirstCard;
		bool bFindThreeLine = true;
		BYTE ThreeLineCount = 0;
		BYTE ThreeLineCard[20];
		//开始判断
		while (LeftCardCount >= 6 && bFindThreeLine)
		{
			BYTE LastCard = TmpCardData[FirstCard];
			BYTE SameCount = 1;
			ThreeLineCount = 0;
			bFindThreeLine = false;
			for (BYTE i = FirstCard + 1; i < LeftCardCount + FirstCard; ++i)
			{
				//搜索同牌
				while (GetCardLogicValue(LastCard) == GetCardLogicValue(TmpCardData[i]) && i < LeftCardCount + FirstCard)
				{
					++SameCount;
					++i;
				}

				BYTE LastThreeCardValue;
				if (ThreeLineCount > 0) LastThreeCardValue = GetCardLogicValue(ThreeLineCard[ThreeLineCount - 1]);

				//重新开始
				if ((SameCount < 3 || (ThreeLineCount>0 && (LastThreeCardValue - GetCardLogicValue(LastCard)) != 1)) && i <= LeftCardCount + FirstCard)
				{
					if (ThreeLineCount >= 6) break;

					if (SameCount >= 3) i -= SameCount;
					LastCard = TmpCardData[i];
					ThreeLineCount = 0;
				}
				//保存数据
				else if (SameCount >= 3)
				{
					ThreeLineCard[ThreeLineCount] = TmpCardData[i - SameCount];
					ThreeLineCard[ThreeLineCount + 1] = TmpCardData[i - SameCount + 1];
					ThreeLineCard[ThreeLineCount + 2] = TmpCardData[i - SameCount + 2];
					ThreeLineCount += 3;

					//结尾判断
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

			//保存数据
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
	//三带一单
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		BYTE HandThreeCard[MAX_COUNT] = { 0 };
		BYTE HandThreeCount = 0;

		//移除炸弹
		BYTE AllBomCardData[MAX_COUNT] = { 0 };
		BYTE AllBomCardCount = 0;
		GetAllBombCard(TmpCardData, HandCardCount, AllBomCardData, AllBomCardCount);
		RemoveCards(AllBomCardData, AllBomCardCount, TmpCardData, HandCardCount);

		GetAllThreeCard(TmpCardData, HandCardCount - AllBomCardCount, HandThreeCard, HandThreeCount);

		{
			BYTE Index;
			//去掉三条
			BYTE RemainCardData[MAX_COUNT] = { 0 };
			CopyMemory(RemainCardData, TmpCardData, HandCardCount - AllBomCardCount);
			BYTE RemainCardCount = HandCardCount - AllBomCardCount - HandThreeCount;
			RemoveCards(HandThreeCard, HandThreeCount, RemainCardData, HandCardCount - AllBomCardCount);
			//三条带一张
			for (BYTE i = 0; i < HandThreeCount; i += 3)
			{
				//三条带一张
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

		//三连带单
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
				//连续判断
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


				//移除三条（还应该移除炸弹王等）
				CopyMemory(RemainCard, TmpCardData, (HandCardCount - AllBomCardCount)*sizeof(BYTE));
				RemoveCards(HandThreeCard, HandThreeCount, RemainCard, HandCardCount - AllBomCardCount);

				for (BYTE start = 0; start < ThreeLineCardCount - 3; start += 3)
				{
					//本顺数目
					BYTE ThisTreeLineCardCount = ThreeLineCardCount - start;
					//单牌个数
					BYTE SingleCardCount = (ThisTreeLineCardCount) / 3;

					//单牌不够
					if (RemainCardCount < SingleCardCount) continue;

					//单牌组合
					BYTE ComCard[5] = { 0 };
					BYTE ComResCard[MAX_TYPE_COUNT][5] = { 0 };
					BYTE ComResLen = 0;

					Combination(ComCard, 0, ComResCard, ComResLen, RemainCard, RemainCardCount, SingleCardCount, SingleCardCount);
					for (BYTE i = 0; i < ComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount;
						CardTypeResult[CT_THREE_TAKE_ONE].CardType = CT_THREE_TAKE_ONE;
						//保存三条
						CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index], ThreeLineCard + start, sizeof(BYTE)*ThisTreeLineCardCount);
						//保存单牌
						CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index] + ThisTreeLineCardCount, ComResCard[i], SingleCardCount);


						CardTypeResult[CT_THREE_TAKE_ONE].EachHandCardCount[Index] = ThisTreeLineCardCount + SingleCardCount;
						CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount++;

						assert(CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount < MAX_TYPE_COUNT);
					}

				}

				//移除三连
				bFindThreeLine = true;
				RemoveCards(ThreeLineCard, ThreeLineCardCount, HandThreeCard, LeftThreeCardCount);
				LeftThreeCardCount -= ThreeLineCardCount;
			}
		}
	}

	//三带一对
	{
		//恢复扑克，防止分析时改变扑克
		CopyMemory(TmpCardData, ReserveCardData, HandCardCount);

		BYTE HandThreeCard[MAX_COUNT] = { 0 };
		BYTE HandThreeCount = 0;
		BYTE RemainCarData[MAX_COUNT] = { 0 };
		BYTE RemainCardCount = 0;

		//抽取三条
		GetAllThreeCard(TmpCardData, HandCardCount, HandThreeCard, HandThreeCount);

		//移除三条（还应该移除炸弹王等）
		CopyMemory(RemainCarData, TmpCardData, HandCardCount);
		RemoveCards(HandThreeCard, HandThreeCount, RemainCarData, HandCardCount);
		RemainCardCount = HandCardCount - HandThreeCount;

		//抽取对牌
		BYTE AllDoubleCardData[MAX_COUNT] = { 0 };
		BYTE AllDoubleCardCount = 0;
		GetAllDoubleCard(RemainCarData, RemainCardCount, AllDoubleCardData, AllDoubleCardCount);

		//三条带一对
		for (BYTE i = 0; i < HandThreeCount; i += 3)
		{
			BYTE Index;

			//三条带一张
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

		//三连带对
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
				//连续判断
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
					//本顺数目
					BYTE ThisTreeLineCardCount = ThreeLineCardCount - start;
					//对牌张数
					BYTE DoubleCardCount = ((ThisTreeLineCardCount) / 3);

					//对牌不够
					if (RemainCardCount < DoubleCardCount) continue;

					BYTE DoubleCardIndex[10]; //对牌下标
					for (BYTE i = 0, j = 0; i < AllDoubleCardCount; i += 2, ++j)
						DoubleCardIndex[j] = i;

					//对牌组合
					BYTE ComCard[5];
					BYTE ComResCard[254][5];
					BYTE ComResLen = 0;

					//利用对牌的下标做组合，再根据下标提取出对牌
					Combination(ComCard, 0, ComResCard, ComResLen, DoubleCardIndex, AllDoubleCardCount / 2, DoubleCardCount, DoubleCardCount);

					assert(ComResLen <= 254);

					for (BYTE i = 0; i < ComResLen; ++i)
					{
						Index = CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount;
						CardTypeResult[CT_THREE_TAKE_TWO].CardType = CT_THREE_TAKE_TWO;
						//保存三条
						CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index], ThreeLineCard + start, sizeof(BYTE)*ThisTreeLineCardCount);
						//保存对牌
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
				//移除三连
				bFindThreeLine = true;
				RemoveCards(ThreeLineCard, ThreeLineCardCount, HandThreeCard, LeftThreeCardCount);
				LeftThreeCardCount -= ThreeLineCardCount;
			}
		}
	}
	return;
}

//分析牌型
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
		//双王炸弹
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
		//炸弹牌型
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
	case CT_SINGLE:				//单牌类型
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
	case CT_DOUBLE:				//对牌类型
	{
		//扑克分析
		for (BYTE i = 0; i < HandCardCount; i++)
		{
			//变量定义
			BYTE SameCount = 1;
			BYTE LogicValue = GetCardLogicValue(TmpCard[i]);

			//搜索同牌
			for (BYTE j = i + 1; j<HandCardCount; j++)
			{
				//获取扑克
				if (GetCardLogicValue(TmpCard[j]) != LogicValue) break;

				//设置变量
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
			//设置索引
			i += SameCount - 1;
		}
		break;
	}
	case CT_THREE:				//三条类型
	{
		//扑克分析
		for (BYTE i = 0; i < HandCardCount; i++)
		{
			//变量定义
			BYTE SameCount = 1;
			BYTE LogicValue = GetCardLogicValue(TmpCard[i]);

			//搜索同牌
			for (BYTE j = i + 1; j<HandCardCount; j++)
			{
				//获取扑克
				if (GetCardLogicValue(TmpCard[j]) != LogicValue) break;

				//设置变量
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
			//设置索引
			i += SameCount - 1;
		}
		break;
	}
	case CT_SINGLE_LINE:		//单连类型
	{
		BYTE FirstCard = 0;
		//去除2和王
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

		//连牌判断
		while (LeftCardCount >= TurnCardCount && bFindSingleLine)
		{
			SingleLineCount = 1;
			bFindSingleLine = false;
			BYTE LastCard = TmpCard[FirstCard];
			SingleLineCard[SingleLineCount - 1] = TmpCard[FirstCard];
			for (BYTE i = FirstCard + 1; i < LeftCardCount; i++)
			{
				BYTE CardData = TmpCard[i];

				//连续判断
				if (1 != (GetCardLogicValue(LastCard) - GetCardLogicValue(CardData)) && GetCardValue(LastCard) != GetCardValue(CardData))
				{
					LastCard = TmpCard[i];
					//是否合法
					if (SingleLineCount<TurnCardCount)
					{
						SingleLineCount = 1;
						SingleLineCard[SingleLineCount - 1] = TmpCard[i];
						continue;
					}
					else break;
				}
				//同牌判断
				else if (GetCardValue(LastCard) != GetCardValue(CardData))
				{
					LastCard = CardData;
					SingleLineCard[SingleLineCount] = CardData;
					++SingleLineCount;
				}
			}

			//保存数据
			if (SingleLineCount >= TurnCardCount && 
				GetCardLogicValue(SingleLineCard[0])>GetCardLogicValue(TurnCardData[0]))
			{
				BYTE Index;
				BYTE Start = 0;
				//所有连牌
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
	case CT_DOUBLE_LINE:		//对连类型
	{
		//连牌判断
		BYTE FirstCard = 0;
		//去除2和王
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
		//开始判断
		while (LeftCardCount >= TurnCardCount && bFindDoubleLine)
		{
			BYTE LastCard = TmpCard[FirstCard];
			BYTE SameCount = 1;
			DoubleLineCount = 0;
			bFindDoubleLine = false;
			for (BYTE i = FirstCard + 1; i < LeftCardCount + FirstCard; ++i)
			{
				//搜索同牌
				while (GetCardValue(LastCard) == GetCardValue(TmpCard[i]) && i < LeftCardCount + FirstCard)
				{
					++SameCount;
					++i;
				}

				BYTE LastDoubleCardValue;
				if (DoubleLineCount > 0) LastDoubleCardValue = GetCardLogicValue(DoubleLineCard[DoubleLineCount - 1]);
				//重新开始
				if ((SameCount < 2 || (DoubleLineCount>0 && (LastDoubleCardValue - GetCardLogicValue(LastCard)) != 1)) && i <= LeftCardCount + FirstCard)
				{
					if (DoubleLineCount >= TurnCardCount) break;

					if (SameCount >= 2) i -= SameCount;

					LastCard = TmpCard[i];
					DoubleLineCount = 0;
				}
				//保存数据
				else if (SameCount >= 2)
				{
					DoubleLineCard[DoubleLineCount] = TmpCard[i - SameCount];
					DoubleLineCard[DoubleLineCount + 1] = TmpCard[i - SameCount + 1];
					DoubleLineCount += 2;

					//结尾判断
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

			//保存数据
			if (DoubleLineCount >= TurnCardCount)
			{
				BYTE Index;
				BYTE Start = 0;
				//所有连牌
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
	case CT_THREE_LINE:			//三连类型
	{
		//连牌判断
		BYTE FirstCard = 0;
		//去除2和王
		for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardLogicValue(TmpCard[i]) < 15)	{ FirstCard = i; break; }

		BYTE LeftCardCount = HandCardCount - FirstCard;
		bool bFindThreeLine = true;
		BYTE ThreeLineCount = 0;
		BYTE ThreeLineCard[20];
		//开始判断
		while (LeftCardCount >= TurnCardCount && bFindThreeLine)
		{
			BYTE LastCard = TmpCard[FirstCard];
			BYTE SameCount = 1;
			ThreeLineCount = 0;
			bFindThreeLine = false;
			for (BYTE i = FirstCard + 1; i < LeftCardCount + FirstCard; ++i)
			{
				//搜索同牌
				while (GetCardValue(LastCard) == GetCardValue(TmpCard[i]) && i < LeftCardCount + FirstCard)
				{
					++SameCount;
					++i;
				}

				BYTE LastThreeCardValue;
				if (ThreeLineCount > 0) LastThreeCardValue = GetCardLogicValue(ThreeLineCard[ThreeLineCount - 1]);

				//重新开始
				if ((SameCount < 3 || (ThreeLineCount>0 && (LastThreeCardValue - GetCardLogicValue(LastCard)) != 1)) && i <= LeftCardCount + FirstCard)
				{
					if (ThreeLineCount >= TurnCardCount) break;

					if (SameCount >= 3) i -= 3;
					LastCard = TmpCard[i];
					ThreeLineCount = 0;
				}
				//保存数据
				else if (SameCount >= 3)
				{
					ThreeLineCard[ThreeLineCount] = TmpCard[i - SameCount];
					ThreeLineCard[ThreeLineCount + 1] = TmpCard[i - SameCount + 1];
					ThreeLineCard[ThreeLineCount + 2] = TmpCard[i - SameCount + 2];
					ThreeLineCount += 3;

					//结尾判断
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

			//保存数据
			if (ThreeLineCount >= TurnCardCount)
			{
				BYTE Index;
				BYTE Start = 0;
				//所有连牌
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
	case CT_THREE_TAKE_ONE://三带一单
	{
		BYTE TurnThreeCard[MAX_COUNT];
		BYTE TurnThreeCount = 0;
		BYTE HandThreeCard[MAX_COUNT];
		BYTE HandThreeCount = 0;
		BYTE SingleCardCount = TurnCardCount / 4;

		//移除炸弹
		BYTE AllBomCardData[MAX_COUNT];
		BYTE AllBomCardCount = 0;
		GetAllBombCard(TmpCard, HandCardCount, AllBomCardData, AllBomCardCount);
		RemoveCards(AllBomCardData, AllBomCardCount, TmpCard, HandCardCount);

		//三条扑克
		GetAllThreeCard(TurnCardData, TurnCardCount, TurnThreeCard, TurnThreeCount);

		BYTE FirstCard = 0;

		//去除2和王
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
				//连续判断
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

				//单牌组合
				BYTE ComCard[5];
				BYTE ComResCard[254][5];
				BYTE ComResLen = 0;
				Combination(ComCard, 0, ComResCard, ComResLen, RemainCard, (HandCardCount - AllBomCardCount) - TurnThreeCount, SingleCardCount, SingleCardCount);
				for (BYTE i = 0; i < ComResLen; ++i)
				{
					Index = CardTypeResult[CT_THREE_TAKE_ONE].CardTypeCount;
					CardTypeResult[CT_THREE_TAKE_ONE].CardType = CT_THREE_TAKE_ONE;
					//保存三条
					CopyMemory(CardTypeResult[CT_THREE_TAKE_ONE].CardData[Index], ThreeLineCard, sizeof(BYTE)*TurnThreeCount);
					//保存单牌
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
	case CT_THREE_TAKE_TWO://三带一对
	{
		BYTE TurnThreeCard[MAX_COUNT];
		BYTE TurnThreeCount = 0;
		BYTE HandThreeCard[MAX_COUNT];
		BYTE HandThreeCount = 0;
		BYTE DoubleCardCount = TurnCardCount / 5;

		//三条扑克
		GetAllThreeCard(TurnCardData, TurnCardCount, TurnThreeCard, TurnThreeCount);

		BYTE FirstCard = 0;

		//去除2和王
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
				//连续判断
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


				BYTE DoubleCardIndex[10]; //对牌下标
				for (BYTE i = 0, j = 0; i < AllDoubleCardCount; i += 2, ++j)
					DoubleCardIndex[j] = i;

				//对牌组合
				BYTE ComCard[5];
				BYTE ComResCard[254][5];
				BYTE ComResLen = 0;

				//利用对牌的下标做组合，再根据下标提取出对牌
				Combination(ComCard, 0, ComResCard, ComResLen, DoubleCardIndex, AllDoubleCardCount / 2, DoubleCardCount, DoubleCardCount);
				for (BYTE i = 0; i < ComResLen; ++i)
				{
					Index = CardTypeResult[CT_THREE_TAKE_TWO].CardTypeCount;
					CardTypeResult[CT_THREE_TAKE_TWO].CardType = CT_THREE_TAKE_TWO;
					//保存三条
					CopyMemory(CardTypeResult[CT_THREE_TAKE_TWO].CardData[Index], ThreeLineCard, sizeof(BYTE)*TurnThreeCount);
					//保存对牌
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
	case CT_FOUR_TAKE_ONE://四带两单
	{
		BYTE FirstCard = 0;
		//去除王牌
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
		//抽取四张
		GetAllBombCard(TmpCard + FirstCard, HandCardCount - FirstCard, HandAllFourCardData, HandAllFourCardCount);
		GetAllBombCard(TurnCardData, TurnCardCount, TurnAllFourCardData, TurnAllFourCardCount);

		if (HandAllFourCardCount > 0 && GetCardLogicValue(HandAllFourCardData[0]) < GetCardLogicValue(TurnAllFourCardData[0])) return;

		BYTE CanOutFourCardData[MAX_COUNT];
		BYTE CanOutFourCardCount = 0;
		//可出的牌
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
			//单牌组合
			BYTE ComCard[5];
			BYTE ComResCard[254][5];
			BYTE ComResLen = 0;
			//单牌组合
			Combination(ComCard, 0, ComResCard, ComResLen, RemainCard, HandCardCount - CanOutFourCardCount, 2, 2);
			for (BYTE i = 0; i < ComResLen; ++i)
			{
				//不能带对
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
	case CT_FOUR_TAKE_TWO://四带两对
	{
		BYTE FirstCard = 0;
		//去除王牌
		for (BYTE i = 0; i < HandCardCount; ++i)	if (GetCardColor(TmpCard[i]) != 0x40)	{ FirstCard = i; break; }

		BYTE HandAllFourCardData[MAX_COUNT];
		BYTE HandAllFourCardCount = 0;
		BYTE TurnAllFourCardData[MAX_COUNT];
		BYTE TurnAllFourCardCount = 0;
		//抽取四张
		GetAllBombCard(TmpCard + FirstCard, HandCardCount - FirstCard, HandAllFourCardData, HandAllFourCardCount);
		GetAllBombCard(TurnCardData, TurnCardCount, TurnAllFourCardData, TurnAllFourCardCount);

		if (HandAllFourCardCount > 0 && GetCardLogicValue(HandAllFourCardData[0]) < GetCardLogicValue(TurnAllFourCardData[0])) 
			return;

		BYTE CanOutFourCardData[MAX_COUNT];
		BYTE CanOutFourCardCount = 0;
		//可出的牌
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


			BYTE DoubleCardIndex[10]; //对牌下标
			for (BYTE i = 0, j = 0; i < AllDoubleCardCount; i += 2, ++j)
				DoubleCardIndex[j] = i;

			//对牌组合
			BYTE ComCard[5];
			BYTE ComResCard[254][5];
			BYTE ComResLen = 0;

			//利用对牌的下标做组合，再根据下标提取出对牌
			Combination(ComCard, 0, ComResCard, ComResLen, DoubleCardIndex, AllDoubleCardCount / 2, 2, 2);
			for (BYTE i = 0; i < ComResLen; ++i)
			{
				BYTE Index = CardTypeResult[CT_FOUR_TAKE_TWO].CardTypeCount;
				CardTypeResult[CT_FOUR_TAKE_TWO].CardType = CT_FOUR_TAKE_TWO;
				CopyMemory(CardTypeResult[CT_FOUR_TAKE_TWO].CardData[Index], CanOutFourCardData + Start, 4);

				//保存对牌
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
	case CT_BOMB_CARD:			//炸弹类型
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
	case CT_MISSILE_CARD:		//火箭类型
	{
		//没有比火箭更大的牌了
		break;
	}
	default:
		assert(false);
		break;
	}
	return;
}

/********************************************************************
函数名：Combination
参数：
CombineCardData：存储单个的组合结果
ResComLen：已得到的组合长度，开始调用时此参数为0
ResultCardData：存储所有的组合结果
ResCardLen：ResultCardData的第一下标的长度，组合结果的个数
SrcCardData：需要做组合的数据
SrcLen：cbSrcCardData的数据数目
CombineLen2，cbCombineLen1：组合的长度，开始调用时两者相等。
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
			//去掉SrcCardData中的第一个数据
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

//地主（先出牌）
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

	BYTE UndersideOfBanker = (m_BankerUser + 1) % GAME_PLAYER;		//地主下家
	BYTE UpsideOfBanker = (UndersideOfBanker + 1) % GAME_PLAYER;	//地主上家

	//如果只剩顺牌和单只(或对牌)，则先出顺牌
	{
		BYTE TmpHandCard[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(LineCard, LineCardCount, TmpHandCard, TmpHandCardLen);
		TmpHandCardLen -= LineCardCount;

		//如果是顺牌加单牌，先出顺牌
		if (LineCardCount + 1 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount))
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}
		//如果是顺牌加对牌，先出顺牌
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

		//如果三顺牌加单牌，先出顺牌
		if (ThreeLineCardCount + 1 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount))
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}
		//如果三顺牌加对牌，先出顺牌
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

		//如果双顺加单牌，先出顺牌
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

		//双王炸弹和一手
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

	//对王加一只
	if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4f;
		OutCardResult.ResultCard[1] = 0x4e;
		return;
	}
	//对王
	else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4f;
		OutCardResult.ResultCard[1] = 0x4e;
		return;
	}
	//只剩一手牌
	else if (CT_ERROR != GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID]))
	{
		OutCardResult.CardCount = m_UserCardCount[MeChairID];
		CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		return;
	}

	//只剩一张和一手
	if (m_UserCardCount[MeChairID] >= 2)
	{
		//上家扑克
		tagOutCardTypeResult UpsideCanOutCardType1[13];
		ZeroMemory(UpsideCanOutCardType1, sizeof(UpsideCanOutCardType1));
		tagOutCardTypeResult UpsideCanOutCardType2[13];
		ZeroMemory(UpsideCanOutCardType2, sizeof(UpsideCanOutCardType2));

		//下家扑克
		tagOutCardTypeResult UndersideCanOutCardType1[13];
		ZeroMemory(UndersideCanOutCardType1, sizeof(UndersideCanOutCardType1));
		tagOutCardTypeResult UndersideCanOutCardType2[13];
		ZeroMemory(UndersideCanOutCardType2, sizeof(UndersideCanOutCardType2));

		//只剩一张和一手
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

			//上家扑克
			tagOutCardTypeResult UpsideOutCardTypeResult[CT_NUM];
			ZeroMemory(UpsideOutCardTypeResult, sizeof(UpsideOutCardTypeResult));

			//下家扑克
			tagOutCardTypeResult UndersideOutCardTypeResult[CT_NUM];
			ZeroMemory(UndersideOutCardTypeResult, sizeof(UndersideOutCardTypeResult));

			for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
			{
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(tmpCardResult.CardData[i][0])) == 4)
				{
					continue;
				}

				//去掉该手牌后的单牌
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

			//选择最小牌
			for (BYTE i = 0; i < 4; ++i)
			{
				if (OutcardType[i] == MinCardType &&
					MinSingleCardCount[i] <= MinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[Index][0]))
					Index = NumIndex[i];
			}

			//对王加一只
			if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4f;
				OutCardResult.ResultCard[1] = 0x4e;
				return;
			}
			//对王
			else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4f;
				OutCardResult.ResultCard[1] = 0x4e;
				return;
			}
			else
			{
				//设置变量
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

	//先判断偏家是否有人只剩一张牌了，如果是，则出最大的单牌，如果单牌压不过则出炸弹
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

	//正常出单牌
	BYTE FirstCard = 0;
	//过滤王
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

	//计算单牌
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

	//除炸弹外的牌
	for (BYTE CardType = CT_SINGLE; CardType < CT_BOMB_CARD; ++CardType)
	{
		tagOutCardTypeResult const &tmpCardResult = CardTypeResult[CardType];
		for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
		{
			BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

			BYTE MaxValue = 0;
			BYTE Index = 0;

			//搜索MinSingleCardCount[4]的最大值
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

			//保存最小值
			if (MinSingleCountInFour >= TmpCount)
			{
				//最小牌型
				MinCardType = CardType;
				//最小牌型中的最小单牌
				MinSingleCountInFour = TmpCount;
				//最小牌型中的最小牌
				MinIndex = i;
			}
		}
	}

	if (MinSingleCountInFour != MAX_COUNT)
	{
		BYTE Index = MinIndex;

		//选择最小牌
		for (BYTE i = 0; i < 4; ++i)
		{
			if (OutcardType[i] == MinCardType &&
				MinSingleCardCount[i] <= MinSingleCountInFour &&
				GetCardLogicValue(CardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(CardTypeResult[MinCardType].CardData[Index][0]))
			{
				Index = NumIndex[i];
			}
		}

		//对王加一只
		if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		//对王
		else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		else
		{
			//设置变量
			OutCardResult.CardCount = CardTypeResult[MinCardType].EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[MinCardType].CardData[Index], CardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
			return;
		}

		assert(OutCardResult.CardCount > 0);

		return;
	}
	//如果只剩炸弹
	else
	{
		BYTE Index = 0;
		BYTE LogicCardValue = GetCardLogicValue(0x4F) + 1;
		//最小炸弹
		for (BYTE i = 0; i < CardTypeResult[CT_BOMB_CARD].CardTypeCount; ++i)
			if (LogicCardValue>GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]))
			{
				LogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]);
				Index = i;
			}

		//设置变量
		OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index];
		CopyMemory(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData[Index], CardTypeResult[CT_BOMB_CARD].EachHandCardCount[Index] * sizeof(BYTE));

		return;
	}

	//如果都没有搜索到就出最小的一张
	OutCardResult.CardCount = 1;
	OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];

	return;
}

//地主（后出牌）
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
		//如果是单牌，先判断偏家是否有人只剩一张牌了，如果是，则出最大的单牌，如果单牌压不过则出炸弹
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

		//如果是对牌，先判断偏家是否有人只剩对牌了，如果是，则出最大的对牌，如果对牌压不过则出炸弹
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

	//取出四个最小的单牌
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE MinSingleCountInFour = MAX_COUNT;

	//可出扑克（这里已经过滤掉炸弹了）
	const tagOutCardTypeResult &CanOutCard = CardTypeResult[OutCardType];
	
	for (BYTE i = 0; i < CanOutCard.CardTypeCount; ++i)
	{
		//不拆炸弹
		if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[i][0])) == 4)
		{
			continue;
		}

		//最小单牌
		BYTE TmpCount = AnalyseSingleCardCount(
			m_UserCardData[MeChairID],
			m_UserCardCount[MeChairID],
			CanOutCard.CardData[i],
			CanOutCard.EachHandCardCount[i]);
		BYTE Maxvalue = 0;
		BYTE Index = 0;

		//搜索MinSingleCardCount[4]的最大值
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
	
	//原始单牌数
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
				//针对大牌
				bool NoLargeCard = true;

				//当出牌玩家手上的牌数量大于4，而且出的是小于K的牌而且不是出牌手上的最大牌时，不能出2去打
				if (m_UserCardCount[OutCardUser] >= 5 && 
					m_UserCardCount[MeChairID] >= 5 &&
					GetCardLogicValue(CanOutCard.CardData[Index][0]) >= 15 &&
					GetCardLogicValue(TurnCardData[0]) <= 14)
				{
					NoLargeCard = false;
				}

				//搜索有没有大牌（针对飞机带翅膀后面的带牌）
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
			//最大牌
			BYTE LargesLogicCard;
			if (OutCardUser == UndersideUser) LargesLogicCard = GetCardLogicValue(m_UserCardData[UndersideUser][0]);
			else if (OutCardUser == UpsideUser) LargesLogicCard = GetCardLogicValue(m_UserCardData[UpsideUser][0]);
			bool CanOut = true;

			//王只压二
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
			//闲家的最大牌
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
				//取一张大于等于2而且要比闲家出的牌大的牌
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

			//跟单牌时，不能过于早拆王或出王
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

	//出不了，要考虑炸弹
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

		//判断除了炸弹后的单牌数
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

//地主上家（先出牌）
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

	//分析牌
	tagOutCardTypeResult MeOutCardTypeResult[CT_NUM];
	memset(MeOutCardTypeResult, 0, sizeof(MeOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeOutCardTypeResult);

	tagOutCardTypeResult FriendOutCardTypeResult[CT_NUM];
	ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID], FriendOutCardTypeResult);

	tagOutCardTypeResult BankerOutCardTypeResult[CT_NUM];
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult));
	AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerOutCardTypeResult);

	//如果有顺牌和单只或一对，而且单只或对比地主的小，则先出顺
	{
		BYTE TmpHandCard[MAX_COUNT] = { 0 };
		BYTE TmpHandCardLen = m_UserCardCount[MeChairID];
		memcpy(TmpHandCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		RemoveCards(LineCard, LineCardCount, TmpHandCard, TmpHandCardLen);
		TmpHandCardLen -= LineCardCount;

		//如果是顺牌加单牌，先出顺牌
		if (LineCardCount + 1 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount))
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}
		//如果是顺牌加对牌，先出顺牌
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

		//如果三顺牌加单牌，先出顺牌
		if (ThreeLineCardCount + 1 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount))
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}
		//如果三顺牌加对牌，先出顺牌
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

		//如果双顺加单牌，先出顺牌
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


		//双王炸弹和一手
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
	//对王加一只
	if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4F;
		OutCardResult.ResultCard[1] = 0x4E;
		return;
	}
	//对王
	else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4F;
		OutCardResult.ResultCard[1] = 0x4E;
		return;
	}
	//只剩一手牌
	else if (CT_ERROR != GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID]))
	{
		OutCardResult.CardCount = m_UserCardCount[MeChairID];
		CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		return;
	}

	//只剩一张和一手
	if (m_UserCardCount[MeChairID] >= 2)
	{
		//地主扑克
		tagOutCardTypeResult BankerCanOutCardType1[CT_NUM];
		ZeroMemory(BankerCanOutCardType1, sizeof(BankerCanOutCardType1));
		tagOutCardTypeResult BankerCanOutCardType2[CT_NUM];
		ZeroMemory(BankerCanOutCardType2, sizeof(BankerCanOutCardType2));

		BYTE FirstHandCardType = GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
		BYTE SecondHandCardType = GetCardType(m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);

		//地主可以出的牌
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

	//配合队友逻辑
	{
		//得地主没有炸弹才行
		if (BankerOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			//如果队友有比庄家更大的单牌和另一手（单/对/组）牌
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

			//如果队友有比庄家更大的对牌和另一手（单/对/组）牌
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

	//正常分析
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

			//相同牌型、相同长度，单顺，双顺等相同牌型的长度可能不一样
			BYTE ThisHandCardCount = MAX_COUNT;
			//地主扑克
			tagOutCardTypeResult BankerCanOutCard[CT_NUM];
			memset(BankerCanOutCard, 0, sizeof(BankerCanOutCard));
			tagOutCardTypeResult FriendOutCardTypeResult[CT_NUM];
			memset(FriendOutCardTypeResult, 0, sizeof(FriendOutCardTypeResult));

			for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
			{
				//不拆炸弹
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

				//重新分析
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

				//是否有大牌
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

				//地主是否可以走掉，这里都没有考虑炸弹
				if (tmpCardResult.EachHandCardCount[i] == m_UserCardCount[m_BankerUser] &&
					GetCardLogicValue(m_UserCardData[m_BankerUser][0])>GetCardLogicValue(tmpCardResult.CardData[i][0])) continue;

				//搜索MinSingleCardCount[4]的最大值
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

				//保存最小值
				if (MinSingleCountInFour >= TmpCount)
				{
					//最小牌型
					MinCardType = CardType;
					//最小牌型中的最小单牌
					MinSingleCountInFour = TmpCount;
					//最小牌型中的最小牌
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

			//选择最小牌
			for (BYTE i = 0; i < 4; ++i)
			{
				if (OutCardType[i] == MinCardType && MinSingleCardCount[i] <= MinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[Index][0]))
				{
					Index = NumIndex[i];
				}
			}

			//对王加一只
			if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			//对王
			else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4f;
				OutCardResult.ResultCard[1] = 0x4e;
				return;
			}
			else
			{
				//设置变量
				OutCardResult.CardCount = MeOutCardTypeResult[MinCardType].EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, MeOutCardTypeResult[MinCardType].CardData[Index], MeOutCardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
			}
			return;
		}

		//如果地主扑克少于5，还没有找到适合的牌则从大出到小
		if (OutCardResult.CardCount <= 0 && m_UserCardCount[m_BankerUser] <= 5)
		{
			//只有一张牌时不能放地主走
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
	//过滤王
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

	//计算单牌
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

	//分析地主单牌
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

	//除炸弹外的牌
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
			//不拆炸弹
			if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(tmpCardResult.CardData[i][0])) == 4)
			{
				continue;
			}

			//不能放走地主小牌
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

			//搜索MinSingleCardCount[4]的最大值
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

			//保存最小值
			if (MinSingleCountInFour >= TmpCount)
			{
				//最小牌型
				MinCardType = CardType;
				//最小牌型中的最小单牌
				MinSingleCountInFour = TmpCount;
				//最小牌型中的最小牌
				MinIndex = i;
			}
		}
	}

	if (MinSingleCountInFour != MAX_COUNT)
	{
		BYTE Index = MinIndex;

		//选择最小牌
		for (BYTE i = 0; i < 4; ++i)
		{
			if (OutcardType[i] == MinCardType && MinSingleCardCount[i] <= MinSingleCountInFour &&
				GetCardLogicValue(CardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(CardTypeResult[MinCardType].CardData[Index][0]))
				Index = NumIndex[i];
		}

		//对王加一只
		if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4f;
			OutCardResult.ResultCard[1] = 0x4e;
			return;
		}
		//对王
		else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4f;
			OutCardResult.ResultCard[1] = 0x4e;
			return;
		}
		else
		{
			//设置变量
			OutCardResult.CardCount = CardTypeResult[MinCardType].EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[MinCardType].CardData[Index], CardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
			return;
		}

		return;
	}
	//如果只剩炸弹
	else if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0)
	{
		BYTE Index = 0;
		BYTE LogicCardValue = GetCardLogicValue(0x4F) + 1;
		//最小炸弹
		for (BYTE i = 0; i < CardTypeResult[CT_BOMB_CARD].CardTypeCount; ++i)
		{
			if (LogicCardValue > GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]))
			{
				LogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]);
				Index = i;
			}
		}
		//设置变量
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

		//如果都没有搜索到就出最小的一张
		OutCardResult.CardCount = 1;
		OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];
	}
	
	return;
}

//地主上家（后出牌）
void robotAI::UpsideOfBankerOutCard(BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult)
{
	//零下标没用
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//出牌类型
	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);

	//搜索可出牌
	tagOutCardTypeResult BankerOutCardTypeResult[CT_NUM];
	ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult));

	AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerOutCardTypeResult);
	AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], TurnCardData, TurnCardCount, CardTypeResult);

	if (IsCanOneHandFollow(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeChairID, TurnCardData, TurnCardCount, OutCardUser, OutCardResult))
	{
		return;
	}

	{
		//如果是单牌，先判断偏家是否有人只剩一张牌了，如果是，则出最大的单牌，如果单牌压不过则出炸弹
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

		//如果是对牌，先判断偏家是否有人只剩对牌了，如果是，则出最大的对牌，如果对牌压不过则出炸弹
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

	//配合队友打
	{
		tagOutCardTypeResult FriendCardTypeResult[CT_NUM];
		ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult));
		tagOutCardTypeResult BankerCardTypeResult[CT_NUM];
		ZeroMemory(BankerCardTypeResult, sizeof(BankerCardTypeResult));

		BYTE FriendID = (m_BankerUser + 1) % GAME_PLAYER;
		AnalyseOutCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID], FriendCardTypeResult);
		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerCardTypeResult);

		bool CanRun = false;

		//如果队友有比庄家更大的单牌和另一手（单/对/组）牌
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

		//如果队友有比庄家更大的对牌和另一手（单/对/组）牌
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

		//队友可跑，庄家没有炸弹
		if (OutCardUser == m_BankerUser &&
			(OutCardType == CT_SINGLE || OutCardType == CT_DOUBLE) &&
			CanRun && 
			BankerCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			//当前要跟的牌型，自己能出的比地主更大时，出最大的
			if (CardTypeResult[OutCardType].CardTypeCount > 0 &&
				BankerOutCardTypeResult[OutCardType].CardTypeCount > 0 &&
				GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))
			{
				memcpy(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0]);
				OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[0];
				return;
			}
			//地主压不了，则自己也不用出
			if (BankerOutCardTypeResult[OutCardType].CardTypeCount == 0)
			{
				return;
			}
			//地主可以压，且自己有炸，则出炸弹
			if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0 &&
				BankerOutCardTypeResult[OutCardType].CardTypeCount >= 0)
			{
				OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0];
				memcpy(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData[0], CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0]);
				return;
			}
		}
	}

	//如果地主没有此牌型了则不压对家牌
	if (OutCardUser != m_BankerUser &&																						//出牌的人是队友
		(BankerOutCardTypeResult[OutCardType].CardTypeCount == 0 ||															//地主没有此类牌型
		GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]) <= GetCardLogicValue(TurnCardData[0])) &&	//地主的牌压不了
		CardTypeResult[OutCardType].EachHandCardCount[0] != m_UserCardCount[MeChairID])													//不能一次出完
	{
		//放弃出牌
		return;
	}

	/*//下家为地主，而且地主扑克少于5张
	if (m_UserCardCount[m_BankerUser] <= 5 &&																									 //地主的牌数小于等于5张
		CardTypeResult[OutCardType].CardTypeCount > 0 &&																						 //本家可以出
		OutCardType != CT_BOMB_CARD &&																											 //且出的牌不是炸弹
		(
			(GetCardLogicValue(TurnCardData[0]) < 12 && OutCardUser != m_BankerUser && BankerOutCardTypeResult[OutCardType].CardTypeCount > 0) ||//队友出牌较小且地主可以压住
			(OutCardUser == m_BankerUser)																										 //地主出牌
		))
	{
		BYTE Index = 0;
		//寻找可以压住地主的最小一手牌
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
	}//下家为地主，而且地主扑克少于5张*/

	//取出四个最小单牌
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE MinSingleCountInFour = MAX_COUNT;

	//可出扑克（这里已经过滤掉炸弹了）
	tagOutCardTypeResult const &CanOutCard = CardTypeResult[OutCardType];

	for (BYTE i = 0; i<CanOutCard.CardTypeCount; ++i)
	{
		//不拆炸弹
		if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[i][0])) == 4)
		{
			continue;
		}

		//最小单牌
		BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CanOutCard.CardData[i], CanOutCard.EachHandCardCount[i]);
		BYTE MaxValue = 0;
		BYTE Index = 0;

		//搜索MinSingleCardCount[4]的最大值
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

	//原始单牌数
	BYTE OriginSingleCardCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], NULL, 0);

	//朋友出牌
	bool bFriendOut = m_BankerUser != OutCardUser;
	if (bFriendOut)
	{
		if (CanOutCard.CardTypeCount > 0)
		{
			//分析地主单牌
			BYTE BankerSingleCardData[MAX_COUNT];
			ZeroMemory(BankerSingleCardData, sizeof(BankerSingleCardData));
			BYTE BankerSingleCardCount = AnalyseSingleCardCount(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], NULL, 0, BankerSingleCardData);
			BYTE BankerSingleCardLogic = 0;

			//地主有多个单牌，且小的单牌多
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

				//不拆炸弹
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[Index][0])) == 4)
				{
					continue;
				}

				bool bCanOut = false;
				if (OutCardType == CT_SINGLE &&											//出的是单牌 
					GetCardLogicValue(TurnCardData[0]) < BankerSingleCardLogic &&		//地主可以压队友
					GetCardLogicValue(CanOutCard.CardData[Index][0]) <= 14 && 
					MinSingleCardCount[i] < OriginSingleCardCount + 2)
				{
					bCanOut = true;
				}

				/*if (OutCardType != CT_SINGLE &&
					GetCardLogicValue(CanOutCard.CardData[Index][0]) >= 14 &&			//出牌值大于等于A
					BankerOutCardTypeResult[OutCardType].CardTypeCount > 1 &&			//庄家可压牌型大于1
					GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(TurnCardData[0]) &&
					CardTypeResult[OutCardType].CardTypeCount > 0 &&
					GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))
				{
					continue;
				}*/

				//小于K的牌，或者小于K而且是散牌
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
					//搜索有没有大牌（针对飞机带翅膀后面的带牌）
					bool bNoLargeCard = true;
					for (BYTE k = 3; k < CanOutCard.EachHandCardCount[Index]; ++k)
					{
						//有大牌而且不能一次出完
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

				//设置变量
				OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
				CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

				return;
			}
			//手上少于五张牌
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
					//设置变量
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
	//地主出牌
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

				//不拆炸弹
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[Index][0])) == 4)
				{
					continue;
				}

				if ((MinSingleCardCount[i]<OriginSingleCardCount + 3) && MinSingleCardCount[i] <= MinSingleCountInFour &&
					MinLogicCardValue>GetCardLogicValue(CanOutCard.CardData[Index][0]))
				{
					//针对大牌
					bool bNoLargeCard = true;

					//当地主的可压牌型数量大于1，且牌型最大值比自己小的时候，自己不压，留在后面搞地主
					if (OutCardType != CT_SINGLE &&
						GetNumOfType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], OutCardType) > 1 &&
						GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(TurnCardData[0]) &&
						CardTypeResult[OutCardType].CardTypeCount > 0 &&
						GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))
					{
						bNoLargeCard = false;
					}

					//当地主手上牌数大于10，自己的牌数量大于5，而且地主出的是小于K的牌而且不是地主手上的最大牌时，不能出2去打
					if (m_UserCardCount[m_BankerUser] >= 5 && m_UserCardCount[MeChairID] > 5 && CanOutCard.EachHandCardCount[Index] >= 2 &&
						GetCardLogicValue(CanOutCard.CardData[Index][0]) >= 15 &&
						GetCardLogicValue(TurnCardData[0]) < 13 &&
						GetCardLogicValue(TurnCardData[0]) < GetCardLogicValue(m_UserCardData[m_BankerUser][0]) &&
						CanOutCard.EachHandCardCount[Index] != m_UserCardCount[MeChairID])
					{
						bNoLargeCard = false;
					}

					//搜索有没有大牌（针对飞机带翅膀后面的带牌）
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
				//地主的最大牌
				BYTE LargestLogicCard = GetCardLogicValue(m_UserCardData[m_BankerUser][0]);
				bool bCanOut = true;

				//王只压2
				if (GetCardLogicValue(TurnCardData[0])<LargestLogicCard)
				{
					if (GetCardColor(CanOutCard.CardData[CanOutIndex][0]) == 0x40 && GetCardLogicValue(TurnCardData[0]) <= 14 && m_UserCardCount[MeChairID]>5)
					{
						bCanOut = false;
					}
				}

				if (bCanOut)
				{
					//设置变量
					OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
					CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

					return;
				}
			}

			if (OutCardType == CT_SINGLE)
			{
				//地主的最大牌
				BYTE LargestLogicCard = GetCardLogicValue(m_UserCardData[m_BankerUser][0]);

				if (GetCardLogicValue(TurnCardData[0]) == 14 ||
					GetCardLogicValue(TurnCardData[0]) >= LargestLogicCard ||
					(GetCardLogicValue(TurnCardData[0])<LargestLogicCard - 1) ||
					m_UserCardCount[m_BankerUser] <= 5)
				{
					//取一张大于等于2而且要比地主出的牌大的牌，
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
						//设置变量
						OutCardResult.CardCount = 1;
						OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][Index];

						return;
					}
				}
			}
			//跟单牌时，不能过于早拆王或出王
			if (GetCardLogicValue(CanOutCard.CardData[CanOutCard.CardTypeCount - 1][0]) >= 16 && m_UserCardCount[MeChairID] > 5)
			{
				return;
			}
		}

		//还要考虑炸弹
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

			//判断出了炸弹后的单牌数
			BYTE SingleCardList[MAX_COUNT] = { 0 };
			BYTE SingleCardCount = AnalyseSingleCardCountS(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombCard.CardData[Index], BombCard.EachHandCardCount[Index]);
			if (m_UserCardCount[MeChairID] > 7 || SingleCardCount >= 3 || (OutCardType == CT_SINGLE && GetCardLogicValue(TurnCardData[0]) < 14))
			{
				return;
			}

			//设置变量
			OutCardResult.CardCount = BombCard.EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, BombCard.CardData[Index], BombCard.EachHandCardCount[Index] * sizeof(BYTE));

			return;
		}
		return;
	}
	return;
}

//地主下家（先出牌）
void robotAI::UndersideOfBankerOutCard(BYTE MeChairID, tagOutCardResult &OutCardResult)
{
	//零下标没用
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	BYTE FriendID = (MeChairID + 1) % GAME_PLAYER;

	//分析所有人的牌
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
		//队友只剩下一手牌
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

		//得地主没有炸弹才行
		if (BankerOutCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			//如果队友有最大的炸弹以及一手牌，且地主的拍数量大于1，则自己出个最大的单牌
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

			//如果队友有比庄家更大的单牌和另一手（单/对/组）牌
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

			//如果队友有比庄家更大的对牌和另一手（单/对/组）牌
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

	//如果有顺牌和单只或一对，而且单只或对比地主的小，则先出顺
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

		//如果是顺牌加单牌，先出顺牌
		if (LineCardCount + 1 == m_UserCardCount[MeChairID] && CT_SINGLE_LINE == GetCardType(LineCard, LineCardCount))
		{
			OutCardResult.CardCount = LineCardCount;
			CopyMemory(OutCardResult.ResultCard, LineCard, LineCardCount);
		}
		//如果是顺牌加对牌，先出顺牌
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

		//如果三顺牌加单牌，先出顺牌
		if (ThreeLineCardCount + 1 == m_UserCardCount[MeChairID] && CT_THREE_LINE == GetCardType(ThreeLineCard, ThreeLineCardCount))
		{
			OutCardResult.CardCount = ThreeLineCardCount;
			CopyMemory(OutCardResult.ResultCard, ThreeLineCard, ThreeLineCardCount);
		}
		//如果三顺牌加对牌，先出顺牌
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

		//如果双顺加单牌，先出顺牌
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

		//双王炸弹和一手
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

	//对王加一只
	if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4F;
		OutCardResult.ResultCard[1] = 0x4E;
		return;
	}
	//对王
	else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
	{
		OutCardResult.CardCount = 2;
		OutCardResult.ResultCard[0] = 0x4F;
		OutCardResult.ResultCard[1] = 0x4E;
		return;
	}
	//只剩一手牌
	else if (CT_ERROR != GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID]))
	{
		OutCardResult.CardCount = m_UserCardCount[MeChairID];
		CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID], m_UserCardCount[MeChairID]);
		return;
	}

	//只剩（一张或一对）和一手
	if (m_UserCardCount[MeChairID] >= 2)
	{
		//地主扑克
		tagOutCardTypeResult BankerCanOutCardType_ES[CT_NUM];
		ZeroMemory(BankerCanOutCardType_ES, sizeof(BankerCanOutCardType_ES));
		tagOutCardTypeResult BankerCanOutCardType_EB[CT_NUM];
		ZeroMemory(BankerCanOutCardType_EB, sizeof(BankerCanOutCardType_EB));

		//本家去掉最小的牌后的牌组合类型
		BYTE ExceptSmallHandCardType = GetCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1);
		//本家去掉最大的牌后的牌组合类型
		BYTE ExceptBigHandCardType = GetCardType(m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);

		//地主可以出的牌
		if (ExceptSmallHandCardType != CT_ERROR)
		{
			AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], m_UserCardData[MeChairID], m_UserCardCount[MeChairID] - 1, BankerCanOutCardType_ES);
		}
		if (ExceptBigHandCardType != CT_ERROR)
		{
			AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1, BankerCanOutCardType_EB);
		}

		//除去最小单牌后的一手牌，如果不是4带1或者，4带2，且地主没有相同类型的大牌和炸时才出
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

		//去除最大单牌后的一手牌，如果不是4带1或者4带2，且地主没有相同类型的大牌时，出这手牌
		if (ExceptBigHandCardType != CT_ERROR &&
			ExceptBigHandCardType != CT_FOUR_TAKE_ONE &&
			ExceptBigHandCardType != CT_FOUR_TAKE_TWO &&
			BankerCanOutCardType_EB[ExceptBigHandCardType].CardTypeCount == 0)
		{
			OutCardResult.CardCount = m_UserCardCount[MeChairID] - 1;
			CopyMemory(OutCardResult.ResultCard, m_UserCardData[MeChairID] + 1, m_UserCardCount[MeChairID] - 1);
			return;
		}

		//我方单牌比地主要大，出最大
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
		//分析扑克
		tagOutCardTypeResult MeOutCardTypeResult[CT_NUM];
		ZeroMemory(MeOutCardTypeResult, sizeof(MeOutCardTypeResult));
		AnalyseOutCardType(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], MeOutCardTypeResult);

		//对家扑克
		WORD wFriendID;
		for (WORD wChairID = 0; wChairID<GAME_PLAYER; ++wChairID)
			if (wChairID != m_BankerUser && MeChairID != wChairID) wFriendID = wChairID;

		//计算单牌
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

		//除炸弹外的牌
		for (int CardType = CT_DOUBLE; CardType < CT_BOMB_CARD; CardType++)
		{
			tagOutCardTypeResult const &tmpCardResult = MeOutCardTypeResult[CardType];

			//相同牌型，相同长度，单连，对连等相同牌型可能长度不一样
			BYTE ThisHandCardCount = MAX_COUNT;

			//地主扑克
			tagOutCardTypeResult BankerCanOutCard[CT_NUM];
			ZeroMemory(BankerCanOutCard, sizeof(BankerCanOutCard));

			tagOutCardTypeResult FriendOutCardTypeResult[CT_NUM];
			ZeroMemory(FriendOutCardTypeResult, sizeof(FriendOutCardTypeResult));

			for (BYTE i = 0; i < tmpCardResult.CardTypeCount; ++i)
			{
				//不拆炸弹
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(tmpCardResult.CardData[i][0])) == 4)
				{
					continue;
				}

				BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i]);

				//重新分析
				if (tmpCardResult.EachHandCardCount[i] != ThisHandCardCount)
				{
					AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser],
						tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], BankerCanOutCard);
					AnalyseOutCardType(m_UserCardData[wFriendID], m_UserCardCount[wFriendID],
						tmpCardResult.CardData[i], tmpCardResult.EachHandCardCount[i], FriendOutCardTypeResult);
				}

				//地主是否可以走掉，这里都没有考虑炸弹
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

				//要出的这手牌里是否包含了大牌
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

				//搜索MinSingleCardCount[4]的最大值
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

				//保存最小值
				if (MinSingleCountInFour >= TmpCount)
				{
					//最小牌型
					MinCardType = CardType;
					//最小牌型中的最小单牌
					MinSingleCountInFour = TmpCount;
					//最小牌型中的最小牌
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

			//选择最小牌
			for (BYTE i = 0; i < 4; ++i)
			{
				if (OutcardType[i] == MinCardType &&
					MinSingleCardCount[i] <= MinSingleCountInFour &&
					GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[NumIndex[i]][0])<GetCardLogicValue(MeOutCardTypeResult[MinCardType].CardData[Index][0]))
				{
					Index = NumIndex[i];
				}
			}

			//对王加一只
			if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			//对王
			else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
			{
				OutCardResult.CardCount = 2;
				OutCardResult.ResultCard[0] = 0x4F;
				OutCardResult.ResultCard[1] = 0x4E;
				return;
			}
			else
			{
				//设置变量
				OutCardResult.CardCount = MeOutCardTypeResult[MinCardType].EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, MeOutCardTypeResult[MinCardType].CardData[Index], MeOutCardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
				return;
			}

			return;
		}

		//如果地主扑克少于5，还没有找到适合的牌则从大出到小
		if (OutCardResult.CardCount <= 0 && m_UserCardCount[m_BankerUser] <= 5)
		{
			//只有一张牌时不能放地主走
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
	//过滤王和2
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

	//计算单牌
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

	//除炸弹外的牌
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

			//搜索MinSingleCardCount[4]的最大值
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

			//保存最小值
			if (MinSingleCountInFour >= TmpCount)
			{
				//最小牌型
				MinCardType = CardType;
				//最小牌型中的最小单牌
				MinSingleCountInFour = TmpCount;
				//最小牌型中的最小牌
				MinIndex = i;
			}
		}
	}

	if (MinSingleCountInFour != MAX_COUNT)
	{
		BYTE Index = MinIndex;

		//选择最小牌
		for (BYTE i = 0; i<4; ++i)
		{
			if (OutcardType[i] == MinCardType && 
				MinSingleCardCount[i] <= MinSingleCountInFour &&
				GetCardLogicValue(CardTypeResult[MinCardType].CardData[NumIndex[i]][0]) < GetCardLogicValue(CardTypeResult[MinCardType].CardData[Index][0]))
			{
				Index = NumIndex[i];
			}
		}

		//对王加一只
		if (m_UserCardCount[MeChairID] == 3 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		//对王
		else if (m_UserCardCount[MeChairID] == 2 && GetCardColor(m_UserCardData[MeChairID][0]) == 0x40 && GetCardColor(m_UserCardData[MeChairID][1]) == 0x40)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return;
		}
		else
		{
			//设置变量
			OutCardResult.CardCount = CardTypeResult[MinCardType].EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[MinCardType].CardData[Index], CardTypeResult[MinCardType].EachHandCardCount[Index] * sizeof(BYTE));
			return;
		}

		return;
	}
	//如果只剩炸弹
	else if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0)
	{
		BYTE Index = 0;
		BYTE LogicCardValue = GetCardLogicValue(0x4F) + 1;
		//最小炸弹
		for (BYTE i = 0; i < CardTypeResult[CT_BOMB_CARD].CardTypeCount; ++i)
		{
			if (LogicCardValue > GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]))
			{
				LogicCardValue = GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[i][0]);
				Index = i;
			}
		}
		//设置变量
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

		//如果都没有搜索到就出最小的一张
		OutCardResult.CardCount = 1;
		OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][m_UserCardCount[MeChairID] - 1];
	}
	
	return;
}

//地主下家（后出牌）
void robotAI::UndersideOfBankerOutCard(BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult)
{
	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//零下标没用
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	//出牌类型
	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);

	//搜索可出牌
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
		//如果是单牌，先判断偏家是否有人只剩一张牌了，如果是，则出最大的单牌，如果单牌压不过则出炸弹
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

		//如果是对牌，先判断偏家是否有人只剩对牌了，如果是，则出最大的对牌，如果对牌压不过则出炸弹
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

	//配合队友打
	{
		tagOutCardTypeResult FriendCardTypeResult[CT_NUM];
		ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult));
		tagOutCardTypeResult BankerCardTypeResult[CT_NUM];
		ZeroMemory(BankerCardTypeResult, sizeof(BankerCardTypeResult));

		AnalyseOutCardType(m_UserCardData[FriendID], m_UserCardCount[FriendID], FriendCardTypeResult);
		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerCardTypeResult);

		bool CanRun = false;

		//如果队友有比庄家更大的单牌和另一手（单/对/组）牌
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

		//如果队友有比庄家更大的对牌和另一手（单/对/组）牌
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

		//队友可跑，庄家没有炸弹
		if (OutCardUser == m_BankerUser &&
			(OutCardType == CT_SINGLE || OutCardType == CT_DOUBLE) &&
			CanRun &&
			BankerCardTypeResult[CT_BOMB_CARD].CardTypeCount == 0)
		{
			//当前要跟的牌型，自己能出的比地主更大时，出最大的
			if (CardTypeResult[OutCardType].CardTypeCount > 0 &&
				BankerOutCardTypeResult[OutCardType].CardTypeCount > 0 &&
				GetCardLogicValue(CardTypeResult[OutCardType].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[OutCardType].CardData[0][0]))
			{
				memcpy(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[0], CardTypeResult[OutCardType].EachHandCardCount[0]);
				OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[0];
				return;
			}
			//地主压不了，则自己也不用出
			if (BankerOutCardTypeResult[OutCardType].CardTypeCount == 0)
			{
				return;
			}
			//地主可以压，且自己有炸，则出炸弹
			if (CardTypeResult[CT_BOMB_CARD].CardTypeCount > 0 &&
				BankerOutCardTypeResult[OutCardType].CardTypeCount >= 0)
			{
				OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0];
				memcpy(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData[0], CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0]);
				return;
			}
		}
	}

	//取出四个最小单牌
	BYTE MinSingleCardCount[4];
	MinSingleCardCount[0] = MAX_COUNT;
	MinSingleCardCount[1] = MAX_COUNT;
	MinSingleCardCount[2] = MAX_COUNT;
	MinSingleCardCount[3] = MAX_COUNT;
	BYTE NumIndex[4] = { 0 };
	BYTE MinSingleCountInFour = MAX_COUNT;

	//可出扑克（这里已经过滤掉炸弹了）
	tagOutCardTypeResult const &CanOutCard = CardTypeResult[OutCardType];

	for (BYTE i = 0; i < CanOutCard.CardTypeCount; ++i)
	{
		//不拆炸弹
		if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[i][0])) == 4)
		{
			continue;
		}

		//最小单牌
		BYTE TmpCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], CanOutCard.CardData[i], CanOutCard.EachHandCardCount[i]);
		BYTE MaxValue = 0;
		BYTE Index = 0;

		//搜索MinSingleCardCount[4]的最大值
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
	
	//原始单牌数
	BYTE OriginSingleCardCount = AnalyseSingleCardCount(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], NULL, 0);

	//朋友出牌
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
				
				//不拆炸弹
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[Index][0])) == 4)
				{
					continue;
				}

				//小于J的牌，或者小于A而且是散牌(逻辑与的优先级比逻辑或要高)
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
					//搜索有没有大牌（针对飞机带翅膀后面的带牌）
					bool bNoLargeCard = true;
					for (BYTE k = 3; k < CanOutCard.EachHandCardCount[Index]; ++k)
					{
						//有大牌而且不能一次出完
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
				//设置变量
				OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
				CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

				return;
			}
			//手上少于五张牌
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
					//设置变量
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
	//地主出牌
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

				//不拆炸弹
				if (FindNumSV(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], GetCardLogicValue(CanOutCard.CardData[Index][0])) == 4)
				{
					continue;
				}

				if (MinSingleCardCount[i] < OriginSingleCardCount + 3 &&
					MinSingleCardCount[i] <= MinSingleCountInFour &&
					MinLogicCardValue > GetCardLogicValue(CanOutCard.CardData[Index][0]))
				{
					//针对大牌
					bool bNoLargeCard = true;

					//当地主手上牌数大于4，而且地主出的是小于K的牌而且不是地主手上的最大牌时，不能出2去打
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

					//搜索有没有大牌（针对飞机带翅膀后面的带牌）
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
				//地主的最大牌
				BYTE LargestLogicCard = GetCardLogicValue(m_UserCardData[m_BankerUser][0]);
				bool bCanOut = true;

				//王只压2
				if (GetCardLogicValue(TurnCardData[0]) < LargestLogicCard)
				{
					if (GetCardColor(CanOutCard.CardData[CanOutIndex][0]) == 0x40 && GetCardLogicValue(TurnCardData[0]) <= 14 && m_UserCardCount[MeChairID] > 5)
					{
						bCanOut = false;
					}
				}

				if (bCanOut)
				{
					//设置变量
					OutCardResult.CardCount = CanOutCard.EachHandCardCount[CanOutIndex];
					CopyMemory(OutCardResult.ResultCard, CanOutCard.CardData[CanOutIndex], CanOutCard.EachHandCardCount[CanOutIndex] * sizeof(BYTE));

					return;
				}
			}

			if (OutCardType == CT_SINGLE)
			{
				//地主的最大牌
				BYTE LargestLogicCard = GetCardLogicValue(m_UserCardData[m_BankerUser][0]);

				if (GetCardLogicValue(TurnCardData[0]) == 14 ||
					GetCardLogicValue(TurnCardData[0]) >= LargestLogicCard ||
					(GetCardLogicValue(TurnCardData[0])<LargestLogicCard - 1) || 
					m_UserCardCount[m_BankerUser] <= 5)
				{
					//取一张大于等于2而且要比地主出的牌大的牌，
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
						//设置变量
						OutCardResult.CardCount = 1;
						OutCardResult.ResultCard[0] = m_UserCardData[MeChairID][Index];

						return;
					}
				}
				//跟单牌时，不能过于早拆王或出王
				if (GetCardLogicValue(CanOutCard.CardData[CanOutCard.CardTypeCount - 1][0]) >= 16 && m_UserCardCount[MeChairID] > 5)
				{
					return;
				}
			}

			//当朋友不能拦截地主时
			WORD wMeChairID = (m_BankerUser + 1) % GAME_PLAYER;
			WORD wFriendID = (wMeChairID + 1) % GAME_PLAYER;

			tagOutCardTypeResult FriendCardTypeResult[13];
			ZeroMemory(FriendCardTypeResult, sizeof(FriendCardTypeResult));
			AnalyseOutCardType(m_UserCardData[wFriendID], m_UserCardCount[wFriendID], TurnCardData, TurnCardCount, FriendCardTypeResult);

			//当朋友不能拦截地主时
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
				//设置变量
				OutCardResult.CardCount = CardTypeResult[OutCardType].EachHandCardCount[Index];
				CopyMemory(OutCardResult.ResultCard, CardTypeResult[OutCardType].CardData[Index], OutCardResult.CardCount);

				return;
			}
		}

		//还要考虑炸弹
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

			//判断出了炸弹后的单牌数
			BYTE SingleCardCount = AnalyseSingleCardCountS(m_UserCardData[MeChairID], m_UserCardCount[MeChairID], BombCard.CardData[Index], BombCard.EachHandCardCount[Index]);
			if (m_UserCardCount[MeChairID] > 7 || SingleCardCount >= 3 || (OutCardType == CT_SINGLE && GetCardLogicValue(TurnCardData[0]) < 15))
			{
				return;
			}

			//设置变量
			OutCardResult.CardCount = BombCard.EachHandCardCount[Index];
			CopyMemory(OutCardResult.ResultCard, BombCard.CardData[Index], BombCard.EachHandCardCount[Index] * sizeof(BYTE));

			return;
		}
		return;
	}
	return;
}

/*
函数功能：判断是否可以一手出完或压制出完
HandCardData:出牌玩家手中的牌
HandCardCount:出牌玩家手中的牌数量
OutCardUser:控手玩家的ID（可以是自己，也可以是别人)
TurnCardData:控手牌数据
TurnCardCount:控手牌数量
tagOutCardResult:出牌玩家需要出的牌
*/
bool robotAI::IsCanOneHandFollow(const BYTE HandCardData[], BYTE HandCardCount, BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult)
{
	if (HandCardCount == 0)
	{
		return true;
	}

	//初始变量
	ZeroMemory(&OutCardResult, sizeof(OutCardResult));

	//零下标没用
	tagOutCardTypeResult CardTypeResult[CT_NUM];
	ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

	//出牌类型
	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);

	//控手者是他人,控手者是农民,自己是地主
	if (OutCardUser != m_BankerUser && MeChairID == m_BankerUser)
	{
		BYTE UndersideUser = (m_BankerUser + 1) % GAME_PLAYER;
		BYTE UpsideUser = (UndersideUser + 1) % GAME_PLAYER;

		//搜索可出牌
		tagOutCardTypeResult UndersideOutCardTypeResult[CT_NUM];
		tagOutCardTypeResult UpsideOutCardTypeResult[CT_NUM];
		ZeroMemory(UndersideOutCardTypeResult, sizeof(UndersideOutCardTypeResult));
		ZeroMemory(UpsideOutCardTypeResult, sizeof(UpsideOutCardTypeResult));

		AnalyseOutCardType(m_UserCardData[UndersideUser], m_UserCardCount[UndersideUser], UndersideOutCardTypeResult);
		AnalyseOutCardType(m_UserCardData[UpsideUser], m_UserCardCount[UpsideUser], UpsideOutCardTypeResult);
		AnalyseOutCardType(HandCardData, HandCardCount, TurnCardData, TurnCardCount, CardTypeResult);

		//只剩炸弹
		if (HandCardCount == CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0])
		{
			OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData, OutCardResult.CardCount);
			return true;
		}
		//只剩王炸
		if (HandCardCount == 2 && HandCardData[0] == 0x4F && HandCardData[1] == 0x4E)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return true;
		}
		//双王炸弹和一手
		else if (HandCardCount > 2 && HandCardData[0] == 0x4F && HandCardData[1] == 0x4E && GetCardType(HandCardData + 2, HandCardCount - 2) != CT_ERROR)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return true;
		}
		//如果跟的牌最大，偏家没有炸弹，且跟完后只有一手牌，则出
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
	//控手者是他人,控手者是地主，自己是农民
	else
	{
		//搜索可出牌
		tagOutCardTypeResult BankerOutCardTypeResult[CT_NUM];
		ZeroMemory(BankerOutCardTypeResult, sizeof(BankerOutCardTypeResult));

		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], BankerOutCardTypeResult);
		AnalyseOutCardType(HandCardData, HandCardCount, TurnCardData, TurnCardCount, CardTypeResult);

		BYTE FriendID = (m_BankerUser + 1) % GAME_PLAYER;

		//只剩炸弹
		if (HandCardCount == CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0])
		{
			OutCardResult.CardCount = CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0];
			CopyMemory(OutCardResult.ResultCard, CardTypeResult[CT_BOMB_CARD].CardData, OutCardResult.CardCount);

			return true;
		}
		//只剩王炸
		if (HandCardCount == 2 && HandCardData[0] == 0x4F && HandCardData[1] == 0x4E)
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;

			return true;
		}
		//双王炸弹和一手
		else if (HandCardCount > 2 && HandCardData[0] == 0x4F && HandCardData[1] == 0x4E && CT_ERROR != GetCardType(HandCardData + 2, HandCardCount - 2))
		{
			OutCardResult.CardCount = 2;
			OutCardResult.ResultCard[0] = 0x4F;
			OutCardResult.ResultCard[1] = 0x4E;
			return true;
		}
		//如果跟的牌最大，偏家没有炸弹，且跟完后只有一手牌，则出
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

//自己出牌后判断，不是跟牌的
bool robotAI::IsCanOneHand(const BYTE HandCardData[], BYTE HandCardCount, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser)
{
	//零下标没用
	BYTE TmpHandData[MAX_COUNT] = { 0 };
	BYTE TmpHandLen = HandCardCount;
	memcpy(TmpHandData, HandCardData, HandCardCount);
	RemoveCards(TurnCardData, TurnCardCount, TmpHandData, TmpHandLen);
	TmpHandLen -= TurnCardCount;

	//出牌类型
	BYTE OutCardType = GetCardType(TurnCardData, TurnCardCount);

	//自己是农民
	if (OutCardUser != m_BankerUser)
	{
		//搜索可出牌
		tagOutCardTypeResult *CardTypeResult = new tagOutCardTypeResult[CT_NUM];
		tagOutCardTypeResult *BankerOutCardTypeResult = new tagOutCardTypeResult[CT_NUM];

		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], TurnCardData, TurnCardCount, BankerOutCardTypeResult);
		AnalyseOutCardType(TmpHandData, TmpHandLen, TurnCardData, TurnCardCount, CardTypeResult);

		BYTE FriendID = (m_BankerUser + 1) % GAME_PLAYER;

		//只剩炸弹
		if (TmpHandLen == CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0] && GetCardLogicValue(CardTypeResult[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(BankerOutCardTypeResult[CT_BOMB_CARD].CardData[0][0]))
		{
			delete[] CardTypeResult;
			delete[] BankerOutCardTypeResult;
			return true;
		}
		//只剩王炸
		if (TmpHandLen == 2 && TmpHandData[0] == 0x4F && TmpHandData[1] == 0x4E)
		{
			delete[] CardTypeResult;
			delete[] BankerOutCardTypeResult;
			return true;
		}
		//双王炸弹和一手
		if (TmpHandLen > 2 && TmpHandData[0] == 0x4F && TmpHandData[1] == 0x4E && CT_ERROR != GetCardType(TmpHandData + 2, TmpHandLen - 2))
		{
			delete[] CardTypeResult;
			delete[] BankerOutCardTypeResult;
			return true;
		}

		//如果是自己出的牌型自己能回，偏家没有炸弹，且回完后只有一手牌（或有一个手绝对大的牌带一手小牌）
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
	//自己是地主
	else
	{
		BYTE UndersideUser = (m_BankerUser + 1) % GAME_PLAYER;
		BYTE UpsideUser = (UndersideUser + 1) % GAME_PLAYER;

		//搜索可出牌
		tagOutCardTypeResult *CardTypeResult = new tagOutCardTypeResult[CT_NUM];
		tagOutCardTypeResult *UndersideOutCardTypeResult = new tagOutCardTypeResult[CT_NUM];
		tagOutCardTypeResult *UpsideOutCardTypeResult = new tagOutCardTypeResult[CT_NUM];
		ZeroMemory(UndersideOutCardTypeResult, sizeof(UndersideOutCardTypeResult));
		ZeroMemory(UpsideOutCardTypeResult, sizeof(UpsideOutCardTypeResult));
		ZeroMemory(CardTypeResult, sizeof(CardTypeResult));

		AnalyseOutCardType(m_UserCardData[UndersideUser], m_UserCardCount[UndersideUser], TurnCardData, TurnCardCount, UndersideOutCardTypeResult);
		AnalyseOutCardType(m_UserCardData[UpsideUser], m_UserCardCount[UpsideUser], TurnCardData, TurnCardCount, UpsideOutCardTypeResult);
		AnalyseOutCardType(TmpHandData, TmpHandLen, TurnCardData, TurnCardCount, CardTypeResult);

		//只剩炸弹
		if (TmpHandLen == CardTypeResult[CT_BOMB_CARD].EachHandCardCount[0])
		{
			delete[] CardTypeResult;
			delete[] UndersideOutCardTypeResult;
			delete[] UpsideOutCardTypeResult;
			return true;
		}
		//只剩王炸
		if (TmpHandLen == 2 && TmpHandData[0] == 0x4F && TmpHandData[1] == 0x4E)
		{
			delete[] CardTypeResult;
			delete[] UndersideOutCardTypeResult;
			delete[] UpsideOutCardTypeResult;
			return true;
		}
		//双王炸弹和一手
		if (TmpHandLen > 2 && TmpHandData[0] == 0x4F && TmpHandData[1] == 0x4E && GetCardType(TmpHandData + 2, TmpHandLen - 2) != CT_ERROR)
		{
			delete[] CardTypeResult;
			delete[] UndersideOutCardTypeResult;
			delete[] UpsideOutCardTypeResult;
			return true;
		}
		//如果是自己出的牌型自己能回，偏家没有炸弹，且回完后只有一手牌（或有一个手绝对大的牌带一手小牌）
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

//出牌后是否可以回牌
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

	//从手牌中去掉要出的牌型
	BYTE HandCard[MAX_COUNT] = { 0 };
	BYTE HandCardLen = m_UserCardCount[ChairID];
	memcpy(HandCard, m_UserCardData[ChairID], m_UserCardCount[ChairID]);
	RemoveCards(TurnCardData, TurnCardCount, HandCard, HandCardLen);
	HandCardLen -= TurnCardCount;

	//如果出牌者是地主
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

		//农民能跟牌
		if (FarmerCardType1[OutCardType].CardTypeCount > 0 || FarmerCardType2[OutCardType].CardTypeCount > 0 ||
			FarmerCardType1[CT_BOMB_CARD].CardTypeCount > 0 || FarmerCardType2[CT_BOMB_CARD].CardTypeCount > 0)
		{
			//地主没有相应牌型
			if (BankerCardType[OutCardType].CardTypeCount == 0)
			{
				//地主的炸弹能压制
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
			//地主有相应的牌型
			else
			{
				//地主没有炸弹
				if (BankerCardType[CT_BOMB_CARD].CardTypeCount == 0)
				{
					//农民都没有炸弹且地主的牌比他们都大时，能压制
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
					//地主的炸弹比他们都大,能压制
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
			//农民都要不起
			return true;
		}
	}
	//如果出牌者是农民
	else
	{
		tagOutCardTypeResult MeFarmerCardType[CT_NUM];
		tagOutCardTypeResult BankerCardType[CT_NUM];
		ZeroMemory(MeFarmerCardType, sizeof(MeFarmerCardType));
		ZeroMemory(BankerCardType, sizeof(BankerCardType));

		AnalyseOutCardType(m_UserCardData[ChairID], m_UserCardCount[ChairID], TurnCardData, TurnCardCount, MeFarmerCardType);
		AnalyseOutCardType(m_UserCardData[m_BankerUser], m_UserCardCount[m_BankerUser], TurnCardData, TurnCardCount, BankerCardType);

		//如果地主可以跟牌
		if (BankerCardType[OutCardType].CardTypeCount > 0 || BankerCardType[CT_BOMB_CARD].CardTypeCount > 0)
		{
			//如果农民没有相应的牌型
			if (MeFarmerCardType[OutCardType].CardTypeCount == 0)
			{
				//如果农民的炸弹能压制
				if (GetCardLogicValue(MeFarmerCardType[CT_BOMB_CARD].CardData[0][0]) > GetCardLogicValue(BankerCardType[CT_BOMB_CARD].CardData[0][0]))
				{
					return true;
				}
				//农民没有相应的牌型也没有炸弹
				else
				{
					return false;
				}
			}
			//农民有相应的牌型
			else
			{
				//农民没有炸弹
				if (MeFarmerCardType[CT_BOMB_CARD].CardTypeCount == 0)
				{
					//地主没有炸弹且自己的牌比他都大时，能压制
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
					//农民的炸弹比地主大,能压制
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
		//地主跟不了牌，能压制
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

	//分析扑克
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