#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// Link this program with Winsock library
#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 10

SOCKET clientSockets[MAX_CLIENTS];
int clientCount = 0;
CRITICAL_SECTION clientListLock;

DWORD WINAPI ClientHandler(LPVOID lpParam) {
    SOCKET clientSocket = *(SOCKET*)lpParam;
    char buffer[512];
    int receivedBytes;

    while (1) {
        receivedBytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (receivedBytes > 0) {
            buffer[receivedBytes] = '\0';
            printf("Received data: %s\n", buffer);

            // Broadcast data
            EnterCriticalSection(&clientListLock);
            for (int i = 0; i < clientCount; i++) {
                if (clientSockets[i] != clientSocket) {
                    send(clientSockets[i], buffer, receivedBytes, 0);
                }
            }
            LeaveCriticalSection(&clientListLock);
        } else if (receivedBytes == 0) {
            printf("Connection closed by client.\n");
            break;
        } else {
            printf("Receive data -> {Failed}\n");
            break;
        }
    }

    // Clean up
    closesocket(clientSocket);
    EnterCriticalSection(&clientListLock);
    for (int i = 0; i < clientCount; i++) {
        if (clientSockets[i] == clientSocket) {
            clientSockets[i] = clientSockets[clientCount - 1];
            clientCount--;
            break;
        }
    }
    LeaveCriticalSection(&clientListLock);

    return 0;
}

int main(void) {
    
    WSADATA wsaData;
    SOCKET serverSocketFD, clientSocketFD;
    struct sockaddr_in serverAddress, clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    HANDLE threads[MAX_CLIENTS];
    DWORD threadID;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Initialize Winsock -> {Failed}\n");
        return 1;
    }

    // Create a socket
    serverSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketFD == INVALID_SOCKET) {
        printf("Create a socket -> {Failed}\n");
        WSACleanup();
        return 1;
    }

    // Setup the address information
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("Bind the socket -> {Failed}\n");
        closesocket(serverSocketFD);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    if (listen(serverSocketFD, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen for connections -> {Failed}\n");
        closesocket(serverSocketFD);
        WSACleanup();
        return 1;
    }

    printf("Start listening on port 8000...\n");

    // Initialize critical section
    InitializeCriticalSection(&clientListLock);

    while (1) {
        // Accept connection
        clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);
        if (clientSocketFD == INVALID_SOCKET) {
            printf("Accept connection -> {Failed}\n");
            continue;
        }

        // Add to client list
        EnterCriticalSection(&clientListLock);
        if (clientCount < MAX_CLIENTS) {
            clientSockets[clientCount++] = clientSocketFD;
            // Create a thread to handle this client
            threads[clientCount - 1] = CreateThread(NULL, 0, ClientHandler, (LPVOID)&clientSocketFD, 0, &threadID);
        } else {
            printf("Maximum clients reached. Connection rejected.\n");
            closesocket(clientSocketFD);
        }
        LeaveCriticalSection(&clientListLock);
    }

    // Clean up
    for (int i = 0; i < clientCount; i++) {
        WaitForSingleObject(threads[i], INFINITE);
    }
    DeleteCriticalSection(&clientListLock);
    closesocket(serverSocketFD);
    WSACleanup();

    return 0;
}
