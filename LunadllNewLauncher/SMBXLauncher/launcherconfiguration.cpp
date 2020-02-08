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
                    {"version-4", 0},
                    {"version-5", 0},
                    {"version-6", 0},
                    {"version-7", 0}
                }
            },
            {"updates", QJsonObject
                {
                    {"version-1-update", false},
                    {"version-2-update", false},
                    {"version-3-update", false},
                    {"version-4-update", false},
                    {"version-5-update", false},
                    {"version-6-update", false},
                    {"version-7-update", false}
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
        std::make_pair("version-4", &version4),
        std::make_pair("version-5", &version5),
        std::make_pair("version-6", &version6),
        std::make_pair("version-7", &version7)
    );
    settingsToParse.extractSafe("updates",
        std::make_pair("version-1-update", &version1u),
        std::make_pair("version-2-update", &version2u),
        std::make_pair("version-3-update", &version3u),
        std::make_pair("version-4-update", &version4u),
        std::make_pair("version-5-update", &version5u),
        std::make_pair("version-6-update", &version6u),
        std::make_pair("version-7-update", &version7u)
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

int LauncherConfiguration::hasHigherVersion(int ver1, int ver2, int ver3, int ver4, int ver5, int ver6, int ver7)
{
    if(ver1 > version1 && version1u) return 1;
    if(ver1 < version1) return 0;

    if(ver2 > version2 && version2u) return 2;
    if(ver2 < version2) return 0;

    if(ver3 > version3 && version3u) return 3;
    if(ver3 < version3) return 0;

    if((ver4 > version4 || (ver4 == 0 && version4 > 0)) && version4u) return 4;
    if(ver4 < version4 && ver4 > 0) return 0;

    if((ver5 > version5 || (ver5 == 0 && version5 > 0)) && version5u) return 5;
    if(ver5 < version5 && ver5 > 0) return 0;

    if(ver6 > version6 && version6u) return 6;
    if(ver6 < version6) return 0;

    if(ver7 > version7 && version7u) return 7;
    if(ver7 < version7) return 0;

    return 0;
}
