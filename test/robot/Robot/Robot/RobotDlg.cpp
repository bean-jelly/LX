#include <sstream>
#include "stdafx.h"
#include "Robot.h"
#include "RobotDlg.h"
#include "afxdialogex.h"  
#include "basicbuffer\BasicTranscode.h" 
#include "BasicBuffer.h"
#include "json\json.h" 
#include "basiclog\basicloginterface.h" 
#include "usertask.h" 
#include "basichttphelper.h"

#include "basicdump/basicdumpinterface.h" 
#ifdef _DEBUG
#define new DEBUG_NEW   
#pragma comment(lib,"basiccode.lib")
#endif
#pragma warning(disable:4996)


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

	ON_WM_PAINT()
	ON_MESSAGE(WM_COUNT_MSG, &CRobotDlg::OnCountMsg)

END_MESSAGE_MAP()


// CRobotDlg 对话框
CRobotDlg::CRobotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRobotDlg::IDD, pParent)
	, m_robot_count(1)
	, m_user_id(0)
	, m_port(0)
	, m_strIP(_T("127.0.0.1"))
	, m_PHP_Adress(_T(""))
	, m_user_pass(_T(""))
	, m_game_id(10000149)
	, m_user_first_id(6023800)
	, m_user_password(_T("0"))
	, m_login_desk_num(3)
	, m_private_num(1)
	, m_club_room_id(12)
	, m_rule_id(2)
{
	BASIC_LOG_INIT("dachuan/log/Robot.log");
	m_ubinfo.clear();
	httpresult.clear();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_ichangetimespan = 720;

	basicdump::basicdumpinterface::get_instacne()->start_dump(true);
}

CRobotDlg::~CRobotDlg()
{
	basicdump::basicdumpinterface::get_instacne()->stop_dump();
	BASIC_LOG_CLOSE();
}

void CRobotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERID, m_robot_count);
	DDV_MinMaxInt(pDX, m_robot_count, 1, 99);
	DDX_Text(pDX, IDC_EDIT6, m_game_id);;
	DDX_Text(pDX, IDC_EDIT_USERID3, m_club_room_id);
	DDX_Text(pDX, IDC_EDIT_USERID2, m_rule_id);
	DDX_Control(pDX, IDC_LIST1, m_ListTem);
	DDX_Control(pDX, IDC_EDIT1, m_EditTest);
}

BEGIN_MESSAGE_MAP(CRobotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &CRobotDlg::OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CRobotDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON4, &CRobotDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, &CRobotDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT5, &CRobotDlg::OnEnChangeEdit5)
	ON_BN_CLICKED(IDC_BUTTON1, &CRobotDlg::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT1, &CRobotDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT8, &CRobotDlg::OnEnChangeEdit8)
	ON_EN_CHANGE(IDC_EDIT10, &CRobotDlg::OnEnChangeEdit10)
	ON_EN_CHANGE(IDC_EDIT_USERID2, &CRobotDlg::OnEnChangeEditUserid2)
	ON_EN_CHANGE(IDC_EDIT_USERID, &CRobotDlg::OnEnChangeEditUserid)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CRobotDlg::OnLvnItemchangedList1)
	ON_EN_CHANGE(IDC_EDIT2, &CRobotDlg::OnEnChangeEdit2)
	ON_MESSAGE(WM_COUNT_MSG, &CRobotDlg::OnCountMsg)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRobotDlg 消息处理程序

BOOL CRobotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	BASIC_LOG_INFO("CRobotDlg OnInitDialog ");
	// 将“关于...”菜单项添加到系统菜单中。

	HWND hWnd = ::FindWindow(NULL, m_game_name);
	if (hWnd != NULL)
	{
		m_hWnd = hWnd;
		m_real_check.set_hwnd(m_hWnd);
	}

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	// 执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	SetWindowText(m_game_name);
	ReadRoomConfig();
	m_thread_pool.InitThreadPool(8);
	
	m_EditTest.ShowWindow(SW_HIDE);
	CRect rect;
	m_ListTem.GetClientRect(&rect);
	m_ListTem.SetExtendedStyle(m_ListTem.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_SHOWSELALWAYS);
	m_ListTem.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 80);
	m_ListTem.InsertColumn(1, _T("玩家id"), LVCFMT_CENTER, 100);
	m_ListTem.InsertColumn(2, _T("游戏Id"), LVCFMT_CENTER, 100);
	m_ListTem.InsertColumn(3, _T("玩家状态"), LVCFMT_CENTER, 100);
	m_ListTem.InsertColumn(4, _T("桌子位置"), LVCFMT_CENTER, 100);
	m_ListTem.InsertColumn(5, _T("桌子号"), LVCFMT_CENTER, 100);
	m_ListTem.InsertColumn(6, _T("玩家金币"), LVCFMT_CENTER, 200);
	m_ListTem.InsertColumn(7, _T("备注"), LVCFMT_CENTER, 200);
	//m_ListTem.InsertColumn(7, _T("上线时间"), LVCFMT_CENTER, 100);
	//m_ListTem.InsertColumn(8, _T("下线时间"), LVCFMT_CENTER, 100);
	//m_ListTem.InsertColumn(9, _T("玩家批次"), LVCFMT_CENTER, 100);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRobotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CRobotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRobotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

std::string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0) return std::string("");
	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;
	std::string strTemp(pszDst);
	delete[] pszDst;
	return strTemp;
}

std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen) {
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0) return NULL;
	WCHAR *pwszDst = new WCHAR[nSize + 1];
	if (NULL == pwszDst) return NULL;
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;  if (pwszDst[0] == 0xFEFF)
		for (int i = 0; i < nSize; i++)
			pwszDst[i] = pwszDst[i + 1];
	std::wstring wcharString(pwszDst);
	delete[] pwszDst;
	return wcharString;
}

