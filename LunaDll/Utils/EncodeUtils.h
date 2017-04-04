#ifndef EncodeUtils_hhhhh
#define EncodeUtils_hhhhh

#include <string>

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
}

#endif