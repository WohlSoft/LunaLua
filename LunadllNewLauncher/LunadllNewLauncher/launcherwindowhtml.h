#ifndef LAUNCHERWINDOWHTML_H
#define LAUNCHERWINDOWHTML_H

#include <QWidget>
#include <QUrl>
#include "settingsobject.h"

namespace Ui {
class LauncherWindowHtml;
}

class LauncherWindowHtml : public QWidget
{
    Q_OBJECT

public:
    explicit LauncherWindowHtml(QWidget *parent = 0);
    ~LauncherWindowHtml();



public slots:
    void initObjects();
    void quitApp();
    void startGame();
    void startLevelEditor();
private slots:
    void linkClicked(QUrl url);
protected:
    void closeEvent(QCloseEvent *ev);
private:
    int selType;
    SettingsObject contentSetting;
    Ui::LauncherWindowHtml *ui;
};

#endif // LAUNCHERWINDOWHTML_H
