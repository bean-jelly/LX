#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_

#include "mjlogic\mjmessage.h"

#define PLAY_COUNT						4						             // ��Ϸ���� 
#define MAX_ZHA_MA						108									 //���������
//������Ϸ״̬����12��ʼ

//����¼���¼���� 0x30��ʼ
//��Ϸ��ʱ�� 1-50 
#define		TIME_SEND_CARD					1							//����
#define		TIME_SEND_ONE_CARD				2							//����  
#define		TIME_FREE_DESK					3							//��ɢ����
#define		TIME_SEND_KING_CARD				4							//������
#define		TIME_USER_OUT_CARD				5							//��ҳ��ƶ�ʱ��
#define     TIME_USER_BLOCK_OPER            6                           //������Ʋ���
#define		TIME_NOTIFY_BLOCK				9							//֪ͨ��ҳ��� 
#define		TIME_AUTO_OUT_CARD				14							//�Զ�����
#define		TIME_DRAW_CARD					15							//�������
#define		TIME_NOTIFY_OUT					16							//֪ͨ����
#define	    GAME_TIME_JS_YANSHI			    17	                		//��ʱ��ʱ��
#define	    GAME_TIME_FORCE_LEAVE         	18	     			        //ǿ������
#define	    GAME_TIME_FREE_DESK         	19	     			        //ǿ������
#define     GAME_TIME_START_GAME            20                          //��ʼ����ʱ
#define     TIME_ONE_SECOND                 1000                        //һ��
#define		TIME_SEND_DICE					21							//������ҡ����

//��Ϸ��Ϣ
//server to  client 
#define STC_SEND_MAKER_INFO				1000						//����ׯ����Ϣ
#define STC_SEND_DICE_INFO				1001						//����������Ϣ
#define STC_SEND_CARD_INFO				1002						//�����˿���Ϣ
#define STC_SEND_OUT_CARD				1003						//��ҳ���
#define STC_SEND_GET_CARD				1004						//�������
#define STC_SEND_BLOCK_INFO				1005						//����������Ϣ
#define STC_SEND_BLOCK_RESULT			1006						//�������ƽ����Ϣ
#define STC_SEND_NOTIFY_OUT				1007						//֪ͨ��ҳ��� 
#define STC_SEND_NOTIFY_WAIT			1008						//���͵ȴ���Ϣ
#define STC_SEND_GAME_RESULT			1009						//������Ϸ������Ϣ 
#define STC_SEND_TING					1010						//����������Ϣ 
#define STC_SEND_USER_AGREE				1011						//�������׼����Ϣ	
#define STC_SEND_GAME_RECORD			1012						//�����ƾ���Ϣ
#define STC_SEND_TOTAL_RECORD			1013						//���������ƾ� 
#define STC_SEND_FAST_OUT				1014						//���Ϳ��ٷ���
#define	STC_SEND_USER_FREE				1015						//������ҽ�ɢ��Ϸ��Ϣ
#define STC_SEND_FREE_RESUTL			1016						//��ɢ���
#define STC_SEND_GAME_INFO				1017						//��Ϸ������Ϣ
#define STC_SEND_TIME_OUT				1018						//֪ͨ��ҳ�ʱ
#define STC_SEND_USER_TING_RESULT		1019						//����֪ͨ�������
#define STC_SEND_KING_CARD				1020						//����֪ͨ��������� 
#define STC_SEND_NOTIFY_FLOWER			1021						//����֪ͨ������Ϣ	
#define STC_SEND_OTHER_BLOCK			1022						//�ȴ�����������Ʋ���
#define STC_SEND_AUTO_PALY   			1023						//����йܽ��
#define STC_SEND_START_COUNT  			1024						//��ʼ����ʱ��ť

//���͹�����Ϣ
#define STC_UPDATE_USER_INFO			1101						//���������Ϣ	 
#define CTS_GET_GAME_CONFIG				1201						//�����ȡ��Ϸ����
#define CTS_UPDATE_USER_INFO			1202						//��������Լ���Ϣ

//client to server
#define CTS_RECV_PLAY_DICE				2000						//�յ�ׯ�Ҵ�����Ϣ
#define CTS_RECV_OUT_CARD				2001						//�յ���ҳ���
#define CTS_RECV_BLOCK_OPERATE			2002						//�յ�������Ʋ���
#define CTS_RECV_SET_NEXT_WALL			2003						//������һ����ǽ
#define CTS_RECV_USER_AGREE				2004						//�յ����׼����Ϣ
#define CTS_RECV_USER_FREE_GAME			2005						//�յ���ҽ�ɢ������Ϣ
#define CTS_RECV_USER_TING				2006						//�յ����������Ϣ
#define CTS_RECV_AUTO_PLAY				2007						//�յ�����й���Ϣ

