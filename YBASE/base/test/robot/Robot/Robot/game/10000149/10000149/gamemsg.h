#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_ 
#include <windows.h>
#include <stdarg.h> 
#include "mjdata.h"
#define NAME						    hzmj								 // �����齫 
//#define PLAY_COUNT						4						             // ��Ϸ����
#define MAX_ZHA_MA						68									//���������
#define PLAY_COUNT					8						             // ��Ϸ����
#define MAX_COUNT					5									//�����Ŀ
//��Ϸ��ʱ�� 1-50 (40-50 ����)
#define		TIME_SEND_CARD					1							//����
#define		TIME_SEND_ONE_CARD				2							//���� 
#define		TIME_FREE_DESK					3							//��ɢ����
#define		TIME_USER_OUT_1					4							//���1��ʱ
#define		TIME_USER_OUT_2					5							//���2��ʱ
#define		TIME_USER_OUT_3					6							//���3��ʱ
#define		TIME_USER_OUT_4					7							//���4��ʱ
#define		TIME_USER_CUT_INFO_1			8							//���Ͷ�����Ϣ-
#define		TIME_USER_CUT_INFO_2			9							//���Ͷ�����Ϣ-
#define		TIME_USER_CUT_INFO_3			10							//���Ͷ�����Ϣ-
#define		TIME_USER_CUT_INFO_4			11							//���Ͷ�����Ϣ-

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
/***************************************�����ࡿ********************************************/
///״̬��Ϣ����
#define  GAME_MSG_STATUS_READY                 50           //׼��״̬��Ϣ
#define  GAME_MSG_STATUS_CALLBANKER            51           //��ׯ״̬��Ϣ
#define  GAME_MSG_STATUS_XIAZHU                52           //��ע״̬��Ϣ
#define  GAME_MSG_STATUS_OPEN                  53           //����״̬��Ϣ
#define  GAME_MSG_STATUS_SET_NUM               54           //��������������Ϣ
#define  GAME_MSG_STATUS_CALLBANKER_DGN        55           //����ţ��ׯ״̬��Ϣ
#define  GAME_MSG_STATUS_KPQZ_SET_NUM          56           //������ׯ��������������Ϣ
#define  GAME_MSG_STATUS_START_GAME            57           //��Ϸ��ʼ��Ϣ
#define  GAME_MSG_STATUS_WAIT_START            58           //��Ϸ�ȴ���ʼ��Ϣ

///�������Ϣ
#define  GAME_MSG_SEND_CALLBANKER              61           //�����ׯ��Ϣ
#define  GAME_MSG_SEND_BANKER                  62           //����ׯ����Ϣ
#define  GAME_MSG_SEND_USERCARD                63           //���������������Ϣ
#define  GAME_MSG_SEND_OPENCARD                64           //���̯����Ϣ
#define  GAME_MSG_SEND_RESULT                  65           //��Ϸ��Ӯ�����Ϣ
#define  GAME_MSG_SEND_XIAZHU                  66           //�����ע��Ϣ
#define  GAME_MSG_SEND_XIAZHURESULT            67           //�����ע��Ϣ���
#define  GAME_MSG_SEND_OPENCARDALONE           68           //��ҵ���̯����Ϣ
#define  GAME_MSG_SEND_USER_JOIN               69           //������Ҽ�����Ϸ
#define  GAME_MSG_SEND_USER_LEFT               70           //��������뿪��Ϸ
#define  GAME_MSG_SEND_GAME_RULE			   71           //���͹���	
#define	 GAME_MSG_SEND_FAST_OUT			       72			//���Ϳ��ٷ���
#define  GAME_MSG_SINGLE_INFO                  73           //С�ּ�¼
#define  GAME_MSG_TOTLE_INFO                   74           //�ּܾ�¼
#define  GAME_MSG_CUOPAI_INFO                  75           //���ƽ����Ϣ
#define  GAME_MSG_COINS_NOT_ENOUTH             76           //��Ҳ�����Ϣ
#define  GAME_MSG_SEND_GAME_OVER               77           //������Ϸ������Ϣ

