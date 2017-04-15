#ifndef StringUtils_hhhhh
#define StringUtils_hhhhh

#include <string>
#include <iterator>
#include <type_traits>
#include <string_view>
#include <vector>



namespace LunaLua::StringUtils {
    // string/wstring literal: http://stackoverflow.com/q/9040247 (if I ever need it)

    template<typename Elem, typename Traits = std::char_traits<Elem>, typename Alloc = std::allocator<Elem>>
    auto split(const std::basic_string_view<Elem, Traits>& input,
               const std::basic_string_view<Elem, Traits>& sep) {
        using StringType = std::basic_string<Elem, Traits, Alloc>;
        using StringViewType = std::basic_string_view<Elem, Traits>;
        std::size_t sizeOfSep = sep.size();

        std::vector<StringType> output;
        
        StringViewType st = input;
        while (true)
        {
            size_t pos = st.find_first_of(sep);
            output.push_back(StringType(st.substr(0, pos)));
            if (pos == StringType::npos)
                break;
            st = st.substr(pos + sizeOfSep);
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

    template<typename Elem, typename Traits = std::char_traits<Elem>, typename Alloc = std::allocator<Elem>>
    auto splitCmdArgs(const std::basic_string_view<Elem, Traits>& input) {
        std::vector<std::basic_string<Elem, Traits, Alloc>> args;
        std::basic_string<Elem, Traits, Alloc> buf;
        bool quote_opened = false;
        for (unsigned int i = 0; i < input.size(); i++)
        {
            if (!quote_opened) 
            {
                if (input[i] == static_cast<Elem>(' ')) 
                {
                    if (!buf.empty())
                        args.push_back(std::move(buf));
                    buf.clear();
                } 
                else if (input[i] == static_cast<Elem>('\"'))
                {
                    quote_opened = true;
                }
                else 
                {
                    buf.push_back(input[i]);
                }
            } 
            else 
            {
                if (input[i] == static_cast<Elem>('\"'))
                {
                    if (!buf.empty())
                        args.push_back(std::move(buf));
                    buf.clear();
                    quote_opened = false;
                }
                else 
                {
                    buf.push_back(input[i]);
                }
            }            
        }

        if (!buf.empty())
            args.push_back(buf);

        return args;
    }

    template<typename Elem, typename Traits = std::char_traits<Elem>, typename Alloc = std::allocator<Elem>>
    auto splitCmdArgs(const std::basic_string<Elem, Traits, Alloc>& input) {
        return splitCmdArgs(std::basic_string_view<Elem, Traits>(input));
    }

    template<typename Elem, typename Traits = std::char_traits<Elem>>
    auto splitCmdArgs(const Elem* input) {
        return splitCmdArgs(std::basic_string_view<Elem, Traits>(input));
    }

    template<typename Elem, typename Traits = std::char_traits<Elem>>
    auto isNumber(const std::basic_string_view<Elem>& input) {
        return std::all_of(input.cbegin(), input.cend(), isdigit);
    }

    template<typename Elem, typename Traits = std::char_traits<Elem>, typename Alloc = std::allocator<Elem>>
    auto isNumber(const std::basic_string<Elem, Traits, Alloc>& input) {
        return isNumber(std::basic_string_view<Elem, Traits>(input));
    }
}

#endif