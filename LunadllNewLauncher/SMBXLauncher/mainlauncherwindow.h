#ifndef MAINLAUNCHERWINDOW_H
#define MAINLAUNCHERWINDOW_H

#include <QMainWindow>
#include <smbxconfig.h>
#include <QScopedPointer>
#include <QProcess>

namespace Ui {
class MainLauncherWindow;
}

class MainLauncherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainLauncherWindow(QWidget *parent = 0);
    ~MainLauncherWindow();

public slots:
    //Webkit Stuff
    void addJavascriptObject();
    void loadDefaultWebpage();
    void loadConfig(const QString& configName);

    void runSMBX();
    void runSMBXEditor();
    void loadEpisodeWebpage(const QString& file);

private:
    void writeLunaConfig();
    void internalRunSMBX(const QString& smbxExeFile, const QList<QString>& args);

    QScopedPointer<SMBXConfig> m_smbxConfig;
    QString m_smbxExe;
    Ui::MainLauncherWindow *ui;
};

#endif // MAINLAUNCHERWINDOW_H
