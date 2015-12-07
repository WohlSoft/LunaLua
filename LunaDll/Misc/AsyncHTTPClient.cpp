#include "AsyncHTTPClient.h"

#include "../httprequest.h"
#include "../GlobalFuncs.h"

#include <comutil.h>
#include <winhttp.h>


AsyncHTTPClient::AsyncHTTPClient() :
    m_url(""),
    m_currentStatus(AsyncHTTPClient::HTTP_READY),
    m_response(""),
    m_responseCode(0)
{}


AsyncHTTPClient::~AsyncHTTPClient()
{

}

void AsyncHTTPClient::addArgument(const std::string & argName, const std::string & content)
{
    m_args.emplace_back(argName, content);
}

#include <iostream>
#define dbgHR std::cout << "HRESULT = " << std::hex << (DWORD)hr << std::dec << std::endl;
void AsyncHTTPClient::asyncSendWorker()
{
    // To prevent the destructor from beeing executed.
    std::shared_ptr<AsyncHTTPClient> lock = shared_from_this();

    m_currentStatus.store(HTTP_PROCESSING, std::memory_order_relaxed);

    // FIXME: Only apply arg list in URL for GET, otherwise pass as data.
    std::string fullArgList = "";
    for (const std::pair<std::string, std::string>& argPack : m_args) {
        fullArgList += argPack.first + "=" + url_encode(argPack.second) + "&";
    }
    if (fullArgList.length() > 0) {
        fullArgList = fullArgList.substr(0, fullArgList.length() - 1); // To remove the extra "&"
    }

    std::cout << "HTTP Client: Args --> " << fullArgList << std::endl;

    std::string method = "";
    switch (m_method) {
    case HTTP_POST:
        method = "POST";
        break;
    case HTTP_GET:
    default:
        method = "GET";
        break;

    }

    std::cout << "HTTP Client: Method --> " << method << std::endl;


    HRESULT hr;
    CLSID clsid;
    IWinHttpRequest *pIWinHttpRequest = NULL;

    _variant_t varFalse(false);
    _variant_t varData(fullArgList.c_str());
    _variant_t varEmpty("");
    
    std::cout << "HTTP Client: Create Object " << method << std::endl;

    hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1", &clsid);
    dbgHR
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(clsid, NULL,
            CLSCTX_INPROC_SERVER,
            IID_IWinHttpRequest,
            (void **)&pIWinHttpRequest);
    }

    dbgHR
    std::cout << "HTTP Client: Set timeout" << std::endl;

    if (SUCCEEDED(hr)) {
        hr = pIWinHttpRequest->SetTimeouts(1000, 1000, 2000, 1000);
    }

    dbgHR
    std::cout << "HTTP Client: Open connect" << std::endl;
    if (SUCCEEDED(hr)) {
        _bstr_t method(method.c_str());
        _bstr_t url((m_url + (fullArgList.empty() ? "" : "?") + fullArgList).c_str());
        hr = pIWinHttpRequest->Open(method, url, varFalse);
    }

    dbgHR
    std::cout << "HTTP Client: Send data" << std::endl;
    if (SUCCEEDED(hr)) {
        hr = pIWinHttpRequest->Send(varEmpty);
        std::cout << "Err part: " << std::hex << (hr & 0xFFFF) << std::dec << std::endl;
        if ((hr & 0xFFFF) == ERROR_WINHTTP_TIMEOUT)
            m_responseCode = 408; // If timeout then set the HTTP response code.
    }

    dbgHR
    std::cout << "HTTP Client: Get status code" << std::endl;
    if (SUCCEEDED(hr)) {
        LONG statusCode = 0;
        hr = pIWinHttpRequest->get_Status(&statusCode);
        m_responseCode = statusCode;
    }

    dbgHR
    std::cout << "HTTP Client: Get response text (Status code: " << m_responseCode << ")" << std::endl;
    BSTR responseText = 0;
    if (SUCCEEDED(hr)) {
        if (m_responseCode == 200) {
            hr = pIWinHttpRequest->get_ResponseText(&responseText);
        }
        else {
            hr = E_FAIL;
        }
    }

    dbgHR
    std::cout << "HTTP Client: FINISHED!" << std::endl;

    if (SUCCEEDED(hr)) {
        m_response = ConvertBSTRToMBS(responseText);
    }

    m_currentStatus.store(HTTP_FINISHED, std::memory_order_relaxed);
    m_asyncSendWorkerThread.detach(); // To be absolutely safe

    pIWinHttpRequest->Release();
    m_finishNotifier.notify_all();
}



void AsyncHTTPClient::asynSend()
{
    if (getStatus() != HTTP_READY)
        return;
    m_asyncSendWorkerThread = std::thread([this]() { asyncSendWorker(); });
}

std::string AsyncHTTPClient::getResponseData() const
{
    if (getStatus() != HTTP_FINISHED)
        return "";
    return m_response;
}

AsyncHTTPClient::AsyncHTTPStatus AsyncHTTPClient::getStatus() const
{
    return m_currentStatus.load(std::memory_order_relaxed);
}

void AsyncHTTPClient::setUrl(const std::string& url)
{
    if (getStatus() != HTTP_READY)
        return;
    m_url = url;
}

std::string AsyncHTTPClient::getUrl() const
{
    if (getStatus() != HTTP_READY)
        return "";
    return m_url;
}

int AsyncHTTPClient::getStatusCode() const
{
    if (getStatus() != HTTP_FINISHED)
        return 0;
    return m_responseCode;
}

void AsyncHTTPClient::waitUntilResponse()
{
    if (getStatus() != HTTP_PROCESSING)
        return;
    std::unique_lock<std::mutex> lock(m_asyncSendMutex);
    m_finishNotifier.wait(lock);
}

