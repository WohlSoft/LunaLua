#ifndef QNETWORKREQUESTTIMEOUTEXCEPTION_H
#define QNETWORKREQUESTTIMEOUTEXCEPTION_H

#include <stdexcept>

class QNetworkReplyTimeoutException : public std::logic_error
{
public:
    QNetworkReplyTimeoutException();
};

#endif // QNETWORKREQUESTTIMEOUTEXCEPTION_H
