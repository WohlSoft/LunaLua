#ifndef AUTOSTARTCONFIG_H
#define AUTOSTARTCONFIG_H

#include <QObject>

class AutostartConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool useAutostart READ useAutostart WRITE setUseAutostart)
    Q_PROPERTY(QString episodeName READ episodeName WRITE setEpisodeName)
    Q_PROPERTY(bool singleplayer READ singleplayer WRITE setSingleplayer)
    Q_PROPERTY(int character1 READ character1 WRITE setCharacter1)
    Q_PROPERTY(int character2 READ character2 WRITE setCharacter2)
    Q_PROPERTY(int saveSlot READ saveSlot WRITE setSaveSlot)

public:
    explicit AutostartConfig(QObject *parent = 0);

    bool useAutostart() const
    {
        return m_useAutostart;
    }

    QString episodeName() const
    {
        return m_episodeName;
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

signals:

public slots:
    void setUseAutostart(bool arg)
    {
        m_useAutostart = arg;
    }
    void setEpisodeName(QString arg)
    {
        m_episodeName = arg;
    }
    void setSingleplayer(bool arg)
    {
        m_singleplayer = arg;
    }

    void setCharacter1(int arg)
    {
        m_character1 = arg;
    }

    void setCharacter2(int arg)
    {
        m_character2 = arg;
    }

    void setSaveSlot(int arg)
    {
        m_saveSlot = arg;
    }

private:
    bool m_useAutostart;
    QString m_episodeName;
    bool m_singleplayer;
    int m_character1;
    int m_character2;
    int m_saveSlot;
};

#endif // AUTOSTARTCONFIG_H
