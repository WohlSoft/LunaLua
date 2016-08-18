#ifndef QNETWORKREPLYEXCEPTION_H
#define QNETWORKREPLYEXCEPTION_H

#include <QNetworkReply>

class QNetworkReplyException : public std::exception
{
    QNetworkReply::NetworkError m_replyError;
    QString m_replyErrorString;
public:
    QNetworkReplyException(QNetworkReply::NetworkError replyError, QString replyErrorString);

    QNetworkReply::NetworkError error() const;
    bool hasErrorString() const;
    QString errorString() const;
};

#endif // QNETWORKREPLYEXCEPTION_H
