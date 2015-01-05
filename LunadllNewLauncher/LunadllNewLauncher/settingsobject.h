#ifndef SETTINGSOBJECT_H
#define SETTINGSOBJECT_H

#include <QObject>

class SettingsObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool noframeskip READ noframeskip WRITE setNoframeskip)
    Q_PROPERTY(bool nosound READ nosound WRITE setNosound)
public:
    explicit SettingsObject(QObject *parent = 0);

    bool noframeskip() const;
    void setNoframeskip(bool noframeskip);

    bool nosound() const;
    void setNosound(bool nosound);

signals:
    void quitApp();
    void startGame();
    void startLevelEditor();
public slots:

private:
    bool m_noframeskip;
    bool m_nosound;
};

#endif // SETTINGSOBJECT_H
