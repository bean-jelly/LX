// RobotDlg.h : ͷ�ļ�
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

//CRobotDlg �Ի���
class CRobotDlg : public CDialogEx
{
//����
public:
	//��׼���캯��
	CRobotDlg(CWnd* pParent = NULL);	
	CRobotDlg::~CRobotDlg();
protected:
	virtual void						DoDataExchange(CDataExchange* pDX);// DDX/DDV ֧��
protected:
	HICON								m_hIcon;
	//���ɵ���Ϣӳ�亯��
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
	//��ʼ�������������
	void								init_robot_cnt_param(int index);
	//��MFC��ӽ�����ʾ����
	void								push_robot_info_to_mfc(int index);
	//��ӵ�½��������
	void								add_robot_login_task(int index);
	//ˢ�½�����ʾ
	void								refresh_mfc_show(int index);
	//�����˳�ֵ���
	void								checkAddMoney(unsigned int userId, int current_money);
	//�������Զ������ǳƺ�logo��Ϣ
	bool								robotchangeInfo(unsigned int userId, std::string& strnickname, std::string& strlogoinfo);
	//֪ͨƽ̨���������Ϣ
	void								notify_update_user_info(unsigned int userId);
	//���»�ȡPHP��Ϣ
	void								RobotReLogin(unsigned int userId);
public:
	//�������
	int									m_robot_count;						//���û���������
	utility::BasicHttpContext			m_http_handle;
	std::map<int, std::string>			m_user_info;
	desk_info							m_desk_info[MAX_USER_PLAY];
	userserver::usermanager*			m_pUserMgr[MAX_USER_PLAY];
	utility::BasicThreadPool			m_thread_pool;
private:
	std::vector <userBaseInfo>			m_ubinfo;							//�����ļ���ȡ�������Ϣ 
	std::vector <Httplogin>				httpresult;							//����PHP���ص���Ϣ
	userSitInfo							m_user_sit_info[MAX_USER_PLAY];
	unsigned int						m_robot_login_count;				//�����˵�¼����	
	std::vector<mfc_show_user_info>		mfc_user_info;						//����ˢ������
	long long                           m_first_time;						//��һ�ε�¼��ʱ��
	int                                 play_time;							//�����Ϸʱ��
	int									loginSeq[MAX_USER_PLAY];			//����������Ϣ����
	int									loginSeq_count;
	int									m_loginedSeq_count;					//�ѵ�¼����
	int                                 m_rabot_off_player_count;			//���������ߵĵ��������ֵ
public:
	//�Ի�������
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
	int									m_play_game_count;					//���پֺ���
	int									m_play_game_count_min;				//����������
	int									m_play_game_count_max;				//���پ�������
	//�Զ�����logo���ǳ���Ϣ
	int									m_ichangetimespan;
	CListCtrl							m_ListTem;
	CEdit								m_EditTest;
	HWND								m_hWnd;
	bool								b_real_socket_cnt;
	char								m_room_real_host[MAX_BUFFER];		//PHP��ַ
	int									m_real_port;						//PHP�˿�
	CheckC								m_real_check;						//��������
	unsigned int						m_allow_robat_count;				//ÿ���������������������
	DCGR::stuAllDeskInfo				m_all_desk_info;
	int									m_recharge_limit;					//�����˽�ҵ��ڶ���ʱ,�Զ���ֵ
	int									m_recharge_money;					//�����˳�ֵ���
	int									m_user_lock_gold[MAX_USER_PLAY];	//��Ҳ�������
	int									m_user_lock_count;
};