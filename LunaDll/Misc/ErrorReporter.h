#ifndef ErrorReporter_hhh
#define ErrorReporter_hhh

#include <string>

namespace ErrorReport{

    enum VB6ErrorCode{
        VB6ERR_OVERFLOW = 6,
        VB6ERR_OUTOFMEMORY = 7,
        VB6ERR_OUTOFRANGE = 9,
        VB6ERR_DIVBYZERO = 11,
        VB6ERR_TYPEMISMATCH = 13
    };
    void sendErrorReport(const std::string &url);
    void writeErrorLog(const std::string &text);

    std::string generateStackTrace();
    std::string getCustomVB6ErrorDescription(VB6ErrorCode errCode);

    //USE THIS METHOD TO REPORT ERRORS
    void ReportVB6Error(VB6ErrorCode errCode);

}
#endif