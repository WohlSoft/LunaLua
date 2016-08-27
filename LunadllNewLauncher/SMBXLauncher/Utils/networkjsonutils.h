#ifndef NETWORKJSONUTILS_H
#define NETWORKJSONUTILS_H

#include "Json/extendedqjsonreader.h"

namespace NetworkJsonUtils
{
    ExtendedQJsonReader getJSON(const QString& website, int timeout = 4000);
}

#endif // NETWORKJSONUTILS_H
