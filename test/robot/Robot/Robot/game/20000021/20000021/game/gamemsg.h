#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_ 
#include <windows.h>
#include <stdarg.h> 
#include "basictable\GameRoomMsg.h"

#define PLAY_COUNT						3						             // ��Ϸ���� 

//��Ϸ����״̬
#define  GS_GAME_LOOK_STATION       1           
#define  GS_GAME_PLAYING            2

//��Ϸ״̬����
#define  GS_WAIT_SETGAME			0			    //�ȴ���������״̬
#define  GS_WAIT_ARGEE				1				//�ȴ�ͬ������
#define  GS_SEND_CARD				20				//����״̬
#define  GS_WAIT_BACK				21				//�ȴ���ѹ����
#define  GS_PLAY_GAME				22				//��Ϸ��״̬
#define  GS_WAIT_NEXT				23				//�ȴ���һ�̿�ʼ 

//��Ϸ���ݶ���
#define  ALL_CARD_COUNT             54             //�������
#define  HAND_CARD_MAX				20             //���������
#define  OTHER_CARD_COUNT           17             //�����˷����Ƶ�����
#define  BACK_CARD_COUNT            3		       //����
#define	 ERROR_PLAYER_POS           0XFF           //��Ҵ���λ�� 
#define  HAND_CARD_PIINT_TYPE		18             //�Ƶ�������

//��Ϸ�е�С״̬
enum  EnumNotifyFlag : BYTE
{
	STATUS_FLAG_GAME_NORMAL = 0,       //ͨ��״̬
	STATUS_FLAG_GAME_BEGIN = 1,		//��ʼ
	STATUS_FLAG_SEND_CARD = 2,       //��ҷ���
	STATUS_FLAG_CALL_SCORE = 3,		//�з�
	STATUS_FLAG_ROB_NT = 4,		//������
	STATUS_FLAG_ADD_DOUBLE = 5,		//�ӱ�
	STATUS_FLAG_SHOW_CARD = 6,       //����
	STATUS_FLAG_NT_ADD_DOUBLE = 7,       //�����ӱ�
	STATUS_FLAG_OUT_CARD = 8,		//��ʼ����
	STATUS_FLAG_GAME_FINISH = 9,       //��Ϸ����
	STATUS_FLAG_CHANGE_CARD = 10,		//������
	STATUS_FLAG_CUT_LIST = 11,      //����
	STATUS_FLAG_SHOW_FIRST = 12,    //������
	OPERATE_FLAG_AUTO = 20       //�йܲ���
};


enum GAME_STATUS
{
	STATUS_FREE = 1,				//����״̬
	STATUS_PLAY,					//��Ϸ��״̬(����չ)
	STATUS_END						//����״̬
}; 

//���״̬
enum USER_STATUS
{
	USER_SEE = 0,				//�Թ�
	USER_JOIN					//������Ϸ
};
 
 
enum Game_Sprint_Type
{
	type_sprint_normal = 0,
	type_sprint_fan = 1,
	type_sprint_sucess = 2
};

//����¼���¼���� 30��ʼ
#define	   RECORD_PRIVATE_INFO            30					//������Ϣ
#define	   RECORD_GAME_RULE				  31					//��Ϸ����
#define	   RECORD_USER_INFO				  32					//��Ϸ����
#define	   RECORD_NOTIFY_INFO			  33					//֪ͨ����Ϣ
#define	   RECORD_SEND_CARD				  34					//������Ϣ
#define	   RECORD_CHANGE_CARD			  35					//��������Ϣ
#define	   RECORD_DEAL_RES				  36					//��Ҳ���������
#define	   RECORD_BACK_CARD				  37					//������Ϣ
#define	   RECORD_OUT_CARD				  38					//���ƽ����Ϣ
#define	   RECORD_NEW_TRUN				  39					//��һ����Ϣ
#define	   RECORD_AWARD_POINT			  40					//������
#define	   RECORD_GAEM_END				  41					//��Ϸ����
#define	   RECORD_COIN_CHANGE			  42					//��ҽ�ұ仯
#define	   RECORD_SHOW_CARD				  43					//�������

