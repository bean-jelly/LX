#ifndef GAME_ROBOT_AI_H_
#define GAME_ROBOT_AI_H_

#include "basiccode/utility/BasicCriticalSection.h"


typedef unsigned char BYTE;

#define CountArray(a) (sizeof(a) / sizeof(*(a)))

#define MAX_COUNT					20									//�����Ŀ
#define MAX_TYPE_COUNT				200
#define FULL_COUNT					54									//ȫ����Ŀ
#define ST_ORDER					0									//��С����
#define ST_COUNT					1									//��Ŀ����
#define ST_CUSTOM					2									//�Զ�����
#define GAME_PLAYER					3									//��Ϸ����

//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//�߼�����
enum CT
{
	CT_ERROR			= 0,									//��������
	CT_SINGLE			= 1,									//��������
	CT_DOUBLE			= 2,									//��������
	CT_THREE			= 3,									//��������
	CT_SINGLE_LINE		= 4,									//��������
	CT_DOUBLE_LINE		= 5,									//��������
	CT_THREE_LINE		= 6,									//��������
	CT_THREE_TAKE_ONE	= 7,									//����һ��
	CT_THREE_TAKE_TWO	= 8,									//����һ��
	CT_FOUR_TAKE_ONE	= 9,									//�Ĵ�����
	CT_FOUR_TAKE_TWO	= 10,									//�Ĵ�����
	CT_BOMB_CARD		= 11,									//ը������
	CT_MISSILE_CARD		= 12,									//�������
	CT_NUM				= 13
};

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							BlockCount[4];						//�˿���Ŀ
	BYTE							CardData[4][MAX_COUNT];				//�˿�����
	tagAnalyseResult()
	{
		memset(this, 0, sizeof(tagAnalyseResult));
	}
};

//���ƽ��
struct tagOutCardResult
{
	BYTE							CardCount;							//�˿���Ŀ
	BYTE							ResultCard[MAX_COUNT];				//����˿�
	tagOutCardResult()
	{
		memset(this, 0, sizeof(tagOutCardResult));
	}
};

//�ֲ���Ϣ
struct tagDistributing
{
	BYTE							CardCount;							//�˿���Ŀ
	BYTE							Distributing[15][6];				//�ֲ���Ϣ
	tagDistributing()
	{
		memset(this, 0, sizeof(tagDistributing));
	}
};

struct tagOutCardTypeResult
{
	BYTE							CardType;							//�˿�����
	BYTE							CardTypeCount;						//������Ŀ
	BYTE							EachHandCardCount[MAX_TYPE_COUNT];	//ÿ�ָ���
	BYTE							CardData[MAX_TYPE_COUNT][MAX_COUNT];//�˿�����
	tagOutCardTypeResult()
	{
		memset(this, 0, sizeof(tagOutCardTypeResult));
	}
};

