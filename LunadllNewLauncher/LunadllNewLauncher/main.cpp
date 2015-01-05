#include "mainwindow.h"
#include "setting.h"
#include <QApplication>
#include <QtCore>
#include <QMessageBox>
//#include <QThread>

#include <Windows.h>
#include <thread>

#include "asyncdebugger.h"
#include "logger.h"
#include "launcherwindowhtml.h"


resultStruct* Settings = 0;
AsyncDebugger* asyncDebuggerWnd = 0;
QApplication* asyncApp = 0;
std::thread* thr = 0;

QApplication* asyncLoggerApp = 0;
Logger* asyncLoggerWnd = 0;
std::thread* asyncLoggerThread = 0;

extern "C" Q_DECL_EXPORT bool run()
{
    char pathBuf[500];
    HMODULE hModule = GetModuleHandleA(NULL);
    if(!hModule)
        return false;

    if(!GetModuleFileNameA(hModule, pathBuf, sizeof(pathBuf)))
        return false;

    QApplication::addLibraryPath( QFileInfo( pathBuf ).dir().path() );

    Settings = new resultStruct;

    char* myBuffer = pathBuf;

    int n = 1;
    QApplication a(n, &myBuffer);
    //MainWindow w;
    //w.show();
    LauncherWindowHtml lwh;
    lwh.show();
    a.exec();
    a.quit();
    return true;
}

extern "C" Q_DECL_EXPORT void GetPromptResult(void* setting){
    memcpy(setting, Settings, sizeof(resultStruct));
}

extern "C" Q_DECL_EXPORT void FreeVars(){
    delete Settings;
}

void initAndRunAsyncDebugger(){
    char pathBuf[500];
    HMODULE hModule = GetModuleHandleA(NULL);
    if(!hModule)
        return;

    if(!GetModuleFileNameA(hModule, pathBuf, sizeof(pathBuf)))
        return;

    QApplication::addLibraryPath( QFileInfo( pathBuf ).dir().path() );

    char* myBuffer = pathBuf;

    int n = 1;
    QApplication a(n, &myBuffer);
    asyncDebuggerWnd = new AsyncDebugger();
    asyncDebuggerWnd->show();
    a.exec();
    a.quit();
    delete asyncDebuggerWnd;
    asyncDebuggerWnd = 0;
    return;
}

extern "C" Q_DECL_EXPORT void runAsyncDebugger(){
    thr = new std::thread(&initAndRunAsyncDebugger);
    thr->detach();
}

extern "C" Q_DECL_EXPORT __stdcall WINBOOL asyncBitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, unsigned int dwRop){
    if(asyncDebuggerWnd){
        if(!asyncDebuggerWnd->skipCatching()){
            SetForegroundWindow((HWND)asyncDebuggerWnd->winId());
            AsyncDebugger::bitbltData data;
            data.hdcDest = hdcDest,
            data.nXDest = nXDest;
            data.nYDest = nYDest;
            data.nWidth = nWidth;
            data.nHeight = nHeight;
            data.hdcSrc = hdcSrc;
            data.nXSrc = nXSrc;
            data.nYSrc = nYSrc;
            data.dwRop = dwRop;
            asyncDebuggerWnd->setRunningText("Not Running");
            asyncDebuggerWnd->setData(data);
            asyncDebuggerWnd->update();
            while(!asyncDebuggerWnd->getState()){};
            asyncDebuggerWnd->setState(0);
            asyncDebuggerWnd->setRunningText("Running");
         }
    }
    return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop);
}

void asyncLogger(){
    char pathBuf[500];
    HMODULE hModule = GetModuleHandleA(NULL);
    if(!hModule)
        return;

    if(!GetModuleFileNameA(hModule, pathBuf, sizeof(pathBuf)))
        return;

    QApplication::addLibraryPath( QFileInfo( pathBuf ).dir().path() );

    char* myBuffer = pathBuf;

    int n = 1;
    QApplication a(n, &myBuffer);
    asyncLoggerWnd = new Logger();
    asyncLoggerWnd->show();
    a.exec();
    a.quit();
    delete asyncLoggerWnd;
    asyncLoggerWnd = 0;
    return;
}

extern "C" Q_DECL_EXPORT void runAsyncLogger(){
    asyncLoggerThread = new std::thread(&asyncLogger);
    asyncLoggerThread->detach();
}

extern "C" Q_DECL_EXPORT void asyncLog(const char* text){
    if(asyncLoggerWnd){
        asyncLoggerWnd->log(text);
    }
}