//��Ϸ��ʱ�� 1-50   
#define   ONE_SECOND_VALUE		  1000
///��ʱ������
#define	   GAME_TIME_AUTO_LEFT	   19	  //�Զ�����׼�������
#define    GAME_TIME_SEND_CARD     20     //����
#define    GAME_TIME_CALL_SCORE    21	  //�з�
#define    GAME_TIME_ROB_NT        22	  //������
#define    GAME_TIME_ADD_DOUBLE    23     //�ӱ�
#define    GAME_TIME_SHOW_CARD     24     //����
#define    GAME_TIME_OUT_CARD      25     //����
#define    GAME_TIME_NEW_TURN      26     //��һ��
#define    GAME_TIME_RESLUT        27     //����
#define    GAME_TIME_CONNECT       28     //��������
#define    GAME_TIME_FREE_DESK     32     //��ҽ�ɢ��ʱ��
#define    GAME_TIME_READY         33     //��Ϸ׼��
#define    GAME_TIME_CHANGE_CARD   34     //������ 
#define    GAME_TIME_CHANGE_RES    35	  //���ƽ��
#define    GAME_TIME_CUT_LIST      36     //֪ͨ�������
#define    GAME_TIME_CUT_ANI       37     //֪ͨ�������
#define    GAME_TIME_SHOW_FIRST    38     //֪ͨ�������չʾ������
#define    GAME_TIME_READY_FOR_NEXT 39    //�������һ��׼���Ľ�ɢ����ʱ
#define	   GAME_TIME_CLEAR_CUT_USER 40	  //����������
#define    GAME_TIME_NOTIFY_OUT_CARD 41	  //֪ͨ��ҳ���
//��Ϸ��Ϣ 
//send data from server to  client
//S->C
#define   GAME_MSG_SEND_ALL_CARD			1010				//����������(һ�·���ȫ��)
#define   GAME_MSG_GAME_MULTIPLE			1020				//��Ϸ����(���������ӱ�)
#define   GAME_MSG_BACK_CARD_EX				1030				//��������
#define   GAME_MSG_NOTIFY_INFO              1040                //�����֪ͨ�ͻ�����Ϣ
#define   GAME_MSG_DEAL_RES                 1041                //����˴��������͸��ͻ���
#define	  GAME_MSG_SHOW_CARD                1042                //����˷���������Ϣ
#define   GAME_MSG_CHANGE_CARD              1043                //����
#define   GAME_MSG_USER_AGREE               1044                //���ͬ��
#define   GAME_MSG_USER_COIN_CHANGE         1045                //�����֪ͨ�ͻ�����ҽ�ұ仯������������ʱ��ұ仯��
#define   GAME_MSG_USER_NEED_CHANGE_DESK	1046				//������������Ҫ������Ϣ
#define   GAME_MSG_SEND_CARD_TO_ROBOT		1047				//�����������������Ƹ�������

#define   GAME_MSG_OUT_CARD_RESULT			1050				//���ƽY��
#define   GAME_MSG_NEW_TURN					1060				//��һ�ֿ�ʼ
#define   GAME_MSG_AWARD_POINT				1070				//����(ը�����)
#define   GAME_MSG_SET_GAEM_END				1080				//��Ϸ����


//recv data from client to  server 
#define   GAME_MSG_USER_OPER_INFO           2010                //��Ҳ������
#define   GAME_MSG_OUT_CARD					2020				//�û����� 


#pragma pack(push,1)
//��Ϸ�߼��У���Ҫ�Ľṹ��
//��������
typedef struct sg_game_basic_config
{
	unsigned int					m_free_mode;							//��ɢģʽ  0:������ͬ��  1���󲿷���ͬ��
	unsigned int					m_player_mode;							//���ģʽ  0:���� 1��ȫ��ͬ��
	unsigned int					m_min_player;							//��������
	sg_game_basic_config()
	{
		memset(this, 0, sizeof(sg_game_basic_config));
	}

}game_basic_config;
 

