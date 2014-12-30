#include "logger.h"
#include "ui_logger.h"

#include <QFile>
#include <QTextStream>

Logger::Logger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Logger)
{
    ui->setupUi(this);
    connect(&timer, SIGNAL(timeout()), this, SLOT(doLogging()));
    timer.start(1);

}

Logger::~Logger()
{
    delete ui;
}

void Logger::log(const QString &logText)
{
    locker.lock();
    loggerBuf << logText;
    locker.unlock();

    QFile outFile("smbx_log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << logText << "\n";
    outFile.close();
}

void Logger::doLogging()
{
    locker.lock();
    foreach(QString strs, loggerBuf){
        ui->editLogger->appendPlainText(strs);
    }
    loggerBuf.clear();
    locker.unlock();
}
