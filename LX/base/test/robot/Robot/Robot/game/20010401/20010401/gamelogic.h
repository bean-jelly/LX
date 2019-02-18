#pragma once
#ifndef __GAMELOGIC_H_
#define __GAMELOGIC_H_
#include "basegamelogic.h"
#include "gamelogic.h"
#include "ppl_logic/ppl_logic.h"
#include "gamemsg.h"
#include "basiccode/utility/BasicCriticalSection.h"

#ifdef _WINDOWS_
#undef _WINDOWS_
#include <afx.h>
#endif

struct pick_record
{
	int row;
	unsigned char pick_card;
	int list_type;
	int card_num;
	pick_record()
	{
		memset(this, 0, sizeof(pick_record));
	}
};

class gamelogic :public basegamelogic
{
public:
	gamelogic();
	virtual ~gamelogic();
	virtual bool gameframe(unsigned int sub_id, char* buf, int len, unsigned int code_id);
	virtual bool gamenotify(unsigned int sub_id, char* buf, int len, unsigned int code_id);

public:
	//处理换桌
	virtual void deal_send_change_desk(unsigned int);
	virtual void kill_timer(unsigned int event_id);
	virtual void set_timer(unsigned int event_id, unsigned int elapse);
	virtual void OnTimer(unsigned int event_id, unsigned int user_id);

private:
	//读取配置文件 ini
	void read_config_ini();
	//清理数据
	void init_data();
	//玩家出牌
	void deal_user_out_card();
	//找到最佳出牌
	void find_best_out_card(unsigned char &out_card, int &row, int &col);
	//查找相同权值牌的数量
	int  find_same_value(unsigned char card);
	//查找相同花色牌的数量
	int  find_same_color(unsigned char card);
	//检查某行的摆牌是否完成，若没完成则输出随机的空格
	bool find_blank(int row, int &out_row, int &out_col);
	//检查某行的空个数
	int	 find_blank_num(int row);
	//打印手牌
	void printhandcard();
	//检测剩余牌的个数
	int CountCardNum(unsigned char card);
	//查找牌值位置
	int	check_pos(unsigned char card);

	//检查是否能组合成杂花235
	bool check_combine_235(unsigned char &out_card, int &row, int &col);
	//检查是否能组合成三条
	bool check_combine_three(unsigned char &out_card, int &row, int &col);
	//检查是否能组合成同花顺
	bool check_combine_straightflush(unsigned char &out_card, int &row, int &col);
	//检查是否能组合成同花
	bool check_combine_flush(unsigned char &out_card, int &row, int &col);
	//检查是否能组合成顺子
	bool check_combine_straight(unsigned char &out_card, int &row, int &col);
	//检查是否能组合成对子
	bool check_combine_pair(unsigned char &out_card, int &row, int &col);
	//检查是否能组合成高牌
	bool check_combine_high(unsigned char &out_card, int &row, int &col);

	std::string translate(unsigned char card);
private:
	//结算后准备游戏
	void deal_count_socre(char* buf, int len);

private:
	game_base_info					m_base_info;								//基本信息

	int								m_has_out[ALL_CARD_COUNT];					//牌桌上已经出过的牌
	unsigned char					m_hand_card[HAND_CARD_NUM];					//玩家手牌
	int								m_hand_card_num;							//手牌个数
	user_placed_card				m_placed_card[PLAY_COUNT];					//摆牌
	pick_record						m_placed_status[HAND_CARD_NUM][ROW_COUNT];	//摆牌记录
	bool							m_is_done[ROW_COUNT];						//是否完成某行的摆牌
	unsigned char					m_public_card[ROW_COUNT];					//公共牌
	int								m_public_num;								//公共牌个数
	bool							m_have_king;								//是否有癞子牌
	unsigned char					m_king_card[2];								//癞子牌
	int								m_current_turn;								//当前轮数
	CGameLogic						m_ppl_logic;								//拼拼乐逻辑
	bool							m_is_sp235[ROW_COUNT];						//是否该行有杂花235
	bool							m_is_three[ROW_COUNT];						//是否该行有3条

	int								m_time_out_card1;							//出牌最短时间
	int								m_time_out_card2;							//出牌最长时间
};

#endif	