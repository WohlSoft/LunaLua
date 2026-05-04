#ifndef LAUNCHERURLREQUESTINTERCEPTOR_H
#define LAUNCHERURLREQUESTINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include <QObject>
#include <QDir>
#include <QString>

class LauncherUrlRequestInterceptor : public QWebEngineUrlRequestInterceptor {
    // Path to the data folder after symlink resolution
    QString dataFolderCanonicalPath;

public:
    LauncherUrlRequestInterceptor(QObject* parent, QDir dataFolder);
    virtual void interceptRequest(QWebEngineUrlRequestInfo &info) override;
};

#endif