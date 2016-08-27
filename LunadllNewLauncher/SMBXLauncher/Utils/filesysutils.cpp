#include "filesysutils.h"





void FilesysUtils::writeDefaultIfNotExist(QFile &file, const QByteArray &data)
{
    if(!file.exists()){
        if(file.open(QIODevice::WriteOnly)){
            file.write(data);
            file.close();
        }
    }
}
