#include <catch.hpp>

#include <string>
#include <Utils/EncodeUtils.h>

TEST_CASE("Tests all encoding", "[lunalua-utils-encode]") {
    // צהü
    const char* char_utf8 = "\xC3\xB6\xC3\xA4\xC3\xBC\x00";
    std::wstring wchar_utf8 = LunaLua::EncodeUtils::Str2WStr(std::string_view(char_utf8));
    
    REQUIRE(wchar_utf8 == L"צהü");
}


