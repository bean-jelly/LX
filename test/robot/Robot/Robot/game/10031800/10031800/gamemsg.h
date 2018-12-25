#pragma once
#ifndef __GAMEMSG_H_
#define __GAMEMSG_H_

#define PLAY_COUNT						    180						     //��Ϸ���� 
#define TIME_ONE_SECOND				        1000						 //��ʱ��һ��
#define MAX_CARDS                           416                          //�������
#define MAX_COUNT                           2                            //���������
#define BANKERLISTLENGTH                    20                           //�����ׯ�б�
#define NOSITLENGTH                         180                          //��������б���
#define CHOUMA_COUNT						5							 //�������
#define AREA_COUNT							3							 //��ע�������
#define SEAT_COUNT							7							 //��λ��
#define AREA_TOTAL_COUNT					3 * 5						 //������� * �������
//��ע������
#define SUCCESS                             0                            //�ɹ�
#define FAIL                                1                            //ʧ��
#define NOTENOUGH                           2                            //Ǯ����
#define COINLIMIT                           3                            //��������
#define WRONGAREA                           4                            //��������
#define NONOTE                              5                            //û����ע
#define CONFIRMED							6							 //��ȷ����ע
//������Ϸ״̬����12��ʼ
#define     GAME_STATUS_LOOK_STATE          12                           //��ʼ״̬
#define     GAME_STATUS_SHUFFLE             13                           //ϴ��״̬
#define     GAME_STATUS_SEND_CARD           14                           //����״̬
#define     GAME_STATUS_USER_NOTE           15                           //��ע״̬
#define     GAME_STATUS_OPEN                16                           //����״̬
#define     GAME_STATUS_RESULT              17                           //����״̬
//����¼���¼���� 0x30��ʼ
//��Ϸ��ʱ�� 1-50 
#define     TIMER_READY                     1                            //׼��ʱ��
#define		TIME_SHUFFLE					2							 //ϴ��
#define		TIME_SEND_CARD					3							 //����
#define		TIME_USER_NOTE					4							 //�����ע
#define		TIME_WAIT_NEX					5							 //�ȴ��¾ֿ�ʼ
#define		TIME_CARD_OPEN     				6							 //����

//��Ϸ��Ϣ
//server to  client 
#define STC_SEND_USER_AGREE				    1000						 //���ͬ����Ϣ
#define STC_SEND_SEND_CARD				    1001						 //������Ϣ
#define STC_SEND_SHUFFLE				    1002						 //ϴ����Ϣ
#define STC_SEND_MAKER						1003						 //ׯ����Ϣ
#define STC_SEND_USER_NOTE				    1004						 //��ע״̬��Ϣ
#define STC_SEND_OPEN_RES				    1005						 //���ƽ����Ϣ
#define STC_SEND_SIT_RESULT				    1006						 //���½����Ϣ
#define STC_SEND_GIVEUP_SIT_RES				1007						 //վ������Ϣ
#define STC_SEND_NO_SIT_COUNT				1008						 //����������Ϣ
#define STC_SEND_GET_BANKER_RESULT          1009                         //��ׯ�����Ϣ
#define STC_SEND_GIVEUP_BANKER_RESULT       1010                         //��ׯ�����Ϣ
#define STC_SEND_NOSITLIST_RESULT           1011                         //�����б�����Ϣ
#define STC_SEND_LUTU_RESULT                1012                         //·ͼ�����Ϣ
#define STC_SEND_USER_NOTE_RESULT			1013						 //��ע�����Ϣ
#define STC_SEND_USER_REPEATNOTE_RESULT		1014						 //�ظ���ע�����Ϣ
#define STC_SEND_USER_CONFIRMNOTE_RESULT	1015						 //ȷ����ע�����Ϣ
#define STC_SEND_USER_CANCELNOTE_RESULT	    1016						 //ȡ����ע�����Ϣ
#define STC_SEND_MAKER_LIST					1017						 //������ׯ�б�
#define STC_SEND_OPEN_CARD					1018						 //������Ϣ
//���͹�����Ϣ
#define STC_UPDATE_USER_INFO			    1101						 //���������Ϣ	 
#define CTS_GET_GAME_CONFIG				    1201						 //�����ȡ��Ϸ����
#define CTS_UPDATE_USER_INFO			    1202						 //��������Լ���Ϣ

