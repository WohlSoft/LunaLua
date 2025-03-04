#ifndef ErrorReporter_hhh
#define ErrorReporter_hhh

#include <string>
#include <sstream>
#include "../libs/stackwalker/StackWalker.h"



namespace ErrorReport{

    class CrashContext
    {
    private:
        static CrashContext* volatile m_curContext;
        CrashContext* m_prevContext;

    public:
        CrashContext(const CrashContext&) = delete;
        CrashContext& operator=(const CrashContext&) = delete;
        CrashContext(CrashContext&&) = default;
        CrashContext& operator=(CrashContext&&) = default;

        static CrashContext* Get()
        {
            return m_curContext;
        }

        CrashContext() : 
            m_prevContext(m_curContext)
        {
            // Set current crash context object
            m_curContext = this;
        }

        ~CrashContext()
        {
            // Destruct current crash context object
            m_curContext = m_prevContext;
        }

        virtual std::string asText() = 0;
    };

    template <typename... Types>
    class CrashContextGeneric : public CrashContext
    {
    private:
        std::tuple<Types...> m_values;

        template<size_t i>
        void fillArgs(std::stringstream& stream)
        {
            fillArgs<i - 1>(stream);
            stream << ", " << std::get<i-1>(m_values);
        }

        template<>
        void fillArgs<1>(std::stringstream& stream)
        {
            stream << std::get<0>(m_values);
        }

        template<>
        void fillArgs<0>(std::stringstream& stream)
        {
        }

    public:
        template <typename... Types>
        CrashContextGeneric(std::tuple<Types...> val) :
            CrashContext(),
            m_values(val)
        {
        }
        ~CrashContextGeneric() {}
        virtual std::string asText()
        {
            std::stringstream stream;
            fillArgs<std::tuple_size<std::tuple<Types...>>::value>(stream);
            return stream.str();
        }
    };

    class CustomStackTracer : public StackWalker 
    {
    public:
        CustomStackTracer() : theOutput(""), StackWalker() {}
        
        std::stringstream theOutput;
    protected:
        virtual void OnOutput(LPCSTR szText)
        {
            theOutput << szText;
        }
    };


    enum VB6ErrorCode{
        VB6ERR_INVCALLARG = 5,
        VB6ERR_OVERFLOW = 6,
        VB6ERR_OUTOFMEMORY = 7,
        VB6ERR_OUTOFRANGE = 9,
        VB6ERR_DIVBYZERO = 11,
        VB6ERR_TYPEMISMATCH = 13,
        VB6ERR_FILENOTFOUND = 53,
        VB6ERR_INPUTPASTEOF = 62,
        VB6ERR_PATHNOTFOUND = 76,
        VB6ERR_OBJVARNOTSET = 91,
    };
    void manageErrorReport(const std::string &url, std::string &errText);
    void writeErrorLog(const std::string &text);

    std::string generateStackTrace(CONTEXT* context = NULL);
    std::string getCustomVB6ErrorDescription(VB6ErrorCode errCode);

    //USE THIS METHOD TO REPORT ERRORS
    void SnapshotError(EXCEPTION_RECORD* exception, CONTEXT* context);
    void report();

}

namespace ErrorReportVars
{
    extern std::string lastErrDesc;
    extern ErrorReport::VB6ErrorCode lastVB6ErrCode;
    extern CONTEXT lastVB6ErrContext;
    extern bool pendingVB6ErrContext;
    extern bool activeVB6ErrContext;
}
#endif