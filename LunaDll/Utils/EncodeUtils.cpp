#include "EncodeUtils.h"

#include <type_traits>
#include <cassert>
#include <string>

using namespace std::string_literals;

// For WideCharToMultiByte and MultiByteToWideChar
#include <Windows.h>

// Private function
namespace LunaLua::EncodeUtils::detail {
    // TODO: Replace with constexpr if later (if MSVC supports is)
    template<typename ToCharType>
    struct GenericConvert;

    template<>
    struct GenericConvert<char> {
        static auto Convert(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte) {
            return WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, NULL, NULL);
        }
    };

    template<>
    struct GenericConvert<wchar_t> {
        static auto Convert(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar) {
            return MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
        }
    };

    template<int ConversionType, 
             typename OutStrElem, 
             typename OutStrTraits = std::char_traits<OutStrElem>, 
             typename OutStrAlloc = std::allocator<OutStrElem>, 
             typename InStrElem, 
             typename InStrTraits>
    auto EncodeConvert(const std::basic_string_view<InStrElem, InStrTraits>& input) {
        // Ensure that the type is not the same --> Doesn't make sense!
        static_assert(!std::is_same_v<OutStrElem, InStrElem>, "Output string element and input string element cannot be the same type");
        static_assert(std::is_same_v<OutStrElem, char> || std::is_same_v<OutStrElem, wchar_t>, "Output char type must be char or wchar!");
        static_assert(std::is_same_v<InStrElem, char> || std::is_same_v<InStrElem, wchar_t>, "Input char type must be char or wchar!");
        
        // TODO: Move to a better conversion library or use icu4c?
        /*
         * http://stackoverflow.com/a/15128103/5082374
         * We have to ensure that there is enought space for allocation. 
         * So if we convert from UTF-16 (wchar) to UTF-8 (char), we must ensure 
         * that conversion is done right.
         * 
         * Currently we must assume, that we have a BMP char in our UTF-16 text.
         * 1x Character Wide Char (2 bytes) --> 3x Character UTF-8 Char (1 byte)
         */
        constexpr bool is_output_char = std::is_same_v<OutStrElem, char>;
        constexpr std::size_t max_widen = (is_output_char ? 3u : 1u);

        std::basic_string<OutStrElem, OutStrTraits, OutStrAlloc> output(input.length() * max_widen, '\x0');
        
        // TODO: Use Constexpr If, when msvc support it
        int newlen = GenericConvert<OutStrElem>::Convert(ConversionType, 0, input.data(), input.length(), &output[0], output.length());
        if(newlen == 0)
        {
            // Throw error
            DWORD errVal = GetLastError();
            std::error_code ec(errVal, std::system_category());
            throw std::system_error(ec, "Failed to do conversion from "s + (!is_output_char ? "char" : "wchar_t") + " to " + (is_output_char ? "char" : "wchar_t"));
        }

        assert(newlen <= input.length() * max_widen);

        // Remove left chars from buffer
        output.resize(newlen);
        
        // Output result
        return output;
    }
}

std::string LunaLua::EncodeUtils::WStr2Str(const std::wstring & wstr)
{
    return LunaLua::EncodeUtils::WStr2Str(std::wstring_view(wstr));
}

std::string LunaLua::EncodeUtils::WStr2Str(const std::wstring_view & wstr)
{
    return LunaLua::EncodeUtils::detail::EncodeConvert<CP_UTF8, char>(wstr);
}

std::wstring LunaLua::EncodeUtils::Str2WStr(const std::string & str)
{
    return LunaLua::EncodeUtils::Str2WStr(std::string_view(str));
}

std::wstring LunaLua::EncodeUtils::Str2WStr(const std::string_view & str)
{
    return LunaLua::EncodeUtils::detail::EncodeConvert<CP_UTF8, wchar_t>(str);
}

std::string LunaLua::EncodeUtils::WStr2StrA(const std::wstring & wstr)
{
    return LunaLua::EncodeUtils::WStr2StrA(std::wstring_view(wstr));
}

std::string LunaLua::EncodeUtils::WStr2StrA(const std::wstring_view & wstr)
{
    return LunaLua::EncodeUtils::detail::EncodeConvert<CP_ACP, char>(wstr);
}

std::wstring LunaLua::EncodeUtils::StrA2WStr(const std::string & str)
{
    return LunaLua::EncodeUtils::StrA2WStr(std::string_view(str));
}

std::wstring LunaLua::EncodeUtils::StrA2WStr(const std::string_view & str)
{
    return LunaLua::EncodeUtils::detail::EncodeConvert<CP_ACP, wchar_t>(str);
}
