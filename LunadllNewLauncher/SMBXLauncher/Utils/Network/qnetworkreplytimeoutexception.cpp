#include "qnetworkreplytimeoutexception.h"

QNetworkReplyTimeoutException::QNetworkReplyTimeoutException() :
    std::exception("network request timeout")
{}
