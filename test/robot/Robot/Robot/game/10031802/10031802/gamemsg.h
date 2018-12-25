#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_


#include <windows.h>
#include <stdarg.h> 


#define   ONE_SECOND_VALUE					1000						//һ��
#define	  PLAY_COUNT						180							// ��Ϸ���� 
#define   CHOUMA_COUNT						5							//�������		
#define   SESSION_COUNT						4							//����
#define   PLAY_LEAST_COUNT				    1							//��������
#define   SEAT_COUNT						6							//��λ��
#define   LINE_COUNT						100							//·��ͼ������
#define	  AREA_COUNT						3							//��ע�������
#define   AREA_TOTAL_COUNT					15							//������� * �������
#define   NOSITLENGTH						180							//��������б���
//������Ϸ״̬����12��ʼ
//��Ϸ��ʱ�� 1-50 
#define		TIME_CHECK_CUT					19							//���߼��
#define		TIME_NOTIFY_OUT					20							//�����ע 
#define		TIME_SEND_CARD					21							//����
#define		TIME_USER_CHIP					24							//��Ҳ���ʱ��
#define		TIME_AUTO_START					25							//�Զ���ʼʱ��



//С״̬
#define MJ_STATUS_FREE				0									//���У��ȴ�׼��
#define MJ_STATUS_CON_ANIMATION		1									//�Կ�+����ǽ����
#define MJ_STATUS_CHIP				2									//��ע
#define MJ_STATUS_SEND_CARD			3									//����
#define MJ_STATUS_END				9									//����

//��Ϸ״̬����
#define  GS_WAIT_SETGAME			0									//�ȴ���������״̬
#define  GS_WAIT_ARGEE				1									//�ȴ�ͬ������
#define  GS_SEND_CARD				20									//����״̬
#define  GS_PLAY_GAME				22									//��Ϸ��״̬
#define  GS_WAIT_NEXT				23									//�ȴ���һ�̿�ʼ

//��Ϸ��Ϣ
//server to  client 
#define STC_SEND_GAME_STATUS			1006							//������ϷС״̬
#define STC_SEND_NOTIFY_OUT				1007							//֪ͨ�����ע
#define STC_SEND_CARD_INFO				1008							//��������
#define STC_SEND_UPDATE_SEAT			1009							//�������������Ϣ
#define STC_SEND_GAME_RESULT			1010							//������Ϸ������Ϣ 
#define STC_SEND_USER_AGREE				1011							//�������׼����Ϣ	
#define STC_SEND_GAME_RECORD			1012							//�����ƾ���Ϣ
#define STC_SEND_COUNT_DOWN				1013							//���͵���ʱ
#define STC_SEND_GAME_INFO				1017							//��Ϸ������Ϣ
#define STC_SEND_CHIP_RESULT			1019							//������ע���
#define STC_SEND_SEAT_RESULT			1020							//�����������
#define STC_SEND_NOSEATLIST_RESULT		1021							//���������б���




//���͹�����Ϣ
#define STC_UPDATE_USER_INFO			1101							//���������Ϣ	 
#define CTS_GET_GAME_CONFIG				1201							//�����ȡ��Ϸ����
#define CTS_UPDATE_USER_INFO			1202							//��������Լ���Ϣ

//client to server
#define CTS_RECV_USER_AGREE				2004							//�յ����׼����Ϣ
#define CTS_RECV_OUT_CARD				2005							//�յ������ע��Ϣ		
#define CTS_RECV_SEAT_OPERATE			2007							//�����������
#define CTS_RECV_SHOW_NOSEAT_LIST		2008							//�����ʾ�����б�


#define MAX_CARD_HAND				    3								//�Ƶĸ���
#define CARD_TOTAL_COUNT                52								//һ���Ƶ�����

#pragma pack(push,1)

//��ϷС״̬
typedef struct sc_notify_status
{
	unsigned int		gameSmallStatus;					//��ϷС״̬
	int					seat_userid[SEAT_COUNT];			//�������userid
	__int64				seat_user_score[SEAT_COUNT];		//������ҽ��
	sc_notify_status()
	{
		memset(this, 0, sizeof(sc_notify_status));
	}
}notify_status;

