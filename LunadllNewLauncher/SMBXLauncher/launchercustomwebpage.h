#ifndef LAUNCHERCUSTOMWEBPAGE_H
#define LAUNCHERCUSTOMWEBPAGE_H

#include <QObject>
#include <QWebEnginePage>

class LauncherCustomWebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    LauncherCustomWebPage(QObject* parent = 0);



    // QWebEnginePage interface
protected:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID);
};

#endif // LAUNCHERCUSTOMWEBPAGE_H
