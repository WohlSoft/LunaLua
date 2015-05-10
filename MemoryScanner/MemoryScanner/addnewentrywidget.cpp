#include "addnewentrywidget.h"
#include "ui_addnewentrywidget.h"

AddNewEntryWidget::AddNewEntryWidget(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::AddNewEntryWidget)
{
    ui->setupUi(this);
}

AddNewEntryWidget::~AddNewEntryWidget()
{
    delete ui;
}

QTreeWidgetItem* AddNewEntryWidget::generateNewEntry()
{
    QTreeWidgetItem* newEntry;
    newEntry->setText(0, ui->editName->text());
    newEntry->setText(1, ui->comboFieldType->currentText());
    newEntry->setText(2, ui->editAddress->text());
    newEntry->setFlags(newEntry->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    return newEntry;
}


void AddNewEntryWidget::on_buttonBox_accepted()
{
    accept();
}

void AddNewEntryWidget::on_buttonBox_rejected()
{
    reject();
}
