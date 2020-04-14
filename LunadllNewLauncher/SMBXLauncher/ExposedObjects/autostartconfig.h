#ifndef AUTOSTARTCONFIG_H
#define AUTOSTARTCONFIG_H

#include <QObject>

class AutostartConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool useAutostart READ useAutostart WRITE setUseAutostart NOTIFY useAutostartUpdated)
    Q_PROPERTY(QString wldPath READ wldPath WRITE setWldPath NOTIFY wldPathUpdated)
    Q_PROPERTY(bool singleplayer READ singleplayer WRITE setSingleplayer NOTIFY singleplayerUpdated)
    Q_PROPERTY(int character1 READ character1 WRITE setCharacter1 NOTIFY character1Updated)
    Q_PROPERTY(int character2 READ character2 WRITE setCharacter2 NOTIFY character2Updated)
    Q_PROPERTY(int saveSlot READ saveSlot WRITE setSaveSlot NOTIFY saveSlotUpdated)

public:
    explicit AutostartConfig(QObject *parent = 0);

    bool useAutostart() const
    {
        return m_useAutostart;
    }

    QString wldPath() const
    {
        return m_wldPath;
    }

    bool singleplayer() const
    {
        return m_singleplayer;
    }

    int character1() const
    {
        return m_character1;
    }

    int character2() const
    {
        return m_character2;
    }

    int saveSlot() const
    {
        return m_saveSlot;
    }

public slots:
    void setUseAutostart(bool arg)
    {
        m_useAutostart = arg;
        emit useAutostartUpdated();
    }
    void setWldPath(QString arg)
    {
        m_wldPath = arg;
        emit wldPathUpdated();
    }
    void setSingleplayer(bool arg)
    {
        m_singleplayer = arg;
        emit singleplayerUpdated();
    }

    void setCharacter1(int arg)
    {
        m_character1 = arg;
        emit character1Updated();
    }

    void setCharacter2(int arg)
    {
        m_character2 = arg;
        emit character2Updated();
    }

    void setSaveSlot(int arg)
    {
        m_saveSlot = arg;
        emit saveSlotUpdated();
    }

signals:
    void useAutostartUpdated();
    void wldPathUpdated();
    void singleplayerUpdated();
    void character1Updated();
    void character2Updated();
    void saveSlotUpdated();

private:
    bool m_useAutostart;
    QString m_wldPath;
    bool m_singleplayer;
    int m_character1;
    int m_character2;
    int m_saveSlot;
};

#endif // AUTOSTARTCONFIG_H
