#include "controlconfig.h"


#include <QFile>
#include <QTextStream>

ControlConfig::ControlConfig(QObject *parent) : ControlConfig("config.dat", parent)
{}

ControlConfig::ControlConfig(const QString &configFilename, QObject *parent) : QObject(parent), m_configFilename(configFilename)
{
    while(m_data.players.size() < 2) m_data.players.push_back(SMBX64_ConfigPlayer());
    m_data.players[0].id = 1;
    m_data.players[1].id = 2;
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