#pragma pack(push,1)

//���״̬
enum USER_STATUS
{
	USER_SEE = 0,				//�Թ�
	USER_JOIN,					//������Ϸ
	USER_REST,                  //��Ϣ
};

//������Ϸ����
typedef struct stu_game_config
{
	bool				qidui;				//�߶�
	bool                sihong;             //�ĺ�ȫ����
	unsigned int        zhama;              //����
	unsigned int        wugui;              //�޹�
	bool                zhuapao;            //�޹��ץ��
	unsigned int        guipai;             //����
	unsigned int        difen;              //�׷�
	unsigned int        startcount;         //��ʼ����
	unsigned int        card_length;        //��ǽ����
	stu_game_config()
	{
		memset(this, 0, sizeof(stu_game_config));
	}
}game_config;

//S-->C
//��Ϸ��Ϣ�ṹ�� 
//��Ϸ״̬������Ϣ
typedef struct sc_game_base_info
{
	unsigned char		game_status;				//��Ϸ״̬
	unsigned int		user_id[PLAY_COUNT];		//���id
	long long			user_score[PLAY_COUNT];		//��ҷ���
	bool				user_cut[PLAY_COUNT];		//���״̬
	unsigned int		total_play_count;
	unsigned int		cur_play_count;
	unsigned char		mj_status;					//�齫�׶�״̬
	unsigned char		banker_station;				//ׯ��λ��
	unsigned char		my_station;					//�Լ���λ��	
	unsigned char       auto_out_time;              //�Զ�����ʱ��
	unsigned char       auto_block_time;            //�Զ�����ʱ��
	game_config			gameconfig;					//��Ϸ����		18 
	sc_game_base_info()
	{
		memset(this, 0, sizeof(sc_game_base_info));
		banker_station = 255;
		my_station = 255;
	}

}game_base_info;
//��Ϸ�������Ϣ
typedef struct Stu_GameUserItem
{
	unsigned int	player_id;						//���ID
	BYTE            chairid;                        //��λid
	BYTE            bsex;                           //�Ա�(1��2Ů)
	bool			look_status;					//�Ƿ����Թ�״̬
	bool            bplaygame;                      //�Ƿ�����Ϸ��
	bool            brest;                          //�Ƿ���Ϣ״̬
	bool			cut_status;						//�Ƿ��Ƕ���״̬
	long long		total_score;					//����ܷ�
	long long		player_money;					//�ܶ�(����)
	long long		player_coins;					//���(����)
	long long		player_lottery;					//��Ʊ(����)
	long long       servercount;                    //�����
	Stu_GameUserItem()
	{
		memset(this, 0, sizeof(Stu_GameUserItem));
		player_id = 255;
		chairid = 255;
	}
}GameUserItem;
//��Ϸ׼���׶�
typedef struct sc_game_status_ready
{
	game_base_info		base_info;
	GameUserItem        useritem[PLAY_COUNT];                       //��Ϸ�������Ϣ
	bool				user_ready[PLAY_COUNT];
	bool                bfirst;                                     //�Ƿ��һ��
	unsigned int		dissolve_time;				                //��ɢʱ�� -����  
	BYTE                bycreater;                                  //����
	sc_game_status_ready()
	{
		memset(user_ready, 0, sizeof(user_ready));
	}

}game_status_ready;

//�����˲���
typedef struct sc_notify_wait_other_block
{
	unsigned char			chair_id;		//���λ��
	unsigned char			block_level;	//�����ȼ�
	sc_notify_wait_other_block()
	{
		chair_id = 255;
		block_level = 0x00;
	}
}notify_wait_other_block;

