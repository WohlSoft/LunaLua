#include "networkjsonutils.h"

#include "Network/networkutils.h"

ExtendedQJsonReader NetworkJsonUtils::getJSON(const QString &website, int timeout)
{
    return ExtendedQJsonReader(NetworkUtils::getString(QUrl(website), timeout));
}