//������Ϸ����-������Ϸ��ͬ������
typedef struct sg_game_config
{  
	int				iSetRoomCount;    //���ô������
	int				iCostOpenCard;    //��Ӧ�������ĵķ�������
	unsigned char	byLimitScore;     //�Ƿ�����ը������
	bool			bTakePair;        //�Ƿ���Դ���
	int				baseCoinScore;	  //��ҵ׷�
	int				iminCoinAccess;	  //׼������
	unsigned char	byplaymode[10];   //��Ϸ�淨0��(˫���ĸ�2�ؽ�3��) 1��(--����ģʽ��1-->���ƣ�2-->�ӱ�),2���Ƿ�����,3�з�������ģʽ(0,1^2�֣�1 1^3�� 2��������),4��ϴ�ƣ�0��ϴ��1��ϴ��5����������6�����ƴ��� 7�׽�(0��˳���׽�,1��Ӯ���׽�,2������׽�)8,��̨��

	sg_game_config()
	{
		memset(this, 0, sizeof(sg_game_config));
	}
	void InitConfig()
	{
		memset(this, 0, sizeof(sg_game_config));
	} 
}game_config;

 
//�����Ϣ
typedef struct sg_game_user_info
{
	unsigned int	user_id;				//���ID 
	unsigned char	chair_id;				//��������ϵ�λ��
	unsigned int	game_status;			//��Ϸ״̬ 0���Թ� 1:������Ϸ
	bool			cut_status;				//����״̬ false������ true������
	bool			user_ready;				//����Ƿ�׼�� false��δ׼�� true��׼��
	long long		game_score;				//��Ϸ����(���ݲ�ͬ���ң����費ֵͬ)
	char			nickname[60];			//����ǳ�
	char			head_url[200];			//ͷ��·��  
	void initdata()
	{
		memset(this, 0, sizeof(sg_game_user_info));
		chair_id = 255;
	}
	sg_game_user_info()
	{
		memset(this, 0, sizeof(sg_game_user_info));
		chair_id = 255;
	}
}game_user_info;


