#include "launcherconfiguration.h"
#include <QJsonObject>

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>
#include "qjsonutil.h"
#include "Utils/networkutils.h"


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
    QJsonObject mainObject = settingsToParse.object();
    QJsonObject gameValue = mainObject.value("game").toObject();
    updateCheckWebsite = gameValue.value("update-check-website").toString(".");
    errConnectionUrl = gameValue.value("update-error-website").toString(".");
    errConnectionMsg = gameValue.value("update-error-message").toString("");
    version1 = gameValue.value("version-1").toInt(0);
    version2 = gameValue.value("version-2").toInt(0);
    version3 = gameValue.value("version-3").toInt(0);
    version4 = gameValue.value("version-4").toInt(0);
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

bool LauncherConfiguration::checkForUpdate(QJsonDocument *result, UpdateCheckerErrCodes &errCode, QString& errDescription)
{
    return loadUpdateJson(updateCheckWebsite, result, errCode, errDescription);
}

bool LauncherConfiguration::loadUpdateJson(const QString& checkWebsite, QJsonDocument *result, UpdateCheckerErrCodes& errCode, QString& errDescription)
{
    errDescription = "";
    if(checkWebsite.isEmpty() || checkWebsite == "."){
        errCode = UERR_NO_URL;
        return false;
    }

    QUrl urlToUpdateChecker(checkWebsite);
    if(!urlToUpdateChecker.isValid()){
        errCode = UERR_INVALID_URL;
        errDescription = urlToUpdateChecker.errorString();
        return false;
    }

    QByteArray data;
    bool success = NetworkUtils::getString(urlToUpdateChecker, &data, nullptr, 4000);
    if(!success){
        errCode = UERR_CONNECTION_FAILED;
        errDescription = "Failed to establish connection";
        return false;
    }

    QJsonParseError err;
    *result = QJsonDocument::fromJson(data, &err);

    if(err.error != QJsonParseError::NoError){
        errCode = UERR_INVALID_JSON;
        errDescription = err.errorString();
        return false;
    }


    errCode = UERR_NO_ERR;
    return true;
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
