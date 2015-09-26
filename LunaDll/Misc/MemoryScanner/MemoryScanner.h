#ifndef MemoryScanner_hhhhhh
#define MemoryScanner_hhhhhh

#include <thread>
#include <map>
#include "MemoryScannerDefines.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

class MemoryScanner
{
private:
    
    /*This section is for the server thread*/

    // 1. Event: Update Event
    // 2. Event: Socket Receive Event
    // 3. Event: Socket Close Event
    HANDLE serverEvents[3];
    
    //Here to store the data which should be updated
    std::map<void*, MemoryScannerDefines::MemoryType> toScan;
    //std::map<void*, std::map<void*, MemoryScannerDefines::MemoryType>> toScanSimpleStruct;

    void serverThreadMain();

    /*Server Section thread end*/

    /*This section is for the main thread*/
    std::thread* serverThread;
    bool running;

public:
    MemoryScanner();
    ~MemoryScanner();

    void run();
    void notifyUpdate();
    


};

#endif



