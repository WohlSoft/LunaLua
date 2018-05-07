#include "devtoolsdialog.h"
#include "ui_devtoolsdialog.h"
#include <QWebEngineView>
#include <QWebEnginePage>

DevToolsDialog::DevToolsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DevToolsDialog)
{
    ui->setupUi(this);
}

DevToolsDialog::~DevToolsDialog()
{
    delete ui;
}

void DevToolsDialog::AssociateWithPage(QWebEnginePage *page)
{
    ui->widget->page()->setInspectedPage(page);
}
