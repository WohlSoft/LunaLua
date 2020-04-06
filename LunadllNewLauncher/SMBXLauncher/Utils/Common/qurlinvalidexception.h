#ifndef QURLINVALIDEXCEPTION_H
#define QURLINVALIDEXCEPTION_H

#include <stdexcept>
#include <QUrl>

class QUrlInvalidException : public std::logic_error
{
    QUrl m_errorUrl;
public:
    QUrlInvalidException(const QUrl& errorUrl);

    QString errorString() const;
    QString url() const;
};

#endif // QURLINVALIDEXCEPTION_H
