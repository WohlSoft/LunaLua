#ifndef QJSONURLVALIDATIONEXCEPTION_H
#define QJSONURLVALIDATIONEXCEPTION_H

#include "qjsonvalidationexception.h"
#include "../Common/qurlinvalidexception.h"

class QJsonUrlValidationException : public QJsonValidationException, public QUrlInvalidException
{
public:
    QJsonUrlValidationException(const QString& fieldName, const QUrl& invalidUrl);

};

#endif // QJSONURLVALIDATIONEXCEPTION_H