///�ͻ�����Ϣ
#define	 GAME_MSG_RECV_CALLBANKER               80          //�����ׯ��Ϣ
#define  GAME_MSG_RECV_XIAZHU                   81          //�����ע��Ϣ
#define  GAME_MSG_RECV_OPENCARD                 82          //���̯����Ϣ
#define  GAME_MSG_SET_CARD_COUNT				83          //������ü����� 
#define  GAME_MSG_USER_AGREE                    84          //���ͬ�⿪ʼ
#define  GAME_MSG_REQUEST_RECORD                85          //����С�ּ�¼
#define  GAME_MSG_USER_READY_INFO               86          //������ҵ�׼��״̬
#define  GAME_MSG_USER_ROBNT_INFO               87          //��ׯ���
#define  GAME_MSG_USER_FALL_INFO                88          //��ׯ���
#define  GAME_MSG_KPQZ_SET_CARD_COUNT			89          //������ׯ����ѡ����Ϣ
#define  GAME_MSG_KPQZ_CUOPAI		        	90          //������Ϣ
#define  GAME_MSG_KPQZ_LEFT  		        	91          //�����뿪��Ϣ

/***********************************************************************************/




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

#pragma pack(push,1)

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
	unsigned char	   draw_card;					//���������
	sc_game_user_info()
	{
		memset(this, 0, sizeof(game_user_info));
	}
}game_user_info;

//���������Ϣ 
typedef struct stu_update_user_info
{
	unsigned char	chair_id;						//��� 1
	game_block_data	block_info[BLOCK_MAX];			//�������  40
	unsigned int	hand_card_count;				//��������� 4
	unsigned int	out_card_count;					//��ҳ����� 4	 
	unsigned int	block_count;					//��������� 4
	unsigned char	hand_card[HAND_CARD_COUNT];		//������� 14
	unsigned char	out_card[MAX_OUT_CARD];			//��ҳ��� 40
	unsigned char	   draw_card;					//��������� 1

	stu_update_user_info()
	{
		memset(this, 0, sizeof(stu_update_user_info));
	}

}update_user_info;


//������Ϸ����
typedef struct stu_game_config
{
	int				room_card;				//���ķ�����
	bool			hu_qxd;					//�Ƿ�������߶�
	int				hongaward_ma;			//���м�����
	int				award_ma;				//������
	int				luck_mode;				//��Ϸģʽ 0:���� 1������ 2:һ��ȫ��
	bool			all_ma;					//�ĺ���ȫ�� 
	stu_game_config()
	{
		room_card = 0;
		hu_qxd = false;
		hongaward_ma = 0;
		award_ma = 0;
		luck_mode = 0; 
		all_ma = false;
	}
}game_config;

//S-->C
//��Ϸ��Ϣ�ṹ�� 
//��Ϸ״̬������Ϣ
typedef struct sc_game_base_info
{
	unsigned char		game_status;				//��Ϸ״̬ 1
	unsigned int		user_id[PLAY_COUNT];		//���id 16 
	long long			user_score[PLAY_COUNT];		//��ҷ��� 32
	bool				user_cut[PLAY_COUNT];		//���״̬ 4
	unsigned int		total_play_count;			//�ܾ���	4
	unsigned int		cur_play_count;				//�ѿ���   4
	bool				bOpenFree;					//�Ƿ�����ɢ 1
	unsigned int		free_apply_id;				//������id   4
	unsigned char		free_apply_station;			//������λ�� 1
	bool				bUserChose[PLAY_COUNT];		//����Ƿ���� 4
	unsigned int		free_remain_time;			//ʣ��ʱ�� 4
	unsigned char		mj_status;					//�齫�׶�״̬ 1
	unsigned char		banker_station;				//ׯ��λ�� 1
	unsigned int		fast_count[PLAY_COUNT];		//���ٳ��ƴ��� 16
	unsigned int		out_count[PLAY_COUNT];		//��ʱ����		16
	unsigned char		my_station;					//�Լ���λ��	1
	unsigned char		private_station;			//����λ��		1
	unsigned int		private_id;					//����λ��		4
	unsigned int		free_time;					//��ɢ����ʱ��
	game_config			gameconfig;					//��Ϸ����		18
	sc_game_base_info()
	{
		memset(this, 0, sizeof(sc_game_base_info));  
		free_apply_station = 255; 
		banker_station = 255;
		my_station = 255;
		private_station = 255;
	}

}game_base_info;
 
