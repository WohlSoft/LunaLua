#include "smbxconfig.h"

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



QVariantList SMBXConfig::getEpisodeInfo(const QString& subDirPerEpisode, const QString& jsonFileName)
{
    //dbg("Getting Episode Info");
    QVariantList episodesInfo;
    QDir worldsDir = QDir::current();
    if(!worldsDir.cd("worlds")){
        //dbg("Did not find worlds folder");
        return QVariantList();
    }

    foreach (QFileInfo fileInfo, worldsDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if(!fileInfo.isDir()){
            continue;
        }


        QDir absoluteDir(fileInfo.canonicalFilePath());

        if(!absoluteDir.cd(subDirPerEpisode)){
            //dbg("Couldn't find launcher folder \"" + subDirPerEpisode + "\" at " + absoluteDir.canonicalPath());
            continue;
        }

        if(!absoluteDir.exists(jsonFileName)){
            //dbg("Couldn't find \"" + jsonFileName + "\" at " + absoluteDir.canonicalPath());
            continue;
        }

        QFile episodeJSONInfoFile(absoluteDir.canonicalPath() + "/" + jsonFileName);
        if(!episodeJSONInfoFile.open(QIODevice::ReadOnly)){
            //dbg("Could not open json file at " + absoluteDir.canonicalPath());
            continue;
        }

        QJsonParseError possibleError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(episodeJSONInfoFile.readAll(), &possibleError);

        if(possibleError.error != QJsonParseError::NoError){
            //dbg("Couldn't parse json (json error): " + possibleError.errorString());
            continue;
        }

        QVariant jsonData = jsonDoc.toVariant().toMap();
        if(jsonData.type() != QVariant::Map){
            //dbg(QString("Invalid JSON Data Type, required map, got ") + jsonData.typeName());
            continue;
        }

        QVariantMap jsonDataMap = jsonData.toMap();
        QString webpageName = jsonDataMap.value("main-page", "index.html").toString();

        if(!absoluteDir.exists(webpageName)){
            continue;
        }

        //dbg("ADDED")
        episodesInfo << jsonData;
    }
    return episodesInfo;
}
