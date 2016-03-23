#include "mainlauncherwindow.h"
#include "ui_mainlauncherwindow.h"
#include "../../LunaLoader/LunaLoaderPatch.h"

#include <QtWebKit/QtWebKit>
#include <QWebFrame>
#include <QMessageBox>
#include <QDesktopServices>


MainLauncherWindow::MainLauncherWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ApplyLunaLoaderPatch(false),
    ui(new Ui::MainLauncherWindow)
{
    ui->setupUi(this);

    connect(ui->webLauncherPage->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJavascriptObject()));
    connect(ui->webLauncherPage->page(), SIGNAL(linkClicked(QUrl)), this, SLOT(openURL(QUrl)));
    ui->webLauncherPage->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
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
    connect(m_smbxConfig.data(), SIGNAL(runPGEEditor()), this, SLOT(runPGEEditor()));
    connect(m_smbxConfig.data(), SIGNAL(loadEpisodeWebpage(QString)), this, SLOT(loadEpisodeWebpage(QString)));
}

void MainLauncherWindow::loadDefaultWebpage()
{
    ui->webLauncherPage->load(QUrl("qrc:///emptyPage.html"));
}

void MainLauncherWindow::loadConfig(const QString &configName)
{
    auto errFunc = [this](VALIDATE_ERROR errType, const QString& errChild){
        jsonErrHandler(errType, errChild);
    };
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
        QByteArray rawData = configurationJSON.readAll();

        QJsonParseError jsonErrCode;
        QJsonDocument jsonSettingsFile = QJsonDocument::fromJson(rawData, &jsonErrCode);
        if(jsonErrCode.error != QJsonParseError::NoError){
            QMessageBox::warning(this, "Error", QString("Failed to parse settings.json:\n") + jsonErrCode.errorString());
            m_launcherSettings.reset(new LauncherConfiguration(LauncherConfiguration::generateDefault()));
        }else{
            m_launcherSettings.reset(new LauncherConfiguration());
            m_launcherSettings->setConfigurationAndValidate(jsonSettingsFile, errFunc);
        }
    }else{
        QMessageBox::warning(this, "Error", "Failed to load config, using default!");
        m_launcherSettings.reset(new LauncherConfiguration(LauncherConfiguration::generateDefault()));
    }

    checkForUpdates();

    QSettings settingFile(configName, QSettings::IniFormat);
    settingFile.setIniCodec("UTF-8");


    if(QFile::exists(configName)){
        m_smbxExe = settingFile.value("smbx-exe", "smbx.exe").toString();
        m_pgeExe = settingFile.value("pge-exe", "PGE/pge_editor.exe").toString();
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
        m_ApplyLunaLoaderPatch = (settingFile.value("apply-lunaloader-patch", "false").toString() == "true");
    }else{
        m_pgeExe = "PGE/pge_editor.exe";
        ui->mainWindowWidget->setWindowTitle("SMBX Launcher");
        m_ApplyLunaLoaderPatch = false;
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
    // Don't need to write luna config for editor
    QList<QString> args;
    args << "--leveleditor";
    internalRunSMBX(m_smbxExe, args);
    close();
}

void MainLauncherWindow::runPGEEditor()
{
    if (m_pgeExe.length() > 0) {
        QProcess::startDetached(m_pgeExe);
        close();
    }
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
        auto errFunc = [this](VALIDATE_ERROR errType, const QString& errChild){
            jsonErrHandler(errType, errChild);
        };

        if(!output.isObject()){
            errFunc(VALIDATE_ERROR::VALIDATE_NO_CHILD, "<root>");
            return;
        }

        QJsonObject outputObj = output.object();
        if(!qJsonValidate<QJsonObject>(outputObj, "current-version", errFunc)) return;
        if(!qJsonValidate<QString>(outputObj, "update-message", errFunc)) return;
        if(!qJsonValidate<QString>(outputObj, "update-url-page", errFunc)) return;

        QJsonObject currentVersionObj = outputObj.value("current-version").toObject();
        if(!qJsonValidate<int>(currentVersionObj, "version-1", errFunc)) return;
        if(!qJsonValidate<int>(currentVersionObj, "version-2", errFunc)) return;
        if(!qJsonValidate<int>(currentVersionObj, "version-3", errFunc)) return;
        if(!qJsonValidate<int>(currentVersionObj, "version-4", errFunc)) return;


        if(m_launcherSettings->hasHigherVersion(currentVersionObj.value("version-1").toInt(),
                                                currentVersionObj.value("version-2").toInt(),
                                                currentVersionObj.value("version-3").toInt(),
                                                currentVersionObj.value("version-4").toInt())){
            QMessageBox::information(this, "New Update!", outputObj.value("update-message").toString());
            QUrl urlOfUpdatePage(outputObj.value("update-url-page").toString());
            if(urlOfUpdatePage.isValid()){
                QDesktopServices::openUrl(urlOfUpdatePage);
            }
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

void MainLauncherWindow::jsonErrHandler(VALIDATE_ERROR errType, const QString &errChild)
{
    if(errType == VALIDATE_ERROR::VALIDATE_NO_CHILD)
        QMessageBox::warning(this, "Error", QString("Invalid Update Server JSON: No \"") + errChild + "\" JSON value.");
    if(errType == VALIDATE_ERROR::VALIDATE_WRONG_TYPE)
        QMessageBox::warning(this, "Error", QString("Invalid Update Server JSON: Invalid \"") + errChild + "\" JSON value. (Wrong Type?)");
}

void MainLauncherWindow::openURL(QUrl url)
{
    QDesktopServices::openUrl(url);
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
        autostartINI.setValue("transient", true);
        autostartINI.endGroup();

    }
}

void MainLauncherWindow::internalRunSMBX(const QString &smbxExeFile, const QList<QString> &args)
{
    QList<QString> runArgs(args);
    runArgs << "--patch";

    if (m_ApplyLunaLoaderPatch) {
        // We're not handling quoting here... but all the arguments currently don't use spaces or quotes so...
        QString argString;
        for (int i=0; i<runArgs.length(); i++) {
            if (i > 0) {
                argString += " ";
            }
            argString += runArgs[i];
        }
        LunaLoaderRun(smbxExeFile.toStdWString().c_str(), argString.toStdWString().c_str());
    }
    else {
        QProcess::startDetached(smbxExeFile, runArgs);
    }
}
