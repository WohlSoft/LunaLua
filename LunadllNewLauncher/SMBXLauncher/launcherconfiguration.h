#ifndef LAUNCHERSETTINGS_H
#define LAUNCHERSETTINGS_H

#include <QJsonDocument>
#include <QPair>
#include <functional>

#include "Utils/Json/extendedqjsonreader.h"

class LauncherConfiguration
{
    QString updateCheckWebsite;
    QString errConnectionMsg;
    QString errConnectionUrl;
    int version1;
    int version2;
    int version3;
    int version4;
    bool version1u;
    bool version2u;
    bool version3u;
    bool version4u;
public:
    static QJsonDocument generateDefault();

    LauncherConfiguration();
    LauncherConfiguration(const QJsonDocument &settingsToParse);

    void setConfigurationAndValidate(ExtendedQJsonReader& settingsToParse);

    int hasHigherVersion(int ver1, int ver2, int ver3, int ver4);
    ExtendedQJsonReader checkForUpdate();

    bool hasValidUpdateSite() const;
    QString getErrConnectionMsg() const;
    QString getErrConnectionUrl() const;
};

#endif // LAUNCHERSETTINGS_H
