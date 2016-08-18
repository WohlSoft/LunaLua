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
public:
    static QJsonDocument generateDefault();

    enum UpdateCheckerErrCodes {
        UERR_NO_ERR,
        UERR_NO_URL,
        UERR_INVALID_URL,
        UERR_CONNECTION_FAILED,
        UERR_NOT_FOUND,
        UERR_INVALID_JSON
    };

    LauncherConfiguration();
    LauncherConfiguration(const QJsonDocument &settingsToParse);

    void setConfigurationAndValidate(ExtendedQJsonReader& settingsToParse);

    bool hasHigherVersion(int ver1, int ver2, int ver3, int ver4);
    ExtendedQJsonReader checkForUpdate();

    bool hasValidUpdateSite() const;
    QString getErrConnectionMsg() const;
    QString getErrConnectionUrl() const;
};

#endif // LAUNCHERSETTINGS_H
