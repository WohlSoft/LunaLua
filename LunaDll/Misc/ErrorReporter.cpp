#include <windows.h>
#include "ErrorReporter.h"
#include <dbghelp.h>
#include "../Globals.h"
#include "../GlobalFuncs.h"
#include "../Defines.h"
#include <array>
#include "Gui/GuiCrashNotify.h"
#include "../SMBXInternal/Variables.h"

/*static*/ ErrorReport::CrashContext* volatile ErrorReport::CrashContext::m_curContext = nullptr;

namespace ErrorReportVars
{
    std::string lastErrDesc;
    ErrorReport::VB6ErrorCode lastVB6ErrCode;
    CONTEXT lastVB6ErrContext;
    bool pendingVB6ErrContext = false;
    bool activeVB6ErrContext = false;
}

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
    errDesc += "VB6 Error Code: " + std::to_string(static_cast<int>(errCode));

    switch (errCode)
    {
    case ErrorReport::VB6ERR_INVCALLARG:
        errDesc += " (Invalid call or argument)\n";
        break;
    case ErrorReport::VB6ERR_OVERFLOW:
        errDesc += " (Overflow)\n";
        break;
    case ErrorReport::VB6ERR_OUTOFMEMORY:
        errDesc += " (Out of Memory)\n";
        break;
    case ErrorReport::VB6ERR_OUTOFRANGE:
        errDesc += " (Subscript out of range)\n";
        break;
    case ErrorReport::VB6ERR_DIVBYZERO:
        errDesc += " (Division by zero)\n";
        break;
    case ErrorReport::VB6ERR_TYPEMISMATCH:
        errDesc += " (Type mismatch)\n";
        break;
    case ErrorReport::VB6ERR_FILENOTFOUND:
        errDesc += " (File not found)\n";
        break;
    case ErrorReport::VB6ERR_INPUTPASTEOF:
        errDesc += " (Input past end of file)\n";
        break;
    case ErrorReport::VB6ERR_PATHNOTFOUND:
        errDesc += " (Path not found)\n";
        break;
    case ErrorReport::VB6ERR_OBJVARNOTSET:
        errDesc += " (Object variable not set)\n";
        break;
    default:
        errDesc += "\n";
        break;
    }
    return errDesc;
}

void ErrorReport::manageErrorReport(const std::string &url, std::string &errText)
{
    GuiCrashNotify notifier(errText);
    notifier.show();

    if (notifier.shouldSend()){
        // sendPUTRequest(url, errText);
    }
}

static_assert(EXCEPTION_FLT_INEXACT_RESULT == 0xc000008f, "BOO");

void ErrorReport::SnapshotError(EXCEPTION_RECORD* exception, CONTEXT* context)
{
    bool isVB6Exception = (exception->ExceptionCode == EXCEPTION_FLT_INEXACT_RESULT);
    std::string stackTrace = generateStackTrace(
        (isVB6Exception && ErrorReportVars::activeVB6ErrContext) ? 
            &ErrorReportVars::lastVB6ErrContext :
            context);
    std::stringstream fullErrorDescription;

    fullErrorDescription << "== Crash Summary ==\n";
    fullErrorDescription << "LunaLua Version: " + std::string(LUNALUA_VERSION) + "\n";
#ifdef __clang__
    fullErrorDescription << "This LunaLua build has been compiled with Clang. Support for this compiler is still experimental so errors might happen.\n";
#endif
    fullErrorDescription << "Exception Code: 0x" << std::hex << exception->ExceptionCode;

    switch (exception->ExceptionCode)
    {
    case EXCEPTION_FLT_INEXACT_RESULT:
        fullErrorDescription << " (VB Error)\n";
        break;
    case EXCEPTION_ACCESS_VIOLATION:
        fullErrorDescription << " (Access Violation)\n";
        break;
    case EXCEPTION_STACK_OVERFLOW:
        fullErrorDescription << " (Stack Overflow)\n";
        break;
    default:
        fullErrorDescription << "\n";
        break;
    }

    if (isVB6Exception) {
        fullErrorDescription << getCustomVB6ErrorDescription(ErrorReportVars::lastVB6ErrCode);
    }

    CrashContext* ctx = CrashContext::Get();
    if (ctx)
    {
        fullErrorDescription << "Context: " << ctx->asText() << "\n";
    }

    fullErrorDescription << "\n== Stack Trace ==\n";
    fullErrorDescription << stackTrace;

    fullErrorDescription << "\n== Counts ==\n";
    fullErrorDescription << std::dec;
    fullErrorDescription << "numWater=" << SMBX13::Vars::numWater << "\n";
    fullErrorDescription << "numWarps=" << SMBX13::Vars::numWarps << "\n";
    fullErrorDescription << "numBlock=" << SMBX13::Vars::numBlock << "\n";
    fullErrorDescription << "numBackground=" << SMBX13::Vars::numBackground << "\n";
    fullErrorDescription << "numNPCs=" << SMBX13::Vars::numNPCs << "\n";
    fullErrorDescription << "numEffects=" << SMBX13::Vars::numEffects << "\n";
    fullErrorDescription << "numPlayers =" << SMBX13::Vars::numPlayers << "\n";
    fullErrorDescription << std::hex;

    fullErrorDescription << "\n== Reporting ==\n";
    fullErrorDescription << "If you like to help us finding the error then please post this log at:\n";
    fullErrorDescription << "* The Codehaus Discord server or\n";
    fullErrorDescription << "* https://www.smbxgame.com/forums/viewforum.php?f=35 or\n";
    fullErrorDescription << "* https://talkhaus.raocow.com/viewforum.php?f=36\n";
    fullErrorDescription << "\n";

    ErrorReportVars::lastErrDesc = fullErrorDescription.str();
}

void ErrorReport::report()
{
    manageErrorReport("http://wohlsoft.ru/LunaLuaErrorReport/index.php", ErrorReportVars::lastErrDesc);
    writeErrorLog(ErrorReportVars::lastErrDesc);
}
