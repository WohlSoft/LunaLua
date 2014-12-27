#include "asyncdebugger.h"
#include "ui_asyncdebugger.h"
#include <windows.h>
#include <QPainter>


AsyncDebugger::AsyncDebugger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AsyncDebugger)
{
    ui->setupUi(this);
    bitbltData d;
    d.nXDest = -1;
    setData(d);
}

AsyncDebugger::~AsyncDebugger()
{
    delete ui;
}
int AsyncDebugger::getState()
{
    int retState = 0;
    locker.lock();
    retState = state;
    locker.unlock();
    return retState;
}

void AsyncDebugger::setState(int value)
{
    locker.lock();
    state = value;
    locker.unlock();
}
AsyncDebugger::bitbltData AsyncDebugger::getData()
{
    bitbltData retData;
    locker.lock();
    retData = data;
    locker.unlock();
    return retData;
}

void AsyncDebugger::setData(const bitbltData &value)
{
    locker.lock();
    data = value;
    locker.unlock();
}

bool AsyncDebugger::skipCatching()
{
    return ui->chkBitBlt->isChecked();
}

void AsyncDebugger::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QBrush brush(QColor(255,255,255));
    painter.fillRect(e->rect(), brush);
    bitbltData d = getData();
    HDC pDC = GetDC((HWND)this->winId());
    if(d.nXDest != -1){
        BitBlt(pDC, 1, 1, d.nWidth, d.nHeight, d.hdcSrc, d.nXSrc, d.nYSrc, d.dwRop);
    }
    ReleaseDC((HWND)this->winId(), pDC);
}

void AsyncDebugger::closeEvent(QCloseEvent *e)
{
    setAboutToDelete(true);
    this->deleteLater();
    e->accept();
}

void AsyncDebugger::on_bntNextBitBlt_clicked()
{
    setState(1);
}
bool AsyncDebugger::getAboutToDelete()
{
    bool retAboutToDelete;
    locker.lock();
    retAboutToDelete = aboutToDelete;
    locker.unlock();
    return retAboutToDelete;
}

void AsyncDebugger::setAboutToDelete(bool value)
{
    locker.lock();
    aboutToDelete = value;
    locker.unlock();
}

void AsyncDebugger::setRunningText(const QString &txt)
{
    ui->labelRun->setText(txt);
}

