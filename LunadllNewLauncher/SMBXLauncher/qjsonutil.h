#ifndef QJSONUTIL_H
#define QJSONUTIL_H

#include <QJsonValue>
#include <QJsonObject>
#include <functional>


enum class VALIDATE_ERROR {
    VALIDATE_NO_CHILD,
    VALIDATE_WRONG_TYPE
};

template<typename T>
inline bool qJsonValidate(const QJsonObject& val, const QString& child, const std::function<void(VALIDATE_ERROR, const QString&)>& errFunc)
{
    throw "err";
    return true;
}

template<>
inline bool qJsonValidate<int>(const QJsonObject& val, const QString& child, const std::function<void(VALIDATE_ERROR, const QString&)>& errFunc)
{
    if(!val.contains(child)){
        if(errFunc)
            errFunc(VALIDATE_ERROR::VALIDATE_NO_CHILD, child);
        return false;
    }
    QJsonValue containedValue = val.value(child);
    if(!containedValue.isDouble()){
        if(errFunc)
            errFunc(VALIDATE_ERROR::VALIDATE_WRONG_TYPE, child);
        return false;
    }
    return true;
}

template<>
inline bool qJsonValidate<QJsonObject>(const QJsonObject& val, const QString& child, const std::function<void(VALIDATE_ERROR, const QString&)>& errFunc)
{
    if(!val.contains(child)){
        if(errFunc)
            errFunc(VALIDATE_ERROR::VALIDATE_NO_CHILD, child);
        return false;
    }
    QJsonValue containedValue = val.value(child);
    if(!containedValue.isObject()){
        if(errFunc)
            errFunc(VALIDATE_ERROR::VALIDATE_WRONG_TYPE, child);
        return false;
    }
    return true;
}

template<>
inline bool qJsonValidate<QString>(const QJsonObject& val, const QString& child, const std::function<void(VALIDATE_ERROR, const QString&)>& errFunc)
{
    if(!val.contains(child)){
        if(errFunc)
            errFunc(VALIDATE_ERROR::VALIDATE_NO_CHILD, child);
        return false;
    }
    QJsonValue containedValue = val.value(child);
    if(!containedValue.isString()){
        if(errFunc)
            errFunc(VALIDATE_ERROR::VALIDATE_WRONG_TYPE, child);
        return false;
    }
    return true;
}


#endif // QJSONUTIL_H
