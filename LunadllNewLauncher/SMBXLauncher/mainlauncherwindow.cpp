#include "mainlauncherwindow.h"
#include "ui_mainlauncherwindow.h"
//#include "../../LunaLoader/LunaLoaderPatch.h"
#include "Utils/filesysutils.h"
#include "Utils/Network/networkutils.h"
#include "Utils/Network/qnetworkreplyexception.h"
#include "Utils/Network/qnetworkreplytimeoutexception.h"
#include "Utils/Common/qurlinvalidexception.h"
#include "Utils/Json/extendedqjsonreader.h"
#include "Utils/Json/qjsonfileopenexception.h"
#include "Utils/Json/qjsonparseexception.h"
#include "Utils/Json/qjsonurlvalidationexception.h"
#include "launchercustomwebpage.h"
#include "hybridlogger.h"

#include "devtoolsdialog.h"

#include <QtWebEngineWidgets/QtWebEngineWidgets>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDesktopWidget>

static DevToolsDialog* devDialogPtr = nullptr;

MainLauncherWindow::MainLauncherWindow(QWidget *parent) :
    QMainWindow(parent),
    m_smbxConfig(new SMBXConfig()),
    m_ApplyLunaLoaderPatch(false),
    m_jsBridgeAlreadInit(false),
    ui(new Ui::MainLauncherWindow),
    m_ControlPollTimer(this)
{
    ui->setupUi(this);
    this->setMinimumSize(640, 360);
    HybridLogger::init(ui->webLauncherPage);

    // 1. General WebEngine Settings
    QWebEngineSettings* globalSettings = QWebEngineSettings::globalSettings();
    globalSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    globalSettings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    ui->webLauncherPage->setPage(new LauncherCustomWebPage(ui->webLauncherPage));
    QWebEnginePage* page = ui->webLauncherPage->page();

    // Set up the development tools dialog
    if (devDialogPtr == nullptr)
    {
        devDialogPtr = new DevToolsDialog(this);
    }
    devDialogPtr->AssociateWithPage(page);

    // Enable inspect element
    page->action(QWebEnginePage::InspectElement)->setEnabled(true);
    connect(page->action(QWebEnginePage::InspectElement), &QAction::triggered, [this]() {
        if (devDialogPtr != nullptr)
        {
            devDialogPtr->show();
        }
    });


    // Only load the javascript bridge when the website has correctly loaded
    connect(ui->webLauncherPage->page(), &QWebEnginePage::loadFinished,
            [this](bool ok){
        if(ok)
        {
            this->loadJavascriptBridge();

            connect(&m_ControlPollTimer, &QTimer::timeout, this, &MainLauncherWindow::pollControlTimeout);
            m_ControlPollTimer.start(32);
        }
    });
}

MainLauncherWindow::~MainLauncherWindow()
{
    delete ui;
}

void MainLauncherWindow::pollControlTimeout()
{
    m_smbxConfig->pollControls();
}

void MainLauncherWindow::closeEvent(QCloseEvent *)
{
    if (devDialogPtr != nullptr)
    {
        devDialogPtr->close();
        delete devDialogPtr;
        devDialogPtr = nullptr;
    }
}

void MainLauncherWindow::loadJavascriptBridge()
{
    QWebEnginePage* currentPage = ui->webLauncherPage->page();
    qDebug() << "Init Javascript Bridge";


    // Get or create new web channel
    QWebChannel * channel = currentPage->webChannel();
    if(channel == nullptr)
        channel = new QWebChannel(currentPage);

    if(!m_jsBridgeAlreadInit) {
        qDebug() << "Placing Launcher object!";
        channel->registerObject(QString("Launcher"), m_smbxConfig.data());
        m_jsBridgeAlreadInit = true;
    }

    qDebug() << "Setting web channel!";
    currentPage->setWebChannel(channel);


    qDebug() << "Connecting QObject signal & slots";
    connect(m_smbxConfig.data(), &SMBXConfig::runSMBXExecuted, this, &MainLauncherWindow::runSMBX);
    connect(m_smbxConfig.data(), &SMBXConfig::runSMBXEditorExecuted, this, &MainLauncherWindow::runSMBXEditor);
    connect(m_smbxConfig.data(), &SMBXConfig::runPGEEditorExecuted, this, &MainLauncherWindow::runPGEEditor);
    connect(m_smbxConfig.data(), &SMBXConfig::loadEpisodeWebpageExecuted, this, &MainLauncherWindow::loadEpisodeWebpage);
    connect(m_smbxConfig.data(), &SMBXConfig::runSMBXLevelExecuted, this, &MainLauncherWindow::runSMBXLevel);
    connect(m_smbxConfig.data(), &SMBXConfig::setWindowHeaderExecuted, this, &MainLauncherWindow::setWindowHeader);

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
                "        if(Launcher === undefined){"
                "            console.warn('Launcher object failed to bind. Please contact Kevsoft!');"
                "        }"
                "        if(typeof onInitLauncher === 'function'){"
                "            console.log('Calling onInitLauncher callback');"
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
    ui->webLauncherPage->load(QUrl("qrc:///featurePage.html"));
}

