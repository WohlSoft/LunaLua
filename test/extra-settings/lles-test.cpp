
#include <FileManager/config_manager.h>

static const char *test_str1 =
"{"
"   \"checkMe\": true,"
"   \"checkMeNot\": true,"
"   \"choice\": 2,"
"   \"choice2\": 22,"
"   \"floatOfWhere\": {"
"       \"x\": 8.02,"
"       \"y\": -12.1"
"   },"
"   \"floatingBrick\": {"
"       \"h\": 34.2,"
"       \"w\": 36.52"
"   },"
"   \"sizeOfMyPlant\": {"
"       \"h\": 34,"
"       \"w\": 35"
"   },"
"   \"teaColor\": \"#9ece0404\","
"   \"whereIsMySombrero\": {"
"       \"x\": 9,"
"       \"y\": 12"
"   }"
"}";

static const char *test_str2 =
"{"
"   \"choice\": 2,"
"   \"choice2\": 22,"
"   \"floatingBrick\": {"
"       \"h\": 34.2,"
"       \"w\": 36.52"
"   },"
"   \"teaColor\": \"#9ece0404\","
"   \"whereIsMySombrero\": {"
"       \"x\": 9,"
"       \"y\": 12"
"   }"
"}";

int main(int argc, char**argv)
{
    if(argc <= 3)
        return 1;

    ConfigPackMiniManager ku;

    ku.setEpisodePath(argv[2]);
    ku.setCustomPath(argv[3]);
    ku.loadConfigPack(argv[1]);

    {
        std::string res1 = ku.mergeLocalExtraSettings(ConfigPackMiniManager::NPC, 1000, test_str1, true);
        printf("Merging source:\n"
               "------------------\n"
               "%s\n"
               "-----------------\n",
               test_str1);
        printf("Result is:\n"
               "------------------\n"
               "%s\n"
               "-----------------\n\n",
               res1.c_str());
    }

    {
        std::string res1 = ku.mergeLocalExtraSettings(ConfigPackMiniManager::NPC, 1000, test_str2, true);
        printf("Merging source:\n"
               "------------------\n"
               "%s\n"
               "-----------------\n",
               test_str2);
        printf("Result is:\n"
               "------------------\n"
               "%s\n"
               "-----------------\n\n",
               res1.c_str());
    }

    {
        std::string res1 = ku.mergeLocalExtraSettings(ConfigPackMiniManager::NPC, 1000, "", true);
        printf("Merging source:\n"
               "------------------\n"
               "<blank string>\n"
               "-----------------\n");
        printf("Result is:\n"
               "------------------\n"
               "%s\n"
               "-----------------\n\n",
               res1.c_str());
    }

    return 0;
}
