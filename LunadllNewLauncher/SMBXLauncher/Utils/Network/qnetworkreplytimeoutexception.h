#ifndef QNETWORKREQUESTTIMEOUTEXCEPTION_H
#define QNETWORKREQUESTTIMEOUTEXCEPTION_H

#include <stdexcept>

class QNetworkReplyTimeoutException : public std::exception
{
public:
    QNetworkReplyTimeoutException();
};

#endif // QNETWORKREQUESTTIMEOUTEXCEPTION_H
