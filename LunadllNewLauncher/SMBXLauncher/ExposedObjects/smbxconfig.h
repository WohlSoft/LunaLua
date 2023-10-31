#ifndef SMBXCONFIG_H
#define SMBXCONFIG_H

#include <QObject>
#include <QVariant>
#include <QPointer>
#include <QUrl>

#include "autostartconfig.h"
#include "controlconfig.h"
#include "../../LunaDll/Input/LunaGameController.h"

class SMBXConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AutostartConfig* Autostart READ Autostart NOTIFY AutostartUpdated)
    Q_PROPERTY(ControlConfig* Controls READ Controls NOTIFY ControlsUpdated)
    Q_PROPERTY(bool hasInternetAccess READ HasInternet NOTIFY InternetAccessUpdated)
    Q_PROPERTY(bool hasUpdate READ HasUpdate NOTIFY UpdateVersionUpdated)
    Q_PROPERTY(QString updateVersionName READ UpdateVersionName NOTIFY UpdateVersionUpdated)
    Q_PROPERTY(QString updateMessage READ UpdateMessage NOTIFY UpdateVersionUpdated)
    Q_PROPERTY(int updateLevel READ UpdateLevel NOTIFY UpdateVersionUpdated)
public:
    explicit SMBXConfig(QObject *parent = nullptr);

    AutostartConfig* Autostart() const
    {
        return m_Autostart.data();
    }

    ControlConfig* Controls() const
    {
        return m_Controls.data();
    }

    bool HasUpdate() const
    {
        return m_hasUpdate;
    }

    bool HasInternet() const
    {
        return m_hasInternetAccess;
    }

    QString UpdateVersionName() const
    {
        return m_updateVersion;
    }

    QString UpdateMessage() const
    {
        return m_updateMessage;
    }

    int UpdateLevel() const
    {
        return m_updateType;
    }

    void pollControls();

    bool m_hasInternetAccess;
    bool m_hasUpdate;
    int m_updateType;
    QString m_updateVersion;
    QString m_updateMessage;
    QUrl m_updateLink;

private:
    QVariant getJSONForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariant getDataForEpisode(const QString& episodeDirPath, const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariantList readSavesInPath(const QString& episodeDirPath, QList<int>* foundSlots);
    static bool sortSaveSlots(const QVariant slot1, const QVariant slot2);

public slots:
    QVariantList getEpisodeInfo(const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariant checkEpisodeUpdate(const QString& directoryName, const QString& jsonSubDirPerEpisode, const QString& jsonFileName);
    QVariantList getSaveInfo(const QString& directoryName);
    void deleteSaveSlot(const QString& directoryName, int slot);
    void openLevelDialog();
    void openUpdateWindow();

    void runSMBX();
    void runSMBXEditor();
    void runPGEEditor();
    void setWindowHeader(const QString& title);
    void loadEpisodeWebpage(const QString& file);


signals:
    void runSMBXExecuted();
    void runSMBXEditorExecuted();
    void runPGEEditorExecuted();
    void setWindowHeaderExecuted(const QString& title);
    void loadEpisodeWebpageExecuted(const QString& file);
    void runSMBXLevelExecuted(const QString& file);

    void AutostartUpdated();
    void ControlsUpdated();
    void UpdateVersionUpdated();
    void InternetAccessUpdated();

    void ControllerButtonPress(int buttonId, const QString& controllerName);

private:

    QString m_Episode;
    QString m_SMBXExeFilename;
    QScopedPointer<AutostartConfig> m_Autostart;
    QScopedPointer<ControlConfig> m_Controls;

    LunaGameControllerManager m_ControllerManager;
};

#endif // SMBXCONFIG_H
