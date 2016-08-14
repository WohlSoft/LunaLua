#include "qjsonfileopenexception.h"

QJsonFileOpenException::QJsonFileOpenException() :
    std::exception("file open error")
{}
