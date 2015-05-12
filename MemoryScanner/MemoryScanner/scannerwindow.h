#ifndef SCANNERWINDOW_H
#define SCANNERWINDOW_H

#include <QMainWindow>


#include <QtCore>
#include <win32Helper.h>
#include <QJsonDocument>
#include <QTreeWidgetItem>

namespace Ui {
class ScannerWindow;
}

class ScannerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScannerWindow(QWidget *parent = 0);
    ~ScannerWindow();

    enum ScannerState {
        SCANNER_IDLE,
        SCANNER_OPEN
    };


protected:
    virtual void closeEvent(QCloseEvent * e);
    virtual void showEvent(QShowEvent *e);

private slots:
    /*void on_buttonConnect_clicked();

    void smbxsocket_connected();
    void smbxsocket_error(QAbstractSocket::SocketError error);
    void smbxsocket_readData();*/

    //Main
    void updateMemoryList();

    //Generel
    void on_buttonOpenSMBX_clicked();

    //Tree List
    void on_treeData_itemDoubleClicked(QTreeWidgetItem *item, int column);

    //New
    void on_buttonAddNewEntry_clicked();

    //Edit entry
    void on_buttonEditSelected_clicked();
    void on_buttonDelete_clicked();
    void on_buttonUp_clicked();
    void on_buttonDown_clicked();

    //Category
    void on_buttonCatAdd_clicked();
    void on_buttonCatRemove_clicked();
    void on_comboMemRange_currentIndexChanged(int index);

    //Data
    void on_buttonJsonExport_clicked();
    void on_buttonJsonImport_clicked();

private:
    Ui::ScannerWindow *ui;

    //Memory Stuff
    void updateMainEntryList(QTreeWidgetItem* item);
    int strTypeToLength(const QString &memType);
    QString dataToString(const QString &memType, QDataStream& streamWithData);

    //JSON and Config Stuff
    QVariant config;
    QList<QVariant> config_getListOfCategory(QString catName);

    void initJSONAndGui();
    void initJSONAndGui(QString fileName);
    void saveJSON();
    void saveJSON(QString fileName);

    void loadCategories();
    void loadCurrentFromSelection();
    void saveCurrentTo(QString name);
    void resetAll();

    //State
    void updateGuiUponState();
    ScannerState m_state;


    HANDLE smbxHandle;

    QTimer* scannerTimer;

    /*void updateStatusText(const QString &text, const QColor &color);
    void resetConnection();*/
    //QTcpSocket* SMBXSocket;

};

#endif // SCANNERWINDOW_H
