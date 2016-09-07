/*
LunaHexer - a tool which physically patches raw SMBX.EXE file with loading dll file of the LunaLUA
*/
#include <stdio.h>
#include <memory.h>
#include <string.h>

void patch(FILE* f, unsigned int at, void* data, unsigned int size)
{
    fseek(f, at, SEEK_SET);
    fwrite(data, 1, size, f);
}

void patchAStr(FILE* f, unsigned int at, char* str, unsigned int maxlen)
{
    char data[maxlen];
    memset(data, 0, maxlen);
    int i;
    int len = strlen(str);
    for(i=0; (i<len) && (i<maxlen-1); i++)
    {
        data[i] = str[i];
    }
    fseek(f, at, SEEK_SET);
    fwrite(data, 1, maxlen, f);
}

void patchUStr(FILE* f, unsigned int at, char* str, unsigned int maxlen)
{
    char data[maxlen];
    memset(data, 0, maxlen);
    int i, j;
    int len = strlen(str);
    for(i=0, j=0; (i<len) && (j<maxlen); i++, j+=2)
    {
        data[j] = str[i];
        data[j+1] = 0;
    }
    fseek(f, at, SEEK_SET);
    fwrite(data, 1, maxlen, f);
}

char lunaPatch[132] =
{
    0x1C, 0x40, 0x72, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x70, 0x42, 0x72, 0x00, 0x00, 0x10, 0x00, 0x00,
    0x79, 0x60, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3C, 0x60, 0x74, 0x00, 0x69, 0x60, 0x74, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x4C, 0x75, 0x6E, 0x61, 0x44, 0x6C, 0x6C, 0x2E, 0x64, 0x6C,
    0x6C, 0x00, 0x00, 0x00, 0x48, 0x55, 0x44, 0x48, 0x6F, 0x6F,
    0x6B, 0x00, 0x00, 0x00, 0x4F, 0x6E, 0x4C, 0x76, 0x6C, 0x4C,
    0x6F, 0x61, 0x64, 0x00, 0x00, 0x00, 0x54, 0x65, 0x73, 0x74,
    0x46, 0x75, 0x6E, 0x63, 0x00, 0x48, 0x60, 0x74, 0x00, 0x52,
    0x60, 0x74, 0x00, 0x5E, 0x60, 0x74, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x48, 0x60, 0x74, 0x00, 0x52, 0x60, 0x74, 0x00, 0x5E,
    0x60, 0x74
};

#ifdef _WIN32
typedef wchar_t LUNACHAR;
#define LunaFOPEN _wfopen
#define LunaR L"rb"
#define LunaW L"wb"
#else
typedef char LUNACHAR;
#define LunaFOPEN fopen
#define LunaR "rb"
#define LunaW "wb"
#endif

int patchSMBX(const LUNACHAR* srcexe, const LUNACHAR* dstexe)
{
    char ch=0; char null[4096];
    memset(null, 0, 4096);
    FILE *src = LunaFOPEN(srcexe, LunaR);
    FILE *dst = LunaFOPEN(dstexe, LunaW);
    if(src == NULL)
        return 1;
    if(dst == NULL)
        return 2;
    //Copy original SMBX.EXE into the new file
    while(fread(&ch, 1, 1, src)==1)
        fwrite(&ch, 1, 1, dst);
    fclose(src);

    patch(dst, 0xBE,  "\x04", 1);
    patch(dst, 0x109, "\x70", 1);
    patch(dst, 0x138, "\x00\x60\x74\x00\x3C", 5);
    patch(dst, 0x188, "\x00\x00\x00\x00\x00", 5);
    patch(dst, 0x1D7, "\xE0", 1);
    patch(dst, 0x228, "\x2E\x4E\x65\x77\x49\x54\x00\x00\x89", 9);
    patch(dst, 0x235, "\x60\x74\x00\x00\x10\x00\x00\x00\xD0\x72\x00\x00\x00\x00\x00", 15);
    patch(dst, 0x24F, "\xC0", 1);
    patchUStr(dst, 0x27614, "LunaLUA-SMBX Version 1.3.0.2 http://wohlsoft.ru", 124);
    patchAStr(dst, 0x67F6A, "LunaLUA-SMBX Version 1.3.0.2 http://wohlsoft.ru", 63);
    patchAStr(dst, 0xA1FE3, "LunaLUA-SMBX Version 1.3.0.2 http://wohlsoft.ru", 78);
    patchAStr(dst, 0xC9FC0, "LunaLUA-SMBX Version 1.3.0.2 http://wohlsoft.ru", 65);

    patchUStr(dst, 0x31A34, "about:blank", 82);//Kill annoying web viewer!

    #ifdef OVERRIDE_VERSIONINFO_1301 //in Redigit's 1.3 exe version info offset is different
    patchUStr(dst, 0x72C584, "Hacked with LunaLUA", 46);//Comment
    patchUStr(dst, 0x72C5D4, "WohlSoft Team", 46);//Company
    patchUStr(dst, 0x72C62C, "www.wohlsoft.ru", 46);//File description
    patchUStr(dst, 0x72C680, "sucks!", 54);//Copyright
    patchUStr(dst, 0x72C6E4, "triple sucks!", 54);//Trade marks
    patchUStr(dst, 0x72C740, "LunaLUA-SMBX", 38);//Product name
    patchUStr(dst, 0x72C788, "1.3.0.2", 14);//Version 1
    patchUStr(dst, 0x72C7BC, "1.3.0.2", 14);//Version 2
    #endif
    patch(dst, 0x4CA23B, "\xFF\x15\x71\x60\xB4\x00", 6);
    patch(dst, 0x4D9446, "\xFF\x15\x6D\x60\xB4\x00\x90", 7);
    patch(dst, 0x56C030, "\xFF\x15\x69\x60\xB4\x00", 6);
    patch(dst, 0x72D000, null, 4096);
    patch(dst, 0x72D000, lunaPatch, 132);
    fclose(dst);

    return 0;
}

int main()
{
    #ifdef _WIN32
    const wchar_t* oldEXE = L"smbx.exe";
    const wchar_t* newEXE = L"lunaengine.exe";
    #else
    const char* oldEXE = "smbx.exe";
    const char* newEXE = "lunaengine.exe";
    #endif
    int err = patchSMBX(oldEXE, newEXE);
    if( err == 0)
    {
        wprintf(L"SMBX has been PATCHED!\nTo start LunaLUA-SMBX, open the %s\n", newEXE);
    }
    else
    {
        switch(err)
        {
            case 1: wprintf(L"FAILED TO PATCH: CAN'T OPEN SOURCE FILE %s FOR READ!\n", oldEXE); break;
            case 2: wprintf(L"FAILED TO PATCH: CAN'T OPEN TARGET FILE %s FOR WRITE!\n", newEXE); break;
        }
    }
    return err;
}
