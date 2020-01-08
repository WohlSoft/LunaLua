#ifndef MAINLAUNCHERWINDOW_H
#define MAINLAUNCHERWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QProcess>
#include <QUrl>

#include "ExposedObjects/smbxconfig.h"
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

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    //Webkit Stuff
    void loadJavascriptBridge();
    void loadDefaultWebpage();
    void init(const QString& configName);

    void runSMBX();
    void runSMBXEditor();
    void runPGEEditor();
    void loadEpisodeWebpage(const QString& file);
    void runSMBXLevel(const QString& file);

    //General stuff
    void checkForUpdates();

    //Error handlers
    void warnError(const QString& msg);
private:
    void writeLunaConfig();
    void internalRunSMBX(const QString& smbxExeFile, const QList<QString>& args);

    QScopedPointer<SMBXConfig> m_smbxConfig;
    QScopedPointer<LauncherConfiguration> m_launcherSettings;
    QString m_smbxExe;
    QString m_pgeExe;
    bool m_ApplyLunaLoaderPatch;
    bool m_jsBridgeAlreadInit;
    Ui::MainLauncherWindow *ui;
};

#endif // MAINLAUNCHERWINDOW_H