void MainLauncherWindow::autoSize()
{
    QSize dw = QGuiApplication::primaryScreen()->availableSize();
    float baseSize = dw.height() * 0.6;
    if (baseSize > 1080) {
        baseSize = 1080;
    } else if (baseSize > 640) {
        baseSize = 640;
    }

    baseSize = baseSize / 9;

    if (dw.width()/16 < dw.height()/9) {
        baseSize = dw.width() * 0.6;

        if (baseSize > 1920) {
            baseSize = 1920;
        }
        if (baseSize < 800) {
            baseSize = 800;
        }
        baseSize = baseSize / 16;
    }

    m_initWidth = 0;
    m_initHeight = 0;
    this->resize(baseSize * 16, baseSize * 9);
}

void MainLauncherWindow::initSize()
{
    //Corrects maximum sizes to account for taskbars and the like - has to be done after the show()
    if(m_initWidth > 0 && m_initHeight > 0){

        QSize dw = QGuiApplication::primaryScreen()->availableSize();

        QSize frameSize = this->frameGeometry().size();
        QSize winSize = this->geometry().size();

        m_initWidth = std::min(m_initWidth, dw.width() - frameSize.width() + winSize.width());
        m_initHeight = std::min(m_initHeight, dw.height() - frameSize.height() + winSize.height());

        this->resize(m_initWidth, m_initHeight);
    }
}

