#include <catch.hpp>

#include <string>
#include <Utils/EncodeUtils.h>

TEST_CASE("UTF-8 chars to UTF-16", "[lunalua-utils-encode]") {
    // צהü
    const char* char_utf8 = u8"צהü";
    std::wstring wchar_utf8 = LunaLua::EncodeUtils::Str2WStr(std::string_view(char_utf8));
    
    REQUIRE(wchar_utf8.length() == 3u);
    REQUIRE(wchar_utf8 == L"צהü");
}

TEST_CASE("UTF16 chars to UTF-8", "[lunalua-utils-encode]")
{
    const wchar_t* wchar_utf16 = L"צהü";
    std::string char_utf8 = LunaLua::EncodeUtils::WStr2Str(std::wstring_view(wchar_utf16));

    REQUIRE(char_utf8.length() == 6u);
    REQUIRE(char_utf8 == u8"צהü");

    // Also test BMP
    const wchar_t* bmp_wchar_utf16 = L"€";
    std::string bmp_char_utf8 = LunaLua::EncodeUtils::WStr2Str(std::wstring_view(bmp_wchar_utf16));

    REQUIRE(bmp_char_utf8.length() == 3u);
    REQUIRE(bmp_char_utf8 == u8"€");
}

TEST_CASE("ANSI chars to UTF-16", "[lunalua-utils-encode]")
{
    const char* char_ansi = "ÿÿÿ";
    std::wstring wchar_utf16 = LunaLua::EncodeUtils::StrA2WStr(std::string_view(char_ansi));
    REQUIRE(wchar_utf16.length() == 3u);
    REQUIRE(wchar_utf16 == L"ÿÿÿ");
}

TEST_CASE("UTF-16 chars to ANSI", "[lunalua-utils-encode]")
{
    const wchar_t* wchar_utf16 = L"ÿÿÿ";
    std::string char_ansi = LunaLua::EncodeUtils::WStr2StrA(std::wstring_view(wchar_utf16));
    REQUIRE(char_ansi.length() == 3u);
    REQUIRE(char_ansi == "ÿÿÿ");

}