/********************************************************************************/
//��Ϸ״̬���ݰ�	�� �ȴ�������ҿ�ʼ ��
struct GSBaseInfo
{
	game_config		gameconfig;						//��Ϸ����
	BYTE			byStation;						//��Ϸ״̬
	BYTE			iBeginTime;						//��ʼ׼��ʱ��
	BYTE			iThinkTime;						//����˼��ʱ��
	BYTE			iCallScoreTime;					//�зּ�ʱ
	BYTE			iAddDoubleTime;					//��ʱ��
	BYTE			iBackCount;						//������
	UINT			user_id[PLAY_COUNT];			//���id	
	long long		user_score[PLAY_COUNT];			//��ҷ���
	bool			bArgee[PLAY_COUNT];				//�û�ͬ��	
	bool			user_cut[PLAY_COUNT];			//���״̬
	UINT			cur_play_count;					//��ǰ����	
	BYTE			my_station;						//�Լ���λ�� 
	UINT			out_count[PLAY_COUNT];			//��ʱ 
	BYTE            bychangetime;                   //������ʱ��
	BYTE            bycutlisttime;                  //����ʱ��
	int				tax;							//̨��
	BYTE            byBuyaoTime;					//Ҫ����ʱ��
	unsigned int    freeGametime;                   //��ɢ����ʱ��
	bool            bfirstcount;                    //�Ƿ��׾�
};
//��Ϸ״̬���ݰ�	�� �ȴ���Ѻ����״̬ ��
struct GameStation_PlayPre
{
	BYTE		byGameFlag;										//�зֱ��
	BYTE		byCallScorePeople;								//��ǰ�з���
	BYTE		byCallScoreResult;								//���еķ�
	BYTE		byNtPeople;										//ׯ��λ��
	int			iLeaveTime;										//������ʱ��ʣ��ʱ��
	BYTE		byDouble[PLAY_COUNT];							//�ӱ����
	bool		bAuto[PLAY_COUNT];								//�й����
	BYTE		byShowCard[PLAY_COUNT];							//�������
	BYTE		byCallScore[PLAY_COUNT];						//���ҽз����
	BYTE		byRobNT[PLAY_COUNT];							//������
	BYTE		byUserCardCount[PLAY_COUNT];					//�û������˿���Ŀ
	BYTE		byUserCardList[PLAY_COUNT][HAND_CARD_MAX];		//�û����ϵ��˿�(������ҿ�������)
	BYTE        byBackCount;                                    //��������
	BYTE        byBackCard[BACK_CARD_COUNT];                    //��������
	BYTE        bychoosestate[PLAY_COUNT];						//ѡ��״̬
	BYTE        bychoosecardlist[3];                            //ѡ�����
	BYTE        byCurrentRobNt;                                 //��ǰ�е������
	BYTE        bycutuser;                                      //�������
	BYTE        bycutnum;                                       //��������
	int         ibase;                                          //��������
	GSBaseInfo  gamebaseInfo;									//��������
};
//��Ϸ״̬���ݰ�	�� ��Ϸ��״̬ ��
struct GameStation_Playing
{
	bool		bIsLastCard;						//�Ƿ�����������		
	BYTE		bIsPass;							//�Ƿ񲻳�
	BYTE		cbRobNtMul;							//����������
	BYTE		cbShowCardMul;						//���Ʊ���
	BYTE		cbBkCardMul;						//���Ʊ���
	BYTE		cbBombMul;							//ը������
	int			iBase;								//��ǰ����
	int 		iCallScoreResult;					//�зֽ��
	int			iLeaveTime;							//������ʱ��ʣ��ʱ��	
	BYTE		byBombNum;							//ը������
	BYTE		byNTPeople;							//ׯ��λ��
	BYTE		byOutCardPeople;					//���ڳ����û�
	BYTE		byFirstOutPeople;					//�ȳ��Ƶ��û�
	BYTE		byBigOutPeople;						//�ȳ��Ƶ��û�
	BYTE		byRobNT[PLAY_COUNT];				//������
	BYTE		byDouble[PLAY_COUNT];				//�ӱ����
	BYTE		byShowCard[PLAY_COUNT];				//�������
	bool		bAuto[PLAY_COUNT];					//�й����
	BYTE		byDeskCardCount[PLAY_COUNT];			 //�����˿˵���Ŀ
	BYTE        byDeskCard[PLAY_COUNT][HAND_CARD_MAX]; //�����ϵ��˿� 
	BYTE		byLastCardCount[PLAY_COUNT];				//�����˿˵���Ŀ
	BYTE		byLastOutCard[PLAY_COUNT][HAND_CARD_MAX];//���ֵ��˿�
	BYTE        byLastPass;                                //��һ�ֲ�Ҫ�����
	BYTE		byUserCardCount[PLAY_COUNT];			     //�û������˿���Ŀ
	BYTE		byUserCardList[PLAY_COUNT][HAND_CARD_MAX];//�û����ϵ��˿�
	BYTE        byBackCount;                                    //��������
	BYTE        byBackCard[BACK_CARD_COUNT];                    //��������
	GSBaseInfo  gamebaseInfo;						//��������	
};

 
//S-->C
//��Ϸ��Ϣ�ṹ�� 
//��Ϸ״̬������Ϣ
typedef struct sc_game_base_info
{
	game_config			gameconfig;					//��Ϸ���� 
	unsigned char		game_status;				//��ǰ����״̬
	unsigned char		cur_status;					//��ǰ��Ϸ�׶� 
	unsigned char		my_station;					//�Լ���λ�� 
	sc_game_base_info()
	{ 
		game_status = USER_LOOK_STATE;
		cur_status = STATUS_FREE;
		my_station = 255; 
	}

}game_base_info;
//��Ϸ׼���׶�
typedef struct sc_game_status_ready
{ 
	game_base_info		base_info;
	game_user_info		user_info[PLAY_COUNT];						//�����Ϣ  
	sc_game_status_ready()
	{ 

	}

}game_status_ready;
    
