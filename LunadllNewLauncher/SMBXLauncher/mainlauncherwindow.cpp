#include "mainlauncherwindow.h"
#include "ui_mainlauncherwindow.h"

#include <QtWebKit/QtWebKit>
#include <QWebFrame>
#include <QMessageBox>

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

    // FIXME: This is a fast hack written for Horikawa, however I would like to remove the old INI at the end anyway.
    // In addition I would like to put all launcher data in the "launcher" folder.

    // Check for the launcher config dir
    QDir launcherDir = QDir::current();
    if(!launcherDir.exists("launcher"))
        launcherDir.mkdir("launcher");

    // Check for the main configuration file
    QFile configurationJSON("launcher/settings.json");

    // If not exist then write the configuration file
    if(!configurationJSON.exists()){
        if(configurationJSON.open(QIODevice::WriteOnly)){
            configurationJSON.write(LauncherConfiguration::generateDefault().toJson());
            configurationJSON.close();
        }
    }

    // The errors are surpressed.
    if(configurationJSON.open(QIODevice::ReadOnly)){
        m_launcherSettings.reset(new LauncherConfiguration(QJsonDocument::fromJson(configurationJSON.readAll())));
    }else{
        QMessageBox::warning(this, "Error", "Failed to load config, using default!");
        m_launcherSettings.reset(new LauncherConfiguration(LauncherConfiguration::generateDefault()));
    }

    checkForUpdates();

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

void MainLauncherWindow::checkForUpdates()
{
    QJsonDocument output;
    LauncherConfiguration::UpdateCheckerErrCodes err;
    QString errDesc;
    if(m_launcherSettings->checkForUpdate(&output, err, errDesc)){
        try{
            if(!output.isObject())
                throw "Invalid Update Server JSON: Root is not object!";

            QJsonObject outputObj = output.object();
            if(!outputObj.contains("current-version"))
                throw "Invalid Update Server JSON: No \"current-version\" JSON object!";

            QJsonValue currentVersionVal = outputObj.value("current-version");
            if(!currentVersionVal.isObject())
                throw "Invalid Update Server JSON: Invalid \"current-version\" Node. Must be object!";

            QJsonObject currentVersionObj = currentVersionVal.toObject();
            if(!currentVersionObj.contains("version-1"))
                throw "Invalid Update Server JSON: No version-1 value!";
            if(!currentVersionObj.contains("version-2"))
                throw "Invalid Update Server JSON: No version-2 value!";
            if(!currentVersionObj.contains("version-3"))
                throw "Invalid Update Server JSON: No version-3 value!";
            if(!currentVersionObj.contains("version-4"))
                throw "Invalid Update Server JSON: No version-4 value!";

            QJsonValue version1Val = currentVersionObj.value("version-1");
            QJsonValue version2Val = currentVersionObj.value("version-2");
            QJsonValue version3Val = currentVersionObj.value("version-3");
            QJsonValue version4Val = currentVersionObj.value("version-4");
            if(!version1Val.isDouble())
                throw "Invalid Update Server JSON: Invalid \"version-1\" Node. Must be double/number!";
            if(!version2Val.isDouble())
                throw "Invalid Update Server JSON: Invalid \"version-2\" Node. Must be double/number!";
            if(!version3Val.isDouble())
                throw "Invalid Update Server JSON: Invalid \"version-3\" Node. Must be double/number!";
            if(!version4Val.isDouble())
                throw "Invalid Update Server JSON: Invalid \"version-4\" Node. Must be double/number!";

            if(!outputObj.contains("update-message"))
                throw "Invalid Update Server JSON: No \"update-message\" JSON value!";

            QJsonValue updateMessageVal = outputObj.value("update-message");
            if(!updateMessageVal.isString())
                throw "Invalid Update Server JSON: Invalid \"update-message\" Value. Must be string!";


            if(m_launcherSettings->hasHigherVersion(version1Val.toInt(), version2Val.toInt(), version3Val.toInt(), version4Val.toInt())){
                QMessageBox::information(this, "New Update!", updateMessageVal.toString());
            }
        }catch(const char* err){
            QMessageBox::warning(this, "Error", err);
        }


    }else{
        switch (err) {
        case LauncherConfiguration::UERR_CONNECTION_FAILED:
            QMessageBox::warning(this, "Error", QString("Failed while connecting to update server:\n") + errDesc);
            break;
        case LauncherConfiguration::UERR_INVALID_JSON:
            QMessageBox::warning(this, "Error", QString("Invalid update server JSON response:\n") + errDesc);
            break;
        case LauncherConfiguration::UERR_INVALID_URL:
            QMessageBox::warning(this, "Error", QString("Invalid update server URL:\n") + errDesc);
            break;
        default:
            break;
        }
    }
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
