#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <QtNetwork>

namespace NetworkUtils {
    bool getString(const QUrl& url, QByteArray *out = nullptr, QNetworkReply::NetworkError* error = nullptr, int timeout = 4000);
    bool checkInternetConnection(int timeout);
}

#endif // NETWORKUTILS_H
