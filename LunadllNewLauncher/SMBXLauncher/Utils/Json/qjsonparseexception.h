#ifndef QJSONPARSEEXCEPTION_H
#define QJSONPARSEEXCEPTION_H

#include <stdexcept>
#include <QString>
#include <QJsonParseError>


class QJsonParseException : public std::logic_error
{
    QJsonParseError m_err;
public:
    QJsonParseException(const QJsonParseError& getParseError);

    QJsonParseError getParseError() const;
};

#endif // QJSONPARSEEXCEPTION_H
