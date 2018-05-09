#pragma once
#ifndef CSVUtils_H
#define CSVUtils_H

#include <exception>
#include <string>

// prints the explanatory string of an exception. If the exception is nested,
// recurses to print the explanatory of the exception it holds
inline std::string exception_to_pretty_string(const std::exception& e, int level = 0)
{
    std::string prettyString = std::string(static_cast<size_t>(level), ' ') + "exception: " + std::string(e.what()) + "\n";
    #if !defined(_MSC_VER) || _MSC_VER > 1800
    try {
        std::rethrow_if_nested(e); //This thing is not presented in MSVC2013 :-P
    }
    catch (const std::exception& e) {
        prettyString += exception_to_pretty_string(e, level + 1);
    }
    catch (...) {}
    #endif

    return prettyString;
}

#endif

