#include "mainlauncherwindow.h"
#include "ui_mainlauncherwindow.h"
#include "../../LunaLoader/LunaLoaderPatch.h"
#include "Utils/networkutils.h"
#include "Utils/filesysutils.h"
#include "Utils/Json/extendedqjsonreader.h"
#include "Utils/Json/qjsonfileopenexception.h"
#include "Utils/Json/qjsonparseexception.h"

#include <QtWebEngineWidgets/QtWebEngineWidgets>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QMessageBox>
#include <QDesktopServices>


MainLauncherWindow::MainLauncherWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ApplyLunaLoaderPatch(false),
    ui(new Ui::MainLauncherWindow)
{
    ui->setupUi(this);

    ui->webLauncherPage->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    connect(ui->webLauncherPage->page(), &QWebEnginePage::loadFinished,
            [this](bool ok){
        if(ok)
        {
            this->loadJavascriptBridge();
        }
    });
}

MainLauncherWindow::~MainLauncherWindow()
{
    delete ui;
}

void MainLauncherWindow::loadJavascriptBridge()
{
    QWebEnginePage* currentPage = ui->webLauncherPage->page();
    qDebug() << "Init Javascript Bridge";


    // Get or create new web channel
    QWebChannel * channel = currentPage->webChannel();
    if(channel == nullptr)
        channel = new QWebChannel(currentPage);

    qDebug() << "Placing Launcher object!";
    if(m_smbxConfig)
        channel->deregisterObject(m_smbxConfig.data());
    m_smbxConfig.reset(new SMBXConfig());
    channel->registerObject(QString("Launcher"), m_smbxConfig.data());

    qDebug() << "Setting web channel!";
    currentPage->setWebChannel(nullptr); // This is a bit hackish, but without it the 'qt' global object would not be set
    currentPage->setWebChannel(channel);

    qDebug() << "Connecting QObject signal & slots";
    connect(m_smbxConfig.data(), &SMBXConfig::runSMBXExecuted, this, &MainLauncherWindow::runSMBX);
    connect(m_smbxConfig.data(), &SMBXConfig::runSMBXEditorExecuted, this, &MainLauncherWindow::runSMBXEditor);
    connect(m_smbxConfig.data(), &SMBXConfig::runPGEEditorExecuted, this, &MainLauncherWindow::runPGEEditor);
    connect(m_smbxConfig.data(), &SMBXConfig::loadEpisodeWebpageExecuted, this, &MainLauncherWindow::loadEpisodeWebpage);

    qDebug() << "Running init javascript!";
    currentPage->runJavaScript(
                "var head = document.getElementsByTagName('head')[0];"
                ""
                "var qWebchannelImporter = document.createElement('script');"
                "qWebchannelImporter.type = 'text/javascript';"
                "qWebchannelImporter.src = 'qrc:///qtwebchannel/qwebchannel.js';"
                ""
                "var callback = function(){"
                "    console.log('Script loaded!');"
                "    new QWebChannel(qt.webChannelTransport, function (channel) {"
                "        console.log('QWebChannel works!');"
                "        Launcher = channel.objects.Launcher;"
                "        if(typeof onInitLauncher === 'function'){"
                "            onInitLauncher(); "
                "        }"
                "    });"
                "};"
                "qWebchannelImporter.onload = callback;"
                ""
                "head.appendChild(qWebchannelImporter);"
    );
}

void MainLauncherWindow::loadDefaultWebpage()
{
    ui->webLauncherPage->load(QUrl("qrc:///emptyPage.html"));
}

void MainLauncherWindow::loadConfig(const QString &configName)
{
    // FIXME: This is a fast hack written for Horikawa, however I would like to remove the old INI at the end anyway.
    // In addition I would like to put all launcher data in the "launcher" folder.

    // Create launcher dir if not exist
    QDir::current().mkdir("launcher");

    // Check for the main configuration file
    QFile configurationJSON("launcher/settings.json");

    // If not exist then write the configuration file
    FilesysUtils::writeDefaultIfNotExist(configurationJSON, LauncherConfiguration::generateDefault().toJson());

    // Game specific settings
    try {
        try {
            ExtendedQJsonReader reader(configurationJSON);
            m_launcherSettings.reset(new LauncherConfiguration());
            m_launcherSettings->setConfigurationAndValidate(reader);
        } catch (const QJsonValidationException& ex)  {
            switch(ex.errorType()){
            case QJsonValidationException::ValidationError::WrongType:
                warnError(QString("Settings.json - wrong type for field:\n") + ex.fieldName());
                break;
            case QJsonValidationException::ValidationError::MissingType:
                warnError(QString("Settings.json - missing field:\n") + ex.fieldName());
                break;
            default:
                warnError(QString("Settings.json - unknown validation error:\n") + ex.fieldName());
                break;
            }
            throw;
        } catch (const QJsonParseException& ex) {
           warnError(QString("Settings.json - failed to parse settings.json:\n") + ex.getParseError().errorString());
            throw;
        } catch (const QJsonFileOpenException&) {
           warnError("Settings.json - failed to load config, using default!");
            throw;
        }
    } catch(...) {
        m_launcherSettings.reset(new LauncherConfiguration(LauncherConfiguration::generateDefault()));
    }


    // check for updates with the information from settings.json
    checkForUpdates();

    QSettings settingFile(configName, QSettings::IniFormat);
    settingFile.setIniCodec("UTF-8");

    // Load launcher settings
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
    internalRunSMBX(m_smbxExe, {"--game"});
    close();
}

void MainLauncherWindow::runSMBXEditor()
{
    // Don't need to write luna config for editor
    internalRunSMBX(m_smbxExe, {"--leveleditor"});
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
    if(!m_launcherSettings->hasValidUpdateSite())
        return;

    if(!NetworkUtils::checkInternetConnection(4000))
        return;


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
        case LauncherConfiguration::UERR_INVALID_JSON:
            QMessageBox::warning(this, "Error", QString("Invalid update server JSON response:\n") + errDesc);
            break;
        case LauncherConfiguration::UERR_INVALID_URL:
            QMessageBox::warning(this, "Error", QString("Invalid update server URL:\n") + errDesc);
            break;
        default:
            QString errMsg = m_launcherSettings->getErrConnectionMsg();
            if(errMsg != ""){
                QMessageBox::warning(this, "Error", m_launcherSettings->getErrConnectionMsg());
            }
            QUrl urlOfErrorPage(m_launcherSettings->getErrConnectionUrl());
            if(urlOfErrorPage.isValid()){
                QDesktopServices::openUrl(urlOfErrorPage);
            }
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

void MainLauncherWindow::warnError(const QString &msg)
{
    QMessageBox::warning(this, "Error", msg);
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
    QFileInfo exeFileInfo(smbxExeFile);
    if(!exeFileInfo.exists()){
        qWarning() << "SMBX file does not exist!";
        return;
    }

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
