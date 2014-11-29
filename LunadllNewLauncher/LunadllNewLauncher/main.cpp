#include "mainwindow.h"
#include <QApplication>


Q_DECL_EXPORT void run(){
    int n = 0;
    QApplication a(n, 0);
    MainWindow w;
    w.show();

    a.exec();
    return;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
