#ifndef StringUtils_hhhhh
#define StringUtils_hhhhh

#include <string>
#include <iterator>
#include <type_traits>
#include <string_view>
#include <vector>

namespace LunaLua::StringUtils {
    template<typename Elem, typename Traits = std::char_traits<Elem>, typename Alloc = std::allocator<Elem>>
    auto split(const std::basic_string_view<Elem, Traits>& input,
               const std::basic_string_view<Elem, Traits>& sep) {
        using StringType = std::basic_string<Elem, Traits, Alloc>;
        using StringViewType = std::basic_string_view<Elem, Traits>;

        std::vector<StringType> output;
        
        StringViewType st = input;
        while (true)
        {
            size_t pos = input.find_first_of(sep);
            output.push_back(StringType(st.substr(0, pos)));
            if (pos == StringType::npos)
                break;
            st = st.substr(pos + 1);
        }

        return output;
    }

    template<typename StringType, typename Elem, typename Traits = std::char_traits<Elem>>
    auto split(const StringType& input, const Elem* sep) {
        return split(std::basic_string_view<Elem, Traits>(input), std::basic_string_view<Elem, Traits>(sep));
    }

    template<typename Elem, typename Traits = std::char_traits<Elem>, typename Alloc = std::allocator<Elem>>
    void replace(std::basic_string<Elem, Traits, Alloc>& str,
                 const std::basic_string_view<Elem, Traits>& from,
                 const std::basic_string_view<Elem, Traits>& to) {
        if (from.empty())
            return;
        
        std::size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::basic_string<Elem, Traits, Alloc>::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    // Make compatible with simple char pointers
    template<typename Elem, typename Traits = std::char_traits<Elem>, typename Alloc = std::allocator<Elem>>
    void replace(std::basic_string<Elem, Traits, Alloc>& str, const Elem* from, const Elem* to) {
        replace(str, std::basic_string_view<Elem, Traits>(from), std::basic_string_view<Elem, Traits>(to));
    }



}

#endif