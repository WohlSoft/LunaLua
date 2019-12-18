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
            },
            {"updates", QJsonObject
                {
                    {"version-1-update", false},
                    {"version-2-update", false},
                    {"version-3-update", false},
                    {"version-4-update", false}
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
    settingsToParse.extractSafe("updates",
        std::make_pair("version-1-update", &version1u),
        std::make_pair("version-2-update", &version2u),
        std::make_pair("version-3-update", &version3u),
        std::make_pair("version-4-update", &version4u)
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

int LauncherConfiguration::hasHigherVersion(int ver1, int ver2, int ver3, int ver4)
{
    if(ver1 > version1 && version1u) return 1;
    if(ver1 < version1) return 0;
    if(ver2 >= version2 && version2u && ver4 == 0 && ver4 != version4) return 2;
    if(ver2 >= version2 && version4u && ver4 != 0 && ver4 != version4) return 4;
    if(ver2 < version2) return 0;
    if(ver3 > version3 && version3u) return 3;
    if(ver3 < version3) return 0;
    if(ver4 > version4 && version4u) return 4;
    if(ver4 < version4) return 0;
    return 0;
}
