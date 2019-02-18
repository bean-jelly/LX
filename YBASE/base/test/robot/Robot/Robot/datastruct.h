#if !defined(_DATA_STRUCT_H_)
#define _DATA_STRUCT_H_
#include <windows.h>
#pragma pack(push,1)

#define  MAX_USER_PLAY							100
#define	 MAX_DESK_NUM							100
#define  MAX_BUFFER								60
#define  ONE_SECONDE							1000
//send -》 MFC
#define  USER_STATE_TO_MFC_LOGIN				2000					//登录消息
#define  USER_STATE_TO_MFC_LOGIN_RESULT			2001					//登录消息
#define  USER_STATE_TO_MFC_GOLD_CHANGE			2002					//金币变化
#define  USER_STATE_TO_MFC_LEFT_RESULT			2003					//玩家离开消息
#define  USER_STATE_TO_MFC_GOLD_LACK			2004					//玩家金币不足，无法准备
#define  USER_STATE_TO_MFC_DESK_INFO			2005					//真人检测返回桌子信息
#define  USER_STATE_TO_MFC_USER_INFO			2006					//成功登录包间后玩家的信息
#define  USER_STATE_TO_MFC_RECHANGE				2007					//机器人充值

//定时器
#define MFC_TIME_START_ID						100						//定时器开始 ID
/******************************************************************************************/

enum MESSAGE_TYPE
{
	GAMELOGIC_TYPE = 1,

};

//GameLogic事件消息ID以1开头（红中麻将游戏ID为20020400为例，）

//红中麻将
#define TIME_USER_HZMJ_BLOCK					12040001				//玩家拦牌
#define TIME_USER_HZMJ_OUT_CARD					12040002				//玩家出牌
#define TIME_USER_HZMJ_HU						12040003				//玩家胡牌
#define TIME_USER_HZMJ_READY					12040004				//玩家准备
#define TIME_USER_HZMJ_GANG						12040005				//玩家杠牌
#define TIME_USER_HZMJ_LEFT_DESK				12040006				//玩家离开桌子

//易博王牌
#define TIME_USER_YBWP_CHOOSE_KING				12040011				//选择王牌

//沧州麻将
#define TIME_USER_CZMJ_RUN						12040031				//玩家跑分或换庄
#define TIME_USER_CZMJ_BLOCK					12040032				//玩家拦牌
#define TIME_USER_CZMJ_OUT_CARD					12040033				//玩家出牌
#define TIME_USER_CZMJ_READY					12040034				//玩家准备
#define TIME_USER_CZMJ_GANG						12040035				//玩家杠牌
#define TIME_USER_CZMJ_HU						12040036				//玩家胡牌
#define TIME_USER_CZMJ_ZUAN						12040037				//玩家钻牌
#define TIME_USER_CZMJ_BIAN						12040038				//玩家边牌
#define TIME_USER_CZMJ_DRAW						12040039				//玩家求和

//八人牛牛
#define TIME_USER_XIA_ZHU						10014901				//玩家下注
#define TIME_USER_TAN_CARDS						10014902				//玩家摊牌
#define TIME_USER_QIANG_BANKER					10014903				//玩家抢庄
#define TIME_USER_NIUNIU_READY					10014904				//玩家发送准备消息

//斗地主
#define TIME_USER_SEND_READY					10002101				//玩家准备超时定时器

//拼拼乐
#define TIME_USER_PPL_PICK_CARD					11040101				//玩家选牌
#define TIME_USER_PPL_READY						11040102				//玩家准备

//龙虎斗
#define TIME_USER_LHD_XIAZHU					13180001				//玩家下注
#define TIME_USER_LEFT_DESK						13180002				//玩家离开
#define TIME_USER_GET_SEAT						13180003				//玩家上座
/******************************************************************************************/

//父类basegamelogic,处理公共消息，使用20开头，再补上四个零：0000，再加上事件ID：00~99(小于10补0)例如准备游戏
#define TIME_USER_READY							20000001				//玩家准备
#define TIME_USER_READY_CHECK_START				20000002				//玩家准备,游戏是否开始
/******************************************************************************************/

//useractionmanager类使用21开头，再补上四个零：0000，再加上事件ID：00~99(小于10补0)
#define TIME_USER_CHECK_SERVOCE					21000001				//心跳包
#define TIME_USER_RESTART_DESK_INFO				21000002				//玩家重新请求桌子信息
/******************************************************************************************/

//usermanger类使用22开头，再补上四个零：0000，再加上事件ID：00~99(小于10补0)
#define TIME_USER_MGR_START						22000001
/******************************************************************************************/

//mfc 类使用23开头，再补上四个零：0000，再加上事件ID：00~99(小于10补0)
#define TIME_USER_LOGIN_PHP						23000001				//用户登录PHP
#define TIME_USER_LOGIN_TIMESTMP				23000002				//用户登录记录时间搓
#define TIME_USER_LOGIN_DESK_INFO				23000003				//用户请求桌子信息
#define TIME_USER_RE_LOGIN_PHP					23000004				//用户再次用户登录

//realcheck 类使用类使用24开头，再补上四个零：0000，再加上事件ID：00~99(小于10补0)
#define TIME_REAL_CHECK_HEART_PAGE				24000001				//真人检测心跳包
#define TIME_REAL_CHECK_STAER					24000002				//检测真实玩家，安排机器人坐桌
#define TIME_ROBBOT_CHECK_STA					24000003				//检测机器人是否可以自行坐桌

