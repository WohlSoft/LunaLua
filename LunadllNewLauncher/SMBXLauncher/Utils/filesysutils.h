#ifndef FILESYSUTILS_H
#define FILESYSUTILS_H

#include <QFile>
#include <QByteArray>

namespace FilesysUtils
{
    void writeDefaultIfNotExist(QFile& file, const QByteArray& data);
}

#endif // FILESYSUTILS_H
