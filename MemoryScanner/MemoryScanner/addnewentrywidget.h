#ifndef ADDNEWENTRYWIDGET_H
#define ADDNEWENTRYWIDGET_H

#include <QDialog>
#include <QVariant>
#include <QTreeWidgetItem>

namespace Ui {
class AddNewEntryWidget;
}

class AddNewEntryWidget : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewEntryWidget(QDialog *parent = 0);
    ~AddNewEntryWidget();

    QTreeWidgetItem *generateNewEntry();
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::AddNewEntryWidget *ui;
};

#endif // ADDNEWENTRYWIDGET_H
