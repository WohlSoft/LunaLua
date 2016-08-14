#include "extendedqjsonreader.h"
#include "qjsonfileopenexception.h"
#include "qjsonparseexception.h"

#include <QIODevice>

QJsonDocument ExtendedQJsonReader::getQJsonDocument() const
{
    return m_internalDocument;
}

ExtendedQJsonReader::ExtendedQJsonReader(QFile &file)
{
    if(!file.open(QIODevice::ReadOnly))
        throw QJsonFileOpenException();

    QByteArray rawData = file.readAll();
    QJsonParseError jsonErrCode;
    m_internalDocument = QJsonDocument::fromJson(rawData, &jsonErrCode);

    if(jsonErrCode.error != QJsonParseError::NoError)
        throw QJsonParseException(jsonErrCode);
}
