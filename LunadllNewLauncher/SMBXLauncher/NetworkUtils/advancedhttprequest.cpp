#include "advancedhttprequest.h"

AdvancedHTTPRequest::AdvancedHTTPRequest(QObject *parent) :
    AdvancedHTTPRequest(QUrl(), parent)
{}

AdvancedHTTPRequest::AdvancedHTTPRequest(const QUrl &url, QObject *parent) :
    m_url(url),
    QObject(parent)
{}

QUrl AdvancedHTTPRequest::url() const
{
    return m_url;
}

void AdvancedHTTPRequest::setUrl(const QUrl &url)
{
    m_url = url;
}

int AdvancedHTTPRequest::getTimeout() const
{
    return m_timeout;
}

void AdvancedHTTPRequest::setTimeout(int timeout)
{
    m_timeout = timeout;
}

void AdvancedHTTPRequest::start()
{

}
