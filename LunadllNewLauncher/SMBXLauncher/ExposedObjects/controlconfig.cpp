#include "controlconfig.h"


#include <QFile>
#include <QTextStream>

ControlConfig::ControlConfig(QObject *parent) : ControlConfig("config.dat", parent)
{}

ControlConfig::ControlConfig(const QString &configFilename, QObject *parent) :
    QObject(parent),
    m_configFilename(configFilename)
{
    while(m_data.players.size() < 2) m_data.players.push_back(SMBX64_ConfigPlayer());
    m_data.players[0].id = 1;
    m_data.players[1].id = 2;

    read();
}

bool ControlConfig::read()
{
    m_data.players.clear();
    //Read file
    QFile file(m_configFilename);
    if (!file.open(QIODevice::ReadOnly))
    {
        //Can't open file to read!
        return false;
    }

    QTextStream in(&file);   //You should read whole RAW content yourself
    in.setAutoDetectUnicode(true);
    in.setLocale(QLocale::system());
    in.setCodec(QTextCodec::codecForLocale());
    m_data =  FileFormats::ReadSMBX64ConfigFile(in.readAll());

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

    if(m_data.ReadFileValid)
        return true;
    else
        return false;
}

bool ControlConfig::write()
{
    //Write file
    int file_format=64; //Target SMBX build
    QFile file(m_configFilename);
    if(!file.open(QFile::WriteOnly))
    {
        return false;
    }

    QString raw = FileFormats::WriteSMBX64ConfigFile(m_data, file_format);
    for(int i=0; i<raw.size(); i++)
    {
        if(raw[i]=='\n')
        {
            //Force writing CRLF to prevent false damage of file on SMBX in Windows
            const char bytes[2] = {0x0D, 0x0A};
            file.write((const char*)(&bytes), 2);
        }
        else
        {
            const char byte[1] = {raw[i].toLatin1()};
            file.write((const char*)(&byte), 1);
        }
    }
    file.close();

    return true;
}