//��Ϸ׼���׶�
typedef struct sc_game_status_ready
{ 
	game_base_info		base_info;
	bool				user_ready[PLAY_COUNT];
	sc_game_status_ready()
	{
		memset(user_ready, 0, sizeof(user_ready));
	}

}game_status_ready;
//���ٳ�����Ϣ
typedef struct sc_fast_out_info
{
	unsigned char			chair_id;		//���λ��
	unsigned int			add_fast_count;		//���Ӵ���
	unsigned int			fast_all_count;	//���ٴ�������
	sc_fast_out_info()
	{
		memset(this, 0, sizeof(sc_fast_out_info));
	}
}fast_out_info;
//��ʾ��ҳ�ʱ
typedef struct sc_user_time_out
{
	unsigned char			chair_id;		//���λ�� 
	unsigned int			out_all_count;	//���ٴ�������
	sc_user_time_out()
	{
		memset(this, 0, sizeof(sc_user_time_out));
	}
}user_time_out;


//��Ϸ��ʼ�׶�
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//������Ϣ
	unsigned char		dice[2];									//����
	unsigned char		cur_operate_station;						//��ǰ�������
	unsigned char		cur_gams_stats;								//��ǰ��Ϸ״̬
	unsigned char		cur_block_level;							//��ǰ���Ƶȼ�
	unsigned char		cur_check_user;								//��ǰ��������
	unsigned char		cur_check_card;								//��ǰ��������
	bool				bdraw_stats;								//�Ƿ������� 

	unsigned int		wall_count;									//��ǽ����
	game_user_info		user_info[PLAY_COUNT];						//�����Ϣ 
	unsigned int		gang_all_count;								//�ܵ�����
	bool				ting;										//�Ƿ���
	unsigned char		ting_list[NORMAL_CARD_NUM + 1];				//������
	unsigned char		ting_count[NORMAL_CARD_NUM + 1];			//���Ƶĸ���

	unsigned char	    block_info[BLOCK_MAX][5];					//����������  
	unsigned int		block_index;								//��������
	 
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
		memset(dice, 255, sizeof(dice));
	}
}dice_info;

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
	sc_notify_block_info()
	{
		block_index = 0;
		checked_id = 255;
		out = false;
		card_value = 0x00; 
		block_level = 0x00;  
		memset(block_info, 0, sizeof(block_info)); 
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
	int						 bWinType;									//���Ʒ�ʽ 0����ׯ   1������ 2������  3������ 4:�ܿ�
	bool					 bWin[PLAY_COUNT];							//�������
	unsigned char			 hand_card[PLAY_COUNT][HAND_CARD_COUNT];	//�������
	unsigned char			 win_card;									//������
	unsigned char			 dianpao_id;								//���ڵ����
	int						 gang_score[PLAY_COUNT];					//��Ҹܷ�
	int						 fan_score[PLAY_COUNT];						//��Һ��Ʒ���
	unsigned char			 luck_card[MAX_ZHA_MA];						//������ֵ 
	long long				 total_score[PLAY_COUNT];					//����ܷ�
	bool					 bAllKing;									//�Ƿ���ȫ��
	unsigned int			 fast_count[PLAY_COUNT];					//���ٴ���
	unsigned int			 out_count[PLAY_COUNT];						//��ʱ����
	unsigned int			 reward_gift[PLAY_COUNT];					//������ȯ
	unsigned int			 user_luck_count[PLAY_COUNT];				//����������
	sc_notify_game_result()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
	}
}notify_game_result;

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

