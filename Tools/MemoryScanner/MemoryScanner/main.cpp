#include "scannerwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScannerWindow w;
    w.show();

    return a.exec();
}