void  CRobotDlg::StartUserMgr()
{
	m_real_check.init(m_club_room_id, m_room_real_host, m_real_port, m_rabot_off_player_count, m_allow_robat_count);
	for (int i = 0; i < MAX_USER_PLAY; i++)
	{
		if (m_pUserMgr[i] != NULL)
		{
			delete m_pUserMgr[i];
			if (m_real_check.m_pUserMgr[i] != NULL)
			{
				delete  m_real_check.m_pUserMgr[i];
			}

			m_pUserMgr[i] = NULL;
			m_real_check.m_pUserMgr[i] = NULL;
		}
		m_pUserMgr[i] = new userserver::usermanager();
		m_real_check.m_pUserMgr[i] = m_pUserMgr[i];
	}
}

void  CRobotDlg::AddUserInfo()
{

}

void CRobotDlg::OnBnClickedButtonCreate()
{
	// TODO:  在此添加控件通知处理程序代码
}

void  CRobotDlg::RobotLogin(int index)
{
	if (m_ubinfo.size() <= index)
	{
		return;
	}
	UpdateData();

	userBaseInfo singleDate = m_ubinfo[index];
	istringstream iss(std::to_string(time(0)));
	Json::Value param;
	Json::Value data;
	param["action"] = "robotlogin";
	param["version"] = "v10001";
	param["key_value"] = 1;
	param["flag_value"] = 1;
	int num;
	iss >> num;
	param["sign_value"] = num;
	data["userid"] = singleDate.user_id;
	data["password"] = singleDate.password;
	data["platform"] = 1;
	param["data_value"] = data;

	Json::FastWriter writer;
	std::string  keyvalue = "param=" + writer.write(param);
	utility::BasicBuffer szBuffer(keyvalue.c_str(), keyvalue.length());
	utility::BasicBuffer szGetBuffer(BUFFER_SIZE, BUFFER_GROW);
	utility::BasicHttpContext http;
	http.HttpSendConnectEx(m_PHP_Adress, BASIC_HTTP_VERB_POST, &szBuffer, szGetBuffer, 5, 1);
	std::string  result((char*)szGetBuffer.buffer());
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(result, root))
	{
		return;
	}
	if (root["data_value"].isNull())
	{
		return;
	}

	Json::Value test;
	test = root["data_value"];
	std::string  data_value = "";
	data_value = writer.write(test);
	Json::Value root_value;
	if (!reader.parse(data_value, root_value))
	{
		return;
	}
	int code_value = root_value["code_value"].asInt();
	if (code_value != 0)
	{
		return;
	}
	Json::Value private_info;
	private_info = root_value["data"];
	data_value = "";
	data_value = writer.write(private_info);
	Json::Value root_desk;
	if (!reader.parse(data_value, root_desk))
	{
		return;
	}
	Httplogin login_info;
	login_info.clear();
	if (gameservice::basichttphelper::parse_login_info((char*)szGetBuffer.buffer(), 0, login_info) == 0)
	{
		Httplogin temloginInfo;
		memcpy(&temloginInfo, &login_info, sizeof(temloginInfo));
		httpresult.push_back(temloginInfo);
		m_user_sit_info[m_robot_login_count].p_user_id = login_info.player_info.player_id;
		m_ubinfo[index].user_id = login_info.player_info.player_id;
		memcpy(m_user_sit_info[m_robot_login_count].p_player_name, &login_info.player_info.player_name, sizeof(m_user_sit_info[m_robot_login_count].p_player_name));
		memcpy(m_user_sit_info[m_robot_login_count].p_player_password, &login_info.player_info.player_password, sizeof(m_user_sit_info[m_robot_login_count].p_player_password));
		memcpy(m_user_sit_info[m_robot_login_count].p_player_pcid, &login_info.player_info.player_pcid, sizeof(m_user_sit_info[m_robot_login_count].p_player_pcid));
		memcpy(m_user_sit_info[m_robot_login_count].p_player_token, &login_info.player_info.player_token, sizeof(m_user_sit_info[m_robot_login_count].p_player_token));
		//php地址
		for (int i = 0; i < m_PHP_Adress.GetLength(); i++)
		{
			m_user_sit_info[m_robot_login_count].php_address[i] = m_PHP_Adress.GetAt(i);
		}
		m_user_sit_info[m_robot_login_count].php_address[m_PHP_Adress.GetLength()] = 0x00;
		//游戏名字
		for (int i = 0; i < m_s_game_name.length(); i++)
		{
			m_user_sit_info[m_robot_login_count].game_name[i] = m_s_game_name[i];
		}
		m_user_sit_info[m_robot_login_count].game_name[m_s_game_name.length()] = '\0';

		m_user_sit_info[m_robot_login_count].club_desk_game_id = m_game_id;

		m_user_sit_info[m_robot_login_count].ilastchangetime = time(0);

		//头像昵称
		std::string strnickname;
		std::string  struserlogo;
		robotchangeInfo(m_user_sit_info[m_robot_login_count].p_user_id, strnickname, struserlogo);
		m_user_sit_info[m_robot_login_count].ilastchangetime = time(0);

		BASIC_LOG_INFO("LoginW m_s_game_name = (%s) 登入PHP 成功 m_robot_login_count = %d", m_s_game_name.c_str(), m_robot_login_count);
		m_robot_login_count++;
	}
}

void CRobotDlg::RobotLoginDesk(int index)
{
	//初始化玩家坐桌参数
	init_robot_cnt_param(index);
	//给MFC添加界面显示数据
	push_robot_info_to_mfc(index);
	//添加登陆包间任务
	add_robot_login_task(index);
}

