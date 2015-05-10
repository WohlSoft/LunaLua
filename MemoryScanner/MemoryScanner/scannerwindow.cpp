#include "scannerwindow.h"
#include "ui_scannerwindow.h"

#include <QMessageBox>

#include <QCloseEvent>
#include <QInputDialog>
#include <addnewentrywidget.h>

ScannerWindow::ScannerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScannerWindow),
    m_state(SCANNER_IDLE),
    smbxHandle(nullptr)
{
    ui->setupUi(this);
    scannerTimer = new QTimer(this);
    connect(scannerTimer, SIGNAL(timeout()), this, SLOT(updateMemoryList()));
    scannerTimer->start(10);
    updateGuiUponState();
}

ScannerWindow::~ScannerWindow()
{
    if(smbxHandle)
        CloseHandle(smbxHandle);


    delete ui;
}

void ScannerWindow::closeEvent(QCloseEvent *e)
{
    saveCurrentTo(ui->treeData->property("latestItem").toString());
    saveJSON();
    e->accept();
}

void ScannerWindow::showEvent(QShowEvent *e)
{
    initJSON();
    e->accept();
}



void ScannerWindow::on_buttonOpenSMBX_clicked()
{
    EnableDebugPriv();

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    bool found = false;
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            QString entryName = QString::fromWCharArray(entry.szExeFile);
            if(entryName == "a2mbxt.exe" || entryName == "smbx.exe"){
                smbxHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                m_state = SCANNER_OPEN;
                updateGuiUponState();
                found = true;
                loadCurrentFromSelection();
            }
        }
    }
    if(!found){
        QMessageBox::warning(this, "Warning", "Did not found SMBX process!");
    }

    CloseHandle(snapshot);
}

void ScannerWindow::updateMemoryList()
{
    if(m_state != SCANNER_OPEN)
        return;

    for(int i = 0; i < ui->treeData->topLevelItemCount(); ++i){
        QTreeWidgetItem* item = ui->treeData->topLevelItem(i);
        if(item->data(0, Qt::UserRole+1).toString() == "entry"){
            if(item->checkState(0) == Qt::Checked){
                updateMainEntryList(item);
            }else{
                item->setText(3, "?");
            }
        }
    }
}

void ScannerWindow::initJSON()
{
    QFile jsonFile("data.json");
    if(!jsonFile.open(QIODevice::ReadWrite)){
        qDebug() << "Failed to open json file";
        return;
    }
    QByteArray jsonData = jsonFile.readAll();
    if(!jsonData.size()){
        qDebug() << "No JSON data!";
        qDebug() << "Writing default template";

        QMap<QString, QVariant> mainOverhead;
        QMap<QString, QVariant> globalOverhead;
        mainOverhead["global"] = globalOverhead;

        QJsonDocument doc = QJsonDocument::fromVariant(mainOverhead);
        jsonFile.write(doc.toJson());

        config = mainOverhead;
        jsonFile.close();
        return;
    }

    QJsonParseError possibleError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &possibleError);
    if(possibleError.error != QJsonParseError::NoError){
        QMessageBox::warning(this, "JSON PARSE ERROR", "Json Parse Error: " + possibleError.errorString());
        abort();
    }

    config = jsonDoc.toVariant();
    jsonFile.close();

    loadCategories();
    loadCurrentFromSelection();
}

void ScannerWindow::saveJSON()
{
    QFile jsonFile("data.json");
    if(!jsonFile.open(QIODevice::WriteOnly)){
        qDebug() << "Failed to open json file";
        return;
    }

    jsonFile.write(QJsonDocument::fromVariant(config).toJson());
    jsonFile.close();
}

void ScannerWindow::loadCategories()
{
    QMap<QString, QVariant> allRanges = config.toMap();
    if(!allRanges.contains("global")){
        QMessageBox::warning(this, "ERROR", "Config data doesn't contain the \"global\" field");
        abort();
    }
    for(QMap<QString, QVariant>::iterator it = allRanges.begin(); it != allRanges.end(); ++it){
        ui->comboMemRange->addItem(it.key());
    }
}

