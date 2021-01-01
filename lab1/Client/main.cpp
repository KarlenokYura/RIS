#include <iostream>
#include <string>
#include <tchar.h>
#include <ctime>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable: 4996)

using namespace std;

struct SYNC
{
	SYNC() {}

	SYNC(string command, int correction)
	{
		this->command = command;
		this->correction = correction;
	}

	string command;
	int correction;
};

string GetErrorMessage(int code);
string DefineError(string header, int code);

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		cout << "Client's running" << endl;

		SOCKET clientSocket;
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw DefineError("Startup: ", WSAGetLastError());

		if ((clientSocket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw DefineError("Socket: ", WSAGetLastError());

		SOCKADDR_IN serverConfig;
		serverConfig.sin_family = AF_INET;
		serverConfig.sin_port = htons(2000);
		serverConfig.sin_addr.s_addr = inet_addr("127.0.0.1");

		SYSTEMTIME systemTime;
		SYNC receivedData, sentData("SYNC", 0);
		const int TC = 1000;

		int serverConfigLength = sizeof(serverConfig);
		int maxCorrection = INT_MIN;
		int minCorrection = INT_MAX;
		int averageCorrection = 0;
		const int MAX_ITERATION_COUNT = 10;


		int sentBytes = sendto(clientSocket, (char*)&sentData, sizeof(sentData), 0, (sockaddr*)&serverConfig, sizeof(serverConfig));
		
		if (sentBytes == SOCKET_ERROR)
			throw DefineError("Send: ", WSAGetLastError());
		
		int receivedBytes = recvfrom(clientSocket, (char*)&receivedData, sizeof(receivedData), 0, (sockaddr*)&serverConfig, &serverConfigLength);

		if (receivedBytes == SOCKET_ERROR)
			throw DefineError("Receive: ", WSAGetLastError());

		sentData.correction += receivedData.correction;


		for (int i = 0; i < MAX_ITERATION_COUNT; i++)
		{
			GetSystemTime(&systemTime);
			
			sendto(clientSocket, (char*)&sentData, sizeof(sentData), 0, (sockaddr*)&serverConfig, sizeof(serverConfig));
			recvfrom(clientSocket, (char*)&receivedData, sizeof(receivedData), 0, (sockaddr*)&serverConfig, &serverConfigLength);
			
			if (maxCorrection < receivedData.correction)
				maxCorrection = receivedData.correction;

			if (minCorrection > receivedData.correction)
				minCorrection = receivedData.correction;

			cout << " DataTime " << systemTime.wMonth 
				<< "." << systemTime.wDay 
				<< ".2020" << endl << " Hours " 
				<< systemTime.wHour + 3 
				<< ":" << systemTime.wMinute 
				<< ":" << systemTime.wSecond 
				<< ":" << systemTime.wMilliseconds << " " << endl 
				<< i + 1 << " " << sentData.correction 
				<< " curvalue = " << receivedData.correction
				<< " max correction / min correction: " 
				<< maxCorrection << "/" << minCorrection << endl << endl << endl;

			sentData.correction += receivedData.correction + TC;
			averageCorrection += receivedData.correction;

			Sleep(TC);
		}

		cout << "Average correction: " << averageCorrection / MAX_ITERATION_COUNT << endl;

		if (closesocket(clientSocket) == SOCKET_ERROR)
			throw DefineError("Closesocket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw DefineError("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMessage)
	{
		cout << endl << errorMessage << endl;
	}
}


string GetErrorMessage(int code)
{
	string message;

	switch (code)
	{
	case WSAEINTR:				 message = "������ ������� ��������\n";						  break;
	case WSAEACCES:				 message = "���������� ����������\n";						  break;
	case WSAEFAULT:				 message = "��������� �����\n";								  break;
	case WSAEINVAL:				 message = "������ � ���������\n";							  break;
	case WSAEMFILE:				 message = "������� ����� ������ �������\n";				  break;
	case WSAEWOULDBLOCK:		 message = "������ �������� ����������\n";					  break;
	case WSAEINPROGRESS:		 message = "�������� � �������� ��������\n";				  break;
	case WSAEALREADY: 			 message = "�������� ��� �����������\n";					  break;
	case WSAENOTSOCK:   		 message = "����� ����� �����������\n";						  break;
	case WSAEDESTADDRREQ:		 message = "��������� ����� ������������\n";				  break;
	case WSAEMSGSIZE:  			 message = "��������� ������� �������\n";				      break;
	case WSAEPROTOTYPE:			 message = "������������ ��� ��������� ��� ������\n";		  break;
	case WSAENOPROTOOPT:		 message = "������ � ����� ���������\n";					  break;
	case WSAEPROTONOSUPPORT:	 message = "�������� �� ��������������\n";					  break;
	case WSAESOCKTNOSUPPORT:	 message = "��� ������ �� ��������������\n";				  break;
	case WSAEOPNOTSUPP:			 message = "�������� �� ��������������\n";					  break;
	case WSAEPFNOSUPPORT:		 message = "��� ���������� �� ��������������\n";			  break;
	case WSAEAFNOSUPPORT:		 message = "��� ������� �� �������������� ����������\n";	  break;
	case WSAEADDRINUSE:			 message = "����� ��� ������������\n";						  break;
	case WSAEADDRNOTAVAIL:		 message = "����������� ����� �� ����� ���� �����������\n";	  break;
	case WSAENETDOWN:			 message = "���� ���������\n";								  break;
	case WSAENETUNREACH:		 message = "���� �� ���������\n";							  break;
	case WSAENETRESET:			 message = "���� ��������� ����������\n";					  break;
	case WSAECONNABORTED:		 message = "����������� ����� �����\n";						  break;
	case WSAECONNRESET:			 message = "����� �������������\n";							  break;
	case WSAENOBUFS:			 message = "�� ������� ������ ��� �������\n";				  break;
	case WSAEISCONN:			 message = "����� ��� ���������\n";							  break;
	case WSAENOTCONN:			 message = "����� �� ���������\n";							  break;
	case WSAESHUTDOWN:			 message = "������ ��������� send: ����� �������� ������\n";  break;
	case WSAETIMEDOUT:			 message = "���������� ���������� ��������  �������\n";		  break;
	case WSAECONNREFUSED:		 message = "���������� ���������\n";						  break;
	case WSAEHOSTDOWN:			 message = "���� � ����������������� ���������\n";			  break;
	case WSAEHOSTUNREACH:		 message = "��� �������� ��� �����\n";						  break;
	case WSAEPROCLIM:			 message = "������� ����� ���������\n";						  break;
	case WSASYSNOTREADY:		 message = "���� �� ��������\n";							  break;
	case WSAVERNOTSUPPORTED:	 message = "������ ������ ����������\n";					  break;
	case WSANOTINITIALISED:		 message = "�� ��������� ������������� WS2_32.DLL\n";		  break;
	case WSAEDISCON:			 message = "����������� ����������\n";						  break;
	case WSATYPE_NOT_FOUND:		 message = "����� �� ������\n";								  break;
	case WSAHOST_NOT_FOUND:		 message = "���� �� ������\n";								  break;
	case WSATRY_AGAIN:			 message = "������������������ ���� �� ������\n";			  break;
	case WSANO_RECOVERY:		 message = "�������������� ������\n";						  break;
	case WSANO_DATA:			 message = "��� ������ ������������ ����\n";				  break;
	case WSA_INVALID_HANDLE:	 message = "��������� ���������� �������  � �������\n";		  break;
	case WSA_INVALID_PARAMETER:	 message = "���� ��� ����� ���������� � �������\n";			  break;
	case WSA_IO_INCOMPLETE:		 message = "������ �����-������ �� � ���������� ���������\n"; break;
	case WSA_IO_PENDING:		 message = "�������� ���������� �����\n";					  break;
	case WSA_NOT_ENOUGH_MEMORY:	 message = "�� ���������� ������\n";						  break;
	case WSA_OPERATION_ABORTED:	 message = "�������� ����������\n";							  break;
	case WSAEINVALIDPROCTABLE:	 message = "��������� ������\n";							  break;
	case WSAEINVALIDPROVIDER:	 message = "������ � ������ �������\n";						  break;
	case WSAEPROVIDERFAILEDINIT: message = "���������� ���������������� ������\n";			  break;
	case WSASYSCALLFAILURE:		 message = "��������� ���������� ���������� ������\n";		  break;
	default:					 message = "Error\n";										  break;
	};

	return message;
}
string DefineError(string header, int code)
{
	return header + GetErrorMessage(code);
}