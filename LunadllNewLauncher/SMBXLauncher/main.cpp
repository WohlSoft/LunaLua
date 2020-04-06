#include "mainlauncherwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //QApplication::setAttribute(Qt::AA_UseDesktopOpenGL, false);
    //QApplication::setAttribute(Qt::AA_UseOpenGLES, false);
    //QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL, true);
    QApplication a(argc, argv);
    MainLauncherWindow w;
    w.init("launcher.ini");
    w.show();
    w.initSize();

    return a.exec();
}
