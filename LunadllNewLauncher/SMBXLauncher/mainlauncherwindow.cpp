#include "mainlauncherwindow.h"
#include "ui_mainlauncherwindow.h"

#include <QtWebKit/QtWebKit>
#include <QWebFrame>
MainLauncherWindow::MainLauncherWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainLauncherWindow)
{
    ui->setupUi(this);

    connect(ui->webLauncherPage->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavascriptObject()));
}

MainLauncherWindow::~MainLauncherWindow()
{
    delete ui;
}

void MainLauncherWindow::addJavascriptObject()
{
    m_smbxConfig.reset(new SMBXConfig());
    ui->webLauncherPage->page()->mainFrame()->addToJavaScriptWindowObject("Launcher", m_smbxConfig.data());

    connect(m_smbxConfig.data(), SIGNAL(runSMBX()), this, SLOT(runSMBX()));
    connect(m_smbxConfig.data(), SIGNAL(runSMBXEditor()), this, SLOT(runSMBXEditor()));
    connect(m_smbxConfig.data(), SIGNAL(loadEpisodeWebpage(QString)), this, SLOT(loadEpisodeWebpage(QString)));
}

void MainLauncherWindow::loadDefaultWebpage()
{
    ui->webLauncherPage->load(QUrl("qrc:///emptyPage.html"));
}

void MainLauncherWindow::loadConfig(const QString &configName)
{
    QSettings settingFile(configName, QSettings::IniFormat);
    settingFile.setIniCodec("UTF-8");

    if(QFile::exists(configName)){
        m_smbxExe = settingFile.value("smbx-exe", "smbx.exe").toString();
        setWindowTitle(settingFile.value("title", "SMBX Launcher").toString());
        QString webpage = settingFile.value("episode-webpage", "").toString();
        if(webpage == ""){
            loadDefaultWebpage();
        }else{
            ui->webLauncherPage->load(QUrl::fromUserInput(webpage, QDir::currentPath(), QUrl::AssumeLocalFile));
        }

        QString iconFilePath = settingFile.value("icon", "").toString();
        if(!iconFilePath.isEmpty()){
            if(QFile(iconFilePath).exists()){
                ui->mainWindowWidget->setWindowIcon(QIcon(iconFilePath));
            }
        }
    }else{
        m_smbxExe = "smbx.exe";
        ui->mainWindowWidget->setWindowTitle("SMBX Launcher");
        loadDefaultWebpage();
    }
}

void MainLauncherWindow::runSMBX()
{
    writeLunaConfig();
    QList<QString> args;
    args << "--game";
    internalRunSMBX(m_smbxExe, args);
    close();
}

void MainLauncherWindow::runSMBXEditor()
{
    writeLunaConfig();
    QList<QString> args;
    args << "--leveleditor";
    internalRunSMBX(m_smbxExe, args);
    close();
}

void MainLauncherWindow::loadEpisodeWebpage(const QString &file)
{
    ui->webLauncherPage->setUrl(QUrl::fromUserInput(file, QDir::currentPath(), QUrl::AssumeLocalFile));
}

void MainLauncherWindow::writeLunaConfig()
{
    AutostartConfig& config = *m_smbxConfig->Autostart();
    if(config.useAutostart()){
        {
            QFile autostartFile("autostart.ini");
            if(autostartFile.exists()){
                autostartFile.remove();
            }
        }
        QSettings autostartINI("autostart.ini", QSettings::IniFormat);
        autostartINI.beginGroup("autostart");
        autostartINI.setValue("do-autostart", config.useAutostart());
        autostartINI.setValue("episode-name", config.episodeName());
        autostartINI.setValue("singleplayer", config.singleplayer());
        autostartINI.setValue("character-player1", config.character1());
        autostartINI.setValue("character-player2", config.character2());
        autostartINI.setValue("save-slot", config.saveSlot());
        autostartINI.endGroup();

    }
}

void MainLauncherWindow::internalRunSMBX(const QString &smbxExeFile, const QList<QString> &args)
{
    QList<QString> runArgs(args);
    runArgs << "--patch";

    QProcess::startDetached(smbxExeFile, runArgs);
}
