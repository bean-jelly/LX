#pragma once 
#ifndef __MJFANCOUNT_H_
#define	__MJFANCOUNT_H_
//计算判断番型和番数

#include "mjdata.h"
#include "mjlogic.h"
 
class CMJFanCount
{
public:
	CMJFanCount();
	virtual~CMJFanCount();
	  
	//初始化数据
	void InitData();
	//初始化部分数据
	void InitPartData(); 
	//初始化逻辑
	void InitLogicParam(game_mode param, unsigned char normal_card[], unsigned int normal_len);

	//设置胡牌方式
	void SetWinType(unsigned char wintype, unsigned char wincard);
	//设置手牌和拦牌
	void SetCardData(CMJHandCard  handcard,CMJBlockCard	blockcard);
	//设置牌墙
	void SetHaiDi(bool hai_di);
	//设置需要检测的番型
	void SetCheckFanType(unsigned int checktype[], unsigned int checkvalue[],unsigned int checkcount);

	//检测番型
	int CheckFanType();
	//获取番型
	int	GetFanCount();
	//获取最大番数
	unsigned int GetMaxFan(int &maxType);
	//获取最大番型
	unsigned int GetMaxFanType();
	//获取番型类型
	void GetFanType(bool fantype[], unsigned int fancount);
	
	//获取花色
	int	GetCardColor(unsigned char cardvalue); 
	//判断是否是1，9
	bool IsOneOrNine(unsigned char cardvalue);
	//判断是否是字牌
	bool IsZiCard(unsigned char cardvalue);

	virtual bool CheckWinType(unsigned int type);
	virtual bool Check00();
	virtual bool Check01();
	virtual bool Check02();
	virtual bool Check03();
	virtual bool Check04();
	virtual bool Check05();
	virtual bool Check06();
	virtual bool Check07();
	virtual bool Check08();
	virtual bool Check09();
	virtual bool Check10();
	virtual bool Check11();
	virtual bool Check12();
	virtual bool Check13();
	virtual bool Check14();
	virtual bool Check15();
	virtual bool Check16();
	virtual bool Check17();
	virtual bool Check18();
	virtual bool Check19();  
	virtual bool Check20();
	virtual bool Check21();
	virtual bool Check22();
	virtual bool Check23();
	virtual bool Check24();
	virtual bool Check25();
	virtual bool Check26();
	virtual bool Check27();
	virtual bool Check28();
	virtual bool Check29();
	virtual bool Check30();
	virtual bool Check31();
	virtual bool Check32();
	virtual bool Check33();
	virtual bool Check34();
	virtual bool Check35();
	virtual bool Check36();
	virtual bool Check37();
	virtual bool Check38();
	virtual bool Check39(); 

public: 
	CMJlogic					m_mj_logic;						//麻将逻辑
	CMJHandCard					m_handcard;						//玩家手牌
	CMJBlockCard				m_blockcard;					//玩家拦牌
	unsigned char				m_wintype;						//胡牌方式  0：荒庄   1：自摸 2：放炮  3：抢杠 4:杠开
	unsigned char				m_wincard;						//胡的牌
	bool						m_hai_di;						//是否是海底
	unsigned int				m_checktype[FAN_TYPE_COUNT];	//需要检测番型
	unsigned int				m_fanvalue[FAN_TYPE_COUNT];		//番型番数
	unsigned int				m_checkcount;					//需要检测的个数
	bool						m_bfan[FAN_TYPE_COUNT];			//番型是否成立  

};

 
#endif // !__MJFANCOUNT_H_
