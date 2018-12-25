#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_

#include <windows.h>
#include <stdarg.h>

#define PLAY_COUNT						4				//��Ϸ���� --������������
#define HAND_CARD_NUM					4				//������
#define ROW_COUNT						3				//�����Ƹ���(������)
#define COL_COUNT						2				//����
#define ALL_CARD_COUNT					54				//�������
#define MAX_TURN						6				//�������

#define CARD_INIT						0x00			//��ֵ��ʼֵ
#define PLAYER_INIT						255				//λ�ó�ʼֵ
#define	BLACK_JOKER						0x4E			//С��
#define RED_JOKER						0x4F			//����

//��Ϸ��ʱ�� 1-40
#define TIME_ONE_SECOND                 1000            //һ��
#define TIME_STARTGAME					1				//��ʼ��Ϸ����ʱ
#define TIME_PICKCARD					2				//ѡ�Ƶ���ʱ
#define TIME_SENDCARD					3				//���Ƶ���ʱ
#define TIME_DELAY						4				//С���ؿ�����ʱ
#define TIME_DISSOLVE					5				//С�ֽ�ɢ����ʱ
#define TIME_CAL						6				//������ʱ
#define TIME_NO_PERPARE					7				//����Ƿ���Կ���

//��Ϸ��Ϣ
//send data from server to  client MsgID>=2000
#define	STC_SEND_GAME_RESULT				2000		//������Ϣ
#define STC_SEND_UPDATE_USER_INFO			2001		//���������Ϣ
#define STC_SEND_USER_AGREE					2002		//�������׼����Ϣ
#define STC_SEND_PUBLIC_INFO				2003		//���͹�������Ϣ
#define STC_SEND_HAND_CARD					2004		//��������
#define STC_NOTIFY_PICK_CARD				2005		//����ѡ��֪ͨ
#define STC_NOTIFY_PICK_RESULT				2006		//����ѡ�ƽ��
#define STC_SEND_START_COUNT  				2007		//��ʼ����ʱ��ť
#define STC_SEND_COMMON_MESSAGE  			2008		//����ͨ����Ϣ
#define STC_SEND_NOTIFY_PREPARE				2009		//֪ͨ�������׼��
#define STC_SEND_ALL_USER_STATUS			2010		//֪ͨ�ͻ����������״̬

//recv data from client to  server MsgID>=1000
#define CTS_UPDATE_USER_PICK				1000		//�������ѡ��

#pragma pack(push,1)

//��Ϸ״̬
enum GAME_STATUS
{
	STATUS_FREE = 1,									//����״̬
	STATUS_BEGIN,										//��ʼ
	STATUS_SEND_PUBLIC_CARD,							//���͹����ƽ׶�
	STATUS_SEND_HAND_CARD,								//�������ƽ׶�
	STATUS_PICK,										//ѡ�ƽ׶�
	STATUS_COUNT,										//����״̬
	STATUS_WAIT_FINISH									//��Ϸ����״̬
};

//���״̬
enum USER_STATUS
{
	USER_SEE = 0,										//�Թ�
	USER_JOIN,											//������Ϸ
	USER_RES											//��Ϣ
};

//��Ϸ�߼��У���Ҫ�Ľṹ��
//��������
typedef struct sg_game_basic_config
{
	unsigned int	player_mode;						//���ģʽ  0:���� 1��ȫ��ͬ��
	int				time_pick_card;						//ѡ��ʱ��
	int				time_restart;						//С���ؿ�ʱ��
	int				time_dissolve;						//С��δ��ʼ��ɢʱ��
	int				time_over_wait;						//С�ֽ����ȴ�ʱ��
	int				look_status_limit;					//�Թ۴�������
	sg_game_basic_config()
	{
		memset(this, 0, sizeof(sg_game_basic_config));
	}
}game_basic_config;

//������Ϸ����-������Ϸ��ͬ������
typedef struct sg_game_config
{
	game_basic_config	basic_config;						//��������
	bool				have_king;							//�Ƿ������
	bool				show_card;							//�����ƿɼ�
	int					player_min;							//��������
	int					base_point;							//�׷�
	int					tax;								//�����
	void initdata()
	{
		memset(this, 0, sizeof(sg_game_config));
	}
	sg_game_config()
	{
		memset(this, 0, sizeof(sg_game_config));
	}
}game_config;

