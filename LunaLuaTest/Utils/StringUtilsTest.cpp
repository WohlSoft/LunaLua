#include <catch.hpp>

#include <Utils/StringUtils.h>

TEST_CASE("Tests split", "[lunalua-utils-string]") {
    // Test with simple string
    const char* text = "Hello;-;good;-;world!";
    auto splitted = LunaLua::StringUtils::split(text, ";-;");
    REQUIRE(splitted[0] == "Hello");
    REQUIRE(splitted[1] == "good");
    REQUIRE(splitted[2] == "world!");

    // Test with wchar
    const wchar_t* wideText = L"Hi, mate!";
    auto splittedWide = LunaLua::StringUtils::split(wideText, L", ");
    REQUIRE(splittedWide[0] == L"Hi");
    REQUIRE(splittedWide[1] == L"mate!");
}

TEST_CASE("Tests replace", "[lunalua-utils-string]") {
    std::string text = "Replace me!";
    LunaLua::StringUtils::replace(text, "me", "you");
    REQUIRE(text == "Replace you!");

    std::wstring wideText = L"Replace very very many words!";
    LunaLua::StringUtils::replace(wideText, L"very", L"few");
    REQUIRE(wideText == L"Replace few few many words!");
}

TEST_CASE("Test command argument split", "[lunalua-utils-string]") {
    const char* text = "--some-argument-1 --fav-food=\"foo bar\" --some-argument-2";
    auto splitted = LunaLua::StringUtils::splitCmdArgs(text);
    REQUIRE(splitted[0] == "--some-argument-1");
    REQUIRE(splitted[1] == "--fav-food=foo bar");
    REQUIRE(splitted[2] == "--some-argument-2");

    const wchar_t* wideText = L"--arg-1 --arg-2";
    auto splittedWide = LunaLua::StringUtils::splitCmdArgs(wideText);
    REQUIRE(splittedWide[0] == L"--arg-1");
    REQUIRE(splittedWide[1] == L"--arg-2");
}

