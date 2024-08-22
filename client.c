#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// Link this program with Winsock library
#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 512

typedef struct {
    SOCKET clientSocketFD;
    char buffer[BUFFER_SIZE];
} ThreadParams;

DWORD WINAPI InputHandler(LPVOID lpParam) {
    ThreadParams *params = (ThreadParams*)lpParam;
    SOCKET clientSocketFD = params->clientSocketFD;
    char *inputBuffer = params->buffer;
    
    while (fgets(inputBuffer, BUFFER_SIZE, stdin) != NULL) {
        size_t len = strlen(inputBuffer);
        if (len > 0 && inputBuffer[len - 1] == '\n') {
            // Remove newline
            inputBuffer[len - 1] = '\0';  
        }
        if (send(clientSocketFD, inputBuffer, (int)strlen(inputBuffer), 0) == SOCKET_ERROR) {
            printf("Send data -> {Failed}\n");
        }
    }
    return 0;
}

int main() {

    WSADATA wsaData;
    SOCKET clientSocketFD;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];
    int receivedBytes;
    HANDLE inputThread;
    DWORD threadID;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Initialize Winsock -> {Failed}\n");
        return 1;
    }

    // Create a socket
    clientSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketFD == INVALID_SOCKET) {
        printf("Create socket -> {Failed}\n");
        WSACleanup();
        return 1;
    }

    // Setup the address information
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(clientSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        printf("Connect to the server -> {Failed}\n");
        closesocket(clientSocketFD);
        WSACleanup();
        return 1;
    }

    // Prepare thread parameters
    ThreadParams params;
    params.clientSocketFD = clientSocketFD;
    strcpy(params.buffer, "");

    // Start the input thread
    inputThread = CreateThread(NULL, 0, InputHandler, (LPVOID)&params, 0, &threadID);

    // Receive data from the server
    while ((receivedBytes = recv(clientSocketFD, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[receivedBytes] = '\0';
        printf("Received data: %s\n", buffer);
    }

    if (receivedBytes == 0) {
        printf("Connection closed by server.\n");
    } else {
        printf("Receive data -> {Failed}\n");
    }

    // Clean up
    WaitForSingleObject(inputThread, INFINITE);
    closesocket(clientSocketFD);
    WSACleanup();

    return 0;
}
