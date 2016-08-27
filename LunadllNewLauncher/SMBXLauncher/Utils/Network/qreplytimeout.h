#ifndef QREPLYTIMEOUT_H
#define QREPLYTIMEOUT_H

#include <QObject>
#include <QTimer>
#include <QNetworkReply>


class QReplyTimeout : public QObject {
    Q_OBJECT
    bool m_isAborted;
public:
    QReplyTimeout(QNetworkReply* reply, const int timeout) :
        QObject(reply),
        m_isAborted(false)
    {
        Q_ASSERT(reply);
        if (reply) {
            QTimer::singleShot(timeout, this, SLOT(timeout()));
        }
    }

    bool isAborted() const;
private slots:
    void timeout() {
        QNetworkReply* reply = static_cast<QNetworkReply*>(parent());
        if (reply->isRunning()) {
            reply->close();
            m_isAborted = true;
        }
    }
};

#endif // QREPLYTIMEOUT_H