//ÿһ�ֵ���Ϸ��¼
typedef struct sc_single_game_record
{
	unsigned int		index;			//���
	unsigned char		win_type;		//Ӯ��ģʽ
	unsigned char		dianpaoer;		//���ڵ����
	unsigned char		win_card;		//������
	long long			finish_time;	//����ʱ��
	bool				win[PLAY_COUNT];//Ӯ�����
	int					win_score[PLAY_COUNT];//����
	int					gang_score[PLAY_COUNT];//�ܷ� 
	int					luck_count[PLAY_COUNT];//��������
	int					mgang_count[PLAY_COUNT];//���ܴ���
	int					agang_count[PLAY_COUNT];//���ܴ���
	int					bgang_count[PLAY_COUNT];//���ܴ���
	int					fgang_count[PLAY_COUNT];//�Ÿܴ���
	unsigned int		fast_count[PLAY_COUNT];	//���ٴ���
	unsigned int		out_count[PLAY_COUNT];//��ʱ����
	unsigned int		reward_gift[PLAY_COUNT];//������ȯ
	sc_single_game_record()
	{
		memset(this, 0, sizeof(sc_single_game_record));
		dianpaoer = 255;
	}

}single_game_record;
//����Ϸ��¼
typedef struct sc_total_record_game_info
{
	unsigned int	play_count;					//�ܾ���
	unsigned int	win_count[PLAY_COUNT];		//���ƴ���
	unsigned int	qiang_win_count[PLAY_COUNT];//���ܺ�����
	unsigned int	ming_gang[PLAY_COUNT];		//���ܴ���
	unsigned int	an_gang[PLAY_COUNT];		//���ܴ���
	unsigned int	bu_gang[PLAY_COUNT];		//���ܴ���
	unsigned int	luck_count[PLAY_COUNT];		//�������
	int				fan_score[PLAY_COUNT];		//��ҵ÷�
	int				gang_score[PLAY_COUNT];		//�ܷ�
	unsigned int	fast_count[PLAY_COUNT];		//���ٴ���
	unsigned int	out_count[PLAY_COUNT];		//��ʱ����
	unsigned int	reward_gift[PLAY_COUNT];	//������ȯ
	long long		finish_time;				//����ʱ��

	sc_total_record_game_info()
	{
		memset(this, 0, sizeof(sc_total_record_game_info));
	}
}total_record_game_info;

