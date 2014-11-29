#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnStartGame_clicked();

    void on_btnLvlEditor_clicked();

    void on_btnExit_clicked();

protected:
    void closeEvent(QCloseEvent *ev);

private:
    Ui::MainWindow *ui;
    int selType;
};

#endif // MAINWINDOW_H
