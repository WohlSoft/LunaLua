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
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    ui->widget->page()->setInspectedPage(page);
#else
    Q_UNUSED(page)
#endif
}
