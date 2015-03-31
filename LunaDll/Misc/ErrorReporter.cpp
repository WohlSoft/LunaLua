#include "ErrorReporter.h"
#include <windows.h>
#include <dbghelp.h>
#include "../GlobalFuncs.h"
#include "../Defines.h"
#include <array>
#include "Gui/GuiCrashNotify.h"

std::string ErrorReport::generateStackTrace()
{
    CustomStackTracer cst;
    cst.ShowCallstack();
    return cst.theOutput;
}

void ErrorReport::writeErrorLog(const std::string &text)
{
    std::string smbxPath = utf8_encode(getModulePath());
    smbxPath += "\\";
    smbxPath += "ERROR_";
    smbxPath += generateTimestampForFilename();
    smbxPath += ".txt";

    writeFile(text, smbxPath);
}


std::string ErrorReport::getCustomVB6ErrorDescription(VB6ErrorCode errCode)
{
    std::string errDesc = "";
    errDesc += "VB6 Error Code: " + std::to_string(static_cast<int>(errCode)) + "\n";

    switch (errCode)
    {
    case ErrorReport::VB6ERR_OVERFLOW:
        errDesc += "VB6 Error Name: Overflow\n";
        errDesc += "This happens when a number gets too big in the memory\n";
        break;
    case ErrorReport::VB6ERR_OUTOFMEMORY:
        errDesc += "VB6 Error Name: Out of Memory\n";
        errDesc += "This happens when you run out of memory.\n";
        break;
    case ErrorReport::VB6ERR_OUTOFRANGE:
        errDesc += "VB6 Error Name: Subscript out of range\n";
        errDesc += "This happens when too many items are in the scenery.\n";
        break;
    case ErrorReport::VB6ERR_DIVBYZERO:
        errDesc += "VB6 Error Name: Division by zero\n";
        break;
    case ErrorReport::VB6ERR_TYPEMISMATCH:
        errDesc += "VB6 Error Name: Type mismatch\n";
        break;
    default:
        errDesc += "VB6 Error Name: Unknown\n";
        break;
    }
    return errDesc;
}

void ErrorReport::manageErrorReport(const std::string &url, const std::string &errText)
{
    GuiCrashNotify notifier(errText);
    notifier.show();
    if (notifier.doSend){
        sendPOSTRequest(url, errText);
    }
}



void ErrorReport::ReportVB6Error(VB6ErrorCode errCode)
{
    std::vector<int> ignoreErrorCodes = { 424 };
    if (std::find(ignoreErrorCodes.begin(), ignoreErrorCodes.end(), static_cast<int>(errCode)) != ignoreErrorCodes.end())
        return;

    std::string fullErrorDescription = "";
    fullErrorDescription += "**************************************************\n";
    fullErrorDescription += "*                  Summary                       *\n";
    fullErrorDescription += "**************************************************\n";
    fullErrorDescription += "SMBX has crashed due an error. See the description for more information!\n";
    fullErrorDescription += "LunaLua Version: " + std::string(LUALUA_VERSION) + "\n";
    fullErrorDescription += std::string("Time/Date: ") + generateTimestamp() + "\n";
    fullErrorDescription += "**************************************************\n";
    fullErrorDescription += "*              Description                       *\n";
    fullErrorDescription += "**************************************************\n";
    fullErrorDescription += getCustomVB6ErrorDescription(errCode);
    fullErrorDescription += "\n";
    fullErrorDescription += "If you like to help us finding the error then please post this log at:\n";
    fullErrorDescription += "* http://engine.wohlnet.ru/forum/ or\n";
    fullErrorDescription += "* http://www.supermariobrosx.org/forums/viewforum.php?f=35 or\n";
    fullErrorDescription += "* http://talkhaus.raocow.com/viewforum.php?f=36\n";
    fullErrorDescription += "\n";
    fullErrorDescription += "**************************************************\n";
    fullErrorDescription += "*              Stacktrace                        *\n";
    fullErrorDescription += "**************************************************\n";
    fullErrorDescription += generateStackTrace();
    
    writeErrorLog(fullErrorDescription);
    //Might make it dynamic in the future by an ini file....
    manageErrorReport("http://engine.wohlnet.ru/LunaLuaErrorReport/index.php", fullErrorDescription);
}


