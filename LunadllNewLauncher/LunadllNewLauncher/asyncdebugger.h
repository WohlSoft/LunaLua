#ifndef ASYNCDEBUGGER_H
#define ASYNCDEBUGGER_H

#include <QWidget>
#include <QMutex>
#include <QTimer>
#include <QPaintEvent>

namespace Ui {
class AsyncDebugger;
}

class AsyncDebugger : public QWidget
{
    Q_OBJECT

public:
    explicit AsyncDebugger(QWidget *parent = 0);
    ~AsyncDebugger();

    struct bitbltData{
        HDC hdcDest;
        int nXDest;
        int nYDest;
        int nWidth;
        int nHeight;
        HDC hdcSrc;
        int nXSrc;
        int nYSrc;
        unsigned int dwRop;
    };


    int getState();
    void setState(int value);

    bitbltData getData();
    void setData(const bitbltData &value);

    bool skipCatching();

    bool getAboutToDelete();
    void setAboutToDelete(bool value);

    void setRunningText(const QString &txt);

protected:
    void paintEvent(QPaintEvent* e);
    void closeEvent(QCloseEvent* e);

private slots:
    void on_bntNextBitBlt_clicked();

private:
    QMutex locker;
    int state;
    bitbltData data;
    Ui::AsyncDebugger *ui;
    bool aboutToDelete;
};

#endif // ASYNCDEBUGGER_H