//��Ϸ��ʼ�׶�
typedef struct sc_game_status_play
{
	game_base_info		base_info;									//������Ϣ
	game_user_info		user_info[PLAY_COUNT];						//�����Ϣ   
	unsigned int		msg_index;									//��Ϣ����-����������Ϣ����
	unsigned int		player_num;									//������Ϸ������ 

	sc_game_status_play()
	{
		msg_index = 0;
		player_num = 0; 
	}
}game_status_play;
 
////֪ͨ�����Ϸ����
//typedef struct sc_notify_game_result
//{  
//	unsigned char			hand_card[PLAY_COUNT][HAND_CARD_MAX];		//�������
//	unsigned char			card_type[PLAY_COUNT];						//����
//	int						 win_score[PLAY_COUNT];						//��Ӯ��
//	int						 total_score[PLAY_COUNT];					//����ܷ�
//	 
//	sc_notify_game_result()
//	{
//		memset(this, 0, sizeof(sc_notify_game_result)); 
//	}
//}notify_game_result;
  
//֪ͨ���ͬ��
typedef struct sc_user_agree
{
	unsigned int	user_id;			//���ID
	unsigned char   chair_id;			//���λ��
	bool			bagree;				//�Ƿ�ͬ��
	sc_user_agree()
	{
		user_id = 0;
		chair_id = 255;
		bagree = false;
	} 
}user_agree;

//֪ͨ���ͬ��
typedef struct sc_user_agree_i
{
	bool		    b_agree[PLAY_COUNT];			//����Ƿ�ͬ����Ϣ  
	unsigned char	bytime;							//���׼������ʱ  0��������  
	sc_user_agree_i()
	{
		memset(b_agree, 0, sizeof(b_agree));
		bytime = 0;
	}
}user_agree_i;

//֪ͨ��ҽ�ұ仯��������С�ֽ�ҽ���仯��
typedef struct sc_user_money_change
{
	unsigned char   chair_id;						//����Ƿ�ͬ����Ϣ  PLAY_COUNT:�������
	unsigned char   money_type;						//������� 0:���  
	long long		cur_money[PLAY_COUNT];			//��һ��ҵ�ǰֵ
	sc_user_money_change()
	{
		chair_id = 255;
		money_type = 255;
		memset(cur_money, 0, sizeof(cur_money));
	}
}user_money_change;

//�û��������ݰ� �������������
struct OutCardStruct
{
	BYTE        byDeskStation;                  //���λ��
	BYTE		byCardCount;						//�˿���Ŀ
	BYTE		byCardList[HAND_CARD_MAX];		//�˿���Ϣ
};
//֪ͨ�����Ϣ
struct  UserOperStruct
{
	BYTE      byFlag;					 //������ʶ	
	BYTE      byChairID;				 //���λ��
	BYTE      byOperValue;               //����ֵ
	BYTE	  byFlagEx[3];              //���ӱ�ʶ(�����ŵ���ֵ)
};
//S-->C
//��Ҫ����
struct NeedChangeDeskStruct
{
	BYTE	byChairID;            //���λ��
	bool	bisNeed;			  //�Ƿ���Ҫ����
};


