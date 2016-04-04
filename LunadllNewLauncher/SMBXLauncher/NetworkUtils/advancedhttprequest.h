#iwfndef ADVANCEDHTTPREQUEST_H
#define ADVANCEDHTTPREQUEST_H

#include <QObject>
#include <QUrl>
#include <QJsonDocument>

class AdvancedHTTPRequest : public QObject
{
    Q_OBJECT
public:
    explicit AdvancedHTTPRequest(QObject *parent = 0);
    explicit AdvancedHTTPRequest(const QUrl& url, QObject *parent = 0);

    // Settings
    QUrl url() const;
    void setUrl(const QUrl &url);
    int getTimeout() const;
    void setTimeout(int timeout);

    enum AdvancedHTTPRequestError {
        NoError,
        NoURLError,
        InvalidURLError,
        ConnectionError,
        InvalidJSONError
    };

    // Error Handling
    QString errorString();
    AdvancedHTTPRequestError error();


    // Get Result
    bool getQJSONDocument(QJsonDocument& output);
    bool getByteData(QByteArray& output);
public slots:
    void start();

private:
    QUrl m_url;
    int m_timeout;
};

#endif // ADVANCEDHTTPREQUEST_H