//�����Ϣ
typedef struct sg_game_user_info
{
	unsigned int	user_id;							//���ID 
	unsigned char	chair_id;							//��������ϵ�λ��
	unsigned int	game_status;						//��Ϸ״̬ 0���Թ� 1:������Ϸ 2:��Ϣ
	bool			cut_status;							//����״̬ false������ true������
	bool			user_ready;							//����Ƿ�׼�� false��δ׼�� true��׼��
	long long		game_score;							//��Ϸ����(���)
	char			nickname[60];						//����ǳ�
	char			head_url[200];						//ͷ��·��
	void initdata()
	{
		memset(this, 0, sizeof(sg_game_user_info));
		chair_id = PLAYER_INIT;
	}
	sg_game_user_info()
	{
		memset(this, 0, sizeof(sg_game_user_info));
		chair_id = PLAYER_INIT;
	}
}game_user_info;

//ÿ����ҵ�ÿ����Ϸ�÷���Ϣ
typedef struct sg_score_info
{
	int row;
	int score;
	int card_type;
	unsigned char high_card;
	bool is_row_winner;
	sg_score_info()
	{
		score = 0;
		card_type = 0;
		is_row_winner = false;
	}
}score_info;

//�������
typedef struct sg_user_placed_card
{
	unsigned char card_value[ROW_COUNT][COL_COUNT];
	sg_user_placed_card()
	{
		memset(card_value, CARD_INIT, sizeof(card_value));
	}
	void init()
	{
		memset(card_value, CARD_INIT, sizeof(card_value));
	}
}user_placed_card;

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////S-->C////////////////////////////////////////////////
//��Ϸ��Ϣ�ṹ�� 
//��Ϸ״̬������Ϣ
typedef struct sc_game_base_info
{
	game_config			gameconfig;							//��Ϸ����
	unsigned char		game_status;						//��ǰ����״̬
	unsigned char		cur_status;							//��ǰ��Ϸ�׶�
	int					time_auto_out;						//�Զ�����ʱ��
	int					game_num;							//��ǰ��Ϸ����
	sc_game_base_info()
	{
		game_status = USER_LOOK_STATE;
		cur_status = STATUS_FREE;
	}
}game_base_info;

//֪ͨ�����Ϸ����
typedef struct sc_notify_game_result
{
	user_placed_card	placed_card[PLAY_COUNT];					//����
	unsigned char		card_type[ROW_COUNT][PLAY_COUNT];			//ÿ�е���ҵ�����
	int					row_score[ROW_COUNT][PLAY_COUNT];			//ÿ�е���ҵ÷�
	unsigned char		row_winner[ROW_COUNT];						//ÿ�мӷֵ����
	int					user_win_score[PLAY_COUNT];					//ÿ����ҵ�Ӯ��
	int					user_score[PLAY_COUNT];						//ÿ����ҵ���Ӯ��
	__int64				total_score[PLAY_COUNT];					//ÿ����ҵ���Ӯ��
	__int64				player_coins[PLAY_COUNT];					//���ʣ����
	unsigned char		win_user;									//����Ӯ��

	sc_notify_game_result()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
		win_user = PLAYER_INIT;
	}
	void init()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
		win_user = PLAYER_INIT;
	}
}notify_game_result;

//��Ϸ׼���׶�
typedef struct sc_game_status_ready
{
	game_base_info		base_info;
	game_user_info		user_info[PLAY_COUNT];			//�����Ϣ
	bool				first_game;						//�Ƿ�Ϊ��һ��
	int					dissolve_time;					//���ѷ�δ��ʼ��Ϸ��ɢʱ��
	int					watch_times[PLAY_COUNT];		//�Թ۴���
	int					ready_time;						//׼��ʱ��
	sc_game_status_ready()
	{
		memset(user_info, 0, sizeof(user_info));
		first_game = false;
		dissolve_time = 900;
		memset(watch_times, 0, sizeof(watch_times));
	}
}game_status_ready;

//��Ϸ��ʼ�׶�
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//������Ϣ
	game_user_info		user_info[PLAY_COUNT];						//�����Ϣ
	bool				first_game;									//�Ƿ�Ϊ��һ��
	unsigned int		player_num;									//������Ϸ������
	unsigned char		public_card[ROW_COUNT];						//������
	user_placed_card	placed_card[PLAY_COUNT];					//����
	unsigned char		hand_card[PLAY_COUNT][HAND_CARD_NUM];		//����
	int					current_turn;								//��ǰ�غ�
	int					time_remain;								//ʣ��ʱ��
	notify_game_result  game_result;								//��Ϸ���
	sc_game_status_play()
	{
		player_num = 0;
		current_turn = 0;
		time_remain = 0;
		memset(public_card, 0, sizeof(public_card));
		memset(hand_card, 0, sizeof(hand_card));
	}
}game_status_play;

