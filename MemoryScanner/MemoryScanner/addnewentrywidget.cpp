#include "addnewentrywidget.h"
#include "ui_addnewentrywidget.h"

AddNewEntryWidget::AddNewEntryWidget(QTreeWidgetItem *editToItem, QDialog *parent) :
    QDialog(parent),
    ui(new Ui::AddNewEntryWidget),
    m_editItem(editToItem)
{
    ui->setupUi(this);
    if(editToItem){
        ui->editName->setText(editToItem->text(0));
        ui->editAddress->setText(editToItem->text(1));
        ui->comboFieldType->setCurrentText(editToItem->text(2));
    }
}

AddNewEntryWidget::~AddNewEntryWidget()
{
    delete ui;
}

QTreeWidgetItem *AddNewEntryWidget::generateNewEntry()
{
    QTreeWidgetItem* newEntry = new QTreeWidgetItem();
    newEntry->setText(0, ui->editName->text());
    newEntry->setText(1, ui->editAddress->text());
    newEntry->setText(2, ui->comboFieldType->currentText());
    newEntry->setData(0, Qt::UserRole+1, "entry");
    newEntry->setFlags(newEntry->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    newEntry->setCheckState(0, Qt::Unchecked);
    return newEntry;
}



void AddNewEntryWidget::on_buttonBox_accepted()

{
    if(m_editItem){
        m_editItem->setText(0, ui->editName->text());
        m_editItem->setText(1, ui->editAddress->text());
        m_editItem->setText(2, ui->comboFieldType->currentText());
    }
    accept();
}

void AddNewEntryWidget::on_buttonBox_rejected()
{
    reject();
}

