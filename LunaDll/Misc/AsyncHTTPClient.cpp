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

void AsyncHTTPClient::asyncSendWorker()
{
    // To prevent the destructor from beeing executed.
    std::shared_ptr<AsyncHTTPClient> lock = shared_from_this();

    // FIXME: Only apply arg list in URL for GET, otherwise pass as data.
    std::string fullArgList = "";
    for (const std::pair<std::string, std::string>& argPack : m_args) {
        fullArgList += argPack.first + "=" + url_encode(argPack.second) + "&";
    }
    if (fullArgList.length() > 0) {
        fullArgList = fullArgList.substr(0, fullArgList.length() - 1); // To remove the extra "&"
    }

   
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

    HRESULT hr;
    CLSID clsid;
    IWinHttpRequest *pIWinHttpRequest = NULL;

    _variant_t varFalse(false);
    _variant_t varData(fullArgList.c_str());
    _variant_t varContent("");
    if (m_method == HTTP_POST)
        varContent = fullArgList.c_str();
    
    
    hr = CLSIDFromProgID(L"WinHttp.WinHttpRequest.5.1", &clsid);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(clsid, NULL,
            CLSCTX_INPROC_SERVER,
            IID_IWinHttpRequest,
            (void **)&pIWinHttpRequest);
    }

    if (SUCCEEDED(hr)) {
        hr = pIWinHttpRequest->SetTimeouts(1000, 1000, 2000, 1000);
    }

    if (SUCCEEDED(hr)) {
        _bstr_t method(method.c_str());
        _bstr_t url;
        if (m_method == HTTP_GET) {
            url = ((m_url + (fullArgList.empty() ? "" : "?") + fullArgList).c_str());
        }
        else 
        {
            url = m_url.c_str();
        }
        hr = pIWinHttpRequest->Open(method, url, varFalse);
    }

    if (m_method == HTTP_POST) {
        if (SUCCEEDED(hr)) {
            hr = pIWinHttpRequest->SetRequestHeader(bstr_t("Content-Type"), bstr_t("application/x-www-form-urlencoded"));
        }
    }

    if (SUCCEEDED(hr)) {
        hr = pIWinHttpRequest->Send(varContent);
        if ((hr & 0xFFFF) == ERROR_WINHTTP_TIMEOUT)
            m_responseCode = 408; // If timeout then set the HTTP response code.
    }

    if (SUCCEEDED(hr)) {
        LONG statusCode = 0;
        hr = pIWinHttpRequest->get_Status(&statusCode);
        m_responseCode = statusCode;
    }

	_variant_t responseRaw("");
    if (SUCCEEDED(hr)) {
        if (m_responseCode == 200) {
			hr = pIWinHttpRequest->get_ResponseBody(&responseRaw);
        }
        else {
            hr = E_FAIL;
        }
    }

    if (SUCCEEDED(hr)) {
		// body
		long upperBounds;
		long lowerBounds;
		byte* buff;
		if (responseRaw.vt == (VT_ARRAY | VT_UI1)) {
			long dims = SafeArrayGetDim(responseRaw.parray);

			if (dims == 1) {
				SafeArrayGetLBound(responseRaw.parray, 1, &lowerBounds);
				SafeArrayGetUBound(responseRaw.parray, 1, &upperBounds);
				upperBounds++;

				SafeArrayAccessData(responseRaw.parray, (void**)&buff);
				m_response = std::string(reinterpret_cast<const char *>(buff), upperBounds-lowerBounds);
				SafeArrayUnaccessData(responseRaw.parray);
			}
		}

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

    m_currentStatus.store(HTTP_PROCESSING, std::memory_order_relaxed);
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

