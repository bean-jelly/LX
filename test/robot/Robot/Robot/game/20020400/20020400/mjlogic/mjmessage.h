#pragma once
#ifndef __MJMESSAGE_H_
#define __MJMESSAGE_H_
//�齫��ض��� 
//#include <windows.h>
#include "memory.h"

typedef unsigned char	BYTE;


#define  MAX_CARD_NUM				144									//���������
#define  NORMAL_CARD_NUM			34									//��ҳ���ֵ 	
#define  CARD_COUNT					136									//������ 
#define  KING_CARD_NUM				10									//������Ӹ���
#define  MAX_OUT_CARD				60									//�������� 
#define	 MASK_COLOR					0xF0								//��ɫ����
#define	 MASK_VALUE					0x0F								//��ֵ����
#define  MAX_FLOWER_NUM				20									//���������	
#define  USER_CARD_MAX				18									//�������������������ƺ����ƣ�
#define  HAND_CARD_COUNT			14									//���������
#define  CARD_INDEX_NUM				0x40								//��ֵ����
#define  JIANG_NUM					7									//�������
#define  BLOCK_MAX					5									//���������

//��������
#define	 BLOCK_TYPE_GAVE			0x00								//�� 
#define  BLOCK_TYPE_CHI				0x01								//��
#define  BLOCK_TYPE_PENG			0x02								//��
#define  BLOCK_TYPE_GANG			0x04								//��
#define  BLOCK_TYPE_TING			0x08								//��
#define  BLOCK_TYPE_HU				0x10								//��

//��������
#define BLOCK_TYPE_CHI_ERR			0x00								//���ʹ���
#define BLOCK_TYPE_CHI_FIS			0x01								//��ͷ
#define BLOCK_TYPE_CHI_MID			0x02								//����
#define BLOCK_TYPE_CHI_END			0x04								//��β

//��������
#define BLOCK_TYPE_GANG_ERR			0x00								//���ʹ���
#define BLOCK_TYPE_GANG_AN			0x01								//����
#define BLOCK_TYPE_GANG_MING		0x02								//����
#define BLOCK_TYPE_GANG_BU			0x04								//����
  
//�齫״̬
#define MJ_STATUS_FREE				0									//���У��ȴ�׼��
#define	MJ_STATUS_BANKER			1									//��ׯ
#define MJ_STATUS_DICE				2									//������
#define MJ_STATUS_SEND				3									//����, ����13��
#define MJ_STATUS_DRAW				4									//����, ׯ��14�Ż�ܣ�������
#define MJ_STATUS_BLOCK				5									//����
#define MJ_STATUS_BLOCK_WAIT		6									//���Ƴ��Ƶȴ�
#define MJ_STATUS_GIVE				7									//����
#define MJ_STATUS_DRAW_WAIT			8									//���Ƴ��Ƶȴ�
#define MJ_STATUS_END				9									//����
#define MJ_STATUS_KING				10									//���� 
#define MJ_STATUS_FLOWER			11									//�����׶�

#define MJ_WIN_TYPE_HZ				0									//��ׯ
#define MJ_WIN_TYPE_ZM				1									//����
#define MJ_WIN_TYPE_DP				2									//����
#define MJ_WIN_TYPE_QG				3									//����
#define MJ_WIN_TYPE_GK				4									//�ܿ�

//�齫��ֵ
//��
#define	MJ_CARD_EMPTY				0x00									//����
#define	MJ_CARD_WAN_1				0x01									//һ��
#define	MJ_CARD_WAN_2				0x02									//����
#define	MJ_CARD_WAN_3				0x03									//����
#define	MJ_CARD_WAN_4				0x04									//����
#define	MJ_CARD_WAN_5				0x05									//����
#define	MJ_CARD_WAN_6				0x06									//����
#define	MJ_CARD_WAN_7				0x07									//����
#define	MJ_CARD_WAN_8				0x08									//����
#define	MJ_CARD_WAN_9				0x09									//����  
//��
#define	MJ_CARD_TIAO_1				0x11									//һ��
#define	MJ_CARD_TIAO_2				0x12									//����
#define	MJ_CARD_TIAO_3				0x13									//����
#define	MJ_CARD_TIAO_4				0x14									//����
#define	MJ_CARD_TIAO_5				0x15									//����
#define	MJ_CARD_TIAO_6				0x16									//����
#define	MJ_CARD_TIAO_7				0x17									//����
#define	MJ_CARD_TIAO_8				0x18									//����
#define	MJ_CARD_TIAO_9				0x19									//����
//Ͳ
#define	MJ_CARD_TONG_1				0x21									//һͲ
#define	MJ_CARD_TONG_2				0x22									//��Ͳ
#define	MJ_CARD_TONG_3				0x23									//��Ͳ
#define	MJ_CARD_TONG_4				0x24									//��Ͳ
#define	MJ_CARD_TONG_5				0x25									//��Ͳ
#define	MJ_CARD_TONG_6				0x26									//��Ͳ
#define	MJ_CARD_TONG_7				0x27									//��Ͳ
#define	MJ_CARD_TONG_8				0x28									//��Ͳ
#define	MJ_CARD_TONG_9				0x29									//��Ͳ 
//��
#define MJ_CARD_WIND_EAST			0x31									//����
#define MJ_CARD_WIND_SOUTH			0x32									//�Ϸ�
#define MJ_CARD_WIND_WEST			0x33									//����
#define MJ_CARD_WIND_NORTH			0x34									//����
//����
#define MJ_CARD_ARROW_HZ			0x35									//����
#define MJ_CARD_ARROW_FC			0x36									//����
#define MJ_CARD_ARROW_BB			0x37									//�װ�
//�� 

