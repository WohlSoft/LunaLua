#ifndef SMBXCONFIG_H
#define SMBXCONFIG_H

#include <QObject>
#include <autostartconfig.h>
#include <QVariant>

#include <controlconfig.h>

class SMBXConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AutostartConfig* Autostart READ Autostart)
    Q_PROPERTY(ControlConfig* Controls READ Controls)
public:
    explicit SMBXConfig(QObject *parent = 0);
    ~SMBXConfig();

    AutostartConfig* Autostart() const
    {
        return m_Autostart;
    }

    ControlConfig* Controls() const
    {
        return m_Controls;
    }

private:
    QVariant getJSONForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariant getDataForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName);

public slots:
    QVariantList getEpisodeInfo(const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariantList getSaveInfo(const QString& directoryName);

signals:
    void runSMBX();
    void runSMBXEditor();
    void loadEpisodeWebpage(const QString& file);

private:

    QString m_Episode;
    QString m_SMBXExeFilename;
    AutostartConfig* m_Autostart;
    ControlConfig* m_Controls;
};

#endif // SMBXCONFIG_H
