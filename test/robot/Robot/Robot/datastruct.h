#if !defined(_DATA_STRUCT_H_)
#define _DATA_STRUCT_H_
#include <windows.h>
#pragma pack(push,1)

#define  MAX_USER_PLAY							100
#define	 MAX_DESK_NUM							100
#define  MAX_BUFFER								60
#define  ONE_SECONDE							1000
//send -�� MFC
#define  USER_STATE_TO_MFC_LOGIN				2000					//��¼��Ϣ
#define  USER_STATE_TO_MFC_LOGIN_RESULT			2001					//��¼��Ϣ
#define  USER_STATE_TO_MFC_GOLD_CHANGE			2002					//��ұ仯
#define  USER_STATE_TO_MFC_LEFT_RESULT			2003					//����뿪��Ϣ
#define  USER_STATE_TO_MFC_GOLD_LACK			2004					//��ҽ�Ҳ��㣬�޷�׼��
#define  USER_STATE_TO_MFC_DESK_INFO			2005					//���˼�ⷵ��������Ϣ
#define  USER_STATE_TO_MFC_USER_INFO			2006					//�ɹ���¼�������ҵ���Ϣ
#define  USER_STATE_TO_MFC_RECHANGE				2007					//�����˳�ֵ

//��ʱ��
#define MFC_TIME_START_ID						100						//��ʱ����ʼ ID
/******************************************************************************************/

enum MESSAGE_TYPE
{
	GAMELOGIC_TYPE = 1,

};

//GameLogic�¼���ϢID��1��ͷ�������齫��ϷIDΪ20020400Ϊ������

//�����齫
#define TIME_USER_HZMJ_BLOCK					12040001				//�������
#define TIME_USER_HZMJ_OUT_CARD					12040002				//��ҳ���
#define TIME_USER_HZMJ_HU						12040003				//��Һ���
#define TIME_USER_HZMJ_READY					12040004				//���׼��
#define TIME_USER_HZMJ_GANG						12040005				//��Ҹ���
#define TIME_USER_HZMJ_LEFT_DESK				12040006				//����뿪����

//�ײ�����
#define TIME_USER_YBWP_CHOOSE_KING				12040011				//ѡ������

//�����齫
#define TIME_USER_CZMJ_RUN						12040031				//����ֻܷ�ׯ
#define TIME_USER_CZMJ_BLOCK					12040032				//�������
#define TIME_USER_CZMJ_OUT_CARD					12040033				//��ҳ���
#define TIME_USER_CZMJ_READY					12040034				//���׼��
#define TIME_USER_CZMJ_GANG						12040035				//��Ҹ���
#define TIME_USER_CZMJ_HU						12040036				//��Һ���
#define TIME_USER_CZMJ_ZUAN						12040037				//�������
#define TIME_USER_CZMJ_BIAN						12040038				//��ұ���
#define TIME_USER_CZMJ_DRAW						12040039				//������

//����ţţ
#define TIME_USER_XIA_ZHU						10014901				//�����ע
#define TIME_USER_TAN_CARDS						10014902				//���̯��
#define TIME_USER_QIANG_BANKER					10014903				//�����ׯ
#define TIME_USER_NIUNIU_READY					10014904				//��ҷ���׼����Ϣ

//������
#define TIME_USER_SEND_READY					10002101				//���׼����ʱ��ʱ��

//ƴƴ��
#define TIME_USER_PPL_PICK_CARD					11040101				//���ѡ��
#define TIME_USER_PPL_READY						11040102				//���׼��

//������
#define TIME_USER_LHD_XIAZHU					13180001				//�����ע
#define TIME_USER_LEFT_DESK						13180002				//����뿪
#define TIME_USER_GET_SEAT						13180003				//�������
/******************************************************************************************/

//����basegamelogic,��������Ϣ��ʹ��20��ͷ���ٲ����ĸ��㣺0000���ټ����¼�ID��00~99(С��10��0)����׼����Ϸ
#define TIME_USER_READY							20000001				//���׼��
#define TIME_USER_READY_CHECK_START				20000002				//���׼��,��Ϸ�Ƿ�ʼ
/******************************************************************************************/

//useractionmanager��ʹ��21��ͷ���ٲ����ĸ��㣺0000���ټ����¼�ID��00~99(С��10��0)
#define TIME_USER_CHECK_SERVOCE					21000001				//������
#define TIME_USER_RESTART_DESK_INFO				21000002				//�����������������Ϣ
/******************************************************************************************/