void ScannerWindow::saveCurrentTo(QString name)
{
    if(ui->comboMemRange->findText(name) == -1) return;

    QMap<QString, QVariant> allRanges = config.toMap();
    QList<QVariant> mainEntries;
    for(int i = 0; i < ui->treeData->topLevelItemCount(); ++i){
        QTreeWidgetItem* nextItem = ui->treeData->topLevelItem(i);
        if(nextItem->data(0, Qt::UserRole+1).toString() == "entry"){
            QMap<QString, QVariant> entrySetting;
            entrySetting["type"] = "entry";
            entrySetting["entry-name"] = nextItem->text(0);
            entrySetting["entry-address"] =  nextItem->text(1);
            entrySetting["entry-type"] = nextItem->text(2);
            entrySetting["entry-checked"] = (nextItem->checkState(0) == Qt::Checked);
            mainEntries << QVariant(entrySetting);
        }
    }
    allRanges[name] = QVariant(mainEntries);
    config = QVariant(allRanges);
}

void ScannerWindow::loadCurrentFromSelection()
{
    ui->treeData->clear();
    ui->treeData->setProperty("latestItem", ui->comboMemRange->currentText());

    foreach(QVariant mainEntry, config_getListOfCategory(ui->comboMemRange->currentText())){
        QMap<QString, QVariant> entrySetting = mainEntry.toMap();
        QString type = entrySetting["type"].toString();
        if(type == "entry"){
            QString name = entrySetting["entry-name"].toString();
            QString dataType = entrySetting["entry-type"].toString();
            QString address = entrySetting["entry-address"].toString();
            bool isChecked = entrySetting["entry-checked"].toBool();

            QTreeWidgetItem* newEntry = new QTreeWidgetItem({name, address, dataType, "?"});
            newEntry->setData(0, Qt::UserRole+1, type);
            newEntry->setFlags(newEntry->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
            newEntry->setCheckState(0, (isChecked ? Qt::Checked : Qt::Unchecked));
            ui->treeData->addTopLevelItem(newEntry);
        }else if(type == "struct"){

        }
    }


}

void ScannerWindow::updateGuiUponState()
{
    ui->buttonOpenSMBX->setEnabled(m_state == SCANNER_IDLE);
    ui->buttonAddNewEntry->setEnabled(m_state == SCANNER_OPEN);
    ui->comboMemRange->setEnabled(m_state == SCANNER_OPEN);
    ui->treeData->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonDelete->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonDown->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonUp->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonEditSelected->setEnabled(m_state == SCANNER_OPEN);


    switch (m_state) {
    case SCANNER_IDLE:
        ui->labelStatus->setText("Status: Idle");
        break;
    case SCANNER_OPEN:
        ui->labelStatus->setText("Status: Open");
        break;
    default:
        break;
    }
}




void ScannerWindow::on_buttonAddNewEntry_clicked()
{
    AddNewEntryWidget* entryDialog = new AddNewEntryWidget();
    if(entryDialog->exec() == QDialog::Accepted){
        ui->treeData->addTopLevelItem(entryDialog->generateNewEntry());
    }
    delete entryDialog;
}

QList<QVariant> ScannerWindow::config_getListOfCategory(QString catName)
{
    QMap<QString, QVariant> allRanges = config.toMap();
    return allRanges[catName].toList();
}


void ScannerWindow::updateMainEntryList(QTreeWidgetItem *item)
{
    QString addr = item->text(1);

    bool success;
    DWORD address = (DWORD)addr.toInt(&success, 16);
    if(!success){
        item->setText(3, "!Address Format Error!");
        return;
    }

    QString type = item->text(2);
    int bufLen = strTypeToLength(type);
    if(bufLen == -1){
        item->setText(3, "!Invalid Type!");
        return;
    }

    char* buffer = new char[bufLen];
    if(ReadProcessMemory(smbxHandle, (void*)address, buffer, bufLen, NULL)){
        QDataStream data(QByteArray(buffer, bufLen));
        item->setText(3, dataToString(type, data));
    }else{
        item->setText(3, "!Invalid Address!");
    }

    delete buffer;
}

int ScannerWindow::strTypeToLength(const QString &memType)
{
    if(memType == "byte") return 1;
    if(memType == "word") return 2;
    if(memType == "float") return 4;
    if(memType == "string") return 4;
    if(memType == "dword") return 4;
    if(memType == "double") return 8;
    return -1;
}

QString ScannerWindow::dataToString(const QString &memType, QDataStream &streamWithData)
{

    if(memType == "byte")
    {
        BYTE data;
        streamWithData.readRawData((char*)&data, 1);
        return QString::number((uint)data);
    }
    if(memType == "word")
    {
        WORD data;
        streamWithData.readRawData((char*)&data, 2);
        return QString::number((uint)data);
    }
    if(memType == "float")
    {
        float data;
        streamWithData.readRawData((char*)&data, 4);
        return QString::number(data);
    }
    if(memType == "dword")
    {
        DWORD data;
        streamWithData.readRawData((char*)&data, 4);
        return QString::number(data);
    }
    if(memType == "double")
    {
        double data;
        streamWithData.readRawData((char*)&data, 8);
        return QString::number(data);
    }
    if(memType == "string")
    {
        DWORD data;
        streamWithData.readRawData((char*)&data, 4);


        unsigned int length;
        ReadProcessMemory(smbxHandle, (void*)(data - 4), &length, 4, NULL);

        wchar_t* buffer = new wchar_t[length/2+1];
        ReadProcessMemory(smbxHandle, (void*)data, buffer, length, NULL);

        QString newStr = QString::fromWCharArray(buffer, length/2);
        delete buffer;
        return newStr;
    }

    return "";

}


void ScannerWindow::on_comboMemRange_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    QString latestItem = ui->treeData->property("latestItem").toString();
    if(!latestItem.isEmpty())
        saveCurrentTo(latestItem);
    loadCurrentFromSelection();
}


