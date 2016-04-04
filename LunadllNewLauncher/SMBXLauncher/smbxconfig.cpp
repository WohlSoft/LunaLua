#include "smbxconfig.h"
#include "PGE_File_Formats/file_formats.h"
#include "launcherconfiguration.h"

#include <QDir>
#include <QJsonDocument>
#include <QtDebug>
#include <QMessageBox>

//#define dbg(text) QMessageBox::information(NULL, "Dbg", text);

SMBXConfig::SMBXConfig(QObject *parent) :
    QObject(parent)
{
    m_Autostart = new AutostartConfig();
    m_Controls = new ControlConfig();
}

SMBXConfig::~SMBXConfig()
{
    delete m_Autostart;
}

QVariant SMBXConfig::getJSONForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName)
{
    QDir episodeDir(episodeDirPath);

    if (jsonSubDirPerEpisode.length() > 0) {
        if(!episodeDir.cd(jsonSubDirPerEpisode)){
            //dbg("Couldn't find launcher folder \"" + subDirPerEpisode + "\" at " + absoluteDir.canonicalPath());
            return QVariant();
        }
    }

    if(!episodeDir.exists(jsonFileName)){
        //dbg("Couldn't find \"" + jsonFileName + "\" at " + absoluteDir.canonicalPath());
        return QVariant();
    }

    QFile episodeJSONInfoFile(episodeDir.canonicalPath() + "/" + jsonFileName);
    if(!episodeJSONInfoFile.open(QIODevice::ReadOnly)){
        //dbg("Could not open json file at " + absoluteDir.canonicalPath());
        return QVariant();
    }

    QJsonParseError possibleError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(episodeJSONInfoFile.readAll(), &possibleError);

    if(possibleError.error != QJsonParseError::NoError){
        //dbg("Couldn't parse json (json error): " + possibleError.errorString());
        return QVariant();
    }

    QVariant jsonData = jsonDoc.toVariant().toMap();
    if(jsonData.type() != QVariant::Map){
        //dbg(QString("Invalid JSON Data Type, required map, got ") + jsonData.typeName());
        return QVariant();
    }

    return jsonData;
}

QVariant SMBXConfig::getDataForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName)
{
    QStringList wldFileFilter;
    wldFileFilter << "*.wld";
    QDir episodeDir(episodeDirPath);
    QFileInfo episodeDirFile(episodeDirPath);
    QMap<QString, QVariant> ret;
    WorldData worldData;
    worldData.ReadFileValid = false;

    foreach (QFileInfo fileInfo, episodeDir.entryInfoList(wldFileFilter, QDir::Files))
    {
        // Break upon first valid world file
        if (FileFormats::ReadSMBX64WldFileHeader(fileInfo.canonicalFilePath(), worldData))
            break;
    }

    // If we didn't get valid data, return null
    if (!worldData.ReadFileValid)
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
    //dbg("Getting Episode Info");
    QVariantList episodesInfo;
    QDir worldsDir = QDir::current();

    if(!worldsDir.cd("worlds")){
        //dbg("Did not find worlds folder");
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

    if(!cursorDir.cd("worlds")){
        return QVariant();
    }

    if(!cursorDir.cd(directoryName)){
        return QVariant();
    }

    QVariant episodeData = getDataForEpisode(cursorDir.canonicalPath(), jsonSubDirPerEpisode, jsonFileName);
    if(episodeData.type() != QVariant::Map){
        return QVariant();
    }
    QMap<QString, QVariant> episode = episodeData.toMap();

    auto it = episode.constFind("update-check-website");
    if ((it == episode.constEnd()) || (it.value().type() != QVariant::String)){
        return QVariant();
    }
    QString updateWebsite = it.value().toString();

    QJsonDocument updateOut;
    LauncherConfiguration::UpdateCheckerErrCodes updateErr;
    QString updateErrDesc;
    if (!LauncherConfiguration::loadUpdateJson(updateWebsite, &updateOut, updateErr, updateErrDesc)) {
        return QVariant();
    }

    return updateOut.toVariant().toMap();
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
        QFile saveFile(saveFilePath);
        GamesaveData data;
        QMap<QString, QVariant> map;
        data.ReadFileValid = false;
        if(saveFile.open(QIODevice::ReadOnly)){
            QString savData = QString(saveFile.readAll());
            FileFormats::ReadSMBX64SavFileRaw(savData, saveFilePath, data);
            saveFile.close();
        }

        map.insert("isPresent", data.ReadFileValid);
        if (data.ReadFileValid) {
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