//������Ϣ
typedef struct sc_game_block_data
{
	unsigned char	block_type;			//�������� 1:�� 2 �� 4 ��
	unsigned char	block_sub_type;		//����������  1:���� 2 ���� 4 ����
	unsigned char	blocked_card;		//��������
	unsigned char	blocked_user;		//��������
	unsigned char	block_data[4];		//��������
	sc_game_block_data()
	{
		memset(this, 0, sizeof(game_block_data));
	}

}game_block_data;
//�����Ϣ
typedef struct sc_game_user_info
{
	game_block_data	block_info[BLOCK_MAX];			//������� 
	unsigned int	hand_card_count;				//���������
	unsigned int	out_card_count;					//��ҳ�����	
	unsigned int	block_count;					//���������	
	unsigned char	hand_card[HAND_CARD_COUNT];		//�������
	unsigned char	out_card[MAX_OUT_CARD];			//��ҳ���
	unsigned char	draw_card;						//��������� 
	unsigned int	flower_count;					//���Ƹ���
	unsigned char	flower_card[MAX_FLOWER_NUM];	//������
	sc_game_user_info()
	{
		memset(this, 0, sizeof(game_user_info));
	}
}game_user_info;
//���������Ϣ 
typedef struct stu_update_user_info
{
	unsigned char	chair_id;						//���
	game_block_data	block_info[BLOCK_MAX];			//������� 
	unsigned int	hand_card_count;				//���������
	unsigned int	out_card_count;					//��ҳ�����	
	unsigned int	block_count;					//���������	
	unsigned char	hand_card[HAND_CARD_COUNT];		//�������
	unsigned char	out_card[MAX_OUT_CARD];			//��ҳ���
	unsigned char	draw_card;						//���������
	unsigned int	flower_count;					//���Ƹ���
	unsigned char	flower_card[MAX_FLOWER_NUM];	//������

	stu_update_user_info()
	{
		memset(this, 0, sizeof(stu_update_user_info));
	}
}update_user_info;

//������Ϣ
typedef struct sc_notify_ting
{
	unsigned char	ting_list[HAND_CARD_COUNT][NORMAL_CARD_NUM + 1];
	unsigned char	ting_list_count[HAND_CARD_COUNT][NORMAL_CARD_NUM + 1];
	unsigned int    win_card_count[HAND_CARD_COUNT];	//���Ƹ���
	unsigned int	max_win_count;						//�����Ƹ���
	sc_notify_ting()
	{
		memset(this, 0, sizeof(sc_notify_ting));
	}
}notify_ting;

//��Ϸ��ʼ�׶�
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//������Ϣ
	game_user_info		user_info[PLAY_COUNT];						//�����Ϣ  
	notify_ting			notify_ting;
	unsigned int		wall_count;									//��ǽ����
	unsigned int		gang_all_count;								//�ܵ�����
	unsigned char		dice[2];									//����
	unsigned char		cur_operate_station;						//��ǰ�������
	unsigned char		cur_gams_stats;								//��ǰ��Ϸ״̬
	unsigned char		cur_block_level;							//��ǰ���Ƶȼ�
	unsigned char		cur_check_user;								//��ǰ��������
	unsigned char		cur_check_card;								//��ǰ��������
	bool				bdraw_stats;								//�Ƿ�������  
	unsigned char	    block_info[BLOCK_MAX][5];					//����������     
	bool				ting[PLAY_COUNT];							//�Ƿ���
	bool				chose_ting[PLAY_COUNT];						//�Ƿ�ѡ����
	unsigned char		ting_list[NORMAL_CARD_NUM + 1];				//������
	unsigned char		ting_count[NORMAL_CARD_NUM + 1];			//���Ƶĸ��� 
	bool				canting;									//�Ƿ�����
	unsigned int		block_index;								//��������
	unsigned int		player_num;									//������� 
	bool                bfirst;                                     //�Ƿ��һ��
	bool                bauto[PLAY_COUNT];                          //�Ƿ��й�
	bool                bautowin[PLAY_COUNT];                       //�Ƿ��Զ�����
	unsigned char       nowtime;                                    //��ǰ����ʣ��ʱ��
	GameUserItem        useritem[PLAY_COUNT];                       //��Ϸ�������Ϣ
	BYTE                bycreater;                                  //����
	sc_game_status_play()
	{
		memset(this, 0, sizeof(sc_game_status_play));
		cur_operate_station = 255;
	}
}game_status_play;

//��ׯ��Ϣ
typedef struct sc_maker_info
{
	unsigned char   chair_id;				//ׯ��λ��
	INT64           server_money;           //�����
	sc_maker_info()
	{
		chair_id = 255;
	}
}maker_info;

