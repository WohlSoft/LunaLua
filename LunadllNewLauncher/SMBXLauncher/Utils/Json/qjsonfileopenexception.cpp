#include "qjsonfileopenexception.h"
#include <exception>

QJsonFileOpenException::QJsonFileOpenException() :
    std::logic_error("file open error")
{}
