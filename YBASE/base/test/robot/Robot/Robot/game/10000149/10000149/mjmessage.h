#pragma once
#ifndef __MJMESSAGE_H_
#define __MJMESSAGE_H_
//�齫��ض���



#define  MAX_CARD_NUM				144									//���������
#define  NORMAL_CARD_NUM			25									//��ҳ���ֵ 	
#define  CARD_COUNT					100									//������ 
#define  KING_CARD_NUM				10									//������Ӹ���
#define  MAX_OUT_CARD				40									//�������� 
#define	 MASK_COLOR					0xF0								//��ɫ����
#define	 MASK_VALUE					0x0F								//��ֵ����
#define  MAX_FLOWER_NUM				20									//���������	
#define  USER_CARD_MAX				18									//�������������������ƺ����ƣ�
#define  HAND_CARD_COUNT			14									//���������
#define  CARD_INDEX_NUM				0x40								//��ֵ����
#define  JIANG_NUM					7									//�������
#define  BLOCK_MAX					5									//���������

//��������
#define	 BLOCK_TYPE_ERR				0x00								//�Ƿ����� 
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
#define MJ_CARD_FLOWER_CHUN			0x38									// ��
#define MJ_CARD_FLOWER_XIA			0x39									// ��
#define MJ_CARD_FLOWER_QIU			0x3A									// ��
#define MJ_CARD_FLOWER_DONG			0x3B									// ��
#define MJ_CARD_FLOWER_MEI			0x3C									// ÷
#define MJ_CARD_FLOWER_LAN			0x3D									// ��
#define MJ_CARD_FLOWER_ZHU			0x3E									// ��
#define MJ_CARD_FLOWER_JU			0x3F									// ��

#pragma pack(push,1)

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
	str_game_mode()
	{
		bqidui = false;
		bmixjiang = false;
		blaizi = false;
		bchi = false;
		barrow = false;
		bwind = false;
		bpeng = false;
		bgang = false;
		b131 = false;
	}

}game_mode;

//�������� 
typedef struct str_block_card_info
{
	unsigned char	block_type;	//��������
	unsigned char	block_card[4];//Ҫ������
	 
}block_card_info;
 


#pragma pack(pop)
#endif//__MJMESSAGE_H_
