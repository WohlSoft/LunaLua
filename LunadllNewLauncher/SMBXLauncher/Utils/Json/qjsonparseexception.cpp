#include "qjsonparseexception.h"

QJsonParseError QJsonParseException::getParseError() const
{
    return m_err;
}

QJsonParseException::QJsonParseException(const QJsonParseError &err) :
    std::logic_error("parse error"),
    m_err(err)
{}
