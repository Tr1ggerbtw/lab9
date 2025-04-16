#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
  
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        cerr << "Can't initialize Winsock! Error: " << wsOk << endl;
        return 1;
    }

  
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Can't create socket! Error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(54001);
   
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); 

   
    int connResult = connect(clientSocket, (sockaddr*)&server, sizeof(server));
    if (connResult == SOCKET_ERROR) {
        cerr << "Can't connect to server! Error: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server!" << endl;

    char buf[4096];
    string userInput;

    do {
        cout << "\n=== Smart Home Control ===\n";
        cout << "1. Turn heating on/off (heating:0/1)\n";
        cout << "2. Open/close door (door:0/1)\n";
        cout << "3. Feed pet (food:amount)\n";
        cout << "4. Get status (status)\n";
        cout << "5. Exit\n";
        cout << "Enter command: ";

        getline(cin, userInput);

        if (userInput == "5" || userInput == "exit") {
            break;
        }

        string command;
        string value;

        switch (userInput[0]) {
        case '1':
            cout << "Enter 0 to turn off, 1 to turn on: ";
            getline(cin, value);
            command = "heating:" + value;
            break;

        case '2':
            cout << "Enter 0 to close, 1 to open: ";
            getline(cin, value);
            command = "door:" + value;
            break;

        case '3':
            cout << "Enter amount of food: ";
            getline(cin, value);
            command = "food:" + value;
            break;

        case '4':
            command = "status";
            break;

        default:
            command = userInput; 
            break;
        }

        
        int sendResult = send(clientSocket, command.c_str(), command.size(), 0);
        if (sendResult == SOCKET_ERROR) {
            cerr << "Error sending message to server! Error: " << WSAGetLastError() << endl;
            break;
        }

     
        ZeroMemory(buf, 4096);
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (bytesReceived == 0) {
            cout << "Server disconnected" << endl;
            break;
        }

  
        cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;

    } while (true);

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}