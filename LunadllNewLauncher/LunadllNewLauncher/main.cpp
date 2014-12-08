#include "mainwindow.h"
#include "setting.h"
#include <QApplication>
#include <QtCore>
#include <QMessageBox>

#include <Windows.h>

extern resultStruct* Settings = 0;

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
    MainWindow w;
    w.show();
    a.exec();
    a.quit();
    return true;
}

extern "C" Q_DECL_EXPORT void GetPromptResult(void* setting){
    memcpy(setting, Settings, sizeof(resultStruct));
    //MessageBoxA(NULL, QString::number(*(int*)Settings).toStdString().c_str(), "dbg", NULL);
}

extern "C" Q_DECL_EXPORT void FreeVars(){
    delete Settings;
}

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

//    return a.exec();
//}