//·��ͼ
typedef struct game_user_line_map
{
	BYTE				cardtype;							//����
	BYTE				winArea;							//ʤ������ 1��2��
	game_user_line_map()
	{
		memset(this, 0, sizeof(game_user_line_map));
	}
	void initdata()
	{
		memset(this, 0, sizeof(game_user_line_map));
	}
}user_line_map;

//������Ϣ
typedef struct sc_hand_card_info
{
	BYTE	hand_card[2][MAX_CARD_HAND];					//������� 0�췽 1�ڷ�
	BYTE	cardtype[2];									//������
	sc_hand_card_info()
	{
		memset(this, 0, sizeof(sc_hand_card_info));
	}
}hand_card_info;

//֪ͨ�����ע
typedef struct sc_notify_out_card
{
	unsigned char                  chair_id;				        //���λ��
	unsigned int				   time_count;						//��Ҳ���ʱ�� 
	bool						   bCanChip[CHOUMA_COUNT];			//��Ӧ�����Ƿ����ע
	sc_notify_out_card()
	{
		memset(this, 0, sizeof(sc_notify_out_card));
		chair_id = 255;
	}
}notify_out_card;

//������Ϸ����
typedef struct stu_game_config
{
	unsigned int               tax;	                    //�����     
	unsigned int			   bet_limit;				//��ע����
	unsigned int			   seat_coin;				//�������
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
	unsigned char		game_status;								//��Ϸ״̬
	unsigned char		sm_gameStatus;								//��ϷС״̬
	unsigned int		free_remain_time;							//ʣ��ʱ��
	unsigned int		tax;										//�����	
	unsigned int        chouma[CHOUMA_COUNT];						//��ǰ��ע����
	unsigned char		chair_id;									//��ǰ���id
	__int64				total_score;								//��ǰ����ܷ�
	int					seatlist[SEAT_COUNT];						//�����б� �����ʱ���id 012������ 345������
	__int64				seat_total_score[SEAT_COUNT];				//�����б���ҵ��ܷ�
	bool				user_cut[SEAT_COUNT];						//�����б���Ҷ���״̬
	unsigned int		noSeatCount;								//�����б�����
	user_line_map		lineMap[LINE_COUNT];						//·��ͼ
	game_config			gameconfig;									//��Ϸ����		
	
	sc_game_base_info()
	{
		memset(this, 0, sizeof(sc_game_base_info));
		chair_id = 255;
		memset(seatlist,255,sizeof(seatlist));
		for (int i = 0; i < LINE_COUNT; i++)
		{
			lineMap[i].initdata();
		}
	}

}game_base_info;

//���������Ϣ 
typedef struct stu_update_user_info
{
	unsigned char	    chair_id;						//���
	unsigned char       userCard[3];                    //�������
	__int64		        taozhu_count[3];                //���������ע��ֵ
	__int64				dici_count[3];                  //�׳س���
	__int64		        user_total_score;			    //����ܷ�

	stu_update_user_info()
	{
		memset(this, 0, sizeof(stu_update_user_info));
	}

}update_user_info;

//��Ϸ��ʼ�׶�
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//������Ϣ
	unsigned int		player_num;									//������� 
	unsigned char       userCard[2][MAX_CARD_HAND];                 //��ڵ���
	unsigned char       cardtype[2];                                //�Ƶ�����
	__int64				area_total_bet[AREA_COUNT];					//���������������ע
	__int64				area_user_bet[AREA_COUNT];					//���������ע
	int					area_total_chouma[AREA_TOTAL_COUNT];		//��������ܳ���	0-4�� 5-9�� 10-14����һ��
	int					area_user_chouma[AREA_TOTAL_COUNT];			//������ҳ���	0-4�� 5-9�� 10-14����һ��
	
	sc_game_status_play()
	{
		memset(this, 0, sizeof(sc_game_status_play));
	}
}game_status_play;

