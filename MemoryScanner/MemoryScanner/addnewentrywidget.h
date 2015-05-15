#ifndef ADDNEWENTRYWIDGET_H
#define ADDNEWENTRYWIDGET_H

#include <QDialog>
#include <QVariant>
#include <qtreewidgetitemsmbxaddress.h>

namespace Ui {
class AddNewEntryWidget;
}

class AddNewEntryWidget : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewEntryWidget(QTreeWidgetItemSMBXAddress* editToItem = 0, QDialog *parent = 0);
    ~AddNewEntryWidget();

    QTreeWidgetItemSMBXAddress *generateNewEntry();



private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:


    Ui::AddNewEntryWidget *ui;
    QTreeWidgetItemSMBXAddress* m_editItem;
};

#endif // ADDNEWENTRYWIDGET_H
