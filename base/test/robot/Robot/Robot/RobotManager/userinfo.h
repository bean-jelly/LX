#pragma once 
#ifndef __USERINFO_H_
#define __USERINFO_H_
#include "../robotmessage.h"
#include <map>
#include "basiccode\utility\BasicCriticalSection.h"

namespace robotserver{
	class usermanager;
	class userinfo
	{
	public:
		userinfo();
		virtual ~userinfo();

	public:
		void add_usermanager(usermanager* mgr);

	public:
		
		int  add_userinfor(unsigned int userid, ROBOT::UserInfo&  userinfo);
		int  del_userinfor(unsigned int userid);
		int  update_userinfor(unsigned int userid, ROBOT::UserInfo&  userinfo);
		int  update_user_station(unsigned int userid, unsigned int deskindex,unsigned int station);
		int  update_user_status(unsigned int userid,unsigned int status);



		bool find_userinfor(unsigned int userid, ROBOT::UserInfo& userinfo);
		bool find_user_by_desk(unsigned int deskindex, unsigned int deskstation);
	protected: 
		std::map<unsigned int, ROBOT::UserInfo*>				m_userinfo; 
		usermanager*											m_usermanager;
		std::recursive_mutex									m_mutex;
	};
}

#endif//__USERINFO_H_