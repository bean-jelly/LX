#include <time.h>
#include "TcpConManager.h"
#include "basictcp/basicnetdef.h"

namespace RobotManager
{
	typedef void (WINAPI* pfn_tcp_event_callback)(int action, void* object, char* in_buf, int in_len, char* out_buf, int out_len);

	TcpConManager::TcpConManager()
	{
		m_tcp_interface = new basictcp::basictcpinterface();
		m_tcp_handle = m_tcp_interface->create_net();
		m_socket_handle = 0;
		m_time = 0;
	}

	TcpConManager::~TcpConManager()
	{
		if (NULL != m_tcp_interface)
		{
			m_tcp_interface->release_net(m_tcp_handle);
		}
	}

	int TcpConManager::uninit_tcp()
	{
		return 0;
	}

	int TcpConManager::init_tcp(char* ip, int port)
	{
		if (NULL == m_tcp_interface)
		{
			return -1;
		}
		if (m_tcp_handle == NULL)
		{
			m_tcp_handle = m_tcp_interface->create_net();
		}

		basicnet::s_net_init_param param;
		param.m_port = port;
		param.m_server = false;
		param.m_max_conet = 0;
		param.m_on_event = (pfn_tcp_event_callback)TcpConManager::on_tcp_event_callback;
		param.m_object = this;
		memcpy(param.m_ip, ip, sizeof(param.m_ip));
		return m_tcp_interface->init_net(m_tcp_handle, &param);
	}

	int TcpConManager::start_tcp()
	{
		//从数据库获取相关信息
		m_time = time(NULL);
		if (m_tcp_interface != NULL)
		{
			return m_tcp_interface->start_net(m_tcp_handle);
		}
		return -1;
	}

	//stop server
	int TcpConManager::stop_tcp()
	{
		if (m_tcp_interface != NULL)
		{
			m_tcp_interface->stop_net(m_tcp_handle);
		}
		return 0;
	}

	int TcpConManager::close_client(unsigned int socket)
	{
		if (m_tcp_interface != NULL)
		{
			m_tcp_interface->close_socket(m_tcp_handle, socket);
		}
		set_handle(0);
		return 0;
	}

	int TcpConManager::get_handle()
	{
		return m_socket_handle;
	}

	void TcpConManager::set_handle(int handle)
	{
		m_socket_handle = handle;
		return;
	}

	void WINAPI TcpConManager::on_tcp_event_callback(int action, void* object, char* in_buf, int in_len, char* out_buf, int out_len)
	{
		TcpConManager* pThis = reinterpret_cast<TcpConManager*>(object);
		if (NULL == pThis || NULL == in_buf || in_len <= 0)
		{
			return;
		}
		basicnet::s_net_packet_param* packet = reinterpret_cast<basicnet::s_net_packet_param*>(in_buf);
		if (NULL == packet)
			return;
		switch (action)
		{
		case basicnet::ENET_EVENT_SOCKET_RECV:
		{
			pThis->on_tcp_revc_event(packet->m_socket, packet->m_buf, packet->m_len);
		}
		break;
		case basicnet::ENET_EVENT_SOCKET_CLOSE:
		{
			pThis->on_tcp_close_event(packet->m_socket, packet->m_buf, packet->m_len);
		}
		break;
		case basicnet::ENET_EVENT_SOCKET_CONNECT:
		{
			pThis->on_tcp_connect_event(packet->m_socket);
		}
		break;
		default:
			break;
		}
	}

	void TcpConManager::on_tcp_revc_event(unsigned int socket, char* buf, int len)
	{
		//处理接收数据
		on_recv_event(socket, buf, len);
	}

	void TcpConManager::on_tcp_close_event(unsigned int socket, char* buf, int len)
	{
		//处理关闭
		printf("close socket  \n");
		set_handle(0);
		on_close_event(socket, buf, len);
		return;
	}

	void TcpConManager::on_tcp_connect_event(unsigned int socket)
	{
		//处理连接成功
		printf("connect socket =%d \n", socket);
		m_socket_handle = socket;
		on_connect_event(socket);
		return;
	}

	int TcpConManager::on_send_event(unsigned int main_id, unsigned int sub_id, void* buf, int len, unsigned int hanlde_code)
	{
		if (SOCKET_BUFFER <= len || m_tcp_interface == NULL || m_socket_handle == NULL)
		{
			return -1;
		}
		unsigned int send_len = sizeof(basicnet::_BasicCommandHeader) + len;
		basicnet::_BasicCommandHeader header;
		header.m_main_id = main_id;
		header.m_msub_id = sub_id;
		header.m_handle_code = hanlde_code;
		header.m_check_code = 0;
		header.m_version = 1;
		header.m_reserved = m_time;

		char sendbuffer[SOCKET_BUFFER] = { 0 };
		memcpy(sendbuffer, &header, sizeof(basicnet::_BasicCommandHeader));
		memcpy(sendbuffer + sizeof(basicnet::_BasicCommandHeader), (char*)buf, len);

		return  m_tcp_interface->send_buf(m_tcp_handle, m_socket_handle, sendbuffer, send_len, 0);
	}
}