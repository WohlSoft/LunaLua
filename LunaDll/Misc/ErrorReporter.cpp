#include <windows.h>
#include "ErrorReporter.h"
#include <dbghelp.h>
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Defines.h"
#include <array>
#include "Gui/GuiCrashNotify.h"

std::string lastErrDesc;
ErrorReport::VB6ErrorCode lastVB6ErrCode;
CONTEXT lastVB6ErrContext;

std::string ErrorReport::generateStackTrace(CONTEXT* context)
{
    CustomStackTracer cst;
    cst.ShowCallstack(GetCurrentThread(), context);
    return cst.theOutput.str();
}

void ErrorReport::writeErrorLog(const std::string &text)
{
    std::string smbxPath = gAppPathUTF8;
    smbxPath += "\\logs\\ERROR_";
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

void ErrorReport::manageErrorReport(const std::string &url, std::string &errText)
{
    GuiCrashNotify notifier(errText);
    notifier.show();

    const std::string& username = notifier.getUsername();
    const std::string& usercomment = notifier.getUsercomment();

    errText += "\n\n\nUSERNAME: \n";
    errText += (username.length() == 0 ? "(NONE)" : username);
    errText += "\n\n\nUSERCOMMENT: \n";
    errText += (usercomment.length() == 0 ? "(NONE)" : usercomment);
    errText += "\n";

    if (notifier.shouldSend()){
        sendPUTRequest(url, errText);
    }
}

void ErrorReport::SnapshotError(EXCEPTION_RECORD* exception, CONTEXT* context)
{
    bool isVB6Exception = (exception->ExceptionCode == 0xc000008f);
    std::string stackTrace = generateStackTrace(isVB6Exception ? &lastVB6ErrContext : context);
    std::stringstream fullErrorDescription;

    fullErrorDescription << "**************************************************\n";
    fullErrorDescription << "*                  Summary                       *\n";
    fullErrorDescription << "**************************************************\n";
    fullErrorDescription << "SMBX has crashed due an error. See the description for more information!\n";
    fullErrorDescription << "LunaLua Version: " + std::string(LUNALUA_VERSION) + "\n";
    fullErrorDescription << std::string("Time/Date: ") + generateTimestamp() + "\n";
    fullErrorDescription << "**************************************************\n";
    fullErrorDescription << "*              Description                       *\n";
    fullErrorDescription << "**************************************************\n";
    fullErrorDescription << "Exception code: 0x" << std::hex << exception->ExceptionCode << "\n";
    if (isVB6Exception) {
        fullErrorDescription << getCustomVB6ErrorDescription(lastVB6ErrCode);
    }
    fullErrorDescription << "\n";
    fullErrorDescription << "If you like to help us finding the error then please post this log at:\n";
    fullErrorDescription << "* http://wohlsoft.ru/forum/ or\n";
    fullErrorDescription << "* http://www.supermariobrosx.org/forums/viewforum.php?f=35 or\n";
    fullErrorDescription << "* http://talkhaus.raocow.com/viewforum.php?f=36\n";
    fullErrorDescription << "\n";
    fullErrorDescription << "**************************************************\n";
    fullErrorDescription << "*              Stacktrace                        *\n";
    fullErrorDescription << "**************************************************\n";
    fullErrorDescription << stackTrace;

    lastErrDesc = fullErrorDescription.str();
}

void ErrorReport::report()
{
    manageErrorReport("http://wohlsoft.ru/LunaLuaErrorReport/index.php", lastErrDesc);
    writeErrorLog(lastErrDesc);
}