//client to server
#define CTS_RECV_USER_NOTE				    2000						//�յ������ע��Ϣ
#define CTS_RECV_USER_SIT                   2002                        //�������
#define CTS_RECV_USER_GIVEUP_SIT            2003                        //��ҷ�������
#define CTS_RECV_USER_GET_BANKER            2004                        //��ׯ���
#define CTS_RECV_USER_GIVEUP_BANKER         2005                        //��ׯ���
#define CTS_RECV_NOSIT_USER                 2006                        //�������������Ϣ��Ϣ
#define CTS_RECV_LUTU                       2007                        //����·ͼ���
#define CTS_RECV_REPEAT_XIAZHU              2008                        //�ظ���ע
#define CTS_RECV_CONFIRM_XIAZHU             2009                        //ȷ����ע
#define CTS_RECV_CANCEL_XIAZHU              2010                        //ȡ����ע
#define CTS_RECV_MAKER_LIST_REQEST			2011						//���������ׯ�б�
#pragma pack(push,1)

//���״̬
enum USER_STATUS
{
	USER_SEE = 0,				//�Թ�
	USER_JOIN,					//������Ϸ
	USER_REST,                  //��Ϣ
};

//��������
enum AREA_TYPE
{
	AREA_LONG = 0,				//��
	AREA_HE,					//��
	AREA_HU,					//��
};
//��λ����
enum SEAT_TYPE
{	
	SEAT_MAKER = 0,				//ׯ��
	SEAT_RICHER,				//�󸻺�
	SEAT_LUCKY_STAR,			//������
	SEAT_1,						//��λ1
	SEAT_2,						//��λ2
	SEAT_3,						//��λ3
	SEAT_4,						//��λ4
};
//S-->C
//��Ϸ��Ϣ�ṹ�� 
//��Ϸ״̬������Ϣ

//��Ϸ�������Ϣ
typedef struct Stu_GameUserItem
{
	unsigned int	player_id;						//���ID
	BYTE            chairid;                        //��λid
	BYTE            bsex;                           //�Ա�(1��2Ů)
	char			player_nickname[60];			//����ǳ�
	int				player_icon_id;					//ϵͳͷ��id
	bool			look_status;					//�Ƿ����Թ�״̬
	bool            bplaygame;                      //�Ƿ�����Ϸ��
	bool			cut_status;						//�Ƿ��Ƕ���״̬
	long long		total_score;					//����ܷ�
	long long       servercount;                    //�����
	
	Stu_GameUserItem()
	{
		memset(this, 0, sizeof(Stu_GameUserItem));
		player_id = 255;
		chairid = 255;
	}
}GameUserItem;

//��Ϸ��ʼ�׶�
typedef struct sc_game_station
{
	BYTE                chairid;                                    //���id
	BYTE                iuser_sit[SEAT_COUNT];						//ui�������
	long long           areamoney[AREA_COUNT];						//������ע�ܽ��
	BYTE                areacard[2];		                        //����
	BYTE	            nowstatus;			                        //��Ϸ״̬
	long long           user_money;									//��ǰ��ҽ��
	BYTE	            nowtime;				                    //��ǰ״̬ʣ��ʱ��
	long long           selfmoney[AREA_COUNT];						//���������ע���
	long long           luckymoney[AREA_COUNT];						//��������ע
	int					area_tatal_chouma[AREA_TOTAL_COUNT];		//�����ܵĳ������ ��0-4  ��5-9  ��10-14
	int					area_user_chouma[AREA_TOTAL_COUNT];			//���������ע�ĳ����� ��0-4  ��5-9  ��10-14
	int					wall_count;									//��ǽ����
	GameUserItem        useritem[SEAT_COUNT];						//��Ϸ�������Ϣ
	int					watch_times;					            //�Թ۴���
	int                 chips[CHOUMA_COUNT];                        //����ֵ
	bool				bBetLastGame;								//��һ���Ƿ�����ע
	sc_game_station()
	{
		memset(this, 0, sizeof(sc_game_station));
	}
}game_station;

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

//ׯ����Ϣ
struct Stu_Maker_Info
{
	BYTE			chairid;		//ׯ��λ��
	int				userid;			//ׯ��id
	long long		usermoney;		//ׯ�ҽ��
	int				wall_length;	//��ǽ����  ʣ���Ƹ���
	Stu_Maker_Info()
	{
		memset(this, 0, sizeof(Stu_Maker_Info));
		chairid = 255;
	}
};

struct Stu_Status
{
	BYTE  status;                   //����״̬
	BYTE  bytime;                   //��ǰ״̬ʱ��
	Stu_Status()
	{
		memset(this, 0, sizeof(Stu_Status));
	}
};

