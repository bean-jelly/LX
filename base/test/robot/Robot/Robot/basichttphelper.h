#pragma once
#ifndef __BASICHTTPHELPER_H_
#define __BASICHTTPHELPER_H_
#include <map>
#include <vector>
#include <string>
#include "datastruct.h"
#include "../../../comlib/basicsdk/basictable/GameRoomMsg.h"
#include "../../../comlib/basicsdk/gameservice/gameservicedef.h"
namespace gameservice
{
	class basichttphelper
	{
	public:

	public:
		static int parse_matching_info(char* buf, int len, SHttpClubDeskInfo& temp);
		static int parse_login_info(char* buf, int len, Httplogin& temp);
		static int parse_desks_info(char* buf, int len, DCGR::stuAllDeskInfo& temp);
		static int parse_add_money(char* buf, int len, SHttpRobortAddMoney& temp);
		//解析机器人自动更换名称信息
		static int parse_change_info(char* buf, int ilen, std::string strplayernickname, std::string strplayerlogo);
	private:
		basichttphelper();
		virtual ~basichttphelper();
	};
}
#endif