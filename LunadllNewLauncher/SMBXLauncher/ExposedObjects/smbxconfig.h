#ifndef SMBXCONFIG_H
#define SMBXCONFIG_H

#include <QObject>
#include <QVariant>
#include <QPointer>

#include "autostartconfig.h"
#include "controlconfig.h"

class SMBXConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AutostartConfig* Autostart READ Autostart NOTIFY AutostartUpdated)
    Q_PROPERTY(ControlConfig* Controls READ Controls NOTIFY ControlsUpdated)
public:
    explicit SMBXConfig(QObject *parent = 0);

    AutostartConfig* Autostart() const
    {
        return m_Autostart.data();
    }

    ControlConfig* Controls() const
    {
        return m_Controls.data();
    }

private:
    QVariant getJSONForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariant getDataForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    static bool sortSaveSlots(const QVariant slot1, const QVariant slot2);

public slots:
    QVariantList getEpisodeInfo(const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariant checkEpisodeUpdate(const QString& directoryName, const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariantList getSaveInfo(const QString& directoryName);
    void deleteSaveSlot(const QString& directoryName, int slot);
    void openLevelDialog();

    void runSMBX();
    void runSMBXEditor();
    void runPGEEditor();
    void loadEpisodeWebpage(const QString& file);


signals:
    void runSMBXExecuted();
    void runSMBXEditorExecuted();
    void runPGEEditorExecuted();
    void loadEpisodeWebpageExecuted(const QString& file);
    void runSMBXLevelExecuted(const QString& file);

    void AutostartUpdated();
    void ControlsUpdated();

private:

    QString m_Episode;
    QString m_SMBXExeFilename;
    QScopedPointer<AutostartConfig> m_Autostart;
    QScopedPointer<ControlConfig> m_Controls;
};

#endif // SMBXCONFIG_H
