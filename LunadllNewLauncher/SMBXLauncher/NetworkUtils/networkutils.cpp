#include "networkutils.h"
#include "qreplytimeout.h"

#if _MSC_VER > 1800
constexpr char timeoutURL[] = "http://google.com";
#else
const char timeoutURL[] = "http://google.com";
#endif

bool NetworkUtils::getString(const QUrl &url, QByteArray *out, QNetworkReply::NetworkError *error, int timeout)
{
    if(error)
        *error = QNetworkReply::NoError;

    QNetworkReply * reply = nullptr;
    bool replyFinished = false;

    QNetworkAccessManager downloader;
    downloader.setNetworkAccessible(QNetworkAccessManager::Accessible);

    QObject::connect(&downloader, &QNetworkAccessManager::finished, [&replyFinished](QNetworkReply *){
        replyFinished = true;
    });

    QNetworkRequest simpleRequest(url);
    reply = downloader.get(simpleRequest);
    new QReplyTimeout(reply, timeout);

    while(!replyFinished)
        qApp->processEvents();


    reply->deleteLater();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode >= 400){
        if(error)
            *error = QNetworkReply::ConnectionRefusedError;
        return false;
    }

    QByteArray replyContent = reply->readAll();
    if(replyContent.isEmpty()){
        if(error)
            *error = reply->error();
        return false;
    } else {
        if(out)
            *out = replyContent;
        return true;
    }
}

bool NetworkUtils::checkInternetConnection(int timeout)
{
    return getString(QUrl(timeoutURL), nullptr, nullptr, timeout);
}