//usermanger��ʹ��22��ͷ���ٲ����ĸ��㣺0000���ټ����¼�ID��00~99(С��10��0)
#define TIME_USER_MGR_START						22000001
/******************************************************************************************/

//mfc ��ʹ��23��ͷ���ٲ����ĸ��㣺0000���ټ����¼�ID��00~99(С��10��0)
#define TIME_USER_LOGIN_PHP						23000001				//�û���¼PHP
#define TIME_USER_LOGIN_TIMESTMP				23000002				//�û���¼��¼ʱ���
#define TIME_USER_LOGIN_DESK_INFO				23000003				//�û�����������Ϣ
#define TIME_USER_RE_LOGIN_PHP					23000004				//�û��ٴ��û���¼

//realcheck ��ʹ����ʹ��24��ͷ���ٲ����ĸ��㣺0000���ټ����¼�ID��00~99(С��10��0)
#define TIME_REAL_CHECK_HEART_PAGE				24000001				//���˼��������
#define TIME_REAL_CHECK_STAER					24000002				//�����ʵ��ң����Ż���������
#define TIME_ROBBOT_CHECK_STA					24000003				//���������Ƿ������������

//�û�����������Ϣ
struct c_MSG_UserSit
{
	unsigned int	dwUserID;							//���ID
	unsigned int	iClubID;							//���ֲ�ID
	unsigned int	iClubRoomID;						//���ֲ�����ID
	unsigned int	iClubDeskID;						//���ֲ�����ID
	unsigned int	iClubDeskStation;					//���ֲ������е�λ��
	char			szPassword[61];						//�������� 
	unsigned int	iDeskID;							//��ʵ���Ӻ�
	unsigned int	iChairID;							//��ʵ��λ��
	c_MSG_UserSit()
	{
		memset(this, 0, sizeof(c_MSG_UserSit));
	}
};

typedef struct Idle_player_info
{
	c_MSG_UserSit	sit;
	bool			socket_cnt;
	Idle_player_info()
	{
		socket_cnt = false;
	}
}idle_player_info;

typedef struct Gold_change
{
	unsigned int	player_id;
	long long		gold;
	Gold_change()
	{
		player_id = 0;
		gold = 0;
	}
}gold_change;

typedef struct Gold_rechange
{
	unsigned int	player_id;
	Gold_rechange()
	{
		player_id = 0;
	}
}gold_rechange;

//������Ϣ
typedef struct sc_desk_info
{
	int				private_id;
	int				private_no;
	int				desk_index;
	int				desk_room_id;
	sc_desk_info()
	{
		private_id = 0;
		private_no = 0;
		desk_index = 255;
		desk_room_id = 0;
	}

}desk_info;

typedef struct SUserInitData
{
	unsigned int    user_id;					//�û�ID
	unsigned int	room_id;					//����ID
	unsigned int	index;
	unsigned int	private_id;
	unsigned int	private_no;
	char			login_ip[MAX_BUFFER];		//IP
	unsigned int    login_port;					//�˿�
	char			user_password[MAX_BUFFER];	//�������
	unsigned int	real_room_id;				//��ʵ����ID
	unsigned int    basic_point;				//�׷�
	int				play_game_count_min;		//��������پֺ���
	int				play_game_count_max;		//�������پֺ���
	int             play_game_count;			//����پֺ���
	int				player_game_id;
	unsigned int	club_desk_club_room_desk_no;//���ֲ���������ID
	int				recharge_limit;				//�����˽�ҵ��ڶ���ʱ,�Զ���ֵ
	int				recharge_money;				//�����˳�ֵ���
	unsigned int    rule_id;
	char            player_token[MAX_BUFFER];	//�û�TOKEN
	char            php_address[260];
	char            game_name[MAX_BUFFER];		//��Ϸ����
	HWND			handle;
	unsigned int    usermgr_index;
	long long       login_time;					//��¼ʱ��
	int             play_time;
	int				robot_off_player_count;		//���������ߵ��������ֵ
	void clear()
	{
		memset(this, 0, sizeof(SUserInitData));

	}
}UserInitData;

