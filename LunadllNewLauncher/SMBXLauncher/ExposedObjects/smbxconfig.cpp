#include "smbxconfig.h"
#include "PGE_File_Formats/file_formats.h"
#include "../Utils/networkjsonutils.h"
#include "../Utils/Json/extendedqjsonreader.h"
#include "../Utils/Json/qjsonfileopenexception.h"
#include "../Utils/Json/qjsonparseexception.h"
#include "../Utils/Network/qnetworkreplyexception.h"
#include "../Utils/Network/qnetworkreplytimeoutexception.h"
#include "../Utils/Network/qnetworkrequestexception.h"

#include <QDir>
#include <QJsonDocument>
#include <QtDebug>
#include <QApplication>
#include <QMessageBox>
#include <regex>
#include <QFileDialog>
#include <QDesktopServices>


//#define dbg(text) QMessageBox::information(NULL, "Dbg", text);

SMBXConfig::SMBXConfig(QObject *parent) :
    QObject(parent),
    m_Autostart(new AutostartConfig()),
    m_Controls(new ControlConfig()),
    m_ControllerManager()
{
    emit AutostartUpdated();
    emit ControlsUpdated();
}

QVariant SMBXConfig::getJSONForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName)
{
    // {SMBX-Root}/worlds
    QDir episodeDir(episodeDirPath);

    // {SMBX-Root}/worlds/{episode-folder}
    if (jsonSubDirPerEpisode.length() > 0) {
        if(!episodeDir.cd(jsonSubDirPerEpisode)){
            qWarning() << "Couldn't not enter episode folder \"" << jsonSubDirPerEpisode<< "\" at " << episodeDir;
            return QVariant();
        }
    }

    // {SMBX-Root}/worlds/{episode-folder}/{relative-path-to-json}
    if(!episodeDir.exists(jsonFileName)){
        qWarning() << "Couldn't find \"" << jsonFileName << "\" at " << episodeDir;
        return QVariant();
    }

    QString fullPath = episodeDir.canonicalPath() + "/" + jsonFileName;
    try{
        // {SMBX-Root}/worlds/{episode-folder} + /{relative-path-to-json}
        QFile fileToRead(fullPath);
        ExtendedQJsonReader reader(fileToRead);
        if(!reader.canConvertToMap()){
            qWarning() << "Invalid JSON format, expected json map for " << fullPath;
            return QVariant();
        }
        return reader.toMap();
    }catch(const QJsonFileOpenException&){
        qWarning() << "Could not open " << fullPath;
    }catch(const QJsonParseError& ex){
        qWarning() << "Json parse error: " << ex.errorString();
    }

    return QVariant();
}

QVariant SMBXConfig::getDataForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName)
{

    QStringList wldFileFilter;
    wldFileFilter << "*.wld" << "*.wldx";
    QDir episodeDir(episodeDirPath);
    QMap<QString, QVariant> ret;
    WorldData worldData;
    FileFormats::CreateWorldData(worldData);
    worldData.meta.ReadFileValid = false;

    QString wldPath;
    foreach (QFileInfo fileInfo, episodeDir.entryInfoList(wldFileFilter, QDir::Files))
    {
        QString thisPath = fileInfo.canonicalFilePath();
        if (fileInfo.suffix().toLower() == "wldx") {
            FileFormats::ReadExtendedWldFileHeader(thisPath, worldData);
        } else {
            FileFormats::ReadSMBX64WldFileHeader(thisPath, worldData);
        }

        // Break upon first valid world file
        if (worldData.meta.ReadFileValid)
        {
            wldPath = thisPath;
            break;
        }
    }

    // If we didn't get valid data, return null
    if (!worldData.meta.ReadFileValid)
    {
        return QVariant();
    }

    ret.insert("directoryName", episodeDir.dirName());
    ret.insert("wldpath", wldPath);
    ret.insert("title", worldData.EpisodeTitle);
    ret.insert("__rawtitle", worldData.EpisodeTitle);
    ret.insert("credits", worldData.authors);
    ret.insert("stars", worldData.stars);
    ret.insert("isHubWorld", worldData.HubStyledWorld);

    // Make a list of the allowed characters
    QVariantList allowedChars;
    if (!worldData.nocharacter1) allowedChars << 1;
    if (!worldData.nocharacter2) allowedChars << 2;
    if (!worldData.nocharacter3) allowedChars << 3;
    if (!worldData.nocharacter4) allowedChars << 4;
    if (!worldData.nocharacter5) allowedChars << 5;
    ret.insert("allowedCharacters", allowedChars);

    // Make note of default character names, can be overridden by episode JSON file
    QStringList charNames;
    charNames << "Mario" << "Luigi" << "Peach" << "Toad" << "Link";
    ret.insert("characterNames", charNames);

    // Merge JSON Data
    QVariant jsonData = getJSONForEpisode(episodeDirPath, jsonSubDirPerEpisode, jsonFileName);
    if (jsonData.type() == QVariant::Map)
    {
        QMap<QString, QVariant> jsonMap = jsonData.toMap();
        QMapIterator<QString, QVariant> i(jsonMap);
        while (i.hasNext())
        {
            i.next();
            ret.insert(i.key(), i.value());
        }
    }

    return ret;
}

