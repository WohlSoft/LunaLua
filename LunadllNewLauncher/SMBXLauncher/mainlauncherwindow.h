#ifndef MAINLAUNCHERWINDOW_H
#define MAINLAUNCHERWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QProcess>
#include <QUrl>
#include <QTimer>

#include "ExposedObjects/smbxconfig.h"
#include "launcherconfiguration.h"

namespace Ui {
class MainLauncherWindow;
}

class MainLauncherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainLauncherWindow(QWidget *parent = nullptr);
    ~MainLauncherWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    //Webkit Stuff
    void loadJavascriptBridge();
    void loadDefaultWebpage();
    void autoSize();
    void init(const QString& configName);
    void initSize();

    void runSMBX();
    void runSMBXEditor();
    void runPGEEditor();
    void setWindowHeader(const QString& title);
    void loadEpisodeWebpage(const QString& file);
    void runSMBXLevel(const QString& file);

    //General stuff
    void checkForUpdates();
    void pollControlTimeout();

    //Error handlers
    void warnError(const QString& msg);
private:
    void internalRunSMBX(const QString& smbxExeFile, const QList<QString>& args);

    QScopedPointer<SMBXConfig> m_smbxConfig;
    QScopedPointer<LauncherConfiguration> m_launcherSettings;
    QString m_smbxExe;
    QString m_editorExe;
    QString m_editorExe32bit;
    QString m_editorBootstrap32;
    bool m_ApplyLunaLoaderPatch;
    bool m_jsBridgeAlreadInit;
    int m_initWidth;
    int m_initHeight;
    bool m_autoWindowTitle;
    Ui::MainLauncherWindow *ui;
    QTimer m_ControlPollTimer;
};

#endif // MAINLAUNCHERWINDOW_H
