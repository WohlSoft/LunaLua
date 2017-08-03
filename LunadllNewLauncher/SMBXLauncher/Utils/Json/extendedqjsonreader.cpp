#include "extendedqjsonreader.h"
#include "qjsonfileopenexception.h"
#include "qjsonparseexception.h"

#include <QVariant>
#include <QDebug>

ExtendedQJsonReader::ExtendedQJsonReader(QFile &file)
{
    if(!file.open(QIODevice::ReadOnly))
        throw QJsonFileOpenException();

    initJson(file.readAll());

    file.close();
}

ExtendedQJsonReader::ExtendedQJsonReader(const QByteArray &data)
{
    initJson(data);
}

ExtendedQJsonReader::ExtendedQJsonReader(const QJsonDocument &document) :
    m_internalDocument(document)
{}

void ExtendedQJsonReader::initJson(const QByteArray &data)
{
    QJsonParseError jsonErrCode;
    m_internalDocument = QJsonDocument::fromJson(data, &jsonErrCode);

    if(jsonErrCode.error != QJsonParseError::NoError)
        throw QJsonParseException(jsonErrCode);
}

QJsonDocument ExtendedQJsonReader::getQJsonDocument() const
{
    return m_internalDocument;
}

QVariantMap ExtendedQJsonReader::toMap() const
{
    return m_internalDocument.toVariant().toMap();
}

bool ExtendedQJsonReader::canConvertToMap() const
{
    return m_internalDocument.isObject(); // QJsonObject represents a VariantMap
}