#define MJ_CARD_FLOWER_MEI			0x38									// ÷
#define MJ_CARD_FLOWER_LAN			0x39									// ��
#define MJ_CARD_FLOWER_ZHU			0x3A									// ��
#define MJ_CARD_FLOWER_JU			0x3B									// ��
#define MJ_CARD_FLOWER_CHUN			0x3C									// ��
#define MJ_CARD_FLOWER_XIA			0x3D									// ��
#define MJ_CARD_FLOWER_QIU			0x3E									// ��
#define MJ_CARD_FLOWER_DONG			0x3F									// ��

#pragma pack(push,1)

//��������
enum FAN_TYPE
{
	FAN_TYPE_PIHU = 0,		//ƽ�� 0
	FAN_TYPE_QIDUI,			//�߶� 1
	FAN_TYPE_QINGYISE,		//��һɫ 2
	FAN_TYPE_HUNYISE,		//��һɫ 3
	FAN_TYPE_PPH,			//������ 4
	FAN_TYPE_MENQING,		//���� 5
	FAN_TYPE_DADIAO,		//��� 6 
	FAN_TYPE_YJ,			//�۾� 11
	FAN_TYPE_QF,			//ȫ��--ȫ�ɷ������� 12
	FAN_TYPE_13Y,			//ʮ���� 13
	FAN_TYPE_QIDUI_1,		//һ���߶� 14
	FAN_TYPE_QIDUI_2,		//�����߶� 15
	FAN_TYPE_QIDUI_3,		//�����߶� 16
	FAN_TYPE_SSL,			//13��
	FAN_TYPE_QXSSL,			//����13��
	FAN_TYPE_COUNT_1,
	FAN_TYPE_COUNT_2,
	FAN_TYPE_COUNT_3,
	FAN_TYPE_COUNT_4,
	FAN_TYPE_COUNT_5,
	FAN_TYPE_COUNT_6,
	FAN_TYPE_COUNT_7,
	FAN_TYPE_COUNT_8,
	FAN_TYPE_COUNT_9,
	FAN_TYPE_COUNT_10,
	FAN_TYPE_COUNT_11,
	FAN_TYPE_COUNT_12,
	FAN_TYPE_COUNT_13,
	FAN_TYPE_COUNT_14,
	FAN_TYPE_COUNT_15,
	FAN_TYPE_COUNT_16,
	FAN_TYPE_COUNT_17,
	FAN_TYPE_COUNT_18,
	FAN_TYPE_COUNT_19,
	FAN_TYPE_COUNT_20,
	FAN_TYPE_COUNT_21,
	FAN_TYPE_COUNT_22,
	FAN_TYPE_COUNT_23,
	FAN_TYPE_COUNT_24,
	FAN_TYPE_COUNT_25,
	FAN_TYPE_COUNT
};

//������Ϸģʽ
typedef struct str_game_mode
{
	bool      bqidui;			//�߶�
	bool	  bmixjiang;		// �ҽ�
	bool	  blaizi;			//����
	bool	  bchi;				//��
	bool	  barrow;			//�Լ�
	bool	  bwind;			//�Է�
	bool      bpeng;			//��
	bool	  bgang;			//��
	bool	  b131;				//13��
	bool	  bqbk;				//13�ҡ�ȫ����
	str_game_mode()
	{
		memset(this, 0, sizeof(str_game_mode)); 
	}

}game_mode;



