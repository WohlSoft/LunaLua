#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "setting.h"


#include <QCloseEvent>
#include <Windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    selType(0)
{
    ui->setupUi(this);
    QList<QToolBar *> toolbars = findChildren<QToolBar *>();
    for(int i = 0; i < toolbars.size(); ++i){
        toolbars[i]->hide();
    }
    QList<QStatusBar *> statusbars = findChildren<QStatusBar *>();
    for(int i = 0; i < statusbars.size(); ++i){
        statusbars[i]->hide();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnStartGame_clicked()
{
    selType = 1;
    close();
}

void MainWindow::on_btnLvlEditor_clicked()
{
    selType = 2;
    close();
}

void MainWindow::on_btnExit_clicked()
{
    selType = 0;
    close();
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    Settings->result = selType;
    Settings->disableFrameskip = ui->chkDisableFrameskip->isChecked();
    Settings->NoSound = ui->chkDisableSound->isChecked();
    ev->accept();
}
