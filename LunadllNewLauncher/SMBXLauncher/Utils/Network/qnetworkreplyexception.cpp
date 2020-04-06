#include "qnetworkreplyexception.h"

QNetworkReplyException::QNetworkReplyException(QNetworkReply::NetworkError replyError, QString replyErrorString) :
    std::logic_error("network reply error"),
    m_replyError(replyError),
    m_replyErrorString(replyErrorString)
{}

QNetworkReply::NetworkError QNetworkReplyException::error() const
{
    return m_replyError;
}

bool QNetworkReplyException::hasErrorString() const
{
    return !m_replyErrorString.isEmpty();
}

QString QNetworkReplyException::errorString() const
{
    return m_replyErrorString;
}
