// http://www.win32developer.com/tutorial/winsock/winsock_tutorial_1.shtm
#include <iostream>
#include <winsock2.h>
#include <shlwapi.h>
#pragma comment(lib,"wsock32.lib")
#pragma comment(lib,"shlwapi.lib")

#define REQUIRED_ARGS 3


int main(int argc, char **argv)
{
	if (argc < REQUIRED_ARGS) {
		printf("usage: %s [Options] <Address> <Port> [Data]\n \
Options:\n\
	\t-c	\t-Disable converting data (from string to digit).\n\
	\t-s	\t-Silent mode. No any messages - only exit code.\n\
	\t-n N	\t-Show only one byte with number N of digit, from responce of server. That byte is also must be exit code.\n\
	\t-t X	\t-X - digit, the time to wait response from server in [ms]\n\
Address - TCP/IP: DNS name | IP \n\
Port - digit\n\
Data - digit or string, if this parameter excluded:\n\
	\tthe programm wait input from operator\n\
NOTE: All digits recognized as:\n\
	\tHEX: with 0x prefix\n\
	\tDEC: without prefix", argv[0]);
        return 1;
	}else{
// try use StrToIntEx():
//		int sendbuf;
//		StrToIntEx("0xFFFF",STIF_SUPPORT_HEX, &sendbuf);
//		std::cout<< sendbuf << "\n";
//		exit(sendbuf);
		//
		// Initialise Winsock
		WSADATA WsaDat;
		if(WSAStartup(MAKEWORD(2,0),&WsaDat)!=0)
		{
			std::cout<<"Winsock error - Winsock initialization failed\r\n";
			WSACleanup();
			system("PAUSE");
			return 0;
		}
		
		// Create our socket
		SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(Socket==INVALID_SOCKET)
		{
			std::cout<<"Winsock error - Socket creation Failed!\r\n";
			WSACleanup();
			system("PAUSE");
			return 0;
		}

		// Resolve IP address for hostname
		struct hostent *host;
		if((host=gethostbyname("localhost"))==NULL)
		{
			std::cout<<"Failed to resolve hostname.\r\n";
			WSACleanup();
			system("PAUSE");
			return 0;
		}

		// Setup our socket address structure
		SOCKADDR_IN SockAddr;
		SockAddr.sin_port=htons(502);
		SockAddr.sin_family=AF_INET;
		SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);

		// Attempt to connect to server
		if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))!=0)
		{
			std::cout<<"Failed to establish connection with server\r\n";
			WSACleanup();
			system("PAUSE");
			return 0;
		}
// try send and receive Modbus request
		char szBuffer[12]={0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x00, 0x00, 0x00, 0x01 };
		std::cout<<"sned: "<< send(Socket,szBuffer,12,0)<< "\n";
		// Display message from server
		char buffer[1000];
		memset(buffer,0,999);
		int inDataLength=recv(Socket,buffer,1000,0);
		std::cout<<"recv: "<< inDataLength <<"\n";

		// Shutdown our socket
		shutdown(Socket,SD_SEND);

		// Close our socket entirely
		closesocket(Socket);

		// Cleanup Winsock
		WSACleanup();
		system("PAUSE");
		return 0;
	}
}