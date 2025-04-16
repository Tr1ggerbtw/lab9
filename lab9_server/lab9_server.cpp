#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

struct SmartHomeState {
    bool heating;
    bool doorOpen;
    float petFood;
    float temperature;
    bool hasPetFood;
};

void parseCommand(const string& command, string& name, string& value) {
    size_t pos = command.find(':');
    if (pos != string::npos) {
        name = command.substr(0, pos);
        value = command.substr(pos + 1);
    }
    else {
        name = command;
        value = "";
    }
}

int main()
{
  
  
    SmartHomeState state = {
        false,  // heating
        false,  // doorOpen
        0.0f,   // petFood
        22.5f,  // temperature
        false   // hasPetFood
    };

    WORD ver = MAKEWORD(2, 2);
    WSADATA wsData;
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0) {
        cerr << "Failed with error: " << wsOk << endl;
        return 1;
    }

    SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        cerr << "Can't create a socket! Error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    int optval = 1;
    setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

    const unsigned short PORT = 54001;
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    int bindResult = bind(listening, (sockaddr*)&hint, sizeof(hint));
    if (bindResult == SOCKET_ERROR) {
        cerr << "Can't bind socket! Error: " << WSAGetLastError() << endl;
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on port " << PORT << "..." << endl;

    int listenResult = listen(listening, SOMAXCONN);
    if (listenResult == SOCKET_ERROR) {
        cerr << "Can't listen! Error: " << WSAGetLastError() << endl;
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    sockaddr_in client;
    int clientSize = sizeof(client);
    SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == INVALID_SOCKET) {
        cerr << "Accept failed! Error: " << WSAGetLastError() << endl;
        closesocket(listening);
        WSACleanup();
        return 1;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        cout << "Client connected: " << host << " using port " << service << endl;
    }
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << "Client connected: " << host << " using port " << ntohs(client.sin_port) << endl;
    }

    closesocket(listening);

    const int BUFFER_SIZE = 4096;
    char buf[BUFFER_SIZE];

    while (true) {
        ZeroMemory(buf, BUFFER_SIZE);

        int bytesReceived = recv(clientSocket, buf, BUFFER_SIZE, 0);
        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Error in recv(). Quitting..." << endl;
            break;
        }

        if (bytesReceived == 0) {
            cout << "Client disconnected" << endl;
            break;
        }

        string receivedMsg = string(buf, 0, bytesReceived);
        cout << "Received: " << receivedMsg << endl;

        string name, value;
        parseCommand(receivedMsg, name, value);

        string response;

        if (name == "heating") {
            state.heating = (value == "1");
            response = "Heating is now " + string(state.heating ? "ON" : "OFF");
        }
        else if (name == "door") {
            state.doorOpen = (value == "1");
            response = "Door is now " + string(state.doorOpen ? "OPEN" : "CLOSED");
        }
        else if (name == "food") {
            try {
                float amount = stof(value);
                state.petFood += amount;
                state.hasPetFood = (state.petFood > 0);
                response = "Added " + value + " food, total: " + to_string(state.petFood);
            }
            catch (const exception& e) {
                response = "Invalid food amount";
            }
        }
        else if (name == "status") {
            stringstream ss;
            ss << "Temperature: " << state.temperature << " C\n";
            ss << "Heating: " << (state.heating ? "ON" : "OFF") << "\n";
            ss << "Door: " << (state.doorOpen ? "OPEN" : "CLOSED") << "\n";
            ss << "Pet has food: " << (state.hasPetFood ? "YES" : "NO") << "\n";
            ss << "Food amount: " << state.petFood;
            response = ss.str();
        }
        else {
            response = "Unknown command: " + name;
        }

        send(clientSocket, response.c_str(), response.size() + 1, 0);
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}