class robotAI
{
public:
	BYTE							m_AllCardData[FULL_COUNT];				//�˿�����
	BYTE							m_UserCardData[GAME_PLAYER][MAX_COUNT];	//�����˿�
	BYTE							m_LandScoreCardData[MAX_COUNT];			//�����˿�
	BYTE							m_UserCardCount[GAME_PLAYER];			//�˿���Ŀ
	BYTE							m_AllCardCount;							//ʣ���˿���Ŀ
	BYTE							m_BankerUser;							//�������
	BYTE							m_LanScoreCurrent;						//��ǰ�з�
public:
	robotAI();
	virtual ~robotAI();
	void init();
public:
	//��ȡ����
	BYTE GetCardType(const BYTE CardData[], BYTE CardCount);
	//get mask-value
	BYTE GetCardValue(BYTE CardData){ return CardData&MASK_VALUE; }
	//get mask-color
	BYTE GetCardColor(BYTE CardData){ return CardData&MASK_COLOR; }
public:
	//sort desc
	void SortCardList(BYTE CardData[], BYTE CardCount, BYTE SortType);
	//remove list
	bool RemoveCards(const BYTE RemoveCard[], BYTE RecmoveCount, BYTE CardData[], BYTE CardCount);
public:
	//check card
	bool IsValidCard(BYTE CardData);
	//get value
	BYTE GetCardLogicValue(BYTE CardData);
	//compare pokers
	bool CompareCard(const BYTE FirstCard[], const BYTE NextCard[], BYTE FirstCount, BYTE NextCount);
	//search out cards
	bool SearchOutCard(const BYTE HandCardData[], BYTE HandCardCOunt, const BYTE TurnCardData[], BYTE TurnCardCount, tagOutCardResult &OutCardResult);
	//search out card
	bool SearchOutCard(const BYTE HandCardData[], BYTE HandCardCount, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, BYTE MeChairID, tagOutCardResult &OutCardResult);
public:
	//analyse handcard out struct tagAnalyseResult
	void AnalysebCardData(const BYTE CardData[], BYTE CardCount, tagAnalyseResult &AnalyseResult);
	//analyse handcard's distribute
	void AnalysebDistributing(const BYTE CardData[], BYTE CardCount, tagDistributing& Distributing);
public:
	//set handcard
	void SetUserCard(BYTE ChairID, BYTE CardData[], BYTE CardCount);
	//set backcard
	void SetBackCard(BYTE ChairID, BYTE BackCardData[], BYTE CardCount);
	//set banker
	void SetBanker(BYTE Banker);
	//remove cards
	void RemoveUserCardData(BYTE ChairID, BYTE RemoveCardData[], BYTE RemoveCardCount);
public:
	//combination algorithm
	void Combination(BYTE CombineCardData[], BYTE ResComLen, BYTE ResultCardData[MAX_TYPE_COUNT][5], BYTE &ResCardLen, BYTE SrcCardData[], BYTE SrcLen, BYTE CombineLen1, BYTE CombineLen2);
	//list algorithm
	void Permutation(BYTE *list, int m, int n, BYTE result[][4], BYTE &len);
	//get all bomb from carddata
	void GetAllBombCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE BombCardData[], BYTE &BombCardCount);
	//get all seq from carddata
	void GetAllLineCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE LineCardData[], BYTE &LineCardCount);
	//get all three from carddata
	void GetAllThreeCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE ThreeCardData[], BYTE &ThreeCardCount);
	//get all double from carddata
	void GetAllDoubleCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE DoubleCardData[], BYTE &DoubleCardCount);
	//get all single from carddata
	void GetAllSingleCard(const BYTE HandCardData[], const BYTE HandCardCount, BYTE SingleCardData[], BYTE &SingleCardCount);
	//get num of type
	BYTE GetNumOfType(const BYTE HandCardData[], const BYTE HandCardCount, BYTE CardType);
public:
	//analyse out,follow
	void AnalyseOutCardType(const BYTE HandCarData[], const BYTE HandCardCount, const BYTE TurnCardData[], const BYTE TurnCardCount, tagOutCardTypeResult CardTypeResult[CT_NUM]);
	//analyse out
	void AnalyseOutCardType(const BYTE HandCardData[], const BYTE HandCardCount, tagOutCardTypeResult CardTypeResult[CT_NUM]);
	//analyse single num(extract cards with some type)
	BYTE AnalyseSingleCardCount(const BYTE HandCardData[], const BYTE HandCardCount, const BYTE WantOutCardData[], const BYTE WantOutCardCount, BYTE SingleCardData[] = NULL);
	//analyse small single num(extract cards with some type and single card bigger than K)
	BYTE AnalyseSingleCardCountS(const BYTE HandCardData[], const BYTE HandCardCount, const BYTE WantOutCardData[], const BYTE WantOutCardCount);
public:
	//banker out first
	void BankerOutCard(BYTE MeChairID, tagOutCardResult &OutCardResult);
	//banker out follow
	void BankerOutCard(BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult);
	//upside banker out first
	void UpsideOfBankerOutCard(BYTE MeChairID, tagOutCardResult &OutCardResult);
	//upside banker out follow
	void UpsideOfBankerOutCard(BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult);
	//under side banker out first
	void UndersideOfBankerOutCard(BYTE MeChairID, tagOutCardResult &OutCardResult);
	//under side banker out follow
	void UndersideOfBankerOutCard(BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult);
public:
	//�Ƿ���Ը���һ�ֳ����ѹ�Ƴ���
	bool IsCanOneHandFollow(const BYTE HandCardData[], BYTE HandCardCount, BYTE MeChairID, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser, tagOutCardResult &OutCardResult);
	bool IsCanOneHand(const BYTE HandCardData[], BYTE HandCardCount, const BYTE TurnCardData[], BYTE TurnCardCount, BYTE OutCardUser);
	//���ƺ��Ƿ���Ի���
	bool IsCanPressCard(BYTE ChairID, const BYTE TurnCardData[], BYTE TurnCardCount);
public:
	BYTE FindNumSV(const BYTE HandCardData[], BYTE HandCardCount, BYTE CardVaue);
public:
	//�з��ж�
	BYTE LandScore(BYTE MeChairID);
};

#endif