//用户请求坐下消息
struct c_MSG_UserSit
{
	unsigned int	dwUserID;							//玩家ID
	unsigned int	iClubID;							//俱乐部ID
	unsigned int	iClubRoomID;						//俱乐部房间ID
	unsigned int	iClubDeskID;						//俱乐部桌子ID
	unsigned int	iClubDeskStation;					//俱乐部桌子中的位置
	char			szPassword[61];						//桌子密码 
	unsigned int	iDeskID;							//真实桌子号
	unsigned int	iChairID;							//真实座位号
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

//桌子信息
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
	unsigned int    user_id;					//用户ID
	unsigned int	room_id;					//房间ID
	unsigned int	index;
	unsigned int	private_id;
	unsigned int	private_no;
	char			login_ip[MAX_BUFFER];		//IP
	unsigned int    login_port;					//端口
	char			user_password[MAX_BUFFER];	//玩家密码
	unsigned int	real_room_id;				//真实房间ID
	unsigned int    basic_point;				//底分
	int				play_game_count_min;		//最少玩多少局后换桌
	int				play_game_count_max;		//最多玩多少局后换桌
	int             play_game_count;			//玩多少局后换桌
	int				player_game_id;
	unsigned int	club_desk_club_room_desk_no;//俱乐部房间桌子ID
	int				recharge_limit;				//机器人金币低于多少时,自动充值
	int				recharge_money;				//机器人充值金币
	unsigned int    rule_id;
	char            player_token[MAX_BUFFER];	//用户TOKEN
	char            php_address[260];
	char            game_name[MAX_BUFFER];		//游戏名字
	HWND			handle;
	unsigned int    usermgr_index;
	long long       login_time;					//登录时间
	int             play_time;
	int				robot_off_player_count;		//机器人下线的真人最大值
	void clear()
	{
		memset(this, 0, sizeof(SUserInitData));

	}
}UserInitData;

typedef struct SRobotInitData
{
	unsigned int    user_id;					//用户ID
	unsigned int	room_id;					//房间ID
	unsigned int	index;
	unsigned int	private_id;
	unsigned int	private_no;
	char			login_ip[MAX_BUFFER];		//IP
	unsigned int    login_port;					//端口
	char			user_password[MAX_BUFFER];	//玩家密码
	unsigned int	real_room_id;				//真实房间ID
	unsigned int    basic_point;				//底分
	int				play_game_count_min;		//最少玩多少局后换桌
	int				play_game_count_max;		//最多玩多少局后换桌
	int             play_game_count;			//玩多少局后换桌
	int				player_game_id;
	unsigned int	club_desk_club_room_desk_no;//俱乐部房间桌子ID
	int				recharge_limit;				//机器人金币低于多少时,自动充值
	int				recharge_money;				//机器人充值金币
	unsigned int    rule_id;
	char            player_token[MAX_BUFFER];	//用户TOKEN
	char            php_address[260];
	char            game_name[MAX_BUFFER];		//游戏名字
	HWND			handle;
	unsigned int    usermgr_index;
	long long       login_time;					//登录时间
	int             play_time;
	int				robot_off_player_count;		//机器人下线的真人最大值
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
	unsigned int	club_desk_club_room_id;				//房间id
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
	/******** 1、登录PHP获得********/
	unsigned int    p_user_id;						//玩家id
	char			p_player_login_ip[MAX_BUFFER];		//ip
	char			p_player_name[MAX_BUFFER];			//玩家名字（游客 guest）
	char		    p_player_nickname[MAX_BUFFER];		//玩家昵称
	char		    p_player_password[MAX_BUFFER];		//玩家密码
	char			p_player_pcid[MAX_BUFFER];			//pcid
	char		    p_player_token[MAX_BUFFER];			//token
	unsigned int    p_push_port;						//端口

	/************* 2、请求桌子信息获取*******************/
	unsigned int    club_desk_id;
	unsigned int	club_desk_club_room_id;				//iClubRoomID
	unsigned int	club_desk_club_id;					//iClubID
	unsigned int	club_desk_club_room_desk_no;		//iClubDeskID
	unsigned int	club_desk_room_id;					
	unsigned int	club_desk_desk_no;					//iDeskID
	unsigned int	club_desk_game_id;
	unsigned int	club_desk_room_no;
	unsigned int	club_desk_rule_id;
	unsigned int    room_srv_port;						//端口号
	char            room_srv_host[MAX_BUFFER];			//IP地址
	char			club_desk_param[1024];
	unsigned int	club_room_basic_points;				//底分

	int				play_game_count;					//多少局后换桌
	int             play_game_count_min;				//最少局数换桌
	int             play_game_count_max;				//最大局数换桌
	int             rule_id;							//规则ID
	char            php_address[260];					//PHP地址
	char            game_name[MAX_BUFFER];				//游戏名称
	unsigned int    ilastchangetime;					//机器人上一次更新用户信息时间
	void userSitInfo()
	{
		memset(this, 0, sizeof(USerSitInfo));
	}
}userSitInfo;

//mfc 界面显示玩家信息
typedef struct Mfc_Show_User_Info
{
	unsigned int    user_id;							//玩家id
	char			player_name[MAX_BUFFER];			//玩家名字（游客 guest）
	unsigned int    user_state;							//用户状态  1:空闲状态 2:坐在游戏桌 3:同意状态 5:断线状态 6:游戏进行中 255:金币不足
	unsigned int    room_no;							//包间号
	unsigned int	game_id;							//游戏id
	unsigned int    desk_no;							//桌子号
	long long		iCoins;								//金币
	int				iChairID;							//桌子位置 
	int             userNote;							//备注	-1 下线
	Mfc_Show_User_Info()
	{
		memset(this, 0, sizeof(Mfc_Show_User_Info));
		iChairID = -1;
		iCoins = -1;
	}
}mfc_show_user_info;

#pragma pack(pop)
#endif//_DATA_STRUCT_H_