typedef struct SRobotInitData
{
	unsigned int    user_id;					//�û�ID
	unsigned int	room_id;					//����ID
	unsigned int	index;
	unsigned int	private_id;
	unsigned int	private_no;
	char			login_ip[MAX_BUFFER];		//IP
	unsigned int    login_port;					//�˿�
	char			user_password[MAX_BUFFER];	//�������
	unsigned int	real_room_id;				//��ʵ����ID
	unsigned int    basic_point;				//�׷�
	int				play_game_count_min;		//��������پֺ���
	int				play_game_count_max;		//�������پֺ���
	int             play_game_count;			//����پֺ���
	int				player_game_id;
	unsigned int	club_desk_club_room_desk_no;//���ֲ���������ID
	int				recharge_limit;				//�����˽�ҵ��ڶ���ʱ,�Զ���ֵ
	int				recharge_money;				//�����˳�ֵ���
	unsigned int    rule_id;
	char            player_token[MAX_BUFFER];	//�û�TOKEN
	char            php_address[260];
	char            game_name[MAX_BUFFER];		//��Ϸ����
	HWND			handle;
	unsigned int    usermgr_index;
	long long       login_time;					//��¼ʱ��
	int             play_time;
	int				robot_off_player_count;		//���������ߵ��������ֵ
	void clear()
	{
		memset(this, 0, sizeof(SRobotInitData));

	}
}RobotInitData;

typedef struct SHttpPlayerInfo
{
	unsigned int	player_id;
	unsigned int	player_money;
	unsigned int	player_coins;
	unsigned int	player_masonry;
	unsigned int	player_safe_box;
	unsigned int	player_lottery;
	unsigned int	player_club_id;
	unsigned int	player_sex;
	unsigned int	player_author;
	unsigned int	player_online;
	unsigned int	player_status;
	unsigned int	player_vip_level;
	unsigned int	player_resigter_time;
	unsigned int	player_robot;
	unsigned int	player_guest;
	unsigned int	player_icon_id;
	unsigned int	player_channel;
	unsigned int	player_time;
	unsigned int	id;
	char			player_name[MAX_BUFFER];
	char			player_nickname[MAX_BUFFER];
	char			player_password[MAX_BUFFER];
	char			player_pcid[MAX_BUFFER];
	char			player_token[512];
	void clear()
	{
		memset(this, 0, sizeof(SHttpPlayerInfo));
	}
}HttpPlayerInfo;

typedef struct SHttplogin
{
	char			action[MAX_BUFFER];
	char			version[MAX_BUFFER];
	unsigned int	sign_value;
	unsigned int	key_value;
	unsigned int	flag_value;
	HttpPlayerInfo  player_info;
	void clear()
	{
		memset(this, 0, sizeof(SHttplogin));
		player_info.clear();
	}
}Httplogin;

typedef struct SHttpRobortAddMoney
{
	char			action[MAX_BUFFER];
	char			version[MAX_BUFFER];
	unsigned int	sign_value;
	unsigned int	key_value;
	unsigned int	flag_value;
	int             code_value;
	char			desc_value[MAX_BUFFER];
	void clear()
	{
		memset(this, 0, sizeof(SHttpRobortAddMoney));
		code_value = -1;
	}
}SHttpRobortAddMoney;

typedef struct SHttpDeskinfo
{
	unsigned int    club_desk_id;
	unsigned int	club_desk_club_room_id;				//����id
	unsigned int	club_desk_club_id;
	unsigned int	club_desk_club_room_desk_no;
	unsigned int	club_desk_room_id;
	unsigned int	club_desk_desk_no;
	unsigned int	club_desk_game_id;
	unsigned int	club_desk_room_no;
	unsigned int	club_desk_player_id;
	unsigned int	club_desk_is_work;
	unsigned int	club_desk_members_count;
	unsigned int	club_desk_members_cur;
	unsigned int	club_desk_rule_id;
	unsigned int	club_desk_time;
	unsigned int	club_desk_status;
	char			club_desk_param[1024];
	char			club_desk_player_list[MAX_BUFFER];
	void clear()
	{
		memset(this, 0, sizeof(SHttpDeskinfo));
	}
}HttpDeskinfo;

typedef struct SHttpRoominfo
{
	unsigned int    room_id;
	unsigned int	room_game_id;
	unsigned int	room_rule;
	unsigned int	room_desk_count;
	unsigned int	room_status;
	unsigned int	room_is_open;
	unsigned int	room_min_money;
	unsigned int	room_max_money;
	unsigned int	room_srv_port;
	unsigned int	room_encrypt;
	unsigned int	room_tax;
	unsigned int	room_base_point;
	unsigned int	room_vip;
	unsigned int	room_type;
	char			room_srv_host[MAX_BUFFER];
	char			room_srv_dll_name[MAX_BUFFER];
	char			room_password[MAX_BUFFER];
	void clear()
	{
		memset(this, 0, sizeof(SHttpRoominfo));
	}
}HttpRoominfo;

