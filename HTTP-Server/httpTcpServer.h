#ifndef INCLUDED_HTTP_TCPSERVER
#define INCLUDED_HTTP_TCPSERVER

#include <iostream>
#include <winsock.h>
#include <string>
#include <stdlib.h>
#include <sstream>

namespace http {
	class TcpServer {
	public:
		TcpServer(std::string ip_address, int port);
		~TcpServer();
		void startListen();

	private:
		std::string m_ip_address;
		int m_port;
		SOCKET m_socket;
		SOCKET m_new_socket;
		long m_incoming_message;
		struct sockaddr_in m_socket_address;
		int m_socket_address_len;
		std::string m_server_message;
		WSADATA m_wsa_data;

		int startServer();
		void closeServer();
		std::string buildResponse();
		void acceptConnection(SOCKET& new_socket);
		void sendResponse();

	};
}
#endif // !INCLUDED_HTTP_TCPSERVER