//������Ϣ
typedef struct sc_dice_info
{
	unsigned char		dice[2];			//������Ϣ
	sc_dice_info()
	{
		memset(dice, 0, sizeof(dice));
	}
}dice_info;
//��������Ϣ
typedef struct sc_king_card_info
{
	unsigned char  card;
	unsigned char  kingcard[2];
	sc_king_card_info()
	{
		card = 0x00;
		kingcard[0] = 0x00;
		kingcard[1] = 0x00;
	}
}king_card_info;
//������Ϣ
typedef struct sc_hand_card_info
{
	unsigned char	hand_card[PLAY_COUNT][HAND_CARD_COUNT];			//�������
	unsigned char	card_count[PLAY_COUNT];							//���Ƹ���
	sc_hand_card_info()
	{
		memset(hand_card, 0, sizeof(hand_card));
		memset(card_count, 0, sizeof(card_count));
	}
}hand_card_info;
//��ҳ���
typedef struct sc_user_out_card_result
{
	unsigned char   chair_id;				//���λ��
	unsigned char   card_value;				//��ֵ
	unsigned char   card_index;             //��λ��
	bool            bting;                  //����Ƿ����ƣ���ʾ�Զ�����
	sc_user_out_card_result()
	{
		chair_id = 255;
		card_value = 0x00;
	}
}user_out_card_result;
//�������
typedef struct sc_user_touch_card
{
	unsigned char   chair_id;				//���λ��
	unsigned char   card_value;				//��ֵ
	bool			bGang;					//�����Ǹ�������
	sc_user_touch_card()
	{
		chair_id = 255;
		card_value = 0x00;
		bGang = false;
	}
}user_touch_card;

//���������Ϣ
typedef struct sc_notify_block_info
{
	unsigned char   card_value;							//��ֵ
	bool			out;								//����/����
	unsigned char   block_level;						//������Ƶȼ�
	unsigned char	checked_id;							//�����Ƶ���
	unsigned char   block_info[BLOCK_MAX][5];			//���������� 
	unsigned int	block_index;						//��������
	unsigned char   block_sub_type;                     //����������(����������Ϊ0x00)
	sc_notify_block_info()
	{
		memset(this, 0, sizeof(sc_notify_block_info));
		checked_id = 255;
	}
	void  cleardata()
	{
		memset(this, 0, sizeof(sc_notify_block_info));
		checked_id = 255;
	}
}notify_block_info;

//֪ͨ��ҵȴ����Ʋ�����Ϣ
typedef struct sc_notify_wait_block_operate
{
	unsigned char   card_value;							//��ֵ
	bool			out;								//����/����
	unsigned char	block_level[PLAY_COUNT];			//������Ƶȼ�
	unsigned char	checked_id;							//�����Ƶ���
	sc_notify_wait_block_operate()
	{
		out = false;
		card_value = 0x00;
		checked_id = 255;
		memset(block_level, 0, sizeof(block_level));
	}
}notify_wait_block_operate;

//֪ͨ������Ʋ������
typedef struct sc_notify_block_reslut
{
	unsigned char   chair_id;				//���λ�� 
	unsigned char	opreate_type;			//�������� 0 ���� 0x01 ��  0x02 �� 0x04 �� 0x08 �� 0x10 ��
	unsigned char   card_value[4];			//��ֵ 0-3 ѡ�����������
	unsigned char	checked_id;				//�����Ƶ���
	unsigned char	block_subtype;			//��������
	unsigned char   card_index[4];          //��λ��
	sc_notify_block_reslut()
	{
		block_subtype = 0;
		chair_id = 255;
		checked_id = 255;
		opreate_type = 0x00;
		memset(card_value, 0, sizeof(card_value));
	}
}notify_block_reslut;

//֪ͨ��ҳ���
typedef struct sc_notify_out_card
{
	unsigned char   chair_id;				//���λ��
	sc_notify_out_card()
	{
		chair_id = 255;
	}
}notify_out_card;

//֪ͨ�����Ϸ����
typedef struct sc_notify_game_result
{
	game_block_data			 block_info[PLAY_COUNT][BLOCK_MAX];			//������� 
	int						 win_type;									//���Ʒ�ʽ 0����ׯ   1������ 2������  3������ 4:�ܿ�
	bool					 win[PLAY_COUNT];							//�������
	unsigned char			 hand_card[PLAY_COUNT][HAND_CARD_COUNT];	//�������
	unsigned char			 win_card;									//������
	unsigned char			 dianpao_id;								//���ڵ����
	bool                     ballking[PLAY_COUNT];                      //�Ƿ�ȫ��
	unsigned char			 luck_card[MAX_ZHA_MA];			            //������ֵ 
	unsigned int			 user_luck_count[PLAY_COUNT];				//����������
	int						 luck_score[PLAY_COUNT];					//�����
	int						 gang_score[PLAY_COUNT];					//��Ҹܷ�
	int						 fan_score[PLAY_COUNT];						//��Һ��Ʒ��� 
	int						 win_score[PLAY_COUNT];						//��Ӯ��
	long long				 total_score[PLAY_COUNT];					//����ܷ� 
	int						 mgang_count[PLAY_COUNT];					//�Ӹܴ���
	int						 agang_count[PLAY_COUNT];					//���ܴ���
	int						 bgang_count[PLAY_COUNT];					//���ܴ���
	int						 fgang_count[PLAY_COUNT];					//�Ÿܴ��� 
	bool					 win_fan_xing[PLAY_COUNT][FAN_TYPE_COUNT];	//��������

	sc_notify_game_result()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
		dianpao_id = 255;
	}

	void init()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
		dianpao_id = 255;
	}
}notify_game_result;