QVariantList SMBXConfig::getEpisodeInfo(const QString& jsonSubDirPerEpisode, const QString& jsonFileName)
{
    QVariantList episodesInfo;
    QDir worldsDir = QDir::current();

    if(!worldsDir.cd("worlds")){
        return QVariantList();
    }

    foreach (QFileInfo fileInfo, worldsDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase)) {
        if(!fileInfo.isDir()){
            continue;
        }

        QVariant episodeData = getDataForEpisode(fileInfo.canonicalFilePath(), jsonSubDirPerEpisode, jsonFileName);
        if(episodeData.type() == QVariant::Map){
            episodesInfo << episodeData;
        }
    }
    return episodesInfo;
}

QVariant SMBXConfig::checkEpisodeUpdate(const QString& directoryName, const QString& jsonSubDirPerEpisode, const QString& jsonFileName)
{
    QDir cursorDir = QDir::current();
    qDebug() << "Check episode update for " << directoryName;

    if(!cursorDir.cd("worlds")){
        qWarning() << "Did not find worlds folder!";
        return QVariant();
    }

    if(!cursorDir.cd(directoryName)){
        return QVariant();
    }

    QVariant episodeData = getDataForEpisode(cursorDir.canonicalPath(), jsonSubDirPerEpisode, jsonFileName);
    if(episodeData.type() != QVariant::Map){
        qWarning() << "Invalid episode data type (expected map): " << episodeData.typeName();
        return QVariant();
    }
    QMap<QString, QVariant> episode = episodeData.toMap();

    auto it = episode.constFind("update-check-website");
    if ((it == episode.constEnd()) || (it.value().type() != QVariant::String)){
        qWarning() << "No update-check-website field";
        return QVariant();
    }
    QString updateWebsite = it.value().toString();
    try{
        return NetworkJsonUtils::getJSON(updateWebsite).toMap();
    }catch(const QJsonParseException& ex){
        qWarning() << "Failed to check episode update with parse exception: " << ex.getParseError().errorString();
    }catch(const QNetworkReplyException& ex){
        qWarning() << "Failed to check for episode update: " << ex.errorString();
    }catch(const QNetworkReplyTimeoutException&){
        qWarning() << "Failed to check for episode update: Timeout";
    }
    return QVariant();
}

bool SMBXConfig::sortSaveSlots(const QVariant slot1, const QVariant slot2)
{
    return slot1.toMap().value("id").toInt() < slot2.toMap().value("id").toInt();
}