//���������� 
typedef struct str_block_data
{
	unsigned char card_value[4];	 //������������
	unsigned char chair_id;			 //��˭��
	unsigned char blocked_card;      //��������
	unsigned char block_type;		 //��������
	unsigned char block_sub_type;	 //�ԡ���������
	str_block_data()
	{
		memset(this, 0, sizeof(str_block_data));
	}
}BlockData;

//�������� 
typedef struct str_block_card_info
{
	unsigned char	block_type;	//��������
	unsigned char	block_card[4];//Ҫ������
	str_block_card_info()
	{
		block_type = 0x00;
		block_card[0] = 0x00;
		block_card[1] = 0x00;
		block_card[2] = 0x00;
		block_card[3] = 0x00;
	}
	 
}block_card_info;
 

//������������
typedef struct str_ting_data
{
	unsigned char  ting_card[HAND_CARD_COUNT][NORMAL_CARD_NUM + 1];//��һ��λ�ñ�ʾ������,�����Ӧ������

	str_ting_data()
	{
		memset(this, 0, sizeof(str_ting_data));
	}

}TingData;

//�齫������Ϣ
typedef struct str_mj_config
{
	bool							m_wan;									//��������
	bool							m_tiao;									//��������
	bool							m_tong;									//��Ͳ����
	bool							m_wind;									//�з�����
	bool							m_arrow;								//�м�����
	bool							m_flower;								//�л�����
	bool							m_allow_dianpao;						//�Ƿ��������
	bool							m_allow_qgh;							//�Ƿ��������ܺ� 
	bool							m_allow_mul_win;						//�Ƿ�����һ�ڶ���
	bool							m_must_ting;							//�Ƿ���Ҫ���ƿɺ�
	bool							m_allow_auto_win;						//�Ƿ������Զ�����
	bool							m_allow_block_check;					//�Ƿ��������ƺ���
	bool							m_allow_gang_after;						//�Ƿ�������ָ�
	bool							m_allow_ting_gang;						//�Ƿ����������
	bool							m_allow_gang;							//�Ƿ������
	bool							m_an_gang_show;							//�����Ƿ���ʾ
	bool							m_allow_peng;							//�Ƿ������� 
	bool							m_allow_chi;							//�Ƿ������
	bool							m_allow_chi_bwind;						//�Ƿ�����Է�
	bool							m_allow_chi_barrow;						//�Ƿ�����Լ� 
	bool							m_allow_flower;							//�Ƿ���Ҫ����
	bool							m_allow_laizi;							//�Ƿ�����������
	unsigned int					m_laizi_count;							//����������
	bool							m_allow_luanjiang;						//�Ƿ������ҽ�
	bool							m_allow_qxd;							//�Ƿ������߶�
	bool							m_allow_131;							//�Ƿ�����ʮ����
	bool							m_allow_qbk;							//�Ƿ�����ȫ����
	bool							m_count_gang;							//����ܷ�
	bool							m_follow_banker;						//�Ƿ��ׯ
	unsigned int					m_huang_score;							//��ׯ����
	unsigned int					m_follow_score;							//��ׯ����
	bool							m_block_ting_info;						//������ʱ�Ƿ���������Ϣ
	bool							m_out_ting_info;						//����ʱ���Ƿ���������Ϣ

	bool							m_gave_win;								//�Ƿ�����
	bool							m_gave_peng;							//�Ƿ�����
	bool							m_gave_chi;								//�Ƿ�����
	bool							m_allow_auto;							//�Ƿ������Զ�����
	//��������
	unsigned int					m_player_mode;							//���ģʽ  0:���� 1��ȫ��ͬ��
	unsigned int					m_min_player;							//��������
	unsigned int					m_send_card_mode;						//����ģʽ 0:(ÿ��13)   1:(4,4,4,1ģʽ)
	//��ɢģʽ
	unsigned int					m_free_mode;							//��ɢģʽ  0:������ͬ��  1���󲿷���ͬ��
	//ʱ����
	unsigned int					m_time_dice;							//����ʱ��
	unsigned int					m_time_send_card;						//����ʱ��
	unsigned int					m_time_auto;							//�Զ�����ʱ�� 
	unsigned int					m_time_king_card;						//��������ʱ��
	unsigned int					m_time_send_one_card;					//����һ����ʱ��
	unsigned int					m_time_user_out_card;					//��ҳ���ʱ��
	unsigned int					m_time_user_draw_card;					//�������ʱ��

	str_mj_config()
	{
		memset(this, 0, sizeof(str_mj_config)); 
	}
}mj_config;

#pragma pack(pop)
#endif//__MJMESSAGE_H_
