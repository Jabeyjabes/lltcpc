#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <shlwapi.h>
#include <iostream>
#include <iomanip>

//#include <strstream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")

#define DEFAULT_BUFLEN 512
#define REQUIRED_ARGS 3
#define ADDR_MAX_LEN 512
#define PORT_MAX_LEN 512

using namespace std;

int __cdecl main(int argc, TCHAR **argv) 
{
	int i,tmp;
	char _i;
	// 000030-Tx:00 7E 00 00 00 06 FF 03 40 01 00 01
	TCHAR address[ADDR_MAX_LEN];
	TCHAR port[PORT_MAX_LEN];
	unsigned int timeout;
	unsigned char sendbuf[DEFAULT_BUFLEN]={0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x06 ,0xFF ,0x03 ,0x40 ,0x01 ,0x00 ,0x01};
    unsigned char recvbuf[DEFAULT_BUFLEN];
// ========================= command line parameters =============================================
	int c;
    // Validate the parameters
	// were parameters: <address> <port> [-n <bytes_receive>] <hex_data_string>
    if (argc < REQUIRED_ARGS) {
		printf("usage: %s [Options] <Address> <Port> [-d Data]\n \
				Options:\n\
					\t-c	\t-disable converting string inputed by operator (ignored with parameter -d)\n\
					\t-s	\t-silent mode\n\
					\t-n N	\t-show only one byte with number N of digit, from responce of server\n\
					\t-t X	\t-digit, X the time for waiting response from server in [ms]\n\
				Address - TCP/IP: DNS name | IP \n\
				Port - digit\n\
				Data - digit, if this parameter excluded:\n\
					\tthe programm wait input from operator\n\
				NOTE: All digits recognized as:\n\
					\tHEX: with 0x prefix\n\
					\tDEC: without prefix", argv[0]);
        return 1;
    }else 

	optind = 0;
	while ((c = getopt(argc, argv, _T("sda:p:t:"))) != EOF)
	{
		switch (c)
		{
			case _T('a'):
				printf_s("\taddress: '%s' (%x)\n", optarg,_T(c));
				strncpy_s((char*)address,ADDR_MAX_LEN,(char*)optarg,ADDR_MAX_LEN);
				break;
				
			case _T('p'):
				printf_s("\tport: '%s' (%x)\n", optarg,c);
				strncpy_s((char*)port,PORT_MAX_LEN,(char*)optarg,PORT_MAX_LEN);
				break;

			case _T('t'):
				
				tmp = StrToIntEx((LPCSTR)optarg,STIF_SUPPORT_HEX, (int *)&timeout);
				printf_s("\treceive timeout: '%s' (%x) rez= %x\n", optarg,c,tmp);
				break;

			case _T('s'):
#ifdef DEBUG
				printf_s("\tmode: silent (%x)\n", c);
#endif
				break;

			case _T('d'):
				printf_s("\tdata: %d: ",optind);
				_i = tmp = 0;
				for (i = optind; i < argc ; i++){
					tmp = StrToIntEx((LPCSTR)argv[i],STIF_SUPPORT_HEX, (int*)&sendbuf[_i]);
					if (!tmp){
						cout << "this: " << argv[i] << "not valid HEX value"<< endl;
						return 2;
					}else
					cout.fill( '0' );
					cout << " " << setw( 2 )<< hex <<int(sendbuf[_i]);
					_i++;
				}
				cout<<endl;
				break;
				
			case _T('?'):		// illegal option - i.e., an option that was
							// not specified in the optstring

				// Note: you may choose to ignore illegal options
#ifdef DEBUG
				printf_s("\tERROR:  illegal option %s\n", argv[optind-1]);
#endif
				//return 3;
				break;
				
			default:		// legal option - it was specified in optstring,
							// but it had no "case" handler 

				// Note: you may choose not to make this an error
#ifdef DEBUG
				printf_s("\tWARNING:  no handler for option %c\n", c);
#endif
				//return 4;
				break;
		}
	}

	if (optind < argc)
	{
		printf_s("argc - optind = %d - %d = %d\n",argc,optind,argc - optind);
	}else printf_s("argc = %d optind = %d \n",argc,optind);
	cout << "adr:"<<address<< endl;
	cout << "port:"<<port<< endl;
// ================================ tcp ==================================================

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
	
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 5;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(address, port, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 6;
    }

	// Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 7;
        }

	// timeout
	int err;
	int toutlen = sizeof(timeout);
	if (err = setsockopt(ConnectSocket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout)) != 0)
	cout << "option set failed " << err << WSAGetLastError() << endl;
	if (getsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, &toutlen) != SOCKET_ERROR) {
		cout << "SO_RCVTIMEO: " << timeout << endl;
  }
	// RES


        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 8;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, (char*)sendbuf, argc - optind, 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 9;
    }

    printf("Bytes Sent: %ld\n", iResult);	// DEBUG
/*
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 10;
    }
*/
    // Receive until the peer closes the connection
    do {
        iResult = recv(ConnectSocket, (char*)recvbuf, recvbuflen, 0);
		if ( iResult > 0 ){
               printf("Bytes received: %d: %x\n", iResult,recvbuf[10]);
				break;
		}
        else
			if ( iResult == 0 ){
				printf("Connection closed %d\n",WSAGetLastError());
				break;
			} else {
				printf("recv failed: %d\n", WSAGetLastError());
				break;
			}

    } while( iResult > 0 );

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
	for (i=0; i< iResult; i++){
		cout.fill( '0' );
		cout << " " << setw( 2 )<< hex <<int(recvbuf[i]);
	}
	cout<<endl;
	cin>>i;
//	printf("the end\n");
    return 0;
}
