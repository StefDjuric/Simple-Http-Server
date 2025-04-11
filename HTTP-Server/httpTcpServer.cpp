#include "httpTcpServer.h"

namespace http
{
	const int BUFFER_SIZE = 30720;

	static void log(const std::string& message) {
		std::cout << message << std::endl;
	}

	static void exitWithError(const std::string& errorMessage) {
		std::cout << WSAGetLastError() << std::endl;
		log("ERROR: " + errorMessage);
		exit(1);
	}

	TcpServer::TcpServer(std::string ip_address, int port) : m_ip_address(ip_address), m_port(port),
		m_socket(), m_new_socket(), m_incoming_message(), m_socket_address(),
		m_socket_address_len(sizeof(m_socket_address)), m_server_message(buildResponse()), m_wsa_data()
	{
		m_socket_address.sin_family = AF_INET;
		m_socket_address.sin_port = htons(m_port);
		m_socket_address.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

		if (startServer() != 0) {
			std::ostringstream ss;
			ss << "Failed to start server with PORT: " << ntohs(m_socket_address.sin_port);
			log(ss.str());
		}

	}
	TcpServer::~TcpServer() {
		closeServer();
	}
	void TcpServer::startListen()
	{
		if (listen(m_socket, 20) < 0) {
			exitWithError("Socekt listen failed.");
		}

		std::ostringstream oss;
		oss << "\n*** Listening on address: " << inet_ntoa(m_socket_address.sin_addr) << " PORT: " << ntohs(m_socket_address.sin_port) << " ***\n\n";
		log(oss.str());

		int bytesReceived;

		while (true) {
			log("=== Waiting for a new connection ===\n\n\n");
			acceptConnection(m_new_socket);
			
			char buffer[BUFFER_SIZE] = { 0 };
			bytesReceived = recv(m_new_socket, buffer, BUFFER_SIZE, 0);

			if (bytesReceived < 0) {
				exitWithError("Failed to receive bytes from client socket connection.");
			}

			std::ostringstream oss;

			oss << "------ Received request from client ------\n\n";
			log(oss.str());

			sendResponse();

			closesocket(m_new_socket);
		}
	}

	int TcpServer::startServer()
	{
		if (WSAStartup(MAKEWORD(2, 0), &m_wsa_data) != 0) {
			exitWithError("WSAStartup failed.");
		}
		m_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (m_socket < 0) {
			exitWithError("Failed to create socket.");
		}

		if (bind(m_socket, (struct sockaddr*)&m_socket_address, sizeof(m_socket_address)) < 0) {
			exitWithError("Socket bind failed.");
		}

		return 0;
	}
	void TcpServer::closeServer()
	{
		closesocket(m_socket);
		closesocket(m_new_socket);
		WSACleanup();
		exit(0);
	}
	std::string TcpServer::buildResponse()
	{
		std::string htmlFile = "<!Doctype html><html lang = \"en\"><body> <h1> Home </h1> <p> Hello from server. </p></body></html>";
		std::ostringstream oss;
		oss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n" << htmlFile;

		return oss.str();
	}

	void TcpServer::acceptConnection(SOCKET& new_socket)
	{
		new_socket = accept(m_socket, (sockaddr*)&m_socket_address, &m_socket_address_len);
		if (new_socket < 0) {
			std::ostringstream oss;
			oss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(m_socket_address.sin_addr) << "; PORT: " << ntohs(m_socket_address.sin_port);
			exitWithError(oss.str());
		}
	
	}

	void TcpServer::sendResponse() {
		int bytesSent;
		int totalBytesSent = 0;

		while (totalBytesSent < m_server_message.size()) {
			bytesSent = send(m_new_socket, m_server_message.c_str(), m_server_message.size(), 0);
			if (bytesSent < 0) {
				break;
			}
			totalBytesSent += bytesSent;
		}

		if (totalBytesSent == m_server_message.size()) {
			log("----- Server response sent to client -----\n\n");

		}
		else {
			log("Error sending response to client. \n\n");
		}
	}


}