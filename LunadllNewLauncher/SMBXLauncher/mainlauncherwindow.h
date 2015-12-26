#ifndef MAINLAUNCHERWINDOW_H
#define MAINLAUNCHERWINDOW_H

#include <QMainWindow>
#include <smbxconfig.h>
#include <QScopedPointer>
#include <QProcess>
#include <QUrl>
#include "launcherconfiguration.h"

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
    void runPGEEditor();
    void loadEpisodeWebpage(const QString& file);

    //General stuff
    void checkForUpdates();

    //Error handlers
    void jsonErrHandler(VALIDATE_ERROR errType, const QString& errChild);
private slots:
    void openURL(QUrl url);
private:
    void writeLunaConfig();
    void internalRunSMBX(const QString& smbxExeFile, const QList<QString>& args);

    QScopedPointer<SMBXConfig> m_smbxConfig;
    QScopedPointer<LauncherConfiguration> m_launcherSettings;
    QString m_smbxExe;
    QString m_pgeExe;
    bool m_ApplyLunaLoaderPatch;
    Ui::MainLauncherWindow *ui;
};

#endif // MAINLAUNCHERWINDOW_H
