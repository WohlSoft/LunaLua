#ifndef QJSONFILEOPENEXCEPTION_H
#define QJSONFILEOPENEXCEPTION_H

#include <stdexcept>

class QJsonFileOpenException : public std::logic_error
{
public:
    QJsonFileOpenException();
};

#endif // QJSONFILEOPENEXCEPTION_H
