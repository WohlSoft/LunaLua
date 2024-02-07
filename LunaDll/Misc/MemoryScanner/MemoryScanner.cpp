#include "MemoryScanner.h"



#include <iomanip>
#include <sstream>

#include "../VB6StrPtr.h"
#include "../../GlobalFuncs.h"

#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1

MemoryScanner::MemoryScanner() :
    serverThread(nullptr),
    running(false)
{

}


MemoryScanner::~MemoryScanner()
{
}

void MemoryScanner::run()
{
    if (serverThread == NULL){
        serverEvents[0] = CreateEvent(
            NULL,       //Default security attributes
            FALSE,      //Auto-reset event
            FALSE,      //Init state is nonsignaled
            TEXT("MemoryScannerUpdateEvent")
            );

        serverThread = new std::thread(
            [this] {
            this->serverThreadMain();
            running = false; 
        });
    }
}

void MemoryScanner::serverThreadMain()
{
    WSADATA wsaData;

    //First startup the socket.
    int theResultCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (theResultCode != 0) {
        char msg[300];
        sprintf_s(msg, "WSAStartup failed with error: %d\n", theResultCode);
        LunaMsgBox::ShowA(NULL, msg, "Error", NULL);
        return;
    }

    //Second add our address information
    addrinfo connectionInfo;
    memset(&connectionInfo, 0, sizeof(connectionInfo));
    connectionInfo.ai_family = AF_INET;
    connectionInfo.ai_socktype = SOCK_STREAM;
    connectionInfo.ai_protocol = IPPROTO_TCP;
    connectionInfo.ai_flags = AI_PASSIVE;

    //Now resolve port and address
    addrinfo* outputResult = NULL;
    theResultCode = getaddrinfo(NULL, "42198", &connectionInfo, &outputResult);
    if (theResultCode != 0) {
        char msg[300];
        sprintf_s(msg, "getaddrinfo failed with error: %d\n", theResultCode);
        LunaMsgBox::ShowA(NULL, msg, "Error", NULL);
        WSACleanup();
        return;
    }

    //Now create the socket for the server
    SOCKET ListenSocket = socket(outputResult->ai_family, outputResult->ai_socktype, outputResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        char msg[300];
        sprintf_s(msg, "socket failed with error: %ld\n", WSAGetLastError());
        LunaMsgBox::ShowA(NULL, msg, "Error", NULL);
        freeaddrinfo(outputResult);
        WSACleanup();
        return;
    }


    // Setup the TCP listening socket
    theResultCode = bind(ListenSocket, outputResult->ai_addr, (int)outputResult->ai_addrlen);
    if (theResultCode == SOCKET_ERROR) {
        char msg[300];
        sprintf_s(msg, "bind failed with error: %d\n", WSAGetLastError());
        LunaMsgBox::ShowA(NULL, msg, "Error", NULL);
        freeaddrinfo(outputResult);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(outputResult);


    theResultCode = listen(ListenSocket, SOMAXCONN);
    if (theResultCode == SOCKET_ERROR) {
        char msg[300];
        sprintf_s(msg, "listen failed with error: %d\n", WSAGetLastError());
        LunaMsgBox::ShowA(NULL, msg, "Error", NULL);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }



    // Accept a client socket
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        char msg[300];
        sprintf_s(msg, "accept failed with error: %d\n", WSAGetLastError());
        LunaMsgBox::ShowA(NULL, msg, "Error", NULL);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    //Now create our events
    serverEvents[1] = WSACreateEvent();
    WSAEventSelect(ClientSocket, serverEvents[1], FD_READ);

    serverEvents[2] = WSACreateEvent();
    WSAEventSelect(ClientSocket, serverEvents[2], FD_CLOSE);

    running = true;
    while (1){
        DWORD indexHandler = WaitForMultipleObjects(
            3, 
            serverEvents, 
            FALSE, 
            INFINITE
            );

        bool exitLoop = false;
        switch (indexHandler){
        case WAIT_OBJECT_0 + 0:
        {
            //Don't send any date if you don't have any
            if (!toScan.size())
                break;

            //1. Event: Update the data
            std::ostringstream writer;

            MemoryScannerDefines::ServerCommands updateCmd = MemoryScannerDefines::ServerCommands::CMD_UPDATELIST;
            writer.write((const char*)&updateCmd, sizeof(updateCmd));
            int sizeOfMap = toScan.size();
            writer.write((const char*)&sizeOfMap, sizeof(sizeOfMap));

            for (decltype(toScan)::iterator it = toScan.begin(); it != toScan.end(); ++it){
                void* addr = it->first;
                MemoryScannerDefines::MemoryType memType = it->second;

                if (!addr)
                    continue;

                if (memType == MemoryScannerDefines::MemoryType::MEM_STRING){
                    VB6StrPtr* ptr = *(VB6StrPtr**)addr;
                    std::string content = *ptr;
                    int lengthOfContent = content.length();
                    writer.write((const char*)&addr, sizeof(addr));
                    writer.write((const char*)&memType, sizeof(memType));
                    writer.write((const char*)&lengthOfContent, sizeof(lengthOfContent));
                    writer.write((const char*)content.c_str(), lengthOfContent);
                }
                else{
                    int lengthOfContent = 0;
                    switch (memType){
                    case MemoryScannerDefines::MemoryType::MEM_BYTE: lengthOfContent = 1; break;
                    case MemoryScannerDefines::MemoryType::MEM_WORD: lengthOfContent = 2; break;
                    case MemoryScannerDefines::MemoryType::MEM_FLOAT:
                    case MemoryScannerDefines::MemoryType::MEM_DWORD: lengthOfContent = 4; break;
                    case MemoryScannerDefines::MemoryType::MEM_DOUBLE: lengthOfContent = 8; break;
                    default:
                        continue;
                    }

                    writer.write((const char*)&addr, sizeof(addr));
                    writer.write((const char*)&memType, sizeof(memType));
                    writer.write((const char*)&lengthOfContent, sizeof(lengthOfContent));
                    writer.write((const char*)addr, lengthOfContent);
                }
            }

            std::string writtenData = writer.str();

            int theResultCode = send(ClientSocket, writtenData.c_str(), writtenData.length(), 0);
            if (theResultCode == SOCKET_ERROR) {
                char msg[300];
                sprintf_s(msg, "send failed with error: %d\n", WSAGetLastError());
                LunaMsgBox::ShowA(NULL, msg, "Error", NULL);
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }

            break;
        }
        case WAIT_OBJECT_0 + 1:
        {
            //2. Event: Got data to process
            const int bufferLen = 10000;
            char buffer[bufferLen];
            int recvBufferLen = recv(ClientSocket, buffer, bufferLen, 0);
            
            std::stringstream theData;
            theData.write(buffer, recvBufferLen);

            MemoryScannerDefines::ServerCommands cmd;
            void* addr;
            MemoryScannerDefines::MemoryType memType;

            theData.read((char*)&cmd, sizeof(cmd));
            theData.read((char*)&addr, sizeof(addr));
            theData.read((char*)&memType, sizeof(memType));

            if (cmd == MemoryScannerDefines::ServerCommands::CMD_UPDATELIST){
                toScan[addr] = memType;
            }

            break;
        }
        case WAIT_OBJECT_0 + 2:
            //3. Event: Close the socket
            exitLoop = true;
        break;

        default:
            exitLoop = true;
        }

        if (exitLoop)
            break;
    }

    // shutdown the connection since we're done
    theResultCode = shutdown(ClientSocket, SD_SEND);
    if (theResultCode == SOCKET_ERROR) {
        char msg[300];
        sprintf_s(msg, "shutdown failed with error: %d\n", WSAGetLastError());
        LunaMsgBox::ShowA(NULL, msg, "Error", NULL);
        closesocket(ClientSocket);
        WSACleanup();
        return;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

}

void MemoryScanner::notifyUpdate()
{
    if (running){
        SetEvent(serverEvents[0]);
    }
}
