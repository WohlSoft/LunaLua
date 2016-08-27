#include "qurlinvalidexception.h"

QUrlInvalidException::QUrlInvalidException(const QUrl &errorUrl) :
    std::exception("invalid url"),
    m_errorUrl(errorUrl)
{}

QString QUrlInvalidException::errorString() const
{
    return m_errorUrl.errorString();
}

QString QUrlInvalidException::url() const
{
    return m_errorUrl.toString();
}
