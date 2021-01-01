#include <iostream>
#include <string>
#include <tchar.h>
#include <WinSock2.h>
#include <ctime>
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

int GetAverageCorrection(int averageCorrection[], int length);

string GetErrorMessage(int code);
string DefineError(string header, int code);


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		cout << "Server's running" << endl;

		SOCKET serverSocket;
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw DefineError("Startup: ", WSAGetLastError());

		if ((serverSocket = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw DefineError("Socket: ", WSAGetLastError());

		SOCKADDR_IN serverConfig;
		serverConfig.sin_family = AF_INET;
		serverConfig.sin_port = htons(2000);
		serverConfig.sin_addr.s_addr = INADDR_ANY;

		if (bind(serverSocket, (LPSOCKADDR)&serverConfig, sizeof(serverConfig)) == SOCKET_ERROR)
			throw DefineError("Bind Server: ", WSAGetLastError());

		SYNC receivedData, sentData("SYNC", 0);
		SYSTEMTIME systemTime;
		int averageResults[10];
		clock_t delta;
		int i = 1;
		const int MAX_ITERATION_COUNT = 10;

		for (int i = 0; i != MAX_ITERATION_COUNT; i++)
		{
			SOCKADDR_IN clientConfig;
			int clientConfigLength = sizeof(clientConfig);
			int average = 0;

			GetSystemTime(&systemTime);

			int recvBytes = recvfrom(serverSocket, (char*)&receivedData, sizeof(receivedData), NULL, (sockaddr*)&clientConfig, &clientConfigLength);

			if (recvBytes == SOCKET_ERROR)
				throw DefineError("Receive: ", WSAGetLastError());

			delta = clock();

			int correctionDelta = delta - receivedData.correction;
			sentData.correction = correctionDelta;
			averageResults[i] = correctionDelta;

			average = GetAverageCorrection(averageResults, i + 1);

			int sentBytes = sendto(serverSocket, (char*)&sentData, sizeof(sentData), 0, (sockaddr*)&clientConfig, sizeof(clientConfig));

			if (sentBytes == SOCKET_ERROR)
				throw DefineError("Send: ", WSAGetLastError());

			cout << "DataTime "
				<< systemTime.wMonth << "."
				<< systemTime.wDay << ".2020" << " " << endl
				<< systemTime.wHour + 3 << " Hours "
				<< systemTime.wMinute << " Minuts "
				<< systemTime.wSecond << " Seconds "
				<< systemTime.wMilliseconds << " MiliSeconds " << endl
				<< inet_ntoa(clientConfig.sin_addr) << " Correction = "
				<< sentData.correction << ", Average Correction = "
				<< average << endl << endl << endl;
		}

		if (closesocket(serverSocket) == SOCKET_ERROR)
			throw DefineError("close socket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw DefineError("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMessage)
	{
		cout << endl << errorMessage << endl;
	}
}


int GetAverageCorrection(int averageCorrection[], int length)
{
	int value = 0;
	for (int i = 0; i < length; i++)
	{
		value += averageCorrection[i];
	}
	return value / length;
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
