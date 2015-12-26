#ifndef AsyncHTTPClient_hhhhh
#define AsyncHTTPClient_hhhhh

#include <memory>
#include <atomic>
#include <vector>
#include <condition_variable>
#include <thread>
#include <mutex>

class AsyncHTTPClient : public std::enable_shared_from_this<AsyncHTTPClient>
{
public:
    enum AsyncHTTPMethod {
        HTTP_GET,
        HTTP_POST
    };

    enum AsyncHTTPStatus {
        HTTP_READY,
        HTTP_PROCESSING,
        HTTP_FINISHED
    };
private:
    // Main URL
    std::string m_url;
    // Arguments to be passed
    std::vector<std::pair<std::string, std::string>> m_args;
    // Method
    AsyncHTTPMethod m_method;
    
    // Current status
    std::atomic<AsyncHTTPStatus> m_currentStatus;

    // Response
    std::string m_response;
    int m_responseCode;
    
    std::thread m_asyncSendWorkerThread;
    std::condition_variable m_finishNotifier;
    std::mutex m_asyncSendMutex;
    void asyncSendWorker();
public:
    //////////////////////////////////////////////////////////////////////////
    //                           General Functions                          //
    //////////////////////////////////////////////////////////////////////////
    AsyncHTTPClient();
    ~AsyncHTTPClient();
    AsyncHTTPStatus getStatus() const;
    AsyncHTTPClient::AsyncHTTPMethod getMethod() const { return m_method; }
    void setMethod(AsyncHTTPClient::AsyncHTTPMethod val) 
    { 
        // You can only change the method before
        if (m_currentStatus != HTTP_READY)
            return;
        m_method = val; 
    }


    //////////////////////////////////////////////////////////////////////////
    //                           Prepare Functions                          //
    //////////////////////////////////////////////////////////////////////////
    void addArgument(const std::string& argName, const std::string& content);
    void setUrl(const std::string& url);
    std::string getUrl() const;
    

    //////////////////////////////////////////////////////////////////////////
    //                           Processing Functions                       //
    //////////////////////////////////////////////////////////////////////////
    void asynSend();
    void waitUntilResponse();
    

    //////////////////////////////////////////////////////////////////////////
    //                           -FINISHED- Functions                       //
    //////////////////////////////////////////////////////////////////////////
    std::string getResponseData()  const;
    int getStatusCode()  const;
};



#endif
