#include "launcherurlrequestinterceptor.h"

LauncherUrlRequestInterceptor::LauncherUrlRequestInterceptor(QObject* parent, QDir dataFolder) :
    QWebEngineUrlRequestInterceptor(parent),
    dataFolderCanonicalPath(dataFolder.canonicalPath())
{
    // Add trailing slash to the data folder canonical path if needed
    if (!dataFolderCanonicalPath.endsWith('/')) {
        dataFolderCanonicalPath.append('/');
    }
}

void LauncherUrlRequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
    QUrl requestedFileUrl = info.requestUrl();
    
    if (requestedFileUrl.isLocalFile()) {
        QDir requestedFilePath(requestedFileUrl.toLocalFile());

        if (!requestedFilePath.canonicalPath().startsWith(dataFolderCanonicalPath)) {
            info.block(true);
        }
    }
}