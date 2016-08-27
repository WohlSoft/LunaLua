#include "qjsonvalidationexception.h"


QJsonValidationException::QJsonValidationException(const QString &fieldName, QJsonValidationException::ValidationError errorType) :
    std::exception("validation error"),
    m_fieldName(fieldName),
    m_errorType(errorType)
{}

QString QJsonValidationException::fieldName() const
{
    return m_fieldName;
}

QJsonValidationException::ValidationError QJsonValidationException::errorType() const
{
    return m_errorType;
}


