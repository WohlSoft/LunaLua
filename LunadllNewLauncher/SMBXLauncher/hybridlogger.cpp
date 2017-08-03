#include "hybridlogger.h"

#include <QDebug>
#include <iostream>
#include <QPointer>

static QPointer<QWebEngineView> internal_used_view;

void hybridLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    auto formatMessage = [&msg](const QString& name) -> QString {
        return QString("Native - [%1]: %2").arg(name, msg);
    };

    auto hybridDispatch = [](QString toOutput) {
        std::cout << toOutput.toLocal8Bit().constData() << std::endl;
        internal_used_view->page()->runJavaScript(QString("console.log('") + toOutput.replace(QString("'"), QString("\\'")) + "');");
    };

    switch (type) {
    case QtDebugMsg:
        hybridDispatch(formatMessage("Debug"));
        break;
    case QtInfoMsg:
        hybridDispatch(formatMessage("Info"));
        break;
    case QtWarningMsg:
        hybridDispatch(formatMessage("Warning"));
        break;
    case QtCriticalMsg:
        hybridDispatch(formatMessage("Critical"));
        break;
    case QtFatalMsg:
        hybridDispatch(formatMessage("Fatal"));
        abort();
    }
}



void HybridLogger::init(QWebEngineView *view)
{
    internal_used_view = QPointer<QWebEngineView>(view);
    qInstallMessageHandler(hybridLogHandler);
}

