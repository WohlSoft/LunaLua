#include "../LuaProxy.h"
#include <stdio.h>
#include <string.h>
#include "../../GlobalFuncs.h"

LuaProxy::Logger::Logger(std::string filename)
{
    std::string custPath = wstr2str(getCustomFolderPath());
    filePath = custPath + "\\" + filename;
    file = fopen(filePath.c_str(), "a");
}

LuaProxy::Logger::~Logger()
{
    if (file) fclose(file);
}

void LuaProxy::Logger::write(const char*line)
{
    if (file)
    {
        fprintf(file, "%s\n", line);
        fflush(file);
    }
}


