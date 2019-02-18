#pragma once
#include "basiccode\utility\BasicThreadPool.h"   
#include "./RobotManager/usermanager.h"
#include "datastruct.h"

class usertask : public utility::BasicThreadJob
{
public:
	usertask();
	virtual ~usertask();

	void add_user_mgr(userserver::usermanager* mgr);
	void add_user_initdata(char* param);
public:  
	void add_type(unsigned int tasktype);
	void add_free(bool bfree);
	void add_agree(bool bagree);
	void set_user_mgr_index(unsigned int index);
public:
	 
	virtual void DoThreadJob();

	virtual void OnFinal();

	userserver::usermanager*	m_user_mgr; 
	unsigned int				m_taks_type;
	bool						m_free;
	bool						m_agree;
	UserInitData				m_UserInitData;
	 
	unsigned int				m_user_mgr_index;
};

