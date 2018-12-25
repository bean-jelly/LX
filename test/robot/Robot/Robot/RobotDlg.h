// RobotDlg.h : 头文件
#include <map>
#include "BasicHttpContext.h" 
#include "datastruct.h"
#include "basiccode\utility\BasicMutex.h"
#include "basiccode\utility\BasicCriticalSection.h"
#include "basiccode\utility\BasicThreadPool.h"
#include "RobotManager\usermanager.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "..\..\comlib\basicsdk\basicbuffer\BasicTranscode.h"
#include "realplayercheck.h"
#define WM_COUNT_MSG WM_USER+100

#pragma once

typedef struct LoginUserBaseInfo
{
	char password[MAX_BUFFER];
	int  user_id;
	LoginUserBaseInfo()
	{
		memset(this, 0, sizeof(LoginUserBaseInfo));
	}
}userBaseInfo;

//CRobotDlg 对话框
class CRobotDlg : public CDialogEx
{
//构造
public:
	//标准构造函数
	CRobotDlg(CWnd* pParent = NULL);	
	CRobotDlg::~CRobotDlg();
protected:
	virtual void						DoDataExchange(CDataExchange* pDX);// DDX/DDV 支持
protected:
	HICON								m_hIcon;
	//生成的消息映射函数
	virtual BOOL						OnInitDialog();
	afx_msg void						OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void						OnPaint();
	afx_msg HCURSOR						OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void								ReadRoomConfig();
	void								ReadRobotConfig();
	void								string2list(char *str, char byStrList[][100], int& iCount);
	void								StartUserMgr();
	void								AddUserInfo();
	void								RobotLogin(int index);
	void								RobotLoginDesk(int index);
public:
	void								updateMfcUserInfo();
	afx_msg void						OnBnClickedButtonCreate();
	afx_msg void						OnBnClickedButtonLogin();
	afx_msg void						OnBnClickedButton4();
	afx_msg void						OnBnClickedButton2();
	afx_msg void						OnBnClickedButton3();
	afx_msg void						OnBnClickedButton5();
	afx_msg void						OnEnChangeEdit5();
	afx_msg void						OnBnClickedButton1();
	afx_msg void						OnEnChangeEdit1();
	afx_msg void						OnEnChangeEdit8();
	afx_msg void						OnEnChangeEdit10();
	afx_msg void						OnEnChangeEditUserid2();
	afx_msg void						OnEnChangeEditUserid();
	afx_msg void						OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT						OnCountMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void						OnEnChangeEdit2();
	afx_msg void						OnTimer(UINT_PTR nIDEvent);
	void								kill_Timer(UINT_PTR nIDEvent);
	//初始化玩家坐桌参数
	void								init_robot_cnt_param(int index);
	//给MFC添加界面显示数据
	void								push_robot_info_to_mfc(int index);
	//添加登陆包间任务
	void								add_robot_login_task(int index);
	//刷新界面显示
	void								refresh_mfc_show(int index);
	//机器人充值金币
	void								checkAddMoney(unsigned int userId, int current_money);
	//机器人自动更换昵称和logo信息
	bool								robotchangeInfo(unsigned int userId, std::string& strnickname, std::string& strlogoinfo);
	//通知平台更新玩家信息
	void								notify_update_user_info(unsigned int userId);
	//重新获取PHP信息
	void								RobotReLogin(unsigned int userId);
public:
	//玩家人数
	int									m_robot_count;						//设置机器人数量
	utility::BasicHttpContext			m_http_handle;
	std::map<int, std::string>			m_user_info;
	desk_info							m_desk_info[MAX_USER_PLAY];
	userserver::usermanager*			m_pUserMgr[MAX_USER_PLAY];
	utility::BasicThreadPool			m_thread_pool;
private:
	std::vector <userBaseInfo>			m_ubinfo;							//配置文件获取的玩家信息 
	std::vector <Httplogin>				httpresult;							//登入PHP返回的信息
	userSitInfo							m_user_sit_info[MAX_USER_PLAY];
	unsigned int						m_robot_login_count;				//机器人登录个数	
	std::vector<mfc_show_user_info>		mfc_user_info;						//界面刷新数据
	long long                           m_first_time;						//第一次登录的时间
	int                                 play_time;							//玩家游戏时间
	int									loginSeq[MAX_USER_PLAY];			//请求桌子信息索引
	int									loginSeq_count;
	int									m_loginedSeq_count;					//已登录人数
	int                                 m_rabot_off_player_count;			//机器人下线的的真人最大值
public:
	//对话框数据
	enum { IDD = IDD_Robot_DIALOG };
public:
	int									m_user_id;
	int									m_port;
	CString								m_strIP;
	CString								m_PHP_Adress;
	CString								m_game_name;
	std::string							m_s_game_name;
	CString								m_user_pass;
	int									m_game_id;
	int									m_user_first_id;
	CString								m_user_password;
	int									m_login_desk_num;
	int									m_private_num;
	int									m_free;
	int									m_club_room_id;
	int									m_real_room_id;
	int									m_rule_id;
	int									m_play_game_count;					//多少局后换桌
	int									m_play_game_count_min;				//最大局数换桌
	int									m_play_game_count_max;				//最少局数换桌
	//自动更换logo和昵称信息
	int									m_ichangetimespan;
	CListCtrl							m_ListTem;
	CEdit								m_EditTest;
	HWND								m_hWnd;
	bool								b_real_socket_cnt;
	char								m_room_real_host[MAX_BUFFER];		//PHP地址
	int									m_real_port;						//PHP端口
	CheckC								m_real_check;						//检查类对象
	unsigned int						m_allow_robat_count;				//每张桌子允许的最大机器人数
	DCGR::stuAllDeskInfo				m_all_desk_info;
	int									m_recharge_limit;					//机器人金币低于多少时,自动充值
	int									m_recharge_money;					//机器人充值金币
	int									m_user_lock_gold[MAX_USER_PLAY];	//金币不足的玩家
	int									m_user_lock_count;
};