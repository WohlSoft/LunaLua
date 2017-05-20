#include "launchercustomwebpage.h"

#include <QDesktopServices>

LauncherCustomWebPage::LauncherCustomWebPage(QObject *parent) :
    QWebEnginePage(parent)
{}

bool LauncherCustomWebPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        qDebug() << url;
        // if(!(url.isRelative() || !url.isLocalFile())) {
            QDesktopServices::openUrl(url);
            return false;
        // }
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}