//����
struct  ChangeCardStruct
{
	bool    bAntiCloseWise;       //�Ƿ���ʱ��    
	BYTE	byDeskPos;            //���λ��
	BYTE	byChooseCardList[3];  //���ѡ�����
	BYTE    byGetCardList[3];     //��ҵõ�����
};
//����
struct ShowCardStruct
{
	BYTE		bDeskStation;									//����
	BYTE		iCardList[HAND_CARD_MAX];						//�˿���Ϣ
	BYTE		iCardCount;										//�˿���Ŀ
	int			iValue;											//����ֵ
	int			iBase;											//����
};
//֪ͨ�����Ϣ
struct  NotifyUserOperData
{
	BYTE      byChairID;				//���λ��
	BYTE      byFlag;					//������ʶ	
	BYTE      byChairIDEx;              //��չֵ,��ʼ��0����ʾ������ʼ��255����ʾ���˽з֣����¿�ʼ
	BYTE	  byFlagEx[PLAY_COUNT];     //���ӱ�ʶ
};
//�����������
struct SendAllStruct
{
	BYTE		byFirstCallScore;								//˭����(˭���Ƚз�)
	BYTE        byCardValue;                                    //���Ƶ���ֵ
	BYTE		byCardCount[PLAY_COUNT];						//��������
	BYTE		byCardList[HAND_CARD_MAX];						//���ƶ���	
};

//�������������
struct SendAllCardInfoStruct
{
	BYTE		byPreCardCount[HAND_CARD_MAX];						//�ϼ�������
	BYTE		byPostCardCount[HAND_CARD_MAX];						//�¼�������
	BYTE        byBackCard[BACK_CARD_COUNT];						//��������
};
//�ͻ��˴���������֮���͸��ͻ���
struct DealUserOperRes
{
	BYTE      byFlag;					//������ʶ	
	BYTE      byChairIDEx;              //��չֵ
	BYTE	  byValueEx;				//���ӱ�ʶ
};
//�������ݰ�
struct BackCardExStruct
{
	BYTE		iGiveBackPeople;				//�������
	BYTE		iBackCardCount;					//�˿���Ŀ
	BYTE		iBackCard[12];					//��������
};
//�û��������ݰ� ������ͻ��ˣ�
struct OutCardMsg
{
	BYTE		byCardCount;					//�˿���Ŀ
	BYTE		bDeskStation;					//��ǰ������	
	BYTE		iCardList[HAND_CARD_MAX];		//�˿���Ϣ
};
//��һ��
struct NewTurnStruct
{
	BYTE		bDeskStation;					//����
	BYTE		bReserve;						//����
};
struct AwardPointStruct
{
	BYTE	bDeskStation;									//����
	BYTE    cbRobNtCnt;										//����
	BYTE	iBombNum;										//ը������
};

//��Ϸ����ͳ�����ݰ�
struct notify_game_result
{
	long long		iTurePoint[PLAY_COUNT];			//��ҵ÷�
	int				iScoreMultiple[PLAY_COUNT];		//��ұ���
	BYTE			iUpGradeStation;				//ׯ��λ��
	BYTE			iCallScore;						//ׯ�ҽз�
	BYTE			iRobMul;						//���������Ʊ���
	BYTE			iBombCount;						//ը���ӱ�
	BYTE			iKingBomb;						//��ը
	int				iSpecialTaskMul;				//��������
	int				iTotalMul;						//�ܱ���
	BYTE			bySprint;                       //��������
	bool			bFinishTask[PLAY_COUNT];		//�Ƿ��������
	BYTE			byWinPeople;					//��˭Ӯ
	BYTE			iUserCardCount[PLAY_COUNT];		//�û������˿���Ŀ
	BYTE			iUserCard[PLAY_COUNT][HAND_CARD_MAX];		//�û����ϵ��˿�
	bool			bLimit[PLAY_COUNT];				//�÷��Ƿ���Ϊ��Ӯ����������
	int				iDouble[PLAY_COUNT];			//���Ҽӱ����
	int				iPower[PLAY_COUNT];				//������Ӯ����
	unsigned int	reward_gift[PLAY_COUNT];					//������ȯ	

	notify_game_result()
	{
		memset(this, 0, sizeof(notify_game_result));
	}
};
//C-->S
//��Ϣ�ṹ�� 
//�����������
 

#pragma pack(pop)

#endif