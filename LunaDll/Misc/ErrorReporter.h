#ifndef ErrorReporter_hhh
#define ErrorReporter_hhh

#include <string>
#include "../libs/stackwalker/StackWalker.h"



namespace ErrorReport{

    class CustomStackTracer : public StackWalker 
    {
    public:
        CustomStackTracer() : theOutput(""), StackWalker() {}
        
        std::string theOutput;
    protected:
        virtual void OnOutput(LPCSTR szText)
        {
            theOutput += std::string(szText);
        }
    };


    enum VB6ErrorCode{
        VB6ERR_OVERFLOW = 6,
        VB6ERR_OUTOFMEMORY = 7,
        VB6ERR_OUTOFRANGE = 9,
        VB6ERR_DIVBYZERO = 11,
        VB6ERR_TYPEMISMATCH = 13
    };
    void manageErrorReport(const std::string &url, const std::string &errText);
    void writeErrorLog(const std::string &text);

    std::string generateStackTrace();
    std::string getCustomVB6ErrorDescription(VB6ErrorCode errCode);

    //USE THIS METHOD TO REPORT ERRORS
    void SnapshotVB6Error(VB6ErrorCode errCode);
    void report();

}

extern std::string lastErrDesc;
#endif