struct Stu_UserXiaZhuResult
{
	BYTE		chairid;			    //��λ��
	BYTE		areaid;					//�����
	BYTE		xiazhuresult;           //��ע��� 
	long long   xiazhumoney;			//��ע���
	long long   userAreaTotalBet;		//��Ҹ���������ע
	long long	areaTotalBet;			//����������ҵ�����ע
	long long	usermoney;				//��ע��ҽ��
	bool		bsit;                   //�Ƿ��������
	Stu_UserXiaZhuResult()
	{
		memset(this, 0, sizeof(Stu_UserXiaZhuResult));
		chairid = 255;
	}
};

//�û�������ׯ���
struct Stu_UserGetBanker_Result
{
	BYTE		chairid;				//��λ��
	long long   bring_coin;				//��ׯ�����
	BYTE		result;					//��ׯ���
};

//���ߺ�ǿ����ׯ�����
struct Stu_Cut_Banker
{
	unsigned int		userid;          //���id
	long long			timecount;       //����ʱ�䣨ʱ�䵽��0��
	Stu_Cut_Banker()
	{
		memset(this, 0, sizeof(Stu_Cut_Banker));
	}
};

//���ƽ׶�
struct Stu_Send_Card
{
	BYTE	status;                 //����״̬
	BYTE	bytime;                 //��ǰ״̬ʱ��
	BYTE	openid[2];              //��������
	BYTE	cards[2];               //����Ϣ0:����1:��
	Stu_Send_Card()
	{
		memset(this, 0, sizeof(Stu_Send_Card));
	}
};

//��ҷ����仯
struct Stu_ChangeMoney
{
	BYTE				chairid;										//��λ��
	BYTE				status;											//��Ϸ״̬
	BYTE				winarea;										//��������
	long long			win_score;										//�����Ӯ��  �������� ȥ�������
	long long           user_score;										//��ҽ��
	long long			seat_win_score[SEAT_COUNT];						//���������Ӯ�� ����ׯ�������Ǵ󸻺�  �������� ȥ�������
	long long           seat_user_score[SEAT_COUNT];					//��λ����ҽ��
	bool				bFlyCoin;										//�����б�����Ƿ���Ҫ�ɽ��
	int					rest_time;										//��Ϣʱ��	
	bool				bBet;											//�þֵ�ǰ����Ƿ�����ע
	Stu_ChangeMoney()
	{
		memset(this, 0, sizeof(Stu_ChangeMoney));
		chairid = 255;
	}
};

//����������
struct Stu_Sit_Result
{
	BYTE		chairid;			        //��λ��
	BYTE		result;                     //�������
	BYTE		sitid;						//���Ӻ� 3-6
	int			userid;						//�������id
	long long   restmoney;					//��ҽ��
	int			nositnum;                   //���������
	Stu_Sit_Result()
	{
		memset(this, 0, sizeof(Stu_Sit_Result));
		chairid = 255;
	}
};

//���վ����
struct Stu_UP_Result
{
	BYTE		chairid;			        //���λ��
	BYTE		sitid;                      //���Ӻ� 3-6
	BYTE		result;                     //������� 0ʧ�� 1�ɹ� 2��Ҳ����Զ�����
	int			userid;						//�������id
	int			nositnum;                   //���������
	Stu_UP_Result()
	{
		memset(this, 0, sizeof(Stu_UP_Result));
		chairid = 255;
	}
};

//����������ҽ��
struct Stu_NoSit_Userinfo_Result
{
	unsigned int    userid[NOSITLENGTH];		//���id
	long long		money[NOSITLENGTH];			//���
	int				win_num[NOSITLENGTH];		//��ʤ����
	Stu_NoSit_Userinfo_Result()
	{
		memset(this, 0, sizeof(Stu_NoSit_Userinfo_Result));
	}
};

//����·ͼ
struct Stu_LuTu_Result
{
	BYTE brecord[100];
	Stu_LuTu_Result()
	{
		memset(this, 255, sizeof(Stu_LuTu_Result));
	}
};

//����ظ���ע���
struct Stu_Repeat_XiaZhu_Result
{
	BYTE			chairid;
	long long		area_total_bet[AREA_COUNT];							//���������������ע
	long long		area_user_bet[AREA_COUNT];							//���������ע
	int				area_user_chouma[AREA_TOTAL_COUNT];					//��Ҹ������Ӧ����ע�������  ��0-4  ��5-9  ��10-14
	long long		user_coin;											//��ҽ��
	BYTE			result;												//0�ɹ� 1��Ҳ��� 2ׯ�Ҳ�����ע
	bool			bsit;
	Stu_Repeat_XiaZhu_Result()
	{
		memset(this, 0, sizeof(Stu_Repeat_XiaZhu_Result));
	}
};

