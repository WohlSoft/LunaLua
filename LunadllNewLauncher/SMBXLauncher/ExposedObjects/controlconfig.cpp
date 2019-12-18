#include "controlconfig.h"


#include <QFile>
#include <QTextStream>

ControlConfig::ControlConfig(QObject *parent) : ControlConfig("config.dat", parent)
{}

ControlConfig::ControlConfig(const QString &configFilename, QObject *parent) :
    QObject(parent),
    m_configFilename(configFilename)
{
    setDefaults();

    read();
}

void ControlConfig::setDefaults()
{
    m_data.players.clear();
    m_data.meta.ReadFileValid = true;
    m_data.fullScreen = false;
    for (int idx = 0; idx < 2; idx++)
    {
        m_data.players.push_back(SMBX64_ConfigPlayer());
        SMBX64_ConfigPlayer& plCfg = m_data.players[idx];
        plCfg.controllerType = 0;
        plCfg.k_up      = 38;
        plCfg.k_down    = 40;
        plCfg.k_left    = 37;
        plCfg.k_right   = 39;
        plCfg.k_run     = 88;
        plCfg.k_jump    = 90;
        plCfg.k_drop    = 16;
        plCfg.k_pause   = 27;
        plCfg.k_altjump = 65;
        plCfg.k_altrun  = 83;
        plCfg.j_run     = 2;
        plCfg.j_jump    = 0;
        plCfg.j_drop    = 6;
        plCfg.j_pause   = 7;
        plCfg.j_altjump = 1;
        plCfg.j_altrun  = 3;
        plCfg.id = static_cast<unsigned int>(idx + 1);
    }
}

bool ControlConfig::read()
{
    m_data.players.clear();

    if (!QFile::exists(m_configFilename))
    {
        setDefaults();
        //Can't open file to read... but let's return true anyway because we set
        //defaults and that'll be fine to use.
        return true;
    }

    bool ret = FileFormats::ReadSMBX64ConfigFileF(m_configFilename, m_data);

    emit fullscreenUpdated();
    emit controllerType1Updated();
    emit keyboardUp1Updated();
    emit keyboardDown1Updated();
    emit keyboardLeft1Updated();
    emit keyboardRight1Updated();
    emit keyboardRun1Updated();
    emit keyboardJump1Updated();
    emit keyboardDrop1Updated();
    emit keyboardPause1Updated();
    emit keyboardAltJump1Updated();
    emit keyboardAltRun1Updated();
    emit joystickRun1Updated();
    emit joystickJump1Updated();
    emit joystickDrop1Updated();
    emit joystickPause1Updated();
    emit joystickAltJump1Updated();
    emit joystickAltRun1Updated();
    emit controllerType2Updated();
    emit keyboardUp2Updated();
    emit keyboardDown2Updated();
    emit keyboardLeft2Updated();
    emit keyboardRight2Updated();
    emit keyboardRun2Updated();
    emit keyboardJump2Updated();
    emit keyboardDrop2Updated();
    emit keyboardPause2Updated();
    emit keyboardAltJump2Updated();
    emit keyboardAltRun2Updated();
    emit joystickRun2Updated();
    emit joystickJump2Updated();
    emit joystickDrop2Updated();
    emit joystickPause2Updated();
    emit joystickAltJump2Updated();
    emit joystickAltRun2Updated();

    return ret;
}

bool ControlConfig::write()
{
    //Write file
    unsigned int file_format = 64; //Target SMBX build
    return FileFormats::WriteSMBX64ConfigFileF(m_configFilename, m_data, file_format);
}
