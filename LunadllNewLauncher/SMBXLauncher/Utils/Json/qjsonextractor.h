#ifndef QJSONEXTRACTOR_H
#define QJSONEXTRACTOR_H

#include <QString>
#include <type_traits>
#include <QJsonObject>
#include <QJsonValue>

#include "qjsonvalidationexception.h"
#include "qjsonurlvalidationexception.h"


namespace detail {
    namespace json {
        template<typename Ret,
                 typename ToFunctionRet,
                 typename ToFunctionParamDefault>
        inline Ret extractByFunctions(const QJsonObject& obj,
                               const QString& child,
                               bool (QJsonValue::* isFunction)() const,
                               ToFunctionRet (QJsonValue::* toFunction)(ToFunctionParamDefault) const){
            QJsonValue containedValue = obj.value(child);

            if(containedValue.isUndefined())
                throw QJsonValidationException(child, QJsonValidationException::ValidationError::MissingType);

            if(!(containedValue.*isFunction)())
                throw QJsonValidationException(child, QJsonValidationException::ValidationError::WrongType);

            return static_cast<Ret>((containedValue.*toFunction)(std::decay<ToFunctionParamDefault>::type()));
        }
    }
}



template<typename T>
struct QJsonExtractor;

template<>
struct QJsonExtractor<QString> {
    QString operator()(const QJsonObject& obj, const QString& child){
        return detail::json::extractByFunctions<QString>(obj, child, &QJsonValue::isString, &QJsonValue::toString);
    }
};

template<>
struct QJsonExtractor<int> {
    int operator()(const QJsonObject& obj, const QString& child){
        return detail::json::extractByFunctions<int>(obj, child, &QJsonValue::isDouble, &QJsonValue::toDouble);
    }
};

template<>
struct QJsonExtractor<QJsonObject> {
    QJsonObject operator()(const QJsonObject& obj, const QString& child){
        return detail::json::extractByFunctions<QJsonObject>(obj, child, &QJsonValue::isObject, &QJsonValue::toObject);
    }
};

template<>
struct QJsonExtractor<QUrl> {
    QUrl operator()(const QJsonObject& obj, const QString& child){
        QString urlString = QJsonExtractor<QString>()(obj, child);
        QUrl urlObj(urlString);
        if(!urlObj.isValid())
            throw QJsonUrlValidationException(child, urlObj);
        return urlObj;
    }
};

#endif // QJSONEXTRACTOR_H