//������Ϣ
typedef struct sc_user_ting_info
{
	bool		ting;
	unsigned char ting_list[NORMAL_CARD_NUM+1];
	unsigned char ting_count[NORMAL_CARD_NUM+1]; 
	
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
/*******************************************************************************************/
//����Ϣ
typedef struct Stu_UserCard
{
	BYTE   chairid;			       //��λ��
	BYTE   status;                 //��Ϸ״̬
	BYTE   bytime;                 //��ǰ״̬ʱ��
	BYTE   byUsercard[5];          //���������
	BYTE   byUserSecCard[5];       //�мҵڶ�����
	BYTE   byCardType[2];          //�������
	bool   bHaveSecCard;           //�Ƿ��еڶ�����
	UINT   bServeMoney[PLAY_COUNT];//�����
	BYTE   byUserstatus[PLAY_COUNT];//��ҵ�ǰ״̬
	Stu_UserCard()
	{
		memset(this, 0, sizeof(Stu_UserCard));
		chairid = 255;
	}
}stu_usercard;
//��ׯ״̬
typedef struct Stu_Status_Qiang
{
	BYTE  status;                   //����״̬
	BYTE  bytime;                   //��ǰ״̬ʱ��
	bool  bcan[PLAY_COUNT];         //�Ƿ�����
	BYTE  beishu;                   //��ׯ����
	INT64 byxianzhi[PLAY_COUNT];    //�����������
	Stu_Status_Qiang()
	{
		memset(this, 0, sizeof(Stu_Status_Qiang));
	}
}stu_status_qiang;


//�����ׯ
typedef  struct Stu_CallBanker
{
	BYTE  chairid;			       //��λ��
	BYTE  byValue;			       //��ׯ��־ 0Ϊ����  1Ϊ��ׯ,2Ϊ�ӱ���ׯ
}stu_callBanker;

typedef  struct Stu_Status_XZ
{
	BYTE  status;                   //����״̬
	BYTE  bytime;                   //��ǰ״̬ʱ��
	INT   iscore[PLAY_COUNT];       //��ע����
	bool  blimite[PLAY_COUNT];      //�Ƿ���С��ע������
	bool  btuilimite[PLAY_COUNT];   //�Ƿ���ע����
	INT64 unxiazhu[PLAY_COUNT];     //��ע���Ʒ���
	Stu_Status_XZ()
	{
		memset(this, 0, sizeof(Stu_Status_XZ));
	}
}stu_status_XZ;


typedef  struct Stu_UserXiaZhu
{
	BYTE   chairid;			       //��λ��
	INT    xiazhumoney;            //��ע���
}stu_userXiaZhu;

//�������̯������
typedef  struct Stu_UserOpenCard
{
	BYTE   chairid;              //��λ��
	BYTE   openflag;             //̯�Ʊ�ʶ���ڼ����ƣ�
	BYTE   cardlist[5];          //������
}stu_userOpenCard;

//���е�������
struct Stu_AllUserCard
{
	BYTE	status;								  //��Ϸ״̬
	BYTE	bytime;								  //��ǰ״̬ʱ��
	BYTE	allusercard[PLAY_COUNT][5];           //���������
	BYTE	cardtype[PLAY_COUNT]; 			      //����
	BYTE	alluserSecCard[PLAY_COUNT][5];        //��ҵڶ����Ƶ�����
	BYTE	cardSectype[PLAY_COUNT];              //�ڶ�������
	BYTE	sortrank[PLAY_COUNT];                 //�������

	Stu_AllUserCard()
	{
		memset(this, 0, sizeof(Stu_AllUserCard));
	}
};
//��ҷ����仯
struct Stu_ChangeMoney
{
	Stu_AllUserCard  allCardData;
	BYTE			 chairid;			       //��λ��
	BYTE			 status;                   //��Ϸ״̬
	int				 changemoney[PLAY_COUNT];  //��Ӯ���ֵ 
	int				 iGetscore[PLAY_COUNT][2]; //ÿ���ƻ�ȡ�ķ���
	bool             benough[PLAY_COUNT][2];   //�Ƿ�ÿ���ƶ�����
	bool             changebanker;             //�Ƿ�ׯ
	BYTE             btanpai[PLAY_COUNT];
	BYTE             byUserstatus[PLAY_COUNT]; //�����Ϸ״̬
	BYTE             bnotenouth;               //����Ƿ��Ҳ���
	Stu_ChangeMoney()
	{
		memset(this, 0, sizeof(Stu_ChangeMoney));
		memset(btanpai, 255, sizeof(btanpai));
		chairid = 255;
	}
};
//��Ϸ�߼��趨
struct GameLogicRuleSet
{
	BYTE		byGameLogicType;    //�������ã���Enum_RoomLogicSet��Ӧ��ȡ���룩
	BYTE        byKingType;        //�������ͣ�0��û�����ƣ�1������10��2����С���ٱ䣩	
};


//��Ϸ��ȡ�ſ�
struct GameRoomConfigSet
{
	GameLogicRuleSet	 GameLogicSet;
	int					 iSetRoomCount;			//���ô������
	int					 iCostOpenCard;			//��Ӧ�������ĵķ�������
	BYTE				 byBeginPlayer;			//��ʼ����
	BYTE				 byFirstNT;				//����ׯ��
	BYTE				 bychangeNTType;		//��ׯģʽ  1~7��Ӧ��Ϸģʽ
	BYTE                 byHandCardNum;			//����������1��2��
	BYTE                 byplaymode[5];			//0������ģʽ 1��3��ģʽ 2,����ѡ��
	int  				 byTuizhu;		        //��ע
	WORD                 byFengding;            //�ⶥ
	bool                 byxiazhulimit;         //��ע����
	int                  ixuanfen;              //ѡ��
	WORD                 izhuangfen;            //ׯ��
	bool                 bylunzhuang;           //����ţ��ׯ 0��ׯ 1��ׯ
	bool                 byfreelowbank;         //��������ׯ
	int                  byGgbs;                //��������
	bool                 bFangfei;              //����
	BYTE                 byFanbei;              //��������
	BYTE                 byZuidaqz;             //�����ׯ
	BYTE                 byStart;               //�Զ���ʼ
	bool                 bPlaygame;             //��Ϸ��ʼ�����
	bool                 bChuopai;              //��ֹ����
	GameRoomConfigSet()
	{
		GameLogicSet.byGameLogicType = 0;
		GameLogicSet.byKingType = 0;
		byHandCardNum = 1;
		iSetRoomCount = 12;
		iCostOpenCard = 1;
		byFirstNT = 0;
		bychangeNTType = 0;
		byTuizhu = 0;
		byFengding = 0;
		byxiazhulimit = false;
		ixuanfen = 0;
		izhuangfen = 0;
		bylunzhuang = false;
		byfreelowbank = false;
		memset(byplaymode, 0, sizeof(byplaymode));
	}
	void InitConfig()
	{
		GameLogicSet.byGameLogicType = 0;
		GameLogicSet.byKingType = 0;
		byHandCardNum = 1;
		iSetRoomCount = 12;
		iCostOpenCard = 1;
		byFirstNT = 0;
		bychangeNTType = 0;
		byTuizhu = 0;
		byFengding = 0;
		byxiazhulimit = false;
		ixuanfen = 0;
		izhuangfen = 0;
		bylunzhuang = false;
		byfreelowbank = false;
		memset(byplaymode, 0, sizeof(byplaymode));
	}
};
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
	INT64           byRobbeishu;                    //��ׯ�����
	INT64           unXiazhu;                       //�����ע��
	Stu_GameUserItem()
	{
		memset(this, 0, sizeof(Stu_GameUserItem));
		player_id = 255;
		chairid = 255;
	}
}GameUserItem;


