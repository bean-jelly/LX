#include "basichttphelper.h"
#include "basicredis/basicredisdef.h" 
#include "basiccode/json/json.h"
#include "basiclog/basicloginterface.h"
#include "datastruct.h"
#include<algorithm>
namespace gameservice
{
	basichttphelper::basichttphelper()
	{
	}

	basichttphelper::~basichttphelper()
	{
	}

	int basichttphelper::parse_login_info(char* buf, int len, Httplogin& temp)
	{
		int rt_code = -1;
		std::string  strResult((char*)buf);
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(strResult, root))
		{
			BASIC_LOG_ERROR("parse_login_info json parse fail(%s)", strResult.c_str());
			return rt_code;
		}
		if (root["data_value"].isNull())
		{
			BASIC_LOG_ERROR("parse_login_info data_value is null (%s)", strResult.c_str());
			return rt_code;
		}

		Json::FastWriter writer;
		std::string  data_value = "";
		data_value = writer.write(root["data_value"]);
		Json::Value root_data_value;
		if (!reader.parse(data_value, root_data_value))
		{
			BASIC_LOG_ERROR("parse_login_info data_value parse fail(%s)", strResult.c_str());
			return rt_code;
		}

		if (root_data_value["playerinfo"].isNull())
		{
			BASIC_LOG_ERROR("parse_login_info playerinfo is null (%s)", strResult.c_str());
			return rt_code;
		}

		if (!root_data_value["code_value"].isNull())
		{
			rt_code = root_data_value["code_value"].asInt();
		}

		//player_info
		std::string  player_info = "";
		Json::Value root_player_info;
		player_info = writer.write(root_data_value["playerinfo"]);
		if (!reader.parse(player_info, root_player_info))
		{
			BASIC_LOG_ERROR("parse_desk_info root_desk_info parse fail(%s)", strResult.c_str());
			return rt_code;
		}
		if (!root_player_info["player_id"].isNull())
		{
			temp.player_info.player_id = root_player_info["player_id"].asInt();
		}
		if (!root_player_info["player_token"].isNull())
		{
			std::string temp_temp = "";
			temp_temp = root_player_info["player_token"].asString();
			if (sizeof(temp.player_info.player_token) > (temp_temp.length()+1))
			{
				memcpy(temp.player_info.player_token, temp_temp.c_str(), temp_temp.length());
			}
			temp.player_info.player_token[temp_temp.length()] = 0x00;
		}
		if (!root_player_info["player_password"].isNull())
		{
			std::string temp_temp = "";
			temp_temp = root_player_info["player_password"].asString();
			if (sizeof(temp.player_info.player_password) > (temp_temp.length() + 1))
			{
				memcpy(temp.player_info.player_password, temp_temp.c_str(), temp_temp.length());
			}
			temp.player_info.player_password[temp_temp.length()] = 0x00;
		}

