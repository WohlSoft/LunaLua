#include "qurlinvalidexception.h"

QUrlInvalidException::QUrlInvalidException(const QUrl &errorUrl) :
    std::logic_error("invalid url"),
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
