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
#include <QMessageBox>
#include <QDebug>


//#define dbg(text) QMessageBox::information(NULL, "Dbg", text);

SMBXConfig::SMBXConfig(QObject *parent) :
    QObject(parent),
    m_Autostart(new AutostartConfig()),
    m_Controls(new ControlConfig())
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
    wldFileFilter << "*.wld";
    QDir episodeDir(episodeDirPath);
    QFileInfo episodeDirFile(episodeDirPath);
    QMap<QString, QVariant> ret;
    WorldData worldData;
    FileFormats::CreateWorldData(worldData);

    foreach (QFileInfo fileInfo, episodeDir.entryInfoList(wldFileFilter, QDir::Files))
    {
        FileFormats::ReadSMBX64WldFileHeader(fileInfo.canonicalFilePath(), worldData);

        // Break upon first valid world file
        if (worldData.meta.ReadFileValid) break;
    }

    // If we didn't get valid data, return null
    if (!worldData.meta.ReadFileValid)
    {
        return QVariant();
    }

    ret.insert("directoryName", episodeDirFile.baseName());
    ret.insert("title", worldData.EpisodeTitle);
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

    foreach (QFileInfo fileInfo, worldsDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)) {
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

QVariantList SMBXConfig::getSaveInfo(const QString& directoryName)
{
    QDir episodeDir = QDir::current();

    if(!episodeDir.cd("worlds")){
        return QVariantList();
    }

    if(!episodeDir.cd(directoryName)){
        return QVariantList();
    }

    // For each possible savefile
    QVariantList ret;
    for (int i=1; i<=3; i++) {
        QString saveFilePath = episodeDir.canonicalPath() + "/save" + QString::number(i) + ".sav";
        GamesaveData data;
        QMap<QString, QVariant> map;
        FileFormats::ReadSMBX64SavFileF(saveFilePath, data);

        map.insert("isPresent", data.meta.ReadFileValid);
        if (data.meta.ReadFileValid) {
            map.insert("starCount", data.gottenStars.length());
            map.insert("gameCompleted", data.gameCompleted);
            map.insert("coinCount", data.coins);
        }
        ret << map;
    }
    return ret;
}

void SMBXConfig::deleteSaveSlot(const QString& directoryName, int slot)
{
    if (slot < 1 || slot > 3) return;

    QDir episodeDir = QDir::current();

    if(!episodeDir.cd("worlds")){
        return;
    }

    if(!episodeDir.cd(directoryName)){
        return;
    }

    QString saveFilePath = episodeDir.canonicalPath() + "/save" + QString::number(slot) + ".sav";
    QFile saveFile(saveFilePath);
    if (saveFile.exists()){
        saveFile.remove();
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

void SMBXConfig::loadEpisodeWebpage(const QString &file)
{
    emit loadEpisodeWebpageExecuted(file);
}