//֪ͨ���ͬ��(��ʼ-��������)
typedef struct sc_user_agree
{
	unsigned int		 user_id;									//���ID
	unsigned char		 chair_id;									//���λ��
	bool				 bagree;									//�Ƿ�ͬ��
	sc_user_agree()
	{
		user_id = 0;
		chair_id = PLAYER_INIT;
		bagree = false;
	}
}user_agree;

//���������Ϣ
typedef struct sc_update_user_info
{
	unsigned int		user_id;									//���ID
	unsigned char		chair_id;									//���λ��
	long long			score;										//����
	sc_update_user_info()
	{
		user_id = 0;
		chair_id = PLAYER_INIT;
		score = 0;
	}
}update_user_info;

//��������
typedef struct sc_hand_card_info
{
	unsigned char		card_count;									//����
	unsigned char		hand_card[HAND_CARD_NUM];					//�������
	sc_hand_card_info()
	{
		card_count = 0;
		memset(hand_card, CARD_INIT, sizeof(hand_card));
	}
}hand_card_info;

//��������Ϣ
typedef struct sc_public_card_info
{
	unsigned char		replacecard[2];								//�����
	unsigned char		card_count;									//�����Ƹ���
	unsigned char		public_card[ROW_COUNT];						//��������Ϣ
	sc_public_card_info()
	{
		memset(replacecard, 0, sizeof(replacecard));
		card_count = 0;
		memset(public_card, CARD_INIT, sizeof(public_card));
	}
}public_card_info;

//֪ͨѡ����Ϣ
typedef struct sc_notify_pickcard
{
	unsigned char		current_turn;
	sc_notify_pickcard()
	{
		current_turn = 255;
	}
}notify_pickcard;

//�ظ�ѡ�ƽ����Ϣ
typedef struct sc_pickcard_result
{
	unsigned char		chair_id;									//���λ��
	unsigned char		current_turn;								//��ǰ�غ�
	unsigned char		pick_card;									//ѡ����ֵ
	unsigned char		choose_row;									//��
	unsigned char		choose_col;									//��
	sc_pickcard_result()
	{
		chair_id = PLAYER_INIT;
		current_turn = 0;
		pick_card = CARD_INIT;
		choose_row = 255;
		choose_col = 255;
	}
}pickcard_result;

//ͨ����Ϣ
typedef struct  sc_notify_user_operate
{
	unsigned char		flag;										//������ʶ   2:֪ͨ��Ϸ����׶ν��� 3:֪ͨ�����ʾ��������
	unsigned char		chair_id;									//���λ��
	unsigned char		operate_val;								//����ֵ
	unsigned char		flag_ex[PLAY_COUNT];						//���ӱ�ʶ
	int					time;										//����ʱ��
	sc_notify_user_operate()
	{
		memset(this, 0, sizeof(sc_notify_user_operate));
	}
}notify_user_operate;

//��Ϸ��ʼ
typedef struct sc_set_start_game
{
	unsigned char	time_countdown;									//��ʼ����ʱ
	int				watch_times[PLAY_COUNT];						//�Թ۴���
	sc_set_start_game()
	{
		time_countdown = 10;
		memset(watch_times, 0, sizeof(watch_times));
	}
}set_start_game;

typedef struct sc_notify_prepare
{
	unsigned char		chair_id;									//���λ��
	bool				is_prepare;									//�Ƿ�׼��          //true��ʾ֪ͨ�����׼����false��ʾ֪ͨ�����ȡ��׼��
}notify_prepare;

typedef struct sc_notify_user_status
{
	unsigned int		user_status[PLAY_COUNT];					//0:�Թ� 1:��Ϸ�� 2:����
	sc_notify_user_status()
	{
		memset(user_status, 2, sizeof(user_status));
	}
}notify_user_status;

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////C-->S////////////////////////////////////////////////
//��Ϣ�ṹ��

//ѡ����Ϣ
typedef struct cs_pickcard_info
{
	unsigned char		chair_id;									//���λ��
	unsigned char		current_turn;								//��ǰ�غ�
	unsigned char		pick_card;									//ѡ����ֵ
	unsigned char		choose_row;									//��
	unsigned char		choose_col;									//��
	cs_pickcard_info()
	{
		chair_id = PLAYER_INIT;
		current_turn = 0;
		pick_card = CARD_INIT;
		choose_row = 255;
		choose_col = 255;
	}
}pickcard_info;

//���ͨ�ò�����Ϣ
typedef struct  cs_user_operate
{
	unsigned char		flag;										//������ʶ	
	unsigned char		chair_id;									//���λ��
	unsigned char		operate_val;								//����ֵ
	unsigned char		flag_ex[PLAY_COUNT];						//���ӱ�ʶ
}user_operate;

#pragma pack(pop)

#endif