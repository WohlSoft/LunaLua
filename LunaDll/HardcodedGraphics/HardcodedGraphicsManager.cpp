
#include <fstream>
#include <iostream>
#include <sstream>

#include "HardcodedGraphicsManager.h"
#include "../Defines.h"
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../SdlMusic/MusicManager.h" //Need to get SMBX's application path
#include <IniProcessor/ini_processing.h> //Ini files reader


std::string HardcodedGraphicsManager::root="";

HardcodedGraphicsManager::HardcodedGraphicsManager()
{}


HardcodedGraphicsManager::~HardcodedGraphicsManager()
{}

int HardcodedGraphicsManager::patchGraphics(unsigned int offset_i, const char* filepath)
{
    //Check existing of address
    std::map<unsigned int, int>::iterator it = hardcoded_data_map.find(offset_i);
    if(it == hardcoded_data_map.end())
    {
        return -3;
    }

    //get internal memory size
    int sizeOfMemory = hardcoded_data_map[offset_i]-8;

    std::ifstream graphFile;
    graphFile.open(filepath, std::ios::in|std::ios::binary);
    if(!graphFile.is_open())
        return -2;

    graphFile.seekg(0, graphFile.end);
    std::streamoff length = graphFile.tellg();
    graphFile.seekg(0, graphFile.beg);

    if(length > sizeOfMemory)
    {
        graphFile.close();
        return sizeOfMemory;
    }

    char* buffer = new char[sizeOfMemory];
    for(int i=0;i<sizeOfMemory;i++)
    {
        buffer[i] = 0;
    }
    graphFile.read(buffer, length);
    // The actual patching
    void* addr = FN_OFFSET2ADDR(offset_i);
    memset(addr, 0, sizeOfMemory);
    memcpy(addr, buffer, sizeOfMemory);

    // finalize
    graphFile.close();
    delete buffer;
    return -1;
}

void HardcodedGraphicsManager::loadIniImage(unsigned int hex, unsigned int hex_m,
                                            IniProcessing &ini, std::string value)
{
    std::string imageFile;

    ini.read(value.c_str(), imageFile, "");

    if(imageFile.empty())
        return;

    std::string imageFile_src = imageFile;
    imageFile = root + "graphics\\common\\" + imageFile;

    const char *str = imageFile.c_str();

    //Here we should load file  hexKey - address, imageFile:
    //is a name of image file in the <SMBX>\graphics\common
    patchGraphics(hex, str);

    //if mask no used - abort
    if(hex_m == 0) return;

    //get filename of mask
    imageFile = imageFile_src;
    for(int i = imageFile.size() - 1; i > 0; i--)
    {
        if(imageFile[i]=='.')
        {
            imageFile.insert(i, "m");
            break;
        }
    }

    imageFile = root + "graphics\\common\\" + imageFile;
    const char *str2 = imageFile.c_str();
    patchGraphics(hex_m, str2);
}

void HardcodedGraphicsManager::loadGraphics()
{
    if(hardcoded_data_map.empty())
        buildMap();

    std::wstring smbxPath = gAppPathWCHAR;
    smbxPath = smbxPath.append(L"\\");
    root = WStr2Str(smbxPath);

    std::string ttscrpath = root + "graphics.ini";
    if(!file_existsX(ttscrpath))
        return;

    IniProcessing graphicsINI(ttscrpath);
    if(!graphicsINI.isOpened())
    {
        LunaMsgBox::ShowA(0, std::string(ttscrpath + "\n\nError of read INI file").c_str(), "Error", 0);
        return;
    }

    //Splash
    graphicsINI.beginGroup("splash");
    loadIniImage(0x000ca018, 0, graphicsINI, "game");
    loadIniImage(0x00032a21, 0, graphicsINI, "editor");
    graphicsINI.endGroup();

    //Title
    graphicsINI.beginGroup("title");
    loadIniImage(0x002f460c, 0x002f3a8d, graphicsINI, "title");
    graphicsINI.endGroup();

    /*
    ;Note: Mask should be detected automatically (file with m suffix like other stuff)
    [splash]
    loading=big_splash_loading.gif //000d944b
    coin_ani=editor_splash_coin.gif//000d9010
    coin_ani=coin.gif//002dfe6d, 002df9d4

    [title]
    curtain=curtain.gif //002f6733, 002e9beb
    www=www.gif//002f336b,002f2d5a
    selector_red=selector_red.gif
    selector_green=selector_green.gif//002f9742
    scroll_up=scroll_up.gif//002fb1c1, 002fb568
    scroll_down=scroll_down.gif//002faa77, 002fae1e
    worldmap_back=world_back.gif

    [HUD]
    itemslot=itemslot.gif
    itemslot_p1=itemslot_p1.gif
    itemslot_p2=itemslot_p2.gif
    heard_on=heart.gif//002fa65e
    heard_off=heart_n.gif//002fa247
    star=stars.gif
    key=key.gif
    coin=coins.gif
    bomb=bombs.gif
    rupee=emerald.gif//002f8c65, 002f88d4
    1up=1up.gif
    2up=2up.gif//002f84ed, 002f810a

    [yoshi]
    tongue=yoshi_tongue.gif
    tongue_line=yoshi_tongue_line.gif
    wings=wings.gif

    [shoes]
    green=boot_green.gif
    red=boot_red.gif
    blue=boot_blue.gif

    [clowncar]
    clowncar=clowncar.gif

    [cursors]
    white=cursor_white.gif
    blue=cursor_blue.gif
    cyan=cursor_cyan.gif
    gray=cursor_gray.gif
    green=cursor_green.gif
    magenta=cursor_magenta.gif
    violet=cursor_violet.gif
    red=cursor_red.gif
    yellow=cursor_yellow.gif
    rubber=cursor_rubber.gif

    [battle]
    versus=btl_vs.gif
    mario=btl_mario.gif
    luigi=btl_luigi.gif
    peach=btl_peach.gif
    toad=btl_toad.gif
    link=btl_link.gif
    wins=btl_wins.gif

    [misc]
    warp_point=warp.gif
    messagebox=message_box.gif
    npc_talkable=nps_message.gif
    */

    /******************Hex section************************************/
    graphicsINI.beginGroup("hex");

    std::vector<std::string> list = graphicsINI.allKeys();
    for(std::string &l : list)
    {
        bool wrong = false;
        RemoveSubStr(l, "0x");

        for(char &c : l)
        {
            if(!isdigit(c))
            {
                if((c != 'a')&&(c != 'A')&&
                   (c != 'b')&&(c != 'B')&&
                   (c != 'c')&&(c != 'C')&&
                   (c != 'd')&&(c != 'D')&&
                   (c != 'e')&&(c != 'E')&&
                   (c != 'f')&&(c != 'F'))
                {
                    wrong = true;
                    break;
                }
            }
        }

        if(wrong)
            continue;

        unsigned int hexKey;
        std::stringstream ss;
        ss << std::hex << l;
        ss >> hexKey;

        std::string imageFile;
        graphicsINI.read(l.c_str(), imageFile, "");
        if(imageFile.empty())
            continue;

        imageFile = root + "graphics\\common\\" + imageFile;
        const char *str = imageFile.c_str();

        //Here we should load file  hexKey - address, imageFile:
        //is a name of image file in the <SMBX>\graphics\common
        patchGraphics(hexKey, str);
    }
    graphicsINI.endGroup();
    /******************Hex section**end*******************************/

}
