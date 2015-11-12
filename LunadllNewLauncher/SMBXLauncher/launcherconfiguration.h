#ifndef LAUNCHERSETTINGS_H
#define LAUNCHERSETTINGS_H

#include <QJsonDocument>
#include <QPair>

class LauncherConfiguration
{
    QString updateCheckWebsite;
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
        UERR_INVALID_JSON
    };

    LauncherConfiguration(const QJsonDocument &settingsToParse);
    bool checkForUpdate(QJsonDocument *result, UpdateCheckerErrCodes &errCode, QString& errDescription);
    bool hasHigherVersion(int ver1, int ver2, int ver3, int ver4);

};

#endif // LAUNCHERSETTINGS_H
