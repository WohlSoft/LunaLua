#include "launcherconfiguration.h"
#include <QJsonObject>

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include "Utils/Network/networkutils.h"
#include "Utils/networkjsonutils.h"

QString LauncherConfiguration::getErrConnectionMsg() const
{
    return errConnectionMsg;
}

QString LauncherConfiguration::getErrConnectionUrl() const
{
    return errConnectionUrl;
}
QJsonDocument LauncherConfiguration::generateDefault()
{
    return QJsonDocument(
    QJsonObject
        {
            {"game", QJsonObject
                {
                    {"update-check-website", "."},
                    {"update-error-website", "."},
                    {"update-error-message", "<Default error message>"},
                    {"version-1", 0},
                    {"version-2", 0},
                    {"version-3", 0},
                    {"version-4", 0}
                }
            }
        }
    );

}

LauncherConfiguration::LauncherConfiguration() : LauncherConfiguration(generateDefault())
{}

LauncherConfiguration::LauncherConfiguration(const QJsonDocument &settingsToParse)
{
    ExtendedQJsonReader reader(settingsToParse);
    setConfigurationAndValidate(reader);
}

void LauncherConfiguration::setConfigurationAndValidate(ExtendedQJsonReader& settingsToParse)
{
    settingsToParse.extractSafe("game",
        std::make_pair("update-check-website", &updateCheckWebsite),
        std::make_pair("update-error-website", &errConnectionUrl),
        std::make_pair("update-error-message", &errConnectionMsg),
        std::make_pair("version-1", &version1),
        std::make_pair("version-2", &version2),
        std::make_pair("version-3", &version3),
        std::make_pair("version-4", &version4)
    );
}

ExtendedQJsonReader LauncherConfiguration::checkForUpdate()
{
    return NetworkJsonUtils::getJSON(updateCheckWebsite);
}

bool LauncherConfiguration::hasValidUpdateSite() const
{
    return !(updateCheckWebsite.isEmpty() || updateCheckWebsite == ".");
}

bool LauncherConfiguration::hasHigherVersion(int ver1, int ver2, int ver3, int ver4)
{
    if(ver1 > version1) return true;
    if(ver1 < version1) return false;
    if(ver2 > version2) return true;
    if(ver2 < version2) return false;
    if(ver3 > version3) return true;
    if(ver3 < version3) return false;
    if(ver4 > version4) return true;
    if(ver4 < version4) return false;
    return false;
}
