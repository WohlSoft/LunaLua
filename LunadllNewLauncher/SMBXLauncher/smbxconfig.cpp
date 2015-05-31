#include "smbxconfig.h"

SMBXConfig::SMBXConfig(QObject *parent) :
    QObject(parent)
{
    m_Autostart = new AutostartConfig();
}

SMBXConfig::~SMBXConfig()
{
    delete m_Autostart;
}
