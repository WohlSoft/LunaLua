#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <QtNetwork>

namespace NetworkUtils {
    QByteArray getString(const QUrl& url, int timeout = 4000);
    bool checkInternetConnection(int timeout);
}

#endif // NETWORKUTILS_H
