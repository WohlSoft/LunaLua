#ifndef QJSONVALIDATIONEXCEPTION_H
#define QJSONVALIDATIONEXCEPTION_H

#include <stdexcept>
#include <QString>

class QJsonValidationException : public std::logic_error
{
public:
    enum class ValidationError {
        WrongType,
        MissingType,
        InvalidUrl
    };
private:
    QString m_fieldName;
    ValidationError m_errorType;
public:

    QJsonValidationException(const QString& fieldName, ValidationError errorType);

    QString fieldName() const;
    ValidationError errorType() const;
};

#endif // QJSONVALIDATIONEXCEPTION_H