struct Stu_GameStation
{
	BYTE			chairid;							//�����λ��
	INT				userid;								//���ID
	BYTE			nowtime;							//��ǰ״̬ʣ��ʱ��
	BYTE			readytime;							//׼��ʱ��
	BYTE			callbankertime;						//��ׯʱ��
	BYTE			xiazhutime;							//��עʱ��
	BYTE			sethandcouttime;					//��������ʱ��
	BYTE			opentime;							//����ʱ��
	BYTE			resulttime;							//����ʱ��
	unsigned int    freeGametime;                       //��ɢ����ʱ��
	BYTE			bybankeruser;						//��ǰׯ��λ��
	INT				chip[5];							//����ֵ
	INT64			xzmoneycount[PLAY_COUNT];			//�����ע����ֵ
	INT64			winmoney[PLAY_COUNT];				//�����Ӯ���
	BYTE			usercard[PLAY_COUNT][5];			//�������Ϣ
	BYTE		    userSecCard[PLAY_COUNT][5];			//��ҵڶ�����
	BYTE			cardtype[PLAY_COUNT];				//�������
	BYTE		    cardSectype[PLAY_COUNT];			//�ڶ����Ƶ�����
	BYTE			nowstatus;							//��ǰ״̬
	BYTE			callbanker[PLAY_COUNT];				//һ�ֽ�ׯ��¼��û����255���� 1�� ���� 0
	BYTE			tanpai[PLAY_COUNT][2];				//һ��̯�Ƽ�¼��̯�� 1�� δ̯�� 0
	BYTE			tanpaitype[PLAY_COUNT];				//һ��̯�����ͼ�¼��δ̯��255������Ϊ̯������ֵ	
	BYTE		    byHaveSecCad[PLAY_COUNT];			//����Ƿ��еڶ�����
	BYTE		    tanpaisectype[PLAY_COUNT];			//�ڶ���̯������
	BYTE			private_station;					//����λ��
	int				private_id;							//����id
	BYTE		    byGamePlayNum;						//��Ϸ�������
	bool		    bAgreeStation[PLAY_COUNT];			//���ͬ��״̬
	bool			bOpenFree;							//�Ƿ�����ɢ
	unsigned int	free_apply_id;						//������id
	unsigned char	free_apply_station;					//������λ�� 
	bool			bUserChose[PLAY_COUNT];				//����Ƿ����
	unsigned int	free_remain_time;					//ʣ��ʱ��
	unsigned int	fast_count[PLAY_COUNT];				//���ٳ���
	bool            bChooseIndex[10];					//���ѡ������
	UINT            ituizhu[PLAY_COUNT];                //��ע����
	UINT            iZhuangFen;                         //ׯ��
	bool            blimite[PLAY_COUNT];                //��ͷ���ע����
	BYTE			fourcard[PLAY_COUNT][4];			//��ҿ�����Ϣ
	bool            btuizhulimite[PLAY_COUNT];          //��ͷ���ע����
	BYTE            brestcount[PLAY_COUNT];             //��Ϣ����
	BYTE            bcutcount[PLAY_COUNT];              //���߾���
	bool            bcuopai;                            //�Ƿ��Ѵ���
	bool            bfirstcount;                        //�Ƿ��׾�
	GameRoomConfigSet roomrule;                         //�������	
	GameUserItem    useritem[PLAY_COUNT];               //��Ϸ�������Ϣ
	Stu_GameStation()
	{
		ZeroMemory(this, sizeof(Stu_GameStation));
		memset(callbanker, 255, sizeof(callbanker));
		memset(tanpaitype, 255, sizeof(tanpaitype));
		bybankeruser = 255;
		private_station = 255;
	}
};







