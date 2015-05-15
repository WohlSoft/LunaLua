#ifndef PARSETOOLS_H
#define PARSETOOLS_H

#include <qtreewidgetitemsmbxaddress.h>
#include <QList>

class ParseTools
{
public:
    static QTreeWidgetItemSMBXAddress* generateNewItem(const QString &name, const QString &offset, const QString &fieldType);
    static QList<QTreeWidgetItem*> parseStruct(const QString& data, bool ignoreUnknown, bool& success);
    static QString cppFieldToScannerField(const QString &fieldName);
    static int cppFieldTypeToLength(const QString &fieldName);
};

#endif // PARSETOOLS_H
