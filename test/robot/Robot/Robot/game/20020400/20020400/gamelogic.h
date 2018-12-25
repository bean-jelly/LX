#pragma once
#ifndef __GAMELOGIC_H_
#define __GAMELOGIC_H_
#include "basegamelogic.h"
#include "gamelogic.h"
#include "mjlogic/mjdata.h"
#include "mjlogic/mjlogic.h"
#include "mjlogic/mjfancount.h"
#include "mjlogic/mjmessage.h"
#include "gamemsg.h"
#include "basiccode/utility/BasicCriticalSection.h"
#include "MJGameLogic.h"
#include "gamecount.h"
#ifdef _WINDOWS_
#undef _WINDOWS_
#include <afx.h>
#endif

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
	//设置红中麻将正常牌值
	void set_mj_logic();
	//玩家出牌
	void deal_user_out_card();
	//玩家拦牌
	void deal_user_block();
	//玩家胡牌(是否自摸)
	void deal_send_hu_msg();
	//玩家暗杠
	void deal_send_gang_msg();
	//玩家检测是否在听牌状态中
	bool check_in_ting(int& hu_count);
	//检查玩家碰或杠之后是否能听牌
	bool check_in_ting_after_penggang(unsigned char card_value,int& hu_count,int& has_cout);
	//检测玩家是否需要碰
	bool check_need_peng(unsigned char card_value);
	//新一局游戏初始化
	void init_for_game();

	//找到最佳出牌
	int find_best_out_card();
	//找到最佳出牌---单牌
	unsigned char find_best_out_single_card(CMJHandCard&);
	//找到最佳打法 
	unsigned char find_best_out_other_card(CMJHandCard&);

	//得到机器人自动准备需要的动画时间
	int get_ready_time(notify_game_result* ptr);

	//打印手牌
	void printhandcard(unsigned char chair_id);
	std::string findcard(unsigned char value);

private:
	//结算后准备游戏
	void deal_count_socre(char* buf, int len);

private:
	game_base_info					m_base_info;							//基本信息
	unsigned char					m_makers_station;						//庄家位置
	INT64                           m_servermoney;                          //服务费

	CMJBlockCard					m_user_block[PLAY_COUNT];				//玩家拦牌数据
	CMJOutCard						m_user_outcard[PLAY_COUNT];				//玩家出牌数据
	CMJHandCard						m_user_handcard;						//玩家手牌数据
	notify_block_info				m_cur_block_info;						//玩家拦牌数据
	int								m_has_out[CARD_INDEX_NUM];				//牌桌上已经出过的牌
	CMJGameLgic						m_logic;								//麻将逻辑
};

#endif	