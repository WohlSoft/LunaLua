#include "qjsonparseexception.h"

QJsonParseError QJsonParseException::getParseError() const
{
    return m_err;
}

QJsonParseException::QJsonParseException(const QJsonParseError &err) :
    std::exception("parse error"),
    m_err(err)
{}