//Edit trigger
void ScannerWindow::on_treeData_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(column == 0)
        ui->treeData->editItem(item, 0);
}

void ScannerWindow::on_buttonEditSelected_clicked()
{
    QTreeWidgetItem* current = ui->treeData->currentItem();
    if(!current)
        return;

    QString type = current->data(0, Qt::UserRole+1).toString();
    if(type == "entry"){
        AddNewEntryWidget* entryDialog = new AddNewEntryWidget(current);
        entryDialog->exec();
        delete entryDialog;
    }
}

void ScannerWindow::on_buttonDelete_clicked()
{
    if(ui->treeData->currentItem())
        delete ui->treeData->currentItem();
}


void ScannerWindow::on_buttonUp_clicked()
{
    if(!ui->treeData->currentItem())
        return;
    if(ui->treeData->currentItem()->parent())
        return;

    int row = ui->treeData->currentIndex().row();
    if(row == 0) return;

    QTreeWidgetItem* theItem = ui->treeData->takeTopLevelItem(row);
    ui->treeData->insertTopLevelItem(row-1, theItem);
    ui->treeData->setCurrentItem(theItem);
}

void ScannerWindow::on_buttonDown_clicked()
{
    if(!ui->treeData->currentItem())
        return;
    if(ui->treeData->currentItem()->parent())
        return;

    int row = ui->treeData->currentIndex().row();
    if(row == ui->treeData->topLevelItemCount()-1) return;

    QTreeWidgetItem* theItem = ui->treeData->takeTopLevelItem(row);
    ui->treeData->insertTopLevelItem(row+1, theItem);
    ui->treeData->setCurrentItem(theItem);
}


void ScannerWindow::on_buttonCatAdd_clicked()
{
    QString newName = QInputDialog::getText(this, "Input the new category name", "Please input the category name: ");
    if(newName.isEmpty()) return;

    ui->comboMemRange->addItem(newName);
    ui->comboMemRange->setCurrentText(newName);
}


void ScannerWindow::on_buttonCatRemove_clicked()
{
    ui->comboMemRange->removeItem(ui->comboMemRange->currentIndex());
}


/*
void ScannerWindow::on_buttonConnect_clicked()
{
    ui->buttonConnect->setDisabled(true);

    SMBXSocket = new QTcpSocket();
    updateStatusText("Connecting", QColor(218,165,32));


    connect(SMBXSocket, SIGNAL(connected()), this, SLOT(smbxsocket_connected()));
    connect(SMBXSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(smbxsocket_error(QAbstractSocket::SocketError)));

    SMBXSocket->connectToHost("127.0.0.1", 42198);

}

void ScannerWindow::smbxsocket_connected()
{

}

void ScannerWindow::smbxsocket_error(QAbstractSocket::SocketError error)
{
    QMessageBox::warning(this, "ERROR", SMBXSocket->errorString());


}



void ScannerWindow::updateStatusText(const QString& text, const QColor& color)
{

    ui->labelStatus->setText("<html><head/><body><p>Status: <span style=\" color:" + color.name(QColor::HexRgb) + ";\">" + text + "</span></p></body></html>");
}

void ScannerWindow::resetConnection()
{
    updateStatusText("Not Connected", QColor(255, 0, 0));
    SMBXSocket->abort();
    delete SMBXSocket;
    SMBXSocket = nullptr;
}
*/








