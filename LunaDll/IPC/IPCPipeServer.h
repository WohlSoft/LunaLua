#pragma once

#include <string>
#include <unordered_map>
#include <exception>
#include <mutex>
#include "../libs/json/json.hpp"

class IPCPipeServer
{
public:
    typedef nlohmann::json(*IPCCallback)(const nlohmann::json& params);

private:
    void ReadThread();

    void RunCallback(IPCCallback cb, const nlohmann::json& params, const nlohmann::json& id);

    void SendMsg(const nlohmann::json& pkt);
    void SendMsgString(const std::string& pkt);
    std::string ReadMsgString();
    nlohmann::json MakeJsonError(int errCode, const std::string& errStr, const nlohmann::json& id = nullptr);
    void SendJsonError(int errCode, const std::string& errStr, const nlohmann::json& id = nullptr);

public:
    IPCPipeServer();
    ~IPCPipeServer();

    // Call during startup to attach Stdin and Stdout to this
    void AttachStdinStdout();

    // Register a callback...
    void RegisterMethod(std::string str, IPCCallback cb);

    // Send notification
    void SendSimpleNotification(const std::string& notificationCmd);

private:
    int mInFD;
    int mOutFD;
    std::unordered_map<std::string, IPCCallback> mCallbacks;
    std::mutex mSendMutex;
};

class IPCInvalidParams : public std::exception
{
public:
    IPCInvalidParams() :
        std::exception()
    {
    };
    ~IPCInvalidParams() {};
};

extern IPCPipeServer gIPCServer;