//点击搓桌开始游戏
void CRobotDlg::OnBnClickedButtonLogin()
{
	// TODO:  在此添加控件通知处理程序代码
	GetDlgItem(IDC_BUTTON_LOGIN)->EnableWindow(FALSE);
	
	ReadRobotConfig();
	StartUserMgr();

	if (m_ubinfo.empty())
	{
		BASIC_LOG_INFO("CRobotDlg OnBnClickedButtonLogin 登录php 读取不到的机器人玩家");
		return;
	}

	m_robot_login_count = 0;
	SetTimer(TIME_USER_LOGIN_PHP * MFC_TIME_START_ID, ONE_SECONDE, NULL);
	return;
}

void CRobotDlg::OnBnClickedButton4()
{
	// TODO:  在此添加控件通知处理程序代码
}

//刷新金币不足的玩家金币信息
void CRobotDlg::OnBnClickedButton2()
{
	BASIC_LOG_INFO("CRobotDlg OnBnClickedButton2 刷新金币不足的玩家金币信息");
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	memset(m_user_lock_gold, 0, sizeof(m_user_lock_gold));
	m_user_lock_count = 0;
	for (int i = 0; i < mfc_user_info.size(); i++)
	{
		if (mfc_user_info.at(i).user_state == 255)
		{
			m_user_lock_gold[m_user_lock_count++] = mfc_user_info.at(i).user_id;
		}
	}
	if (m_user_lock_count > 0)
	{
		BASIC_LOG_INFO("CRobotDlg::OnBnClickedButton2() m_user_lock_count= %d ", m_user_lock_count);
		SetTimer(TIME_USER_RE_LOGIN_PHP*MFC_TIME_START_ID, 1000, NULL);
	}
	else
	{
		GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
		BASIC_LOG_INFO("CRobotDlg::OnBnClickedButton2() no user need re login ");
	}
}

void CRobotDlg::OnEnChangeEdit5()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CRobotDlg::ReadRoomConfig()
{
	BASIC_LOG_INFO("CRobotDlg readconfig  读取配置文件");
	UpdateData();
	
	char chpath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, chpath);
	char chPathFile[MAX_PATH] = { 0 };
	sprintf_s(chPathFile, MAX_PATH, "%s\\AI_client_logic_s.ini", chpath);

	std::string strFirstKey = "setup";
	std::string strSecondKey = "game_id";
	m_game_id = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), -1, chPathFile);

	strSecondKey = "room_id";
	m_real_room_id = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), -1, chPathFile);

	strSecondKey = "club_room_id";
	m_club_room_id = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 11, chPathFile);

	strSecondKey = "rule_id";
	m_rule_id = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 1, chPathFile);

	UpdateData(FALSE);
	strSecondKey = "game_name";
	char strValue[MAX_BUFFER];
	GetPrivateProfileStringA(strFirstKey.c_str(), strSecondKey.c_str(), "0", strValue, MAX_BUFFER, chPathFile);
	if (strcmp(strValue, "0") == 0)
	{
		BASIC_LOG_INFO("CRobotDlg readconfig  读取配置文件 读取不到 m_game_name ");
		m_game_name = "游戏机器人平台";
		m_s_game_name = "游戏机器人平台";
	}
	else
	{
		int charLen = strlen(strValue);
		//计算多字节字符的大小，按字符计算。
		int len = MultiByteToWideChar(CP_ACP, 0, strValue, charLen, NULL, 0);
		//为宽字节字符数组申请空间，数组大小为按字节计算的多字节字符大小
		TCHAR *buf = new TCHAR[len + 1];
		//多字节编码转换成宽字节编码
		MultiByteToWideChar(CP_ACP, 0, strValue, charLen, buf, len);
		buf[len] = '\0';
		//将TCHAR数组转换为CString
		m_s_game_name = strValue;
		m_game_name.Append(buf);
		//删除缓冲区
		delete[]buf;
		buf = NULL;

		BASIC_LOG_INFO("CRobotDlg readconfig  读取配置文件  m_game_name = %s ", m_s_game_name.c_str());
	}

	//端口号
	strSecondKey = "svn_port";
	m_real_port = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 0, chPathFile);

	{
		//IP地址
		strSecondKey = "login_ip";
		char strkey[MAX_BUFFER] = { 0 };
		strcpy(strkey, strSecondKey.c_str());
		char strValue[MAX_BUFFER];
		GetPrivateProfileStringA(strFirstKey.c_str(), strkey, "0", strValue, MAX_BUFFER, chPathFile);
		if (strcmp(strValue, "0") == 0)
		{
			//没有值
			BASIC_LOG_INFO("CRobotDlg readconfig  读取配置文件 读取不到 ip 的地址 ");
		}
		else
		{
			strcpy(m_room_real_host, strValue);
			std::string ip = m_room_real_host;
			BASIC_LOG_INFO("CRobotDlg readconfig  读取配置文件  ip 的地址 = %s", ip.c_str());
		}
	}

	{
		strSecondKey = "m_PHP_Address";
		char strkey[MAX_BUFFER] = { 0 };
		strcpy(strkey, strSecondKey.c_str());
		char strValue[MAX_BUFFER];
		GetPrivateProfileStringA(strFirstKey.c_str(), strkey, "0", strValue, MAX_BUFFER, chPathFile);
		if (strcmp(strValue, "0") == 0)
		{
			BASIC_LOG_INFO("CRobotDlg readconfig  读取配置文件 读取不到 m_PHP_Address 的地址 ");
		}
		else
		{
			std::string PHP_address(strValue);
			//计算多字节字符的大小，按字符计算。
			int len = MultiByteToWideChar(CP_ACP, 0, PHP_address.c_str(), PHP_address.length(), NULL, 0);
			//为宽字节字符数组申请空间，数组大小为按字节计算的多字节字符大小
			TCHAR *w_buf = new TCHAR[len + 1];
			//多字节编码转换成宽字节编码
			MultiByteToWideChar(CP_ACP, 0, PHP_address.c_str(), PHP_address.length(), w_buf, len);
			w_buf[len] = '\0'; //添加字符串结尾，注意不是len+1
			//将TCHAR数组转换为CString
			m_PHP_Adress.Append(w_buf);
			BASIC_LOG_INFO("CRobotDlg readconfig  读取配置文件  PHP 的地址 = %s ", PHP_address.c_str());
			//删除缓冲区
			delete[]w_buf;
		}
	}
	return;
}

