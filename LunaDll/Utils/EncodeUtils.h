#ifndef EncodeUtils_hhhhh
#define EncodeUtils_hhhhh

// BSTR
#include <WTypes.h>

#include <string>

// For EncodeUrl
#include <sstream>
#include <iomanip>

namespace LunaLua::EncodeUtils {
    /**
     * \brief Converts std::wstring into UTF8-encoded std::string
     * \param wstr input std::wstring
     * \return output std::string
     */
    std::string WStr2Str(const std::wstring &wstr);

    /**
     * \brief Converts std::wstring_view into UTF8-encoded std::string
     * \param wstr input std::wstring_view
     * \return output std::string
     */
    std::string WStr2Str(const std::wstring_view &wstr);

    /**
     * \brief Converts UTF8-encoded std::string into std::wstring
     * \param str input std::string
     * \return output std::wstring
     */
    std::wstring Str2WStr(const std::string &str);

    /**
     * \brief Converts UTF8-encoded std::string_view into std::wstring
     * \param str input std::string_view
     * \return output std::wstring
     */
    std::wstring Str2WStr(const std::string_view &str);

    /**
     * \brief Converts std::wstring into Local ANSI-encoded std::string
     * \param wstr input std::wstring
     * \return output std::string
     */
    std::string WStr2StrA(const std::wstring &wstr);

    /**
     * \brief Converts std::wstring_view into Local ANSI-encoded std::string
     * \param wstr input std::wstring_view
     * \return output std::string
     */
    std::string WStr2StrA(const std::wstring_view &wstr);

    /**
     * \brief Converts Local ANSI-encoded std::string into std::wstring
     * \param str input std::string
     * \return output std::wstring
     */
    std::wstring StrA2WStr(const std::string &str);

    /**
     * \brief Converts Local ANSI-encoded std::string_view into std::wstring
     * \param str input std::string_view
     * \return output std::wstring
     */
    std::wstring StrA2WStr(const std::string_view &str);

    /**
     * \brief Convers a BSTR to ANSI string
     * \param str input BSTR
     * \return output std::string
     */
    std::string BSTR2AStr(BSTR str);

    /**
     * \brief Encodes text to comply with the URL encoding
     * \param str The string to encode
     * \return Encoded string
     */
    template<typename Elem, typename Traits = std::char_traits<Elem>, typename Alloc = std::allocator<Elem>>
    auto EncodeUrl(const std::basic_string_view<Elem, Traits>& str)
    {
        using StringStreamType = std::basic_stringstream<Elem, Traits, Alloc>;
        using StringViewType = std::basic_string_view<Elem, Traits>;

        StringStreamType escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (typename StringViewType::const_iterator i = str.begin(), n = str.end(); i != n; ++i) {
            Elem c = (*i);

            // Keep alphanumeric and other accepted characters intact
            if (isalnum(c) || 
                c == static_cast<Elem>('-') || 
                c == static_cast<Elem>('_') || 
                c == static_cast<Elem>('.') || 
                c == static_cast<Elem>('~')) 
            {
                escaped << c;
                continue;
            }

            // Any other characters are percent-encoded
            escaped << std::uppercase;
            escaped << static_cast<Elem>('%') << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            escaped << std::nouppercase;
        }

        return escaped.str();
    }
}

#endif