//���ȷ����ע���
struct Stu_Confirm_XiaZhu_Res
{
	BYTE			chairid;
	unsigned int    userid;
	bool			 result;
	Stu_Confirm_XiaZhu_Res()
	{
		chairid = 255;
		userid = 0; 
		result = false;
	}
};

//���ȡ����ע���
struct Stu_Cancel_XiaZhu_Res
{
	BYTE			chairid;
	unsigned int    userid;
	long long		area_total_bet[AREA_COUNT];					//ȡ����ע������������ҵ�����ע
	int				area_user_chouma[AREA_TOTAL_COUNT];			//ȡ����ע�������ע�Ķ�Ӧ�ĳ������
	long long		user_score;									//ȡ����ע���������Ͻ��
	bool			bsit;										//�Ƿ���������� 
	BYTE			result;
	Stu_Cancel_XiaZhu_Res()
	{
		chairid = 255;
		userid = 0;
		result = 0;
	}
};
//C-->S
//��Ϣ�ṹ��
//�û���ע
struct Stu_UserXiaZhu
{
	BYTE   chairid;			       //��λ��
	BYTE   areaid;                 //�����
	INT    xiazhumoney;            //��ע���

	Stu_UserXiaZhu()
	{
		memset(this, 0, sizeof(Stu_UserXiaZhu));
		chairid = 255;
	}
};

//�û�������ׯ
struct Stu_UserGetBanker
{
	BYTE		chairid;			        //��λ��
	long long   bring_coin;			        //��ׯ�����
	long long	maker_time;					//��ׯʱ��
	Stu_UserGetBanker()
	{
		memset(this, 0, sizeof(Stu_UserGetBanker));
		chairid = 255;
	}
};

//����ׯ���б�
struct Stu_Send_MakerLIst
{
	Stu_UserGetBanker	makerlist[BANKERLISTLENGTH];
	int					maker_num;					//ׯ����ׯ����
};

//�û�������ׯ
struct Stu_UserGiveupBanker
{
	int				userid;
	BYTE			chairid;
	BYTE            result;                    //��ׯ��� 0�ɹ� 1ʧ�� 2��Ҳ����Զ���ׯ
	Stu_UserGiveupBanker()
	{
		userid = 0;
		chairid = 255;
		result = 0;
	}
};
//�û����뿪��
struct Stu_UserOpen
{
	BYTE    chairid;			       //��λ��
	BYTE    areaid;                    //����id ����0�� ����1
	Stu_UserOpen()
	{
		chairid = 255;
	}
};

//�����������
struct Stu_NoSit_Count
{
	BYTE     bycount;   //�����������
	Stu_NoSit_Count()
	{
		memset(this, 0, sizeof(Stu_NoSit_Count));
	}
};

//�����������
struct Stu_NoSit_Userinfo
{
	BYTE			chairid;
	unsigned int    userid;
	Stu_NoSit_Userinfo()
	{
		chairid = 255;
		userid = 0;
	}
};

//����·ͼ
struct Stu_LuTu
{
	BYTE			chairid;
	unsigned int    userid;
	Stu_LuTu()
	{
		chairid = 255;
		userid = 0;
	}
};

//�����������
struct Stu_Sit
{
	BYTE    chairid;			       //��λ��
	BYTE    sitid;                     //���Ӻ�
	BYTE    result;                    //���½��
	Stu_Sit()
	{
		memset(this, 0, sizeof(Stu_Sit));
		chairid = 255;
	}
};

//�������վ��
struct Stu_GiveUp_Sit
{
	BYTE    chairid;
	BYTE    sitid;
	Stu_GiveUp_Sit()
	{
		memset(this, 0, sizeof(Stu_GiveUp_Sit));
		chairid = 255;
	}
};

//����ظ���ע
struct Stu_Repeat_XiaZhu
{
	BYTE			chairid;
	unsigned int    userid;
	bool    bnote;
	Stu_Repeat_XiaZhu()
	{
		chairid = 255;
		userid = 0;
		bnote = false;
	}
};

//���ȷ����ע
struct Stu_Confirm_XiaZhu
{
	BYTE			chairid;
	unsigned int    userid;
	Stu_Confirm_XiaZhu()
	{
		chairid = 255;
		userid = 0;
	}
};

//���ȡ����ע
struct Stu_Cancel_XiaZhu
{
	BYTE			chairid;
	int				userid;
	Stu_Cancel_XiaZhu()
	{
		chairid = 255;
		userid = 0;
	}
};
//���������ׯ�б�
struct Stu_View_MakerList
{
	BYTE			chair_id;
	Stu_View_MakerList()
	{
		chair_id = 255;
	}
};
#pragma pack(pop)

#endif