		if (!root_player_info["player_coins"].isNull())
		{
			temp.player_info.player_coins = root_player_info["player_coins"].asInt();
		}
		return rt_code;
	}
	int basichttphelper::parse_add_money(char* buf, int len, SHttpRobortAddMoney& temp)
	{
		int rt_code = -1;
		std::string  strResult((char*)buf);
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(strResult, root))
		{
			BASIC_LOG_ERROR("parse_add_money json parse fail(%s)", strResult.c_str());
			return rt_code;
		}
		if (root["data_value"].isNull())
		{
			BASIC_LOG_ERROR("parse_add_money data_value is null (%s)", strResult.c_str());
			return rt_code;
		}

		Json::FastWriter writer;
		std::string  data_value = "";
		data_value = writer.write(root["data_value"]);
		Json::Value root_data_value;
		if (!reader.parse(data_value, root_data_value))
		{
			BASIC_LOG_ERROR("parse_add_money data_value parse fail(%s)", strResult.c_str());
			return rt_code;
		}

		if (!root_data_value["code_value"].isNull())
		{
			rt_code = root_data_value["code_value"].asInt();
		}
		if (!root_data_value["desc_value"].isNull())
		{
			std::string temp_temp = "";
			temp_temp = root_data_value["desc_value"].asString();
		}
		return rt_code;
	}
	//解析机器人自动更换名称信息
	int basichttphelper::parse_change_info(char* buf, int ilen,std::string strplayernickname,std::string strplayerlogo)
	{
		int rt_code = -1;
		std::string  strResult((char*)buf);
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(strResult, root))
		{
			BASIC_LOG_ERROR("parse_add_money json parse fail(%s)", strResult.c_str());
			return rt_code;
		}
		if (root["data_value"].isNull())
		{
			BASIC_LOG_ERROR("parse_add_money data_value is null (%s)", strResult.c_str());
			return rt_code;
		}
		Json::FastWriter writer;
		std::string  data_value = "";
		data_value = writer.write(root["data_value"]);
		Json::Value root_data_value;
		if (!reader.parse(data_value, root_data_value))
		{
			BASIC_LOG_ERROR("parse_add_money data_value parse fail(%s)", strResult.c_str());
			return rt_code;
		}
		//解析是否有机器人信息
		if(root_data_value["robot_change_info"].isNull())
		{
			return rt_code;
		}
		if (!root_data_value["player_nickname"].isNull())
		{
			strplayernickname = root_data_value["player_nickname"].asString().c_str();
		}
		if (!root_data_value["player_header_image"].isNull())
		{
			strplayerlogo = root_data_value["player_header_image"].asString().c_str();
		}

		return 0;
	}


	int basichttphelper::parse_matching_info(char* buf, int len, SHttpClubDeskInfo&  temp)
	{ 
		int rt_code = -1;
		std::string  strResult((char*)buf);
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(strResult, root))
		{
			BASIC_LOG_ERROR("parse_matching_info json parse fail(%s) rt_code  = %d", strResult.c_str(), rt_code);
			return rt_code;
		}
		if (root["data_value"].isNull())
		{
			BASIC_LOG_ERROR("parse_desk_info data_value is null (%s)", strResult.c_str());
			return rt_code;
		}

		Json::FastWriter writer;
		std::string  data_value = "";
		data_value = writer.write(root["data_value"]);
		Json::Value root_data_value;
		if (!reader.parse(data_value, root_data_value))
		{
			BASIC_LOG_ERROR("parse_desk_info data_value parse fail(%s)", strResult.c_str());
			return rt_code;
		}

		if (!root_data_value["desc_value"].isNull())
		{
			std::string temp_temp = "";
			temp_temp = root_data_value["desc_value"].asString();
			if (sizeof(temp.desc_value) > (temp_temp.length() + 1))
			{
				memcpy(temp.desc_value, temp_temp.c_str(), temp_temp.length());
				temp.desc_value[temp_temp.length()] = 0x00;
			}
		}

		if(root_data_value["desk_info"].isNull() || root_data_value["room_info"].isNull() || root_data_value["club_room"].isNull())
		{
			BASIC_LOG_ERROR("parse_desk_info desk_info||room_info||club_room is null (%s)", strResult.c_str());
			return rt_code;
		}

		if (!root_data_value["code_value"].isNull())
		{
			rt_code = root_data_value["code_value"].asInt();
		}

		//desk_info
		std::string  desk_info = "";
		Json::Value root_desk_info;
		desk_info = writer.write(root_data_value["desk_info"]);
		if (!reader.parse(desk_info, root_desk_info))
		{
			BASIC_LOG_ERROR("parse_desk_info root_desk_info parse fail(%s)", strResult.c_str());
			return rt_code;
		}
		if (!root_desk_info["club_desk_id"].isNull())
		{
			temp.desk_info.club_desk_id = root_desk_info["club_desk_id"].asInt();
		}
		if (!root_desk_info["club_desk_club_room_id"].isNull())
		{
			temp.desk_info.club_desk_club_room_id = root_desk_info["club_desk_club_room_id"].asInt();
		}
		if (!root_desk_info["club_desk_club_id"].isNull())
		{
			temp.desk_info.club_desk_club_id = root_desk_info["club_desk_club_id"].asInt();
		}
		if (!root_desk_info["club_desk_club_room_desk_no"].isNull())
		{
			temp.desk_info.club_desk_club_room_desk_no = root_desk_info["club_desk_club_room_desk_no"].asInt();
		}
		if (!root_desk_info["club_desk_room_id"].isNull())
		{
			temp.desk_info.club_desk_room_id = root_desk_info["club_desk_room_id"].asInt();
		}
		if (!root_desk_info["club_desk_desk_no"].isNull())
		{
			temp.desk_info.club_desk_desk_no = root_desk_info["club_desk_desk_no"].asInt();
		}
		if (!root_desk_info["club_desk_player_id"].isNull())
		{
			temp.desk_info.club_desk_player_id = root_desk_info["club_desk_player_id"].asInt();
		}
		if (!root_desk_info["club_desk_is_work"].isNull())
		{
			temp.desk_info.club_desk_is_work = root_desk_info["club_desk_is_work"].asInt();
		}
		if (!root_desk_info["club_desk_members_count"].isNull())
		{
			temp.desk_info.club_desk_members_count = root_desk_info["club_desk_members_count"].asInt();
		}
		if (!root_desk_info["club_desk_members_cur"].isNull())
		{
			temp.desk_info.club_desk_members_cur = root_desk_info["club_desk_members_cur"].asInt();
		}

		if (!root_desk_info["club_desk_game_id"].isNull())
		{
			temp.desk_info.club_desk_game_id = root_desk_info["club_desk_game_id"].asInt();
		}


		//room_info
		std::string  room_info = "";
		Json::Value root_room_info;
		room_info = writer.write(root_data_value["room_info"]);
		if (!reader.parse(room_info, root_room_info))
		{
			BASIC_LOG_ERROR("parse_desk_info root_room_info parse fail(%s)", strResult.c_str());
			return rt_code;
		}
		if (!root_room_info["room_id"].isNull())
		{
			temp.room_inifo.room_id = root_room_info["room_id"].asInt();
		}
		if (!root_room_info["room_srv_port"].isNull())
		{
			temp.room_inifo.room_srv_port = root_room_info["room_srv_port"].asInt();
		}
		if (!root_room_info["room_srv_host"].isNull())
		{
			std::string temp_temp = "";
			temp_temp = root_room_info["room_srv_host"].asString();
			if (sizeof(temp.room_inifo.room_srv_host) > (temp_temp.length() + 1))
			{
				memcpy(temp.room_inifo.room_srv_host, temp_temp.c_str(), temp_temp.length());
				temp.room_inifo.room_srv_host[temp_temp.length()] = 0x00;
			}
		}
		if (!root_room_info["room_password"].isNull())
		{
			std::string temp_temp = "";
			temp_temp = root_room_info["room_password"].asString();
			if (sizeof(temp.room_inifo.room_password) > (temp_temp.length() + 1))
			{
				memcpy(temp.room_inifo.room_password, temp_temp.c_str(), temp_temp.length());
				temp.room_inifo.room_password[temp_temp.length()] = 0x00;
			}
		}
		
		//club_room
		std::string  club_room = "";
		Json::Value root_club_room;
		club_room = writer.write(root_data_value["club_room"]);
		if (!reader.parse(club_room, root_club_room))
		{
			BASIC_LOG_ERROR("parse_desk_info root_club_room parse fail(%s)", strResult.c_str());
			return rt_code;
		}
		if (!root_club_room["club_room_id"].isNull())
		{
			temp.club_room.club_room_id =root_club_room["club_room_id"].asInt();
		}
		if (!root_club_room["club_room_basic_points"].isNull())
		{
			temp.club_room.club_room_basic_points = root_club_room["club_room_basic_points"].asInt();
		}
		
		if (!root_club_room["club_room_desk_param"].isNull())
		{
			std::string temp_temp = "";
			temp_temp = root_room_info["club_room_desk_param"].asString();
			if (sizeof(temp.club_room.club_room_desk_param) > (temp_temp.length() + 1))
			{
				memcpy(temp.club_room.club_room_desk_param, temp_temp.c_str(), temp_temp.length());
				temp.club_room.club_room_desk_param[temp_temp.length()] = 0x00;
			}
		}

		return rt_code;
	} 

	int basichttphelper::parse_desks_info(char* buf, int len, DCGR::stuAllDeskInfo& temp)
	{
		int rt_code = -1;
		Json::FastWriter writer;
		std::string  strResult((char*)buf);
		Json::Value root;
		Json::Reader reader;

		if (!reader.parse(strResult, root))
		{
			BASIC_LOG_ERROR("parse_desks_info json parse fail(%s) rt_code  = %d", strResult.c_str(), rt_code);
			return rt_code;
		}
		if (root["deskinfo"].isNull())
		{
			BASIC_LOG_ERROR("parse_desks_info deskinfo is null (%s)", strResult.c_str());
			return rt_code;
		}

		if (!root["realPlyerCount"].isNull())
		{
			temp.realPlyerCount = root["realPlyerCount"].asInt();
		}

		int desk_count = 0;
		if (!root["deskcount"].isNull())
		{
			desk_count = root["deskcount"].asInt();
		}

		for (Json::Value::iterator iter = root["deskinfo"].begin(); iter != root["deskinfo"].end(); ++iter)
		{
			Json::Value desk_info1;
			std::string  desk_info = "";

			desk_info1 = *(iter);

			int deskid = 99;
			if (!desk_info1["iDeskID"].isNull())
			{
				deskid = desk_info1["iDeskID"].asInt();
				temp.desk_info[deskid].iDeskID = deskid;
			}

			if (!desk_info1["desk_count"].isNull())
			{
				int desk_count = desk_info1["desk_count"].asInt();
				if (desk_count >= 0 && desk_count < 100)
				{
					temp.desk_info[deskid].desk_count = desk_count;
				}
			}

			if (!desk_info1["chairid"].isNull())
			{
				int desk_count = temp.desk_info[deskid].desk_count;
				if (desk_count < 0 || desk_count > 99)
				{
					desk_count = 0;
				}
				for (int i = 0; i < desk_count; i++)
				{
					temp.desk_info[deskid].chairid[i] = desk_info1["chairid"][i].asInt();
				}
			}
			if (!desk_info1["cur_desk_member"].isNull())
			{
				temp.desk_info[deskid].cur_desk_member = desk_info1["cur_desk_member"].asInt();
			}

			if (!desk_info1["cur_robot_member"].isNull())
			{
				temp.desk_info[deskid].cur_robot_member = desk_info1["cur_robot_member"].asInt();
			}

			if (!desk_info1["iClubDeskID"].isNull())
			{
				temp.desk_info[deskid].iClubDeskID = desk_info1["iClubDeskID"].asInt();
			}

			if (!desk_info1["iClubRoomID"].isNull())
			{
				temp.desk_info[deskid].iClubRoomID = desk_info1["iClubRoomID"].asInt();
			}
		}
		return 0;
	}
}


