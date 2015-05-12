#include "scannerwindow.h"
#include "ui_scannerwindow.h"

#include <QCloseEvent>

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

#include <addnewentrywidget.h>
#include <addnewsimplestructwidget.h>


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
    initJSONAndGui();
    e->accept();
}



void ScannerWindow::on_buttonOpenSMBX_clicked()
{
    if(m_state == SCANNER_IDLE){
        smbxHandle = getSMBXProcessHandle("a2mbxt.exe");
        if(!smbxHandle) smbxHandle = getSMBXProcessHandle("smbx.exe");
        if(!smbxHandle){
            QString possibleProcessName = QInputDialog::getText(this, "Please enter the smbx process name!", "Did not found the SMBX Process.\nPlease enter the smbx process name!\nExample: \"smbx.exe\"");
            smbxHandle = getSMBXProcessHandle(possibleProcessName);
            if(!smbxHandle) smbxHandle = getSMBXProcessHandle(possibleProcessName + ".exe");
            if(!smbxHandle){
                QMessageBox::information(this , "Didn't find process!", "Didn't find process!");
            }
        }
        m_state = SCANNER_OPEN;
        updateGuiUponState();
        loadCurrentFromSelection();
    }else if(m_state == SCANNER_OPEN){
        CloseHandle(smbxHandle);
        smbxHandle = NULL;
        m_state = SCANNER_IDLE;
        updateGuiUponState();
    }


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
        }else if(item->data(0, Qt::UserRole+1).toString() == "struct"){
            updateStructEntryList(item);
        }
    }
}

void ScannerWindow::initJSONAndGui(QString fileName){
    QFile jsonFile(fileName);
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


void ScannerWindow::initJSONAndGui()
{
    initJSONAndGui("data.json");
}


void ScannerWindow::saveJSON(QString fileName){
    QFile jsonFile(fileName);
    if(!jsonFile.open(QIODevice::WriteOnly)){
        qDebug() << "Failed to open json file";
        return;
    }

    jsonFile.write(QJsonDocument::fromVariant(config).toJson());
    jsonFile.close();
}

void ScannerWindow::saveJSON()
{
    saveJSON("data.json");
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
        }else if(nextItem->data(0, Qt::UserRole+1).toString() == "struct"){
            QMap<QString, QVariant> structSettings = nextItem->data(0, Qt::UserRole+2).toMap();
            structSettings["type"] = "struct";
            structSettings["struct-name"] = nextItem->text(0);
            structSettings["struct-baseaddress"] = nextItem->text(1);

            QList<QVariant> subEntries;
            for(int j = 0; j < nextItem->childCount(); ++j){
                QTreeWidgetItem* childEntry = nextItem->child(j);
                QMap<QString, QVariant> subentrySetting;
                subentrySetting["entry-name"] = childEntry->text(0);
                subentrySetting["entry-address"] =  childEntry->text(1);
                subentrySetting["entry-type"] = childEntry->text(2);
                subentrySetting["entry-checked"] = (childEntry->checkState(0) == Qt::Checked);
                subEntries << QVariant(subentrySetting);
            }

            structSettings["struct-entries"] = QVariant(subEntries);
            mainEntries << structSettings;
        }
    }
    allRanges[name] = QVariant(mainEntries);
    config = QVariant(allRanges);
}

void ScannerWindow::resetAll()
{
    ui->treeData->clear();
    ui->treeData->setProperty("latestItem", "");
    config = QVariant();
    ui->comboMemRange->clear();
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
            QString name = entrySetting["struct-name"].toString();
            QString address = entrySetting["struct-baseaddress"].toString();

            entrySetting.remove("type");
            entrySetting.remove("struct-name");
            entrySetting.remove("struct-baseaddress");

            QTreeWidgetItem* newEntry = new QTreeWidgetItem({name, address, "", "---"});
            newEntry->setData(0, Qt::UserRole+1, type);
            newEntry->setData(0, Qt::UserRole+2, entrySetting);
            newEntry->setFlags(newEntry->flags() | Qt::ItemIsEditable);

            QList<QVariant> subEntries = entrySetting["struct-entries"].toList();
            foreach(QVariant nextSubEntry, subEntries){
                QMap<QString, QVariant> subEntry = nextSubEntry.toMap();
                QString name = subEntry["entry-name"].toString();
                QString dataType = subEntry["entry-type"].toString();
                QString address = subEntry["entry-address"].toString();
                bool isChecked = subEntry["entry-checked"].toBool();

                QTreeWidgetItem* newSubEntry = new QTreeWidgetItem({name, address, dataType, "?"});
                newSubEntry->setData(0, Qt::UserRole+1, type);
                newSubEntry->setFlags(newSubEntry->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
                newSubEntry->setCheckState(0, (isChecked ? Qt::Checked : Qt::Unchecked));
                newEntry->addChild(newSubEntry);
            }

            ui->treeData->addTopLevelItem(newEntry);
        }
    }


}

