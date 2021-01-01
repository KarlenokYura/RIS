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
	case WSAEINTR:				 message = "Работа функции прервана\n";						  break;
	case WSAEACCES:				 message = "Разрешение отвергнуто\n";						  break;
	case WSAEFAULT:				 message = "Ошибочный адрес\n";								  break;
	case WSAEINVAL:				 message = "Ошибка в аргументе\n";							  break;
	case WSAEMFILE:				 message = "Слишком много файлов открыто\n";				  break;
	case WSAEWOULDBLOCK:		 message = "Ресурс временно недоступен\n";					  break;
	case WSAEINPROGRESS:		 message = "Операция в процессе развития\n";				  break;
	case WSAEALREADY: 			 message = "Операция уже выполняется\n";					  break;
	case WSAENOTSOCK:   		 message = "Сокет задан неправильно\n";						  break;
	case WSAEDESTADDRREQ:		 message = "Требуется адрес расположения\n";				  break;
	case WSAEMSGSIZE:  			 message = "Сообщение слишком длинное\n";				      break;
	case WSAEPROTOTYPE:			 message = "Неправильный тип протокола для сокета\n";		  break;
	case WSAENOPROTOOPT:		 message = "Ошибка в опции протокола\n";					  break;
	case WSAEPROTONOSUPPORT:	 message = "Протокол не поддерживается\n";					  break;
	case WSAESOCKTNOSUPPORT:	 message = "Тип сокета не поддерживается\n";				  break;
	case WSAEOPNOTSUPP:			 message = "Операция не поддерживается\n";					  break;
	case WSAEPFNOSUPPORT:		 message = "Тип протоколов не поддерживается\n";			  break;
	case WSAEAFNOSUPPORT:		 message = "Тип адресов не поддерживается протоколом\n";	  break;
	case WSAEADDRINUSE:			 message = "Адрес уже используется\n";						  break;
	case WSAEADDRNOTAVAIL:		 message = "Запрошенный адрес не может быть использован\n";	  break;
	case WSAENETDOWN:			 message = "Сеть отключена\n";								  break;
	case WSAENETUNREACH:		 message = "Сеть не достижима\n";							  break;
	case WSAENETRESET:			 message = "Сеть разорвала соединение\n";					  break;
	case WSAECONNABORTED:		 message = "Программный отказ связи\n";						  break;
	case WSAECONNRESET:			 message = "Связь восстановлена\n";							  break;
	case WSAENOBUFS:			 message = "Не хватает памяти для буферов\n";				  break;
	case WSAEISCONN:			 message = "Сокет уже подключен\n";							  break;
	case WSAENOTCONN:			 message = "Сокет не подключен\n";							  break;
	case WSAESHUTDOWN:			 message = "Нельзя выполнить send: сокет завершил работу\n";  break;
	case WSAETIMEDOUT:			 message = "Закончился отведенный интервал  времени\n";		  break;
	case WSAECONNREFUSED:		 message = "Соединение отклонено\n";						  break;
	case WSAEHOSTDOWN:			 message = "Хост в неработоспособном состоянии\n";			  break;
	case WSAEHOSTUNREACH:		 message = "Нет маршрута для хоста\n";						  break;
	case WSAEPROCLIM:			 message = "Слишком много процессов\n";						  break;
	case WSASYSNOTREADY:		 message = "Сеть не доступна\n";							  break;
	case WSAVERNOTSUPPORTED:	 message = "Данная версия недоступна\n";					  break;
	case WSANOTINITIALISED:		 message = "Не выполнена инициализация WS2_32.DLL\n";		  break;
	case WSAEDISCON:			 message = "Выполняется отключение\n";						  break;
	case WSATYPE_NOT_FOUND:		 message = "Класс не найден\n";								  break;
	case WSAHOST_NOT_FOUND:		 message = "Хост не найден\n";								  break;
	case WSATRY_AGAIN:			 message = "Неавторизированный хост не найден\n";			  break;
	case WSANO_RECOVERY:		 message = "Неопределенная ошибка\n";						  break;
	case WSANO_DATA:			 message = "Нет записи запрошенного типа\n";				  break;
	case WSA_INVALID_HANDLE:	 message = "Указанный дескриптор события  с ошибкой\n";		  break;
	case WSA_INVALID_PARAMETER:	 message = "Один или более параметров с ошибкой\n";			  break;
	case WSA_IO_INCOMPLETE:		 message = "Объект ввода-вывода не в сигнальном состоянии\n"; break;
	case WSA_IO_PENDING:		 message = "Операция завершится позже\n";					  break;
	case WSA_NOT_ENOUGH_MEMORY:	 message = "Не достаточно памяти\n";						  break;
	case WSA_OPERATION_ABORTED:	 message = "Операция отвергнута\n";							  break;
	case WSAEINVALIDPROCTABLE:	 message = "Ошибочный сервис\n";							  break;
	case WSAEINVALIDPROVIDER:	 message = "Ошибка в версии сервиса\n";						  break;
	case WSAEPROVIDERFAILEDINIT: message = "Невозможно инициализировать сервис\n";			  break;
	case WSASYSCALLFAILURE:		 message = "Аварийное завершение системного вызова\n";		  break;
	default:					 message = "Error\n";										  break;
	};

	return message;
}
string DefineError(string header, int code)
{
	return header + GetErrorMessage(code);
}