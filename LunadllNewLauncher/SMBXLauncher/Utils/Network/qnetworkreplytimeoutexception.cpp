#include "qnetworkreplytimeoutexception.h"

QNetworkReplyTimeoutException::QNetworkReplyTimeoutException() :
    std::logic_error("network request timeout")
{}
