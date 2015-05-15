#include "addnewsimplestructwidget.h"
#include "ui_addnewsimplestructwidget.h"

#include <QIntValidator>
#include <QMessageBox>

#include <QFileDialog>
#include <parsetools.h>


AddNewSimpleStructWidget::AddNewSimpleStructWidget(QTreeWidgetItemSMBXAddress* editToItem, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewSimpleStructWidget),
    m_editItem(editToItem)
{
    ui->setupUi(this);
    if(editToItem){
        ui->editName->setText(editToItem->text(0));
        ui->editBaseAddress->setText(editToItem->text(1));
    }
    ui->editID->setValidator(new QIntValidator(0, 999, this));
    ui->editIndex->setValidator(new QIntValidator(0, 999, this));
    ui->editSizeOfStruct->setValidator(new QIntValidator(0, INT_MAX, this));
    ui->editTotalCount->setValidator(new QIntValidator(0, INT_MAX, this));

    connect(ui->radioUseId, SIGNAL(toggled(bool)), this, SLOT(updateGuiState(bool)));
    connect(ui->radioUseIndex, SIGNAL(toggled(bool)), this, SLOT(updateGuiState(bool)));
    connect(ui->checkParseHeader, SIGNAL(toggled(bool)), this, SLOT(updateGuiState(bool)));
    updateGuiState(false);

    // TODO add edits
    // TODO add m_subItems on edit

    ui->editName->setFocus();
    ui->editName->selectAll();
}

QPair<QTreeWidgetItemSMBXAddress *, QList<QTreeWidgetItem *> > AddNewSimpleStructWidget::generateNewEntry()
{
    QTreeWidgetItemSMBXAddress* newEntry = new QTreeWidgetItemSMBXAddress();
    newEntry->setText(0, ui->editName->text());
    newEntry->setText(1, ui->editBaseAddress->text());
    newEntry->setText(2, "");
    newEntry->setText(3, "---");
    newEntry->setData(0, Qt::UserRole+1, "struct");
    newEntry->setFlags(newEntry->flags() | Qt::ItemIsEditable);

    /*
     *      QString name = entrySetting["entry-name"].toString();
            QString dataType = entrySetting["entry-type"].toString();
            QString address = entrySetting["entry-address"].toString();
     */

    QMap<QString, QVariant> structMetadata;
    structMetadata["struct-offset"] = ui->editOffset->text();
    structMetadata["struct-size"] = ui->editSizeOfStruct->text();
    structMetadata["struct-totalcount"] = ui->editTotalCount->text();

    structMetadata["struct-lookupValType"] = (ui->radioUseId->isChecked() ? 0 : 1);
    structMetadata["struct-id"] = ui->editID->text();
    structMetadata["struct-id-offset"] = ui->editOffsetID->text();
    structMetadata["struct-index"] = ui->editIndex->text();

    newEntry->setData(0, Qt::UserRole+2, structMetadata);

    if(ui->checkParseHeader->isChecked()){
        QFile f(ui->editHeaderFilePath->text());
        if(!f.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this, "File Error", "Failed to open header file");
            return qMakePair(newEntry, QList<QTreeWidgetItem*>());
        }

        QString data = QString(f.readAll());
        bool success;

        QList<QTreeWidgetItem* > subItems = ParseTools::parseStruct(data, ui->checkHeaderIgnoreUnknown->isChecked(), success);
        if(!success){
            QMessageBox::warning(this, "Parse Error", "Failed to parse header file!");
            qDeleteAll(subItems);
            return qMakePair(newEntry, QList<QTreeWidgetItem*>());
        }
        return qMakePair(newEntry, subItems);
    }

    return qMakePair(newEntry, QList<QTreeWidgetItem*>());
}

AddNewSimpleStructWidget::~AddNewSimpleStructWidget()
{
    delete ui;
}


void AddNewSimpleStructWidget::updateGuiState(bool /*_1*/)
{
    ui->editID->setEnabled(ui->radioUseId->isChecked());
    ui->editOffsetID->setEnabled(ui->radioUseId->isChecked());
    ui->editIndex->setEnabled(ui->radioUseIndex->isChecked());
    ui->editHeaderFilePath->setEnabled(ui->checkParseHeader->isChecked());
    ui->buttonHeaderPathLookup->setEnabled(ui->checkParseHeader->isChecked());
    ui->checkHeaderIgnoreUnknown->setEnabled(ui->checkParseHeader->isChecked());
}

void AddNewSimpleStructWidget::on_buttonHeaderPathLookup_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Please select the header file to parse", ui->editHeaderFilePath->text());
    if(filePath.isEmpty())
        return;

    ui->editHeaderFilePath->setText(filePath);
}

void AddNewSimpleStructWidget::accept()
{
    if(ui->radioUseId->isChecked()){
        int sizeOfStruct = ui->editSizeOfStruct->text().toInt();
        int offsetID = ui->editOffsetID->text().toInt(0, 16);
        if(offsetID  > sizeOfStruct){
            QMessageBox::warning(this, "Offset Error", "Offset is bigger than the size of the struct!");
            return;
        }
    }

    if(ui->checkParseHeader->isChecked())
        if(!QFile(ui->editHeaderFilePath->text()).exists()){
            QMessageBox::warning(this, "Header File Path Error", "Did not find file \"" + ui->editHeaderFilePath->text() + "\"");
            return;
        }

    QDialog::accept();
}