/*******************************************************************************************/
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
	unsigned int	block_index;					//��������
	cs_user_block_operate()
	{
		block_index = 0;
		chair_id = 255;
		opreate_type = 0x00; 
		out = false;
		memset(card_value, 0, sizeof(card_value));
	} 
}user_block_operate;

//���ú���
typedef struct cs_set_next_wall
{
	unsigned char	card_value;
	cs_set_next_wall()
	{
		card_value = 0x00;
	}
}set_next_wall;

//��ɢ��Ϸ
typedef struct cs_user_free_opreate
{
	unsigned int		apply_id;		//������ID
	unsigned char		apply_chair;	//������λ��
	unsigned int		userid;			//���ID
	unsigned char		chair_id;
	bool				free;
	cs_user_free_opreate()
	{
		memset(this, 0, sizeof(cs_user_free_opreate));
		apply_chair = 255;
		chair_id = 255;
	}
}user_free_opreate;

//��ɢ���
typedef struct cs_user_free_reslut
{
	unsigned int owner_id;		//��ͬ�����
	unsigned char  owner_station;
	bool		result;
	cs_user_free_reslut()
	{
		memset(this, 0, sizeof(cs_user_free_reslut));
		owner_station = 255;
	}
}user_free_reslut;


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

//������Ϸ������Ϣ
typedef struct cs_apply_game_config
{
	unsigned char	chair_id;
	cs_apply_game_config()
	{
		memset(this, 0, sizeof(cs_apply_game_config));
		chair_id = 255;
	}
}apply_game_config;

#pragma pack(pop)
#endif