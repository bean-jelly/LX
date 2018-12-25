#ifndef __TCPCONMANAGER_H_
#define __TCPCONMANAGER_H_  

#include "basictcp\basictcpinterface.h" 
#include "basictable\GameRoomMsg.h"

namespace RobotManager
{
	class TcpConManager
	{
	public:
		TcpConManager();
		virtual ~TcpConManager();
	public:
		int					on_send_event(unsigned int main_id, unsigned int sub_id, void* buf, int len, unsigned int hanlde_code);
	public:
		virtual void		on_recv_event(unsigned int socket, char* buf, int len) = NULL;
		virtual void		on_close_event(unsigned int socket, char* buf, int len);
		virtual void		on_connect_event(unsigned int socket);
	public:
		int					init_tcp(char* ip, int port);
		int					uninit_tcp();
		int					start_tcp();
		int					stop_tcp();
		int					close_client(unsigned int socket);
	public:
		void				set_handle(int handle);
		int					get_handle();
	protected:
		//tcp event call back
		static void WINAPI	on_tcp_event_callback(int action, void* object, char* in_buf, int in_len, char* out_buf, int out_len);
	protected:
		void				on_tcp_revc_event(unsigned int socket, char* buf, int len);
		void				on_tcp_close_event(unsigned int socket, char* buf, int len);
		void				on_tcp_connect_event(unsigned int socket);
	private:
		int												m_socket_handle;
		basictcp::basictcpinterface*					m_tcp_interface;
		HANDLE											m_tcp_handle;
		time_t											m_time;
	};
}

#endif//__TCPCONMANAGER_H_