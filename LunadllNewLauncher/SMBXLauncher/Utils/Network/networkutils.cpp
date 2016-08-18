#include "networkutils.h"
#include "qreplytimeout.h"
#include "qnetworkreplytimeoutexception.h"
#include "qnetworkreplyexception.h"
#include "qurlinvalidexception.h"

Q_CONSTEXPR char timeoutURL[] = "http://google.com";

QByteArray NetworkUtils::getString(const QUrl &url, int timeout)
{
    if(!url.isValid())
        throw QUrlInvalidException(url);

    QNetworkAccessManager downloader;
    downloader.setNetworkAccessible(QNetworkAccessManager::Accessible);

    // TODO: is safe?
    QNetworkReply* reply = downloader.get(QNetworkRequest(url));
    QReplyTimeout* timeoutControl = new QReplyTimeout(reply, timeout);

    bool replyFinished = false;
    QObject::connect(&downloader, &QNetworkAccessManager::finished,
                     [&replyFinished](QNetworkReply *){
        replyFinished = true;
    });

    while(!replyFinished)
        qApp->processEvents();

    reply->deleteLater();
    if(timeoutControl->isAborted())
        throw QNetworkReplyTimeoutException();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(statusCode >= 400)
        throw QNetworkReplyException(reply->error(), reply->errorString());

    QByteArray replyContent = reply->readAll();
    if(replyContent.isEmpty())
        throw QNetworkReplyException(reply->error(), reply->errorString());

    return replyContent;
}

bool NetworkUtils::checkInternetConnection(int timeout)
{
    try {
        getString(QUrl(timeoutURL), timeout);
    } catch (const QNetworkReplyTimeoutException&) {
        return false;
    }
    return true;
}


