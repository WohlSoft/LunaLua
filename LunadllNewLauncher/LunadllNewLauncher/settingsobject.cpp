#include "settingsobject.h"

SettingsObject::SettingsObject(QObject *parent) :
    QObject(parent),
    m_noframeskip(false),
    m_nosound(false)
{
}
bool SettingsObject::noframeskip() const
{
    return m_noframeskip;
}

void SettingsObject::setNoframeskip(bool noframeskip)
{
    m_noframeskip = noframeskip;
}
bool SettingsObject::nosound() const
{
    return m_nosound;
}

void SettingsObject::setNosound(bool nosound)
{
    m_nosound = nosound;
}