//֪ͨ�����Ϸ����
typedef struct sc_notify_game_result
{
	unsigned int			 gameStatus;								//��Ϸ��״̬
	unsigned int			 smGameStatus;								//��ϷС״̬
	__int64					 seat_win_score[SEAT_COUNT];				//���������Ӯ��
	__int64					 seat_total_score[SEAT_COUNT];				//��������ܷ� 
	__int64					 win_score;									//�Լ���Ӯ��
	__int64					 total_score;								//�Լ��ܷ�
	BYTE					 winArea;									//1�췽ʤ 2�ڷ�ʤ
	user_line_map			 lineMap[LINE_COUNT];						//·��ͼ
	

	sc_notify_game_result()
	{
		memset(this, 0, sizeof(sc_notify_game_result));
	}
}notify_game_result;

//�����ע��Ϣ
typedef struct user_total_chip_info
{
	__int64    chip_count[3][CHOUMA_COUNT];	    //��ע��Ŀ  0��1��2����һ��  
	user_total_chip_info()
	{
		memset(chip_count, 0, sizeof(user_total_chip_info));
	}
	void initdata()
	{
		memset(chip_count, 0, sizeof(user_total_chip_info));
	}
}total_chip_info;

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

//�����ע���
typedef struct sc_user_out_chip_result
{	
	BYTE			chair_id;					 //��ע���λ��
	int				userid;						 //��ע���id
	BYTE			area;						 //��ע���� 1�� 2�� 3����һ��
	bool            isSeat;						 //�Ƿ���������� ������ ������
	int				xiazhumomey;				 //��ǰ��עֵ
	__int64			area_total_bet[AREA_COUNT];  //���������������ע
	__int64			area_user_bet[AREA_COUNT];	 //���������ע
	BYTE			byresult;					 //��ע������  0�ɹ� 1ʧ�� 2��Ҳ��� 3��ע����Ѵ�ⶥ
	sc_user_out_chip_result()
	{
		memset(this, 0, sizeof(sc_user_out_chip_result));
		chair_id = 255;
	}

}user_out_chip_result;

//�������
typedef struct sc_user_seat_result
{
	BYTE			byFlag;					  //1���� 2����
	BYTE			chair_id;				  //���id
	BYTE			seatStation;			  //��λ�� 1-7
	sc_user_seat_result()
	{
		byFlag = 0;
		chair_id = 255;
		seatStation = 0;
	}

}user_seat_result;

//��ʾ�����б���
typedef struct sc_user_show_noseat_result
{
	unsigned int    userid[NOSITLENGTH];		//���id
	long long		user_money[NOSITLENGTH];	//���
	long long		total_bet[NOSITLENGTH];		//��ʮ����ע�ܽ��
	int				win_num[NOSITLENGTH];		//��ʮ�ֻ�ʤ����
	
	sc_user_show_noseat_result()
	{
		memset(this, 0, sizeof(sc_user_show_noseat_result));
	}

}user_show_noseat_result;

//���͵���ʱ
//��ע����ʱ
typedef struct sc_count_down
{
	int			  remain_time;									//ʣ��ʱ��		
	sc_count_down()
	{
		remain_time = 0;
	}
}count_down;

//C-->S
//��Ϣ�ṹ��

//��Ҽ�ע���ע
typedef struct cs_user_out_chip_operate
{
	BYTE		    chair_id;						//���λ��
	BYTE			area;							//��ע���� 0�� 1�� 2����һ��
	INT				xiazhumoney;					//��ע���
	cs_user_out_chip_operate()
	{
		memset(this, 0, sizeof(cs_user_out_chip_operate));
		chair_id = 255;
	}
}user_out_chip_operate;

//��������
typedef struct cs_user_seat_operate
{
	BYTE			byFlag;					  //1���� 2����
	BYTE			chair_id;				  //���id
	BYTE			seatStation;			  //��λ�� 1-7
	cs_user_seat_operate()
	{
		byFlag = 0;
		seatStation = 0;
		chair_id = 255;
	}

}user_seat_operate;

//��Ҵ���
typedef struct cs_user_rub_operate
{
	BYTE chair_id;				//�������
	BYTE byArea;				//�������� 1�� 2��
	cs_user_rub_operate()
	{
		chair_id = 255;
		byArea = 0;
	}
}user_rub_operate;

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

//��Ҳ鿴��������б�
typedef struct cs_user_show_noseat_oper
{
	BYTE	chair_id;				
	cs_user_show_noseat_oper()
	{
		chair_id = 255;
	}
}user_show_noseat_oper;

#pragma pack(pop)

#endif