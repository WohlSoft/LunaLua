#ifndef QJSONFILEOPENEXCEPTION_H
#define QJSONFILEOPENEXCEPTION_H

#include <stdexcept>

class QJsonFileOpenException : public std::exception
{
public:
    QJsonFileOpenException();
};

#endif // QJSONFILEOPENEXCEPTION_H