typedef struct SHttpClubRoom
{
	unsigned int    club_room_id;
	unsigned int    club_room_club_id;
	unsigned int    club_room_game_id;
	unsigned int    club_room_desk_count;
	unsigned int    club_room_is_work;
	unsigned int    club_room_is_open;
	unsigned int    club_room_desk_members_count;
	unsigned int    club_room_type;
	unsigned int    club_room_level;
	unsigned int    club_room_basic_points;
	unsigned int    club_room_min_coin;
	unsigned int    club_room_max_coin;
	unsigned int    club_room_rule_id;
	unsigned int    club_room_online;
	unsigned int    club_room_time;
	char			club_room_desk_param[1024];
	void clear()
	{
		memset(this, 0, sizeof(SHttpClubRoom));
	}
}HttpClubRoom;

typedef struct SHttpClubDeskInfo
{
	char			action[MAX_BUFFER];
	char			version[MAX_BUFFER];
	unsigned int	sign_value;
	unsigned int	key_value;
	unsigned int	flag_value;
	char			desc_value[MAX_BUFFER];
	SHttpDeskinfo   desk_info;
	SHttpRoominfo   room_inifo;
	SHttpClubRoom	club_room;
	void clear()
	{
		memset(this, 0, sizeof(SHttpClubDeskInfo));
		desk_info.clear();
		room_inifo.clear();
		club_room.clear();
	}
}HttpClubDeskInfo;

typedef struct USerSitInfo
{
	/******** 1����¼PHP���********/
	unsigned int    p_user_id;						//���id
	char			p_player_login_ip[MAX_BUFFER];		//ip
	char			p_player_name[MAX_BUFFER];			//������֣��ο� guest��
	char		    p_player_nickname[MAX_BUFFER];		//����ǳ�
	char		    p_player_password[MAX_BUFFER];		//�������
	char			p_player_pcid[MAX_BUFFER];			//pcid
	char		    p_player_token[MAX_BUFFER];			//token
	unsigned int    p_push_port;						//�˿�

	/************* 2������������Ϣ��ȡ*******************/
	unsigned int    club_desk_id;
	unsigned int	club_desk_club_room_id;				//iClubRoomID
	unsigned int	club_desk_club_id;					//iClubID
	unsigned int	club_desk_club_room_desk_no;		//iClubDeskID
	unsigned int	club_desk_room_id;					
	unsigned int	club_desk_desk_no;					//iDeskID
	unsigned int	club_desk_game_id;
	unsigned int	club_desk_room_no;
	unsigned int	club_desk_rule_id;
	unsigned int    room_srv_port;						//�˿ں�
	char            room_srv_host[MAX_BUFFER];			//IP��ַ
	char			club_desk_param[1024];
	unsigned int	club_room_basic_points;				//�׷�

	int				play_game_count;					//���پֺ���
	int             play_game_count_min;				//���پ�������
	int             play_game_count_max;				//����������
	int             rule_id;							//����ID
	char            php_address[260];					//PHP��ַ
	char            game_name[MAX_BUFFER];				//��Ϸ����
	unsigned int    ilastchangetime;					//��������һ�θ����û���Ϣʱ��
	void userSitInfo()
	{
		memset(this, 0, sizeof(USerSitInfo));
	}
}userSitInfo;

//mfc ������ʾ�����Ϣ
typedef struct Mfc_Show_User_Info
{
	unsigned int    user_id;							//���id
	char			player_name[MAX_BUFFER];			//������֣��ο� guest��
	unsigned int    user_state;							//�û�״̬  1:����״̬ 2:������Ϸ�� 3:ͬ��״̬ 5:����״̬ 6:��Ϸ������ 255:��Ҳ���
	unsigned int    room_no;							//�����
	unsigned int	game_id;							//��Ϸid
	unsigned int    desk_no;							//���Ӻ�
	long long		iCoins;								//���
	int				iChairID;							//����λ�� 
	int             userNote;							//��ע	-1 ����
	Mfc_Show_User_Info()
	{
		memset(this, 0, sizeof(Mfc_Show_User_Info));
		iChairID = -1;
		iCoins = -1;
	}
}mfc_show_user_info;

#pragma pack(pop)
#endif//_DATA_STRUCT_H_