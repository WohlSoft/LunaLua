#include "parsetools.h"





QTreeWidgetItemSMBXAddress *ParseTools::generateNewItem(const QString& name, const QString& offset, const QString& fieldType)
{
    QTreeWidgetItemSMBXAddress* item = new QTreeWidgetItemSMBXAddress();

    item->setText(0, name);
    item->setText(1, offset);
    item->setText(2, fieldType);
    item->setData(0, Qt::UserRole+1, "entry");
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    item->setCheckState(0, Qt::Unchecked);
    return item;
}

QList<QTreeWidgetItem *> ParseTools::parseStruct(const QString &data, bool ignoreUnknown, bool &success)
{
    success = false;
    QList<QTreeWidgetItem *> items;
    QStringList lines = data.split("\n");

    int curOffset = 0;

    bool isLookupState = false;
    foreach (QString line, lines) {
        line = line.simplified();
        if(line.startsWith("struct")){
            isLookupState = true;
            continue;
        }

        if(isLookupState && line.startsWith("{"))
            continue;

        if(isLookupState && (line.startsWith("};") || line.startsWith("}"))){
            break;
        }

        if(isLookupState && !line.isEmpty()){
            line = line.replace(" * ", "* ");
            QStringList fields = line.split(" ");

            if(fields.length() < 2){
                return items;
            }

            QString nextField = fields[0];
            QString nextVarName = fields[1];

            int fieldLength = cppFieldTypeToLength(nextField);
            if(fieldLength == -1){
                return items;
            }

            if(ignoreUnknown){
                if(nextVarName.startsWith("unknown_")){
                    curOffset += fieldLength;
                    continue;
                }
            }

            if(fieldLength == -2){
                items << generateNewItem("x", QString::number(curOffset, 16).toUpper(), "double");
                items << generateNewItem("y", QString::number((curOffset += 8), 16).toUpper(), "double");
                items << generateNewItem("height", QString::number((curOffset += 8), 16).toUpper(), "double");
                items << generateNewItem("width", QString::number((curOffset += 8), 16).toUpper(), "double");
                items << generateNewItem("speedX", QString::number((curOffset += 8), 16).toUpper(), "double");
                items << generateNewItem("speedY", QString::number((curOffset += 8), 16).toUpper(), "double");
                curOffset += 8;
                continue;
            }

            if(nextVarName.endsWith(";")){
                nextVarName.chop(1);
            }

            items << generateNewItem(nextVarName, QString::number(curOffset, 16).toUpper(), cppFieldToScannerField(nextField));
            curOffset += fieldLength;
        }
    }

    success = true;
    return items;
}

QString ParseTools::cppFieldToScannerField(const QString &fieldName)
{

    if(fieldName == "char" ||
            fieldName == "unsigned char" ||
            fieldName == "BYTE") return "byte";
    if(fieldName == "short" ||
            fieldName == "unsigned short" ||
            fieldName == "WORD") return "word";
    if(fieldName == "int" ||
            fieldName == "unsigned int" ||
            fieldName == "DWORD") return "dword";
    if(fieldName.contains("*")) return "dword";
    if(fieldName == "VB6StrPtr") return "string";
    if(fieldName == "float" ||
            fieldName == "unsigned float") return "float";
    if(fieldName == "double" ||
            fieldName == "unsigned double") return "double";

    return "!Header Struct parse error!";
}

int ParseTools::cppFieldTypeToLength(const QString& fieldName)
{

    if(fieldName == "char" ||
            fieldName == "unsigned char" ||
            fieldName == "BYTE") return 1;
    if(fieldName == "short" ||
            fieldName == "unsigned short" ||
            fieldName == "WORD") return 2;
    if(fieldName == "int" ||
            fieldName == "unsigned int" ||
            fieldName == "DWORD") return 4;
    if(fieldName.contains("*")) return 4;
    if(fieldName == "VB6StrPtr") return 4;
    if(fieldName == "float" ||
            fieldName == "unsigned float") return 4;
    if(fieldName == "double" ||
            fieldName == "unsigned double") return 8;

    if(fieldName == "Momentum") return -2;
    return -1;
}
