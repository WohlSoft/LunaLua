#pragma once

#include <string>
#include <unordered_map>
#include "../libs/json/json.hpp"

class IPCPipeServer
{
private:
    void ReadThread();

    void SendMsgString(const std::string& pkt);
    std::string ReadMsgString();
    void SendJsonError(int errCode, const std::string& errStr);

public:
    typedef nlohmann::json(*IPCCallback)(const nlohmann::json& params);

    IPCPipeServer();
    ~IPCPipeServer();

    // Call during startup to attach Stdin and Stdout to this
    void AttachStdinStdout();

    // Register a callback...
    void RegisterMethod(std::string str, IPCCallback cb);

private:
    int mInFD;
    int mOutFD;
    std::unordered_map<std::string, IPCCallback> mCallbacks;
};