//����
typedef struct sc_notify_flower
{
	unsigned char	chair_id;			//���
	unsigned char	flowercard;			//����
	unsigned char	newcard;			//��������
}notify_flower;


//������Ϣ
typedef struct sc_user_ting_info
{
	bool		ting;
	unsigned char ting_list[NORMAL_CARD_NUM + 1];
	unsigned char ting_count[NORMAL_CARD_NUM + 1];

	void clear()
	{
		memset(this, 0, sizeof(sc_user_ting_info));
	}
	sc_user_ting_info()
	{
		memset(this, 0, sizeof(sc_user_ting_info));
	}
}user_ting_info;
//�����б�
typedef struct sc_user_can_ting_info
{
	bool			can_ting;
	notify_ting		ting_info;
	void clear()
	{
		memset(this, 0, sizeof(sc_user_can_ting_info));
	}

	sc_user_can_ting_info()
	{
		memset(this, 0, sizeof(sc_user_can_ting_info));
	}
}user_can_ting_info;

typedef struct sc_user_agree
{
	unsigned int	userid;
	unsigned char  chair_id;
	bool			bagree;
	sc_user_agree()
	{
		userid = 0;
		chair_id = 255;
		bagree = false;
	}
}user_agree;

//�����й�
typedef struct sc_auto_paly_res
{
	unsigned char	chairid;       //�����λ��
	BYTE            bytype;        //״̬��1������2������
	bool            bautowin;      //�Ƿ��й�
	sc_auto_paly_res()
	{
		chairid = 255;
		bautowin = false;
	}
}auto_paly_res;

//��ʼ��ť
typedef struct sc_start_btn
{
	unsigned char	chairid;       //�����λ��
	sc_start_btn()
	{
		chairid = 255;
	}
}start_btn;

//C-->S
//��Ϣ�ṹ��
//ׯ�Ҵ�����
typedef struct cs_maker_play_dice
{
	unsigned char   chair_id;				//ׯ��λ��
}maker_play_dice;

//��ҳ���
typedef struct cs_user_out_card
{
	unsigned char   chair_id;				//���λ��
	unsigned char   card_value;				//��ֵ
	unsigned char   card_index;             //��λ��
	cs_user_out_card()
	{
		chair_id = 255;
		card_value = 0x00;
	}
}user_out_card;

//������Ʋ���
typedef struct cs_user_block_operate
{
	unsigned char   chair_id;				//���λ��
	unsigned char	opreate_type;			//�������� 0 ���� 0x01 ��  0x02 �� 0x04 �� 0x08 �� 0x10 ��
	unsigned char   card_value[4];			//��ֵ 0-3 ѡ�����������
	bool			out;					//����/���� 
	unsigned int	block_index;
	unsigned char   card_index[4];          //��λ��
	cs_user_block_operate()
	{
		block_index = 0;
		chair_id = 255;
		opreate_type = 0x00;
		out = false;
		memset(card_value, 0, sizeof(card_value));
	}
}user_block_operate;

//��ҳ�����Ϣ
typedef struct cs_user_ting_operate
{
	unsigned char   chair_id;				//���λ��
	unsigned char	card_value;				//���ƴ������

}user_ting_operate;

//���ú���
typedef struct cs_set_next_wall
{
	unsigned char	card_value;
	cs_set_next_wall()
	{
		card_value = 0x00;
	}
}set_next_wall;

//������������Ϸ��Ϣ
typedef struct cs_apply_user_game_info
{
	unsigned int	user_id;
	unsigned char	chair_id;

	cs_apply_user_game_info()
	{
		memset(this, 0, sizeof(cs_apply_user_game_info));
		chair_id = 255;
	}
}apply_user_game_info;

//�����й�
typedef struct cs_set_auto_paly
{
	unsigned char	chairid;       //�����λ��
	BYTE            bytype;        //�й�����(1������2����)
	bool            bauto;      //�Ƿ��й�
	cs_set_auto_paly()
	{
		chairid = 255;
		bytype = 0;
		bauto = false;
	}
}set_auto_paly;

//��Ϸ��ʼ
typedef struct cs_set_start_game
{
	unsigned char	watchcount[PLAY_COUNT];       //�Թ۾���
	cs_set_start_game()
	{
		memset(watchcount, 0, sizeof(watchcount));
	}
}set_start_game;

#pragma pack(pop)

#endif