void CRobotDlg::ReadRobotConfig()
{
	UpdateData();

	m_ubinfo.clear();
	char chpath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryA(MAX_PATH, chpath);
	char chPathFile[MAX_PATH] = { 0 };
	sprintf_s(chPathFile, MAX_PATH, "%s\\AI_client_logic_s.ini", chpath);

	std::string strFirstKey = "setup";

	//连续最少退出局数
	string strSecondKey = "play_game_count_min";
	m_play_game_count_min = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 5, chPathFile);

	//连续最多退出局数
	strSecondKey = "play_game_count_max";
	m_play_game_count_max = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 10, chPathFile);

	//机器人自由坐桌最大桌子数
	strSecondKey = "desk_for_robot_count";
	m_real_check.m_desk_for_robot_count = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 2, chPathFile);

	//真实玩家总在线数量,到达时不再配对机器人
	strSecondKey = "rabot_off";
	m_real_check.m_rabot_off_player_count = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 16, chPathFile);

	strSecondKey = "check_time";
	m_real_check.m_real_check_time = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 10, chPathFile);

	//每张桌子允许机器人最大数量
	strSecondKey = "allow_robat_count";
	m_allow_robat_count = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 2, chPathFile);

	//每张桌子有多少真实玩家时,不需要机器人
	strSecondKey = "desk_real_player";
	m_real_check.m_desk_real_player_no_robat = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 5, chPathFile);

	//机器人金币低于多少时,自动充值
	strSecondKey = "recharge_limit";
	m_recharge_limit = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 10000, chPathFile);

	//机器人充值金币
	strSecondKey = "recharge_money";
	m_recharge_money = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 0, chPathFile);

	//机器人自动更换图像时间
	strSecondKey = "changeinfomaxtime";
	m_ichangetimespan = GetPrivateProfileIntA(strFirstKey.c_str(), strSecondKey.c_str(), 720, chPathFile);

	//读取机器人信息
	for (unsigned int i = 1; i <= m_robot_count; i++)
	{
		string user = "robot" + std::to_string(i);
		char strValue[MAX_BUFFER] = { 0 };
		GetPrivateProfileStringA(strFirstKey.c_str(), user.c_str(), "0", strValue, MAX_BUFFER, chPathFile);
		if (strcmp(strValue, "0") == 0 || strcmp(strValue, "") == 0)
		{
			BASIC_LOG_INFO("CRobotDlg readconfig  读取配置文件 读取不到 玩家信息 ");
		}
		else
		{
			char byStrList[5][100] = { 0 };
			int iCount = 0;
			string2list(strValue, byStrList, iCount);
			userBaseInfo ub;
			ub.user_id = atoi(byStrList[0]);
			memcpy(ub.password, byStrList[1], sizeof(ub.password));
			m_ubinfo.push_back(ub);
		}
	}
	return;
}

//将字符串转化成数组
void CRobotDlg::string2list(char *str, char byStrList[][100], int& iCount)
{
	if (str == NULL)
	{
		return;
	}
	iCount = 0;
	const char * split = ",";
	char * p;
	p = strtok(str, split);
	while (p != NULL)
	{
		strcpy(byStrList[iCount++], p);
		p = strtok(NULL, split);
	}
}

//换桌
void CRobotDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData();
}

void CRobotDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CRobotDlg::OnEnChangeEdit8()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CRobotDlg::OnEnChangeEdit10()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CRobotDlg::OnEnChangeEditUserid2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CRobotDlg::OnEnChangeEditUserid()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void CRobotDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}

void CRobotDlg::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	// TODO:  在此添加控件通知处理程序代码
}

