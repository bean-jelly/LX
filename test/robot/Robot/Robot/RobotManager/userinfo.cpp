#include "userinfo.h"



namespace robotserver{
	userinfo::userinfo()
	{
		m_userinfo.clear();
	}
	userinfo::~userinfo()
	{

	}

	void userinfo::add_usermanager(usermanager* mgr)
	{
		m_usermanager = mgr;
	}


	int userinfo::add_userinfor(unsigned int userid, ROBOT::UserInfo&  userinfo)
	{
		utility::BasicCriticalSection lock(m_mutex);
	//	printf_s("add user   count=%d  \n", m_userinfo.size());
		std::map<unsigned int, ROBOT::UserInfo*>::iterator it = m_userinfo.find(userid);
		if (it == m_userinfo.end())
		{
			ROBOT::UserInfo* pInfo = new ROBOT::UserInfo();
			*pInfo = userinfo;
			m_userinfo.insert(std::map<unsigned int, ROBOT::UserInfo*>::value_type(userid, pInfo));
		}
		else
		{
			*(it->second) = userinfo; 

		}
		return 0;
	}

	int  userinfo::del_userinfor(unsigned int userid)
	{
		utility::BasicCriticalSection lock(m_mutex);
		std::map<unsigned int, ROBOT::UserInfo*>::iterator it = m_userinfo.find(userid);
		if (it != m_userinfo.end())
		{
			delete it->second;
			m_userinfo.erase(it);
			return 0;
		}
		return -1;
	}

	int  userinfo::update_userinfor(unsigned int userid, ROBOT::UserInfo&  userinfo)
	{
		utility::BasicCriticalSection lock(m_mutex);
		std::map<unsigned int, ROBOT::UserInfo*>::iterator it = m_userinfo.find(userid);
		if (it != m_userinfo.end())
		{
			*(it->second) = userinfo;
			return 0;
		}
		return -1;
	}


	int  userinfo::update_user_station(unsigned int userid, unsigned int deskindex, unsigned int station)
	{
		utility::BasicCriticalSection lock(m_mutex);
		std::map<unsigned int, ROBOT::UserInfo*>::iterator it = m_userinfo.find(userid);
		if (it != m_userinfo.end())
		{
			it->second->bDeskNO = deskindex;
			it->second->bDeskStation = station;
			if (deskindex != 255)
			{
				it->second->bSendSit = true;
			}
			else
			{
				it->second->bSendSit = false;
			}
			return 0;
		}

		return -1;
	}

	int  userinfo::update_user_status(unsigned int userid, unsigned int status)
	{
		utility::BasicCriticalSection lock(m_mutex);
		std::map<unsigned int, ROBOT::UserInfo*>::iterator it = m_userinfo.find(userid);
		if (it != m_userinfo.end())
		{
			it->second->bUserState = status;
			return 0;
		}
		return -1;
	}

	bool  userinfo::find_userinfor(unsigned int userid, ROBOT::UserInfo& userinfo)
	{
		utility::BasicCriticalSection lock(m_mutex);
		std::map<unsigned int, ROBOT::UserInfo*>::iterator it = m_userinfo.find(userid);
		if (it != m_userinfo.end())
		{
			userinfo = *(it->second);
			return true;
		} 
		return false;
	}

	bool userinfo::find_user_by_desk(unsigned int deskindex, unsigned int deskstation)
	{
		utility::BasicCriticalSection lock(m_mutex);
		std::map<unsigned int, ROBOT::UserInfo*>::iterator it = m_userinfo.begin();
		for (; it != m_userinfo.end();it++ )
		{
			ROBOT::UserInfo*  user = it->second;

			if (NULL != user && user->bDeskNO == deskindex && user->bDeskStation == deskstation )
			{  
				return true;
			}
		}
		return false;
	}
}