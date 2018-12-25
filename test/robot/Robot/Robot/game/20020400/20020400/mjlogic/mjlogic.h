#pragma once
#ifndef  __MJ_LOGIC_H__
#define __MJ_LOGIC_H__

#include "mjmessage.h"
#include "mjdata.h"
class CMJlogic
{
public:
	CMJlogic();
	virtual ~CMJlogic();
	//清理数据
	bool clearpartdata();
	//是否是有效牌值
	bool is_valid_card(unsigned char card_value);

	//添加麻将正常牌值
	void add_normal_card(unsigned char card_value);

	//检测牌数据
	bool check_cardvalue(unsigned char card_value);

	//获取花色
	int get_card_color(unsigned char card_value);

	//获取牌值
	int get_card_value(unsigned char card_value);

	//牌值转换-根据花色
	int change_card_value(unsigned char card_value[], unsigned int card_count, int card_color, int card_index[]);

	//转换所有牌值
	int change_card_index(const unsigned char card_value[], unsigned int card_count, int card_index[]);
	int change_card_index(CMJHandCard handcard,  int card_index[] );
	//获取牌的个数
	int get_card_count(unsigned char hand_card_value[], unsigned int card_count, unsigned char check_card);

	//查找牌的位置
	int find_card_index(unsigned char hand_card_value[], unsigned int card_count, unsigned char check_card);

	//添加牌值
	bool add_card(unsigned char hand_card_value[], unsigned int& card_count, unsigned char add_card[], unsigned int add_count);

	//删除牌值
	bool del_card_value(unsigned char hand_card_value[], unsigned int card_count, unsigned char del_card[], unsigned int del_count);
	bool del_card_value(unsigned char hand_card_value[], unsigned int card_count, unsigned char del_card, unsigned int del_count);

	//排序
	void sort_card(unsigned char hand_card_value[], unsigned int card_count);


	//检测是否可以吃牌
	unsigned char check_can_chi(const unsigned char hand_card[], unsigned int hand_card_count, unsigned char check_card, unsigned char& chi_type, block_card_info blockinfo[]);
	bool	check_chi(CMJHandCard handcard, unsigned char check_user, unsigned char out_user, unsigned char check_card, CMJBlockCard& block_info);
	//检测是否可以碰牌
	unsigned char check_can_peng(const unsigned char hand_card[], unsigned int hand_card_count, unsigned char check_card);

	//检测是否可以杠牌--明杠
	unsigned char check_can_gang(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card);
	//检测是否可以暗杠，补杠
	unsigned char check_can_gang_self(const unsigned char hand_card[], const unsigned int hand_card_count, CMJBlockCard user_block, unsigned char check_card, unsigned char block_card[]);
	//unsigned char check_can_bu_gang(block_data block_value, unsigned char check_card);

	//检测是否可以听牌
	unsigned char check_can_ting(const unsigned char hand_card[], const unsigned int hand_card_count, TingData& ting_info, bool first_card);

	//检测是否放炮
	unsigned char check_can_win(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card, bool first_card, bool add);
	  
	//检测手牌是否听牌
	bool is_ting(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char win_card[], bool first_card);
	  
	//检测是否可以胡牌
	//bool checkwin(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);

	//判断胡
	bool checkwinnew(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);

	//检测指定牌能否组成顺子、刻子,有序的
	bool check_can_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, bool jiang);
	bool is_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount, bool jiang);
	//检测是否全是对子
	bool check_all_double(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);
	//判断两张牌是否可以组合成顺子
	bool check_comb_shun(unsigned char card_value[],unsigned int card_count); 

	//判断是否听是七对
	bool check_qidui(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);
	//判断是否胡131
	bool check_131(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);
	//判断是否胡13烂 
	bool check_qbk(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount);

	//获取索引个数
	int get_index_count(const  int card_index[],unsigned int card_count);
	//分析数据
	bool analyze_card(const  int card_index[], const unsigned int jiang_index, int  remain_index[]);

	//设置游戏模式
	void set_mode(game_mode param);
	//设置赖子牌
	void set_king_value(unsigned  char  kingcard[], unsigned int kingcount);
	//设置 赖子牌超过一定数量后可以直接胡牌
	void set_king_over_win( unsigned int kingcount );

	//检测所有数据是否合法
	bool check_all_data(CMJHandCard user_hand[], CMJBlockCard user_block[],CMJOutCard user_out[],CMJWallCard wall,unsigned int paly_count);

	//检测百变牌是否变换
	bool check_king_usedd(CMJHandCard user_hand);
	//判断是否是赖子
	bool is_king(unsigned char cardvalue);
	//检测是否有赖子
	int is_have_king_value(unsigned char handcard[], unsigned char cardcount);
	//删除牌中的赖子
	int del_king_value(unsigned char handcard[], unsigned char cardcount);
	//判断是否是字牌
	bool  is_zi_card(unsigned char card);
	//判断是否是风牌 东 南 西 北
	bool is_wind_card(unsigned char card);
	//判断是否是箭牌，中 发 白
	bool is_arrow_card(unsigned char card);
	//判断是否是19
	bool  is_one_nine(unsigned char card);
	//判断是否是2、5、8
	bool is_258(unsigned char card); 
	//检测连续数组组成顺子
	int check_index_combox(int index_card[]);
public:
	unsigned int				m_normal_len;						//牌长度
	unsigned char				m_normal_card[NORMAL_CARD_NUM];		//牌型个数
	bool						m_allow_qbk;						//是否允许13烂/全不靠
	bool						m_allow_131;						//是否允许131 
	bool						m_allow_qidui;						//是否允许七对
	bool						m_allow_mix_jiang;					//是否允许乱将
	bool						m_allow_laizi;						//是否允许赖子
	bool						m_allow_chi;						//是否允许吃
	bool						m_allow_wind_chi;					//是否允许吃风牌
	bool						m_allow_arrow_chi;					//是否允许吃箭牌
	bool						m_allow_peng;						//是否允许碰
	bool						m_allow_gang;						//是否允许杠
	unsigned int				m_king_count_limit;					//赖子限制，直接胡 
	unsigned int				m_king_count;						//赖子的个数
	unsigned char				m_king_card[KING_CARD_NUM];			//赖子
	
};
#endif//__MJ_LOGIC_H__