QVariantList SMBXConfig::readSavesInPath(const QString& episodeDirPath, QList<int> *foundSlots) {
    QVariantList ret;
    QDir directory(episodeDirPath);
    QStringList files = directory.entryList(QStringList() << "*.sav" << "*.SAV" << "*.savx" << ".SAVX", QDir::Files);
    foreach(QString filename, files) {
        if (filename.toLower() != "save0.sav" && filename.toLower() != "save0.savx") {
            std::regex rgx("^save(-?[0-9]+)\\.sav[x]*$", std::regex_constants::icase);

            std::smatch match;
            const std::string fname = filename.toUtf8().constData();
            int index = 0;

            if (std::regex_search(fname.begin(), fname.end(), match, rgx)) {
                try {
                    index = std::stoi(match[1]);
                } catch (std::invalid_argument const &e) {
                    Q_UNUSED(e)
                    index = 0;
                } catch (std::out_of_range const &e) {
                    Q_UNUSED(e)
                    index = 0;
                }
            }

            if (foundSlots->contains(index)) {
                continue;
            }

            foundSlots->append(index);

            if (index != 0 && index <= 32767 && index >= -32768) {
                GamesaveData data;
                QMap<QString, QVariant> map;
                double progress = 0;
                QString savefilename = "";
                if (filename.endsWith(".savx")) {
                    FileFormats::ReadExtendedSaveFileF(episodeDirPath + "/" + filename, data);
                    // try reading out SaveData string
                    for (int i = 0; i < data.userData.store.size(); i++) {
                        auto item = data.userData.store[i];
                        if (item.location == saveUserData::DataLocation::DATA_GLOBAL && item.name == "LuaSaveData") {
                            for (int j = 0; j < item.data.size(); j++) {
                                auto entry = item.data[j];
                                if (entry.key == "chunk") {
                                    QString chunk = entry.value;
                                    QTextStream in(&chunk);
                                    // chunk is the save data
                                    QString line("");
                                    std::regex prog("^\\s*\\[\\s*\"__progress\"\\s*\\]\\s*=\\s*(.*)\\s*,?\\s*$");
                                    std::regex savename("^\\s*\\[\\s*\"__savefilename\"\\s*\\]\\s*=\\s*\"(.*)\"\\s*,?\\s*$");

                                    int count = 0;

                                    while (!in.atEnd() && QString::compare(line, "},") != 0 && QString::compare(line, "}") != 0) {
                                        line = in.readLine();
                                        const std::string ln = line.toUtf8().constData();
                                        if (std::regex_search(ln.begin(), ln.end(), match, prog)) {
                                            try {
                                                progress = std::stod(match[1]);
                                            } catch (std::invalid_argument const &e) {
                                                Q_UNUSED(e)
                                                progress = 0;
                                            } catch (std::out_of_range const &e) {
                                                Q_UNUSED(e)
                                                progress = 0;
                                            }
                                            count++;
                                            if (count >= 2) {
                                                break;
                                            }
                                        } else if (std::regex_search(ln.begin(), ln.end(), match, savename)) {
                                            savefilename = QString::fromStdString(match[1]);

                                            count++;
                                            if (count >= 2) {
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }
                } else {
                    FileFormats::ReadSMBX64SavFileF(episodeDirPath + "/" + filename, data);
                    QFile ext(episodeDirPath + "/save" + QString::number(index) + "-ext.dat");
                    if (ext.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&ext);
                        in.setCodec("UTF-8");

                        QString line("");
                        std::regex prog("^\\s*\\[\\s*\"__progress\"\\s*\\]\\s*=\\s*(.*)\\s*,?\\s*$");
                        std::regex savename("^\\s*\\[\\s*\"__savefilename\"\\s*\\]\\s*=\\s*\"(.*)\"\\s*,?\\s*$");

                        int count = 0;

                        while (!in.atEnd() && QString::compare(line, "},") != 0 && QString::compare(line, "}") != 0) {
                            line = in.readLine();
                            const std::string ln = line.toUtf8().constData();
                            if (std::regex_search(ln.begin(), ln.end(), match, prog)) {
                                try {
                                    progress = std::stod(match[1]);
                                } catch (std::invalid_argument const &e) {
                                    Q_UNUSED(e)
                                    progress = 0;
                                } catch (std::out_of_range const &e) {
                                    Q_UNUSED(e)
                                    progress = 0;
                                }
                                count++;
                                if (count >= 2) {
                                    break;
                                }
                            } else if (std::regex_search(ln.begin(), ln.end(), match, savename)) {
                                savefilename = QString::fromStdString(match[1]);

                                count++;
                                if (count >= 2) {
                                    break;
                                }
                            }
                        }

                        ext.close();
                    }
                }

                if (data.meta.ReadFileValid) {
                    map.insert("id", index);
                    map.insert("progress", progress);
                    map.insert("savefilename", savefilename);
                    map.insert("starCount", data.gottenStars.length());
                    map.insert("gameCompleted", data.gameCompleted);
                    map.insert("coinCount", data.coins);
                    ret << map;
                }
            }
        }
    }
    return ret;
}

QVariantList SMBXConfig::getSaveInfo(const QString& directoryName)
{
    QVariantList files;

    QDir episodeDir = QDir::current();
    QList<int> foundSlots;

    if(episodeDir.cd("saves") && episodeDir.cd(directoryName)){
        QVariantList savesFolderFiles = SMBXConfig::readSavesInPath(episodeDir.canonicalPath(), &foundSlots);
        files.append(savesFolderFiles);
    }

    episodeDir = QDir::current();

    if(episodeDir.cd("worlds") && episodeDir.cd(directoryName)){
        QVariantList worldsFolderFiles = SMBXConfig::readSavesInPath(episodeDir.canonicalPath(), &foundSlots);
        files.append(worldsFolderFiles);
    }

    qSort(files.begin(), files.end(), sortSaveSlots);
    return files;
}

void SMBXConfig::deleteSaveSlot(const QString& directoryName, int slot)
{
    if (slot != 0 && slot <= 32767 && slot >= -32768) {


        QMessageBox reply;
        reply.setWindowTitle("Confirm Save Slot Deletion");
        reply.setText("Are you sure you want to permanently delete this save slot?");
        reply.setStandardButtons(QMessageBox::Yes);
        reply.addButton(QMessageBox::Cancel);
        reply.setDefaultButton(QMessageBox::Cancel);

        if (reply.exec() == QMessageBox::Yes) {
            QDir episodeDir = QDir::current();
            QDir savesDir = QDir::current();
            if (episodeDir.cd("worlds") && episodeDir.cd(directoryName)) {
                QString saveFilePath = episodeDir.canonicalPath() + "/save" + QString::number(slot) + ".sav";
                QFile saveFile(saveFilePath);
                if (saveFile.exists()){
                    saveFile.remove();
                }
                QString saveFilePath2= episodeDir.canonicalPath() + "/save" + QString::number(slot) + ".savx";
                QFile saveFile2(saveFilePath2);
                if (saveFile2.exists()){
                    saveFile2.remove();
                }
                //Extended save files, likely temporary
                QString tmpSavePath = episodeDir.canonicalPath() + "/save" + QString::number(slot) + ".tmp";
                QFile tmpSaveFile(tmpSavePath);
                if (tmpSaveFile.exists()){
                    tmpSaveFile.remove();
                }
                QString extSavePath = episodeDir.canonicalPath() + "/save" + QString::number(slot) + "-ext.dat";
                QFile extSaveFile(extSavePath);
                if (extSaveFile.exists()){
                    extSaveFile.remove();
                }
            }
            if (savesDir.cd("saves") && savesDir.cd(directoryName)) {
                QString saveFilePath2= episodeDir.canonicalPath() + "/save" + QString::number(slot) + ".savx";
                QFile saveFile2(saveFilePath2);
                if (saveFile2.exists()){
                    saveFile2.remove();
                }
            }
        }
    }
}


void SMBXConfig::openLevelDialog()
{
    QFileDialog open;
    open.setFileMode(QFileDialog::FileMode::ExistingFile);
    open.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    open.setNameFilter(tr("SMBX Levels (*.lvl *.lvlx)"));
    open.setDirectory(open.directory().path() + "/worlds");
    if (open.exec()) {
        if (open.selectedFiles().length() > 0) {
            emit runSMBXLevelExecuted(open.selectedFiles().first());
        }
    }
}

void SMBXConfig::openUpdateWindow()
{
    if(m_hasUpdate) {
        QDesktopServices::openUrl(m_updateLink);
    }
}

void SMBXConfig::runSMBX()
{
    emit runSMBXExecuted();
}

void SMBXConfig::runSMBXEditor()
{
    emit runSMBXEditorExecuted();
}

void SMBXConfig::runPGEEditor()
{
    emit runPGEEditorExecuted();
}

void SMBXConfig::setWindowHeader(const QString &title)
{
    emit setWindowHeaderExecuted(title);
}

void SMBXConfig::loadEpisodeWebpage(const QString &file)
{
    emit loadEpisodeWebpageExecuted(file);
}

void SMBXConfig::pollControls()
{
    static bool ranInit = false;
    if (!ranInit)
    {
        m_ControllerManager.init();
        ranInit = true;
    }

    m_ControllerManager.pollInputs();

    for (auto it : m_ControllerManager.getPressQueue())
    {
        QString controllerName = QString::fromUtf8(m_ControllerManager.getControllerName(it.first).c_str());
        emit ControllerButtonPress(it.second, controllerName);
    }
    m_ControllerManager.clearPressQueue();
}
