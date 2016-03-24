#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H


#include <QtNetwork>

class NetworkUtils
{
public:
    NetworkUtils() = delete;


    static bool getString(const QUrl& url, QByteArray *out = nullptr, QNetworkReply::NetworkError* error = nullptr, int timeout = 4000);
    static bool checkInternetConnection(int timeout);


};

#endif // NETWORKUTILS_H
