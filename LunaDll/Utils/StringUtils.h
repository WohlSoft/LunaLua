#ifndef StringUtils_hhhhh
#define StringUtils_hhhhh

#include <algorithm>
#include <string>
#include <iterator>
#include <type_traits>

namespace LunaLua::StringUtils {
    template<typename It, typename CharType, typename OutputIt>
    void split(It begin, It end, CharType* sep, OutputIt out) {
        static_assert(std::is_same_v<std::iterator_traits<It>::value_type, CharType>, "string char type and seperator type must be same!");
        std::size_t sizeOfSep = std::strlen(sep);

        It current = begin;
        do {
            current = std::search(current, end, sep, sep + sizeOfSep);
            
        } while (true);
    }
}

#endif