#include "qjsonurlvalidationexception.h"

QJsonUrlValidationException::QJsonUrlValidationException(const QString &fieldName, const QUrl &invalidUrl) :
    QJsonValidationException(fieldName, QJsonUrlValidationException::ValidationError::InvalidUrl),
    QUrlInvalidException(invalidUrl)
{}