void ScannerWindow::updateGuiUponState()
{
    ui->buttonOpenSMBX->setText(m_state == SCANNER_IDLE ? "Open SMBX Process" : "Close SMBX Process");
    ui->buttonAddNewEntry->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonAddNewSimpleStruct->setEnabled(m_state == SCANNER_OPEN);
    ui->comboMemRange->setEnabled(m_state == SCANNER_OPEN);
    ui->treeData->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonDelete->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonDown->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonUp->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonEditSelected->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonJsonExport->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonJsonImport->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonCatAdd->setEnabled(m_state == SCANNER_OPEN);
    ui->buttonCatRemove->setEnabled(m_state == SCANNER_OPEN);


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
        QTreeWidgetItem* entryItem = entryDialog->generateNewEntry();
        if(ui->treeData->currentItem()->data(0, Qt::UserRole+1).toString() == "struct"){
            ui->treeData->currentItem()->addChild(entryItem);
        }else{
            ui->treeData->addTopLevelItem(entryItem);
        }
    }
    delete entryDialog;
}

void ScannerWindow::on_buttonAddNewSimpleStruct_clicked()
{
    AddNewSimpleStructWidget* simpleStructDialog = new AddNewSimpleStructWidget();
    if(simpleStructDialog->exec() == QDialog::Accepted){
        ui->treeData->addTopLevelItem(simpleStructDialog->generateNewEntry());
    }
    delete simpleStructDialog;
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

void ScannerWindow::updateStructEntryList(QTreeWidgetItem *item)
{
    QMap<QString, QVariant> settings = item->data(0, Qt::UserRole+2).toMap();
    QString addr = item->text(1);

    bool success;
    DWORD address = (DWORD)addr.toInt(&success, 16);
    if(!success){
        item->setText(3, "!Address Format Error!");
        return;
    }

    DWORD addressOfSturctArray;
    if(!ReadProcessMemory(smbxHandle, (void*)address, &addressOfSturctArray, sizeof(addressOfSturctArray), NULL)){
        item->setText(3, "!Invalid Address!");
        return;
    }


    int structSize = settings["struct-size"].toString().toInt(&success);
    if(!success){
        item->setText(3, "!Struct size invalid (check settings)!");
        return;
    }

    int totalCount = settings["struct-totalcount"].toString().toInt(&success);
    if(!success){
        item->setText(3, "!Total count invalid (check settings)!");
        return;
    }

    int arrayOffset = settings["struct-offset"].toString().toInt(&success, 16);
    if(!success){
        item->setText(3, "!Array offset invalid (check settings)!");
        return;
    }

    QScopedPointer<BYTE, QScopedPointerArrayDeleter<BYTE> > buffer(new BYTE[structSize*totalCount]);
    if(!ReadProcessMemory(smbxHandle, (void*)(addressOfSturctArray+arrayOffset), buffer.data(), structSize*totalCount, NULL)){
        item->setText(3, "!Failed to copy array!");
        return;
    }

    int useLookupType = settings["struct-lookupValType"].toString().toInt(&success);
    if(!success || useLookupType < 0 || useLookupType > 1){
        item->setText(3, "!Invalid lookup type (check settings)!");
        return;
    }


    if(useLookupType == 0){
        //Use id

        int useID = settings["struct-id"].toString().toInt(&success);
        if(!success){
            item->setText(3, "!Invalid id for lookup (check settings)!");
            return;
        }

        int useIdOffset = settings["struct-id-offset"].toString().toInt(&success, 16);
        if(!success){
            item->setText(3, "!Invalid id offset for lookup (check settings)!");
            return;
        }

        for(int i = 0; i < totalCount; ++i){
            int theID;
            int currentIndex = i * structSize;
            theID = *(WORD*)(&(buffer.data()[currentIndex + useIdOffset]));
            if(theID = useID){
                BYTE* curStruct = &(buffer.data()[currentIndex]);
                for(int j = 0; j < item->childCount(); ++j){
                    QTreeWidgetItem* nextItem = item->child(j);

                    if(nextItem->checkState(0) == Qt::Checked){
                        // I know that it is copy & paste >->
                        QString addr = nextItem->text(1);

                        bool success;
                        DWORD offset = (DWORD)addr.toInt(&success, 16);
                        if(!success){
                            nextItem->setText(3, "!Address Format Error!");
                            return;
                        }

                        QString type = nextItem->text(2);
                        int bufLen = strTypeToLength(type);
                        if(bufLen == -1){
                            nextItem->setText(3, "!Invalid Type!");
                            return;
                        }

                        QDataStream data(QByteArray((const char*)(curStruct+offset), bufLen));
                        nextItem->setText(3, dataToString(type, data));
                    }else{
                        nextItem->setText(3, "?");
                    }
                }
                return;
            }
        }

    }else if(useLookupType == 1){
        //Use index

    }






    item->setText(3, "---");
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



void ScannerWindow::on_buttonJsonExport_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Please select the save location to save the json data.", QString(), QString("JSON data (*.json)"));
    if(filePath.isEmpty())
        return;

    saveJSON(filePath);
}

void ScannerWindow::on_buttonJsonImport_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Please select the location to open the json data.", QString(), QString("JSON data (*.json)"));
    if(filePath.isEmpty())
        return;

    resetAll();
    initJSONAndGui(filePath);
}


