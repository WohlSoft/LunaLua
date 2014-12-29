#ifndef LOGGER_H
#define LOGGER_H

#include <QWidget>
#include <QMutex>
#include <QTimer>

namespace Ui {
class Logger;
}

class Logger : public QWidget
{
    Q_OBJECT

public:
    explicit Logger(QWidget *parent = 0);
    ~Logger();

    void log(const QString &logText);
public slots:
    void doLogging();
private:
    QTimer timer;
    QMutex locker;
    QStringList loggerBuf;
    Ui::Logger *ui;
};

#endif // LOGGER_H
