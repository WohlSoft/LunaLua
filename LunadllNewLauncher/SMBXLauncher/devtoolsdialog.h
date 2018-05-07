#ifndef DEVTOOLSDIALOG_H
#define DEVTOOLSDIALOG_H

#include <QDialog>
#include <QWebEnginePage>

namespace Ui {
class DevToolsDialog;
}

class DevToolsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DevToolsDialog(QWidget *parent = 0);
    ~DevToolsDialog();

    void AssociateWithPage(QWebEnginePage *page);

private:
    Ui::DevToolsDialog *ui;
};

#endif // DEVTOOLSDIALOG_H