void MainLauncherWindow::init(const QString &configName)
{
    qDebug() << "Loading launcher configuration " << configName;
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
    qDebug() << "Checking for updates...";
    checkForUpdates();

    QSettings settingFile(configName, QSettings::IniFormat);
    settingFile.setIniCodec("UTF-8");

    // Load launcher settings
    if(QFile::exists(configName)){
        m_smbxExe = settingFile.value("smbx-exe", "smbx.exe").toString();
        m_pgeExe = settingFile.value("pge-exe", "PGE/pge_editor.exe").toString();

        QString wintitle = settingFile.value("title", "SMBX Launcher").toString();
        m_autoWindowTitle = wintitle == "auto";
        if (m_autoWindowTitle) {
            wintitle = "SMBX Launcher";
        }
        setWindowTitle(wintitle);

        QString webpage = settingFile.value("episode-webpage", "").toString();
        if(webpage == ""){
            loadDefaultWebpage();
        }else{
            ui->webLauncherPage->load(QUrl::fromUserInput(webpage, QDir::currentPath(), QUrl::AssumeLocalFile));
        }

        QString resolution = settingFile.value("resolution", "auto").toString();
        if(resolution == "auto"){
            autoSize();
        }else{
            QStringList resVals = resolution.split('x');
            if (resVals.length() == 2){
                int w = resVals[0].toInt();
                int h = resVals[1].toInt();
                if(w > 0 && h > 0){
                    QSize dw = QGuiApplication::primaryScreen()->availableSize();
                    w = std::min(std::max(w, this->minimumWidth()), dw.width());
                    h = std::min(std::max(h, this->minimumHeight()), dw.height());
                    m_initWidth = w;
                    m_initHeight = h;
                    this->resize(w, h);
                }else{
                    autoSize();
                }
            }else{
                autoSize();
            }
        }

        QString iconFilePath = settingFile.value("icon", "").toString();
        if(!iconFilePath.isEmpty()){
            if(QFile(iconFilePath).exists()){
                ui->mainWindowWidget->setWindowIcon(QIcon(iconFilePath));
            }
        }
        m_ApplyLunaLoaderPatch = (settingFile.value("apply-lunaloader-patch", "false").toString() == "true");
    }else{
        autoSize();
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
    if(m_pgeExe.length() > 0)
    {
#ifdef _WIN32
        QProcess::startDetached(m_pgeExe);
#else
        if(m_pgeExe.endsWith(".exe"))
            QProcess::startDetached("wine", {m_pgeExe});
        else
            QProcess::startDetached(m_pgeExe);
#endif
        close();
    }
}

void MainLauncherWindow::setWindowHeader(const QString &title)
{
    if (m_autoWindowTitle) {
        setWindowTitle(title);
    }
}

void MainLauncherWindow::loadEpisodeWebpage(const QString &file)
{
    ui->webLauncherPage->setUrl(QUrl::fromUserInput(file, QDir::currentPath(), QUrl::AssumeLocalFile));
}

void MainLauncherWindow::runSMBXLevel(const QString& file)
{
    internalRunSMBX(m_smbxExe, {"--testLevel="+file});
    close();
}

void MainLauncherWindow::checkForUpdates()
{
    m_smbxConfig->m_hasUpdate = false;
    m_smbxConfig->m_hasInternetAccess = true;
    emit m_smbxConfig->UpdateVersionUpdated();
    emit m_smbxConfig->InternetAccessUpdated();
    if(!m_launcherSettings->hasValidUpdateSite()) {
        qWarning() << "Invalid update-check-website for launcher configuration";
        return;
    }

    try {
        if(!NetworkUtils::checkInternetConnection(4000))
            m_smbxConfig->m_hasInternetAccess = false;
             emit m_smbxConfig->InternetAccessUpdated();
            return;
    } catch (const QNetworkReplyException& ex) {
        m_smbxConfig->m_hasInternetAccess = false;
        emit m_smbxConfig->InternetAccessUpdated();
        qWarning() << "Failed to check internet connection: " << ex.errorString() << "\nSkipping update check...";
        return;
    }

    qDebug() << "Checking launcher updates...";
    try {
        try {
            ExtendedQJsonReader reader(m_launcherSettings->checkForUpdate());

            int verNum[7];
            reader.extractSafe("current-version",
                std::make_pair("version-1", &verNum[0]),
                std::make_pair("version-2", &verNum[1]),
                std::make_pair("version-3", &verNum[2]),
                std::make_pair("version-4", &verNum[3]),
                std::make_pair("version-5", &verNum[4]),
                std::make_pair("version-6", &verNum[5]),
                std::make_pair("version-7", &verNum[6])
            );
            int higher = m_launcherSettings->hasHigherVersion(verNum[0], verNum[1], verNum[2], verNum[3], verNum[4], verNum[5], verNum[6]);
            if(higher > 0){
                qDebug() << "Higher version, notify user...";
                QString updateMessage;
                QUrl updateUrlObj;
                QString updateVersion;

                reader.extractSafe("",
                    std::make_pair("update-message-" + QString::number(higher), &updateMessage),
                    std::make_pair("update-version-name", &updateVersion),
                    std::make_pair("update-url-page", &updateUrlObj)
                );

                m_smbxConfig->m_hasUpdate = true;
                m_smbxConfig->m_updateMessage = updateMessage;
                m_smbxConfig->m_updateLink = updateUrlObj;
                m_smbxConfig->m_updateVersion = updateVersion;
                m_smbxConfig->m_updateType = higher;
                //This message box is incredibly obnoxious, so let's use something neater instead...
                //QMessageBox::information(this, "New Update!", updateMessage);
                //QDesktopServices::openUrl(updateUrlObj);
            } else {
                qDebug() << "No new updates, skipping notification...";
            }
        } catch (const QJsonUrlValidationException& ex) {
            warnError(QString("Episode updater json - Invalid url for field: ") + ex.fieldName() +
                      "\nError url msg: " + ex.errorString() +
                      "\nUrl: " + ex.url());
        } catch (const QJsonValidationException& ex)  {
            switch(ex.errorType()){
            case QJsonValidationException::ValidationError::WrongType:
                warnError(QString("Episode updater json - wrong type for field:\n") + ex.fieldName());
                break;
            case QJsonValidationException::ValidationError::MissingType:
                warnError(QString("Episode updater json - missing field:\n") + ex.fieldName());
                break;
            default:
                warnError(QString("Episode updater json - unknown validation error:\n") + ex.fieldName());
                break;
            }
        } catch (const QJsonParseException& ex) {
            warnError(QString("Episode updater json - failed to parse settings.json:\n") + ex.getParseError().errorString());
        } catch (const QJsonFileOpenException&) {
            warnError("Episode updater json - failed to load config, using default!");
        } catch (const QUrlInvalidException& ex) {
            warnError(QString("Episode updater json - invalid url for updater json: ") + ex.errorString() + "\nUrl: " + ex.url());
        } catch (const QNetworkReplyException& ex) {
            // Hide this error, and throw to the custom error message
            qWarning() << "Episode updater json - network reply exception: " << ex.errorString();
            throw;
        } catch (const QNetworkReplyTimeoutException&) {
            qWarning() << "Episode updater json - timeout when trying to access ";
            throw;
        }
    } catch(...) {
        qWarning() << m_launcherSettings->getErrConnectionMsg();

        /*
        QUrl urlOfErrorPage(m_launcherSettings->getErrConnectionUrl());
        if(urlOfErrorPage.isValid()){
            QDesktopServices::openUrl(urlOfErrorPage);
        }
        */
    }

    emit m_smbxConfig->UpdateVersionUpdated();
}


void MainLauncherWindow::warnError(const QString &msg)
{
    QMessageBox::warning(this, "Error", msg);
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
        QString loader = qApp->applicationDirPath() + "/LunaLoader.exe";
        runArgs.push_front(smbxExeFile);

        QProcess::startDetached(loader, runArgs);

        /*
        QString argString;
        for (int i=0; i<runArgs.length(); i++) {
            if (i > 0) {
                argString += " ";
            }
            argString += runArgs[i];
        }
        LunaLoaderRun(smbxExeFile.toStdWString().c_str(), argString.toStdWString().c_str());
        */
    }
    else
    {
#ifdef _WIN32
        QProcess::startDetached(smbxExeFile, runArgs);
#else
        runArgs.push_front(smbxExeFile);
        QProcess::startDetached("wine", runArgs);
#endif
    }
}