LRESULT CRobotDlg::OnCountMsg(WPARAM wParam, LPARAM lParam)
{
	desk_info*p_str = (desk_info *)lParam;

	switch (wParam)
	{
	case USER_STATE_TO_MFC_LOGIN_RESULT:
	{
		//登录成功
		DCGR::MSG_UserSitResult* pUserSit = (DCGR::MSG_UserSitResult *)lParam;
		if (pUserSit->userinfo.iUserState != 0)
		{
			for (int i = 0; i < mfc_user_info.size(); i++)
			{
				if (pUserSit->userinfo.iUserid == mfc_user_info.at(i).user_id)
				{
					mfc_user_info.at(i).user_state = pUserSit->userinfo.iUserState;
					mfc_user_info.at(i).desk_no = pUserSit->userinfo.iDeskNO;
					mfc_user_info.at(i).iCoins = pUserSit->userinfo.iCoins;
					mfc_user_info.at(i).iChairID = pUserSit->userinfo.iChairID;
					mfc_user_info.at(i).userNote = 0;
					updateMfcUserInfo();
				}
			}
		}
		else
		{
			for (int i = 0; i < mfc_user_info.size(); i++)
			{
				if (pUserSit->userinfo.iUserid == mfc_user_info.at(i).user_id)
				{
					mfc_user_info.at(i).user_state = 1;
					mfc_user_info.at(i).desk_no = -1;
					mfc_user_info.at(i).iCoins = -1;
					mfc_user_info.at(i).iChairID = -1;
					updateMfcUserInfo();
				}
			}
		}
	}
	break;
	case USER_STATE_TO_MFC_LOGIN:
	{
		updateMfcUserInfo();
	}
	break;
	case USER_STATE_TO_MFC_GOLD_CHANGE:
	{
		//刷新机器人金币
		gold_change* gold_change_param = (gold_change *)lParam;
		for (int i = 0; i < mfc_user_info.size(); i++)
		{
			if (gold_change_param->player_id == mfc_user_info.at(i).user_id)
			{
				BASIC_LOG_INFO("USER_STATE_TO_MFC_GOLD_CHANGE 金币发生变化 玩家 = （%d）,当前值 = (%d),变化值 = (%I64d) ", gold_change_param->player_id, mfc_user_info.at(i).iCoins, gold_change_param->gold);
				mfc_user_info.at(i).iCoins += gold_change_param->gold;
				updateMfcUserInfo();
			}
		}
	}
	break;
	case USER_STATE_TO_MFC_RECHANGE:
	{
		//机器人充值
		gold_rechange* gold_rechange_param = reinterpret_cast<gold_rechange*>(lParam);
		for (int i = 0; i < mfc_user_info.size(); i++)
		{
			if (gold_rechange_param->player_id == mfc_user_info.at(i).user_id)
			{
				checkAddMoney(gold_rechange_param->player_id, mfc_user_info.at(i).iCoins);
			}
		}
	}
	break;
	case USER_STATE_TO_MFC_LEFT_RESULT:
	{
		DCGR::MSG_UserLeftResult *UserSit = (DCGR::MSG_UserLeftResult *)lParam;
		for (int i = 0; i < mfc_user_info.size(); i++)
		{
			if (UserSit->dwUserID == mfc_user_info.at(i).user_id)
			{
				if (UserSit->dwCode == RES_UP_RESULT_CODE_4)
				{
					//金不足离开
					mfc_user_info.at(i).user_state = 255;
					mfc_user_info.at(i).userNote = -1;
					mfc_user_info.at(i).iChairID = -1;
					mfc_user_info.at(i).desk_no = -1;

					//检查是否可以充值
					for (int i = 0; i < mfc_user_info.size(); i++)
					{
						if (UserSit->dwUserID == mfc_user_info.at(i).user_id)
						{
							checkAddMoney(UserSit->dwUserID, mfc_user_info.at(i).iCoins);
						}
					}
				}
				else
				{
					//自动离开
					mfc_user_info.at(i).user_state = 1;
					mfc_user_info.at(i).userNote = -1;
					mfc_user_info.at(i).iChairID = -1;
					mfc_user_info.at(i).desk_no = -1;
				}
				updateMfcUserInfo();
				break;
			}
		}
		//玩家更新信息
		for (int i = 0; i < MAX_USER_PLAY; i++)
		{
			if (m_user_sit_info[i].p_user_id == UserSit->dwUserID)
			{
				int ugettime = time(0) - m_user_sit_info[i].ilastchangetime;
				if (ugettime > m_ichangetimespan * 60)
				{
					std::string strnickname;
					std::string  struserlogo;
					robotchangeInfo(UserSit->dwUserID, strnickname, struserlogo);
					m_user_sit_info[i].ilastchangetime = time(0);
				}
				break;
			}
		}
	}
	break;
	case USER_STATE_TO_MFC_GOLD_LACK:
	{

	}
	break;
	case USER_STATE_TO_MFC_DESK_INFO:
	{
		DCGR::stuAllDeskInfo *param = (DCGR::stuAllDeskInfo *)lParam;
		memcpy(&m_all_desk_info, param, sizeof(m_all_desk_info));
	}
	break;

	case USER_STATE_TO_MFC_USER_INFO:
	{
		DCGR::MSG_UserLoginRoomResult* LoginResult = (DCGR::MSG_UserLoginRoomResult *)lParam;

		idle_player_info ipi;
		ipi.socket_cnt = true;
		ipi.sit.dwUserID = LoginResult->userinfo.iUserid;
		ipi.sit.iClubRoomID = m_club_room_id;
		m_real_check.add_idle_player(ipi);

		//检查是否可以充值
		for (int i = 0; i < mfc_user_info.size(); i++)
		{
			if (LoginResult->userinfo.iUserid == mfc_user_info.at(i).user_id)
			{
				mfc_user_info.at(i).user_state = 1;
				mfc_user_info.at(i).userNote = -1;
				mfc_user_info.at(i).iChairID = -1;
				mfc_user_info.at(i).desk_no = -1;
			}
		}
	}
	break;
	default:
		break;
	}

	return 1;
}

