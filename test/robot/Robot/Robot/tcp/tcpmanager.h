#pragma  once 
#ifndef __TCPMANAGER_H_
#define __TCPMANAGER_H_  
//#include "basictcp\basicnetdef.h"
#include "basictcp\basictcpinterface.h" 
#include "basictable\GameRoomMsg.h"

/*
*tcp manger  class 
*/
namespace  userserver
{
	class tcpmanager
	{

	public:
		tcpmanager();
		virtual ~tcpmanager();
	public:
		//init tcp infor
		int init_tcp(char* ip, int port);

		//uninit tcp infor
		int uninit_tcp();

		//start server 
		int start_tcp();

		//stop server
		int stop_tcp();

		//close  client 
		int close_client(unsigned int socket);

	public:
		/*
		*  @desc:对外接口
		*/ 
		int on_send_event(unsigned int socket, unsigned int main_id, unsigned int sub_id, void* buf, int len, unsigned int hanlde_code);
		/*
		*  @desc:子类实现
		*/
		virtual void on_recv_event(unsigned int socket, char* buf, int len) = NULL;
		/*
		*  @desc:子类实现
		*/
		virtual void on_close_event(unsigned int socket, char* buf, int len) = NULL;

		virtual void on_connect_event(unsigned int socket) = NULL;

	protected:
		//tcp event call back
		static void WINAPI on_tcp_event_callback(int action, void* object, char* in_buf, int in_len, char* out_buf, int out_len);
	protected:
		unsigned long get_tick_count();
		time_t make_time();

	public:
		/*
		*  @desc:内部管理层使用
		*/
		void on_tcp_revc_event(unsigned int socket, char* buf, int len);
		void on_tcp_close_event(unsigned int socket, char* buf, int len);
		void on_tcp_connect_event(unsigned int socket);

		HANDLE											m_tcp_handle;
	protected:
		int												m_socket;

		basictcp::basictcpinterface*					m_tcp_interface;
		time_t											m_time;
		unsigned long									m_begin_tick;
	

	};
}



#endif///__TCPMANAGER_H_