void CRobotDlg::updateMfcUserInfo()
{
	//更新MFC界面玩家信息
	int row = 0;//行
	int cow = 0;//列

	for (auto iter = mfc_user_info.begin(); iter != mfc_user_info.end(); iter++)
	{
		m_ListTem.DeleteItem(row);
		m_ListTem.InsertItem(row, _T(""));
		cow = 0;
		mfc_show_user_info userinfo = (*iter);
		CString strTemp;

		//玩家id
		if (true)
		{
			strTemp.Format(_T("%d"), row + 1);
		}
		else
		{
			strTemp = _T("");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		//玩家id
		if (userinfo.user_id != 0)
		{
			strTemp.Format(_T("%d"), userinfo.user_id);
		}
		else
		{
			strTemp = _T("");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		//游戏Id
		if (userinfo.game_id != 0)
		{
			strTemp.Format(_T("%d"), userinfo.game_id);
		}
		else
		{
			strTemp = _T("");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		//玩家状态
		if (userinfo.user_state == 0)
		{
			strTemp = _T("");
		}
		else if (userinfo.user_state == 1)
		{
			strTemp = _T("空闲状态");
		}
		else if (userinfo.user_state == 2)
		{
			strTemp = _T("坐在游戏桌");
		}
		else if (userinfo.user_state == 3)
		{
			strTemp = _T("同意状态");
		}
		else if (userinfo.user_state == 5)
		{
			strTemp = _T("断线状态");
		}
		else if (userinfo.user_state == 6)
		{
			strTemp = _T("游戏进行中");
		}
		else if (userinfo.user_state == 7)
		{
			strTemp = _T("充值成功");
		}
		else if (userinfo.user_state == 255)
		{
			strTemp = _T("金币不足");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		//桌子位置
		if (userinfo.iChairID != -1)
		{
			strTemp.Format(_T("%d"), userinfo.iChairID);
		}
		else
		{
			strTemp = _T("");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		//桌子号
		if (userinfo.desk_no != -1)
		{
			strTemp.Format(_T("%d"), userinfo.desk_no);
		}
		else
		{
			strTemp = _T("");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		//玩家金币
		if (userinfo.iCoins != -1)
		{
			strTemp.Format(_T("%lld"), userinfo.iCoins);
		}
		else
		{
			strTemp = _T("");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		/*//玩家上线时间
		if (1)
		{
			CString str; //获取系统时间 　　
			int h1 = (userinfo.startTime / 100000);
			int h2 = (userinfo.startTime % 100000) / 10000;

			int m1 = ((userinfo.startTime % 100000) % 10000) / 1000;
			int m2 = (((userinfo.startTime % 100000) % 10000) % 1000) / 100;

			int s1 = ((((userinfo.startTime % 100000) % 10000) % 1000) % 100) / 10;
			int s2 = ((((userinfo.startTime % 100000) % 10000) % 1000) % 100) % 10;

			CString str1;
			str1.Format(_T("%d"), h1);
			CString str2;
			str2.Format(_T("%d"), h2);

			CString str3;
			str3.Format(_T("%d"), m1);

			CString str4;
			str4.Format(_T("%d"), m2);

			CString str5;
			str5.Format(_T("%d"), s1);

			CString str6;
			str6.Format(_T("%d"), s2);

			strTemp = str1 + str2 + _T(":") + str3 + str4 + _T(":") + str5 + str6;
		}
		else
		{
			strTemp = _T("");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		//玩家下线时间
		if (1)
		{
			CString str; //获取系统时间 　　
			int h1 = (userinfo.endTime / 100000);
			int h2 = (userinfo.endTime % 100000) / 10000;

			int m1 = ((userinfo.endTime % 100000) % 10000) / 1000;
			int m2 = (((userinfo.endTime % 100000) % 10000) % 1000) / 100;

			int s1 = ((((userinfo.endTime % 100000) % 10000) % 1000) % 100) / 10;
			int s2 = ((((userinfo.endTime % 100000) % 10000) % 1000) % 100) % 10;

			CString str1;
			str1.Format(_T("%d"), h1);
			CString str2;
			str2.Format(_T("%d"), h2);

			CString str3;
			str3.Format(_T("%d"), m1);

			CString str4;
			str4.Format(_T("%d"), m2);

			CString str5;
			str5.Format(_T("%d"), s1);

			CString str6;
			str6.Format(_T("%d"), s2);

			strTemp = str1 + str2 + _T(":") + str3 + str4 + _T(":") + str5 + str6;
		}
		else
		{
			strTemp = _T("");
		}

		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;

		strTemp = _T("");
		m_ListTem.SetItemText(row, cow, strTemp);
		cow++;*/

		//玩家备注
		if (userinfo.userNote != -1)
		{
			strTemp = _T("");
		}
		else
		{
			strTemp = _T("玩家已下线");
		}
		m_ListTem.SetItemText(row, cow, strTemp);
		row++;
	}
}

void CRobotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	unsigned int EventID	= nIDEvent / MFC_TIME_START_ID;
	unsigned int UserIndex	= nIDEvent % MFC_TIME_START_ID;
	unsigned int user_id	= m_pUserMgr[UserIndex]->m_my_userid;
	unsigned int MainType	= EventID / 10000000;
	unsigned int SubType	= EventID / 1000000;

	if (MainType == 1 || SubType == 20)
	{
		m_pUserMgr[UserIndex]->on_user_gl_timer(EventID, user_id);
	}
	else if (SubType == 21)
	{
		m_pUserMgr[UserIndex]->m_user_action_mgr->on_timer(EventID);
	}
	else if (SubType == 22)
	{
		m_pUserMgr[UserIndex]->on_user_timer(EventID);
	}
	else if (SubType == 24)
	{
		m_real_check.on_timer(EventID);
	}
	else if (SubType == 23)
	{
		//处理mfc类定时器
		switch (EventID)
		{
		case TIME_USER_LOGIN_PHP:
		{
			//玩家登录PHP
			KillTimer(TIME_USER_LOGIN_PHP*MFC_TIME_START_ID);
			for (int index = 0; index < m_ubinfo.size(); index++)
			{
				RobotLogin(index);
			}
			SetTimer(TIME_USER_LOGIN_DESK_INFO*MFC_TIME_START_ID, 1000, NULL);
			break;
		}
		case TIME_USER_LOGIN_DESK_INFO:
		{
			KillTimer(TIME_USER_LOGIN_DESK_INFO*MFC_TIME_START_ID);
			for (int index = 0; index < m_robot_login_count; index++)
			{
				RobotLoginDesk(index);
			}
			refresh_mfc_show(USER_STATE_TO_MFC_LOGIN);
			break;
		}
		case TIME_USER_LOGIN_TIMESTMP:
			break;
		case TIME_USER_RE_LOGIN_PHP:
		{
			if (m_user_lock_count > 0)
			{
				m_user_lock_count--;
				RobotReLogin(m_user_lock_gold[m_user_lock_count]);
			}
			else
			{
				GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
				KillTimer(TIME_USER_RE_LOGIN_PHP*MFC_TIME_START_ID);
			}
			break;
		}
		default:
			break;
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CRobotDlg::init_robot_cnt_param(int index)
{
	srand(time(0) + m_user_sit_info[index].p_user_id);
	int range = m_play_game_count_max - m_play_game_count_min + 1;
	m_play_game_count = rand() % range + m_play_game_count_min;
	m_user_sit_info[index].play_game_count = m_play_game_count;
	m_user_sit_info[index].play_game_count_min = m_play_game_count_min;
	m_user_sit_info[index].play_game_count_max = m_play_game_count_max;
	m_user_sit_info[index].rule_id = m_rule_id;
	m_user_sit_info[index].room_srv_port = m_real_port;
	m_user_sit_info[index].club_desk_room_id = m_real_room_id;
	memcpy(m_user_sit_info[index].room_srv_host, m_room_real_host, sizeof(m_user_sit_info[index].room_srv_host));
}

void CRobotDlg::push_robot_info_to_mfc(int index)
{
	mfc_show_user_info msui;
	msui.user_id = m_user_sit_info[index].p_user_id;
	msui.game_id = m_user_sit_info[index].club_desk_game_id;
	bool exit = false;
	for (int i = 0; i < mfc_user_info.size(); i++)
	{
		if (msui.user_id == mfc_user_info.at(i).user_id)
		{
			exit = true;
			break;
		}
	}
	if (!exit)
	{
		mfc_user_info.push_back(msui);
	}
}

void CRobotDlg::add_robot_login_task(int index)
{
	usertask*							pTask = new usertask();
	UserInitData						m_userinitdata;
	m_userinitdata.room_id				= m_club_room_id;
	m_userinitdata.user_id				= m_user_sit_info[index].p_user_id;
	m_userinitdata.login_port			= m_user_sit_info[index].room_srv_port;
	m_userinitdata.private_id			= 0;
	m_userinitdata.real_room_id			= m_user_sit_info[index].club_desk_room_id;
	m_userinitdata.play_game_count		= m_user_sit_info[index].play_game_count;
	m_userinitdata.play_game_count_min	= m_user_sit_info[index].play_game_count_min;
	m_userinitdata.play_game_count_max	= m_user_sit_info[index].play_game_count_max;
	m_userinitdata.rule_id				= m_user_sit_info[index].rule_id;
	m_userinitdata.handle				= m_hWnd;
	m_userinitdata.usermgr_index		= index;
	m_userinitdata.recharge_limit		= m_recharge_limit;
	m_userinitdata.recharge_money		= m_recharge_money;
	m_userinitdata.login_time			= m_first_time;
	m_userinitdata.play_time			= play_time;
	m_userinitdata.player_game_id		= m_user_sit_info[index].club_desk_game_id;
	m_userinitdata.robot_off_player_count = m_rabot_off_player_count;
	memcpy(&m_userinitdata.user_password, m_user_sit_info[index].p_player_password, sizeof(m_userinitdata.user_password));
	memcpy(&m_userinitdata.player_token, m_user_sit_info[index].p_player_token, sizeof(m_userinitdata.player_token));
	memcpy(&m_userinitdata.php_address, m_user_sit_info[index].php_address, sizeof(m_userinitdata.php_address));
	memcpy(&m_userinitdata.game_name, m_user_sit_info[index].game_name, sizeof(m_userinitdata.game_name));
	memcpy(&m_userinitdata.login_ip, m_user_sit_info[index].room_srv_host, sizeof(m_userinitdata.login_ip));

	//任务类型0是登录房间
	pTask->add_type(0);
	pTask->add_user_initdata((char*)&m_userinitdata);
	pTask->add_user_mgr(m_pUserMgr[index]);
	m_thread_pool.AddThreadJobToThreadPool(pTask);
}

void CRobotDlg::refresh_mfc_show(int index)
{
	HWND hWnd = ::FindWindow(NULL, m_game_name);
	if (hWnd != NULL)
	{
		::SendMessage(hWnd, WM_USER + 100, index, NULL);
	}
	else
	{
		switch (index)
		{
		case USER_STATE_TO_MFC_LOGIN:
		{
			BASIC_LOG_INFO("机器人请求桌子信息 + 坐桌 找不到 Window ");
		}
		break;
		default:
			break;
		}
	}
}

//机器人充值金币
void CRobotDlg::checkAddMoney(unsigned int userId, int current_money)
{
	BASIC_LOG_INFO("CRobotDlg::checkAddMoney 机器人检测充值 userId = %d, 当前金币 = %d, 限制金币 = %d, 充值金币 =%d ", userId, current_money, m_recharge_limit, m_recharge_money);
	if (current_money <= m_recharge_limit &&  m_recharge_money > 0)
	{
		BASIC_LOG_INFO("CRobotDlg::checkAddMoney 机器人需要充值金币 userId = %d, 当前金币 = %d, 充值金币 =%d ", userId, current_money, m_recharge_money);

		Json::Value param;
		Json::Value data;
		param["action"] = "robortaddmoney";
		param["version"] = "v10001";
		param["key_value"] = 1;
		param["flag_value"] = 1;
		istringstream iss(std::to_string(time(0)));
		int num;
		iss >> num;
		param["sign_value"] = num;
		data["player_id"] = userId;
		for (int i = 0; i < MAX_USER_PLAY; i++)
		{
			if (userId == m_user_sit_info[i].p_user_id)
			{
				string token = m_user_sit_info[i].p_player_token;
				data["player_token"] = token.c_str();
			}
		}

		data["add_value"] = m_recharge_money;
		param["data_value"] = data;

		Json::FastWriter writer;
		std::string  keyvalue = "param=" + writer.write(param);

		utility::BasicBuffer szBuffer(keyvalue.c_str(), keyvalue.length());
		utility::BasicBuffer szGetBuffer(BUFFER_SIZE, BUFFER_GROW);
		utility::BasicHttpContext http;
		BASIC_LOG_INFO("checkAddMoney 充值请求路径 = (%s)", (char*)szBuffer.buffer());
		http.HttpSendConnectEx(m_PHP_Adress, BASIC_HTTP_VERB_POST, &szBuffer, szGetBuffer, 5, 1);
		std::string  result((char*)szGetBuffer.buffer());
		BASIC_LOG_INFO("checkAddMoney 充值请求结果 result = (%s)", result.c_str());
		SHttpRobortAddMoney robortAddMoney;
		robortAddMoney.clear();
		if (0 == gameservice::basichttphelper::parse_add_money((char*)szGetBuffer.buffer(), 0, robortAddMoney))
		{
			BASIC_LOG_INFO("checkAddMoney 充值请求成功 更新金币");

			for (int i = 0; i < mfc_user_info.size(); i++)
			{
				if (userId == mfc_user_info.at(i).user_id)
				{
					mfc_user_info.at(i).iCoins += m_recharge_money;
					//设置为充值成功
					mfc_user_info[i].user_state = 7;
					updateMfcUserInfo();
					notify_update_user_info(userId);
					break;
				}
			}
		}
		else
		{
			BASIC_LOG_INFO("checkAddMoney 充值请求失败");
		}
	}
}

//机器自动更换logo
bool CRobotDlg::robotchangeInfo(unsigned int userId, std::string& strnickname, std::string& strlogoinfo)
{
	Json::Value param;
	Json::Value data;
	param["action"] = "robotautochangeinfo";
	param["version"] = "v10001";
	param["key_value"] = 1;
	param["flag_value"] = 1;

	for (int i = 0; i < MAX_USER_PLAY; i++)
	{
		if (userId == m_user_sit_info[i].p_user_id)
		{
			string token = m_user_sit_info[i].p_player_token;
			data["player_token"] = token.c_str();
			break;
		}
	}
	istringstream iss(std::to_string(time(0)));
	int num;
	iss >> num;
	param["sign_value"] = num;
	data["player_id"] = userId;
	param["data_value"] = data;

	Json::FastWriter writer;
	std::string  keyvalue = "param=" + writer.write(param);

	utility::BasicBuffer szBuffer(keyvalue.c_str(), keyvalue.length());
	utility::BasicBuffer szGetBuffer(BUFFER_SIZE, BUFFER_GROW);
	utility::BasicHttpContext http;
	http.HttpSendConnectEx(m_PHP_Adress, BASIC_HTTP_VERB_POST, &szBuffer, szGetBuffer, 5, 1);
	std::string  result((char*)szGetBuffer.buffer());

	BASIC_LOG_INFO("robotchangeInfo robot auto changeinfo id(%d)", userId);

	if (0 == gameservice::basichttphelper::parse_change_info((char*)szGetBuffer.buffer(), szGetBuffer.length(), strnickname, strlogoinfo))
	{
		return true;
	}
	return false;
}

void CRobotDlg::notify_update_user_info(unsigned int userId)
{
	for (int i = 0; i < MAX_USER_PLAY; i++)
	{
		if (m_pUserMgr[i]->m_my_userid == userId)
		{
			m_pUserMgr[i]->updateUserInfo(userId);
			BASIC_LOG_INFO("CRobotDlg::notify_update_user_info 通知更新玩家信息 userId =  %d", userId);
			return;
		}
	}
}

//重新获取PHP信息
void CRobotDlg::RobotReLogin(unsigned int userId)
{
	for (int i = 0; i < m_ubinfo.size(); i++)
	{
		if (m_ubinfo[i].user_id == userId)
		{
			istringstream iss(std::to_string(time(0)));
			userBaseInfo singleDate = m_ubinfo[i];
			
			Json::Value param;
			Json::Value data;
			param["action"] = "robotlogin";
			param["version"] = "v10001";
			param["key_value"] = 1;
			param["flag_value"] = 1;
			int num;
			iss >> num;
			param["sign_value"] = num;
			
			data["userid"] = singleDate.user_id;
			data["password"] = singleDate.password;
			data["platform"] = 1;
			param["data_value"] = data;

			Json::FastWriter writer;
			std::string  keyvalue = "param=" + writer.write(param);
			utility::BasicBuffer szBuffer(keyvalue.c_str(), keyvalue.length());
			utility::BasicBuffer szGetBuffer(BUFFER_SIZE, BUFFER_GROW);
			utility::BasicHttpContext http;;
			http.HttpSendConnectEx(m_PHP_Adress, BASIC_HTTP_VERB_POST, &szBuffer, szGetBuffer, 5, 1);
			
			std::string  result((char*)szGetBuffer.buffer());
			Json::Value root;
			Json::Reader reader;
			if (!reader.parse(result, root))
			{
				return;
			}
			if (root["data_value"].isNull())
			{
				return;
			}
			Json::Value test;
			test = root["data_value"];
			std::string  data_value = "";
			data_value = writer.write(test);
			Json::Value root_value;
			if (!reader.parse(data_value, root_value))
			{
				return;
			}
			int code_value = root_value["code_value"].asInt();
			if (code_value != 0)
			{
				return;
			}
			Json::Value private_info;
			private_info = root_value["data"];
			data_value = "";
			data_value = writer.write(private_info);
			Json::Value root_desk;
			if (!reader.parse(data_value, root_desk))
			{
				return;
			}
			Httplogin login_info;
			login_info.clear();

			if (gameservice::basichttphelper::parse_login_info((char*)szGetBuffer.buffer(), 0, login_info) == 0)
			{
				//请求成功
				for (int j = 0; j < mfc_user_info.size(); j++)
				{
					if (userId == mfc_user_info.at(j).user_id)
					{
						mfc_user_info.at(i).user_state = 1;
						mfc_user_info.at(i).userNote = -1;
						mfc_user_info.at(i).iChairID = -1;
						mfc_user_info.at(i).desk_no = -1;
						mfc_user_info.at(i).iCoins = login_info.player_info.player_coins;
						//更新mfc
						updateMfcUserInfo();
						for (int k = 0; k < MAX_USER_PLAY; k++)
						{
							if (m_real_check.m_pUserMgr[k]->m_my_userid == userId)
							{
								m_real_check.m_pUserMgr[k]->userReConnet();
								break;
							}
						}
						break;
					}
				}
			}
			break;
		}
	}
	return;
}