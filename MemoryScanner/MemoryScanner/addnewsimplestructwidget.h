#ifndef ADDNEWSIMPLESTRUCTWIDGET_H
#define ADDNEWSIMPLESTRUCTWIDGET_H

#include <QDialog>
#include <qtreewidgetitemsmbxaddress.h>

namespace Ui {
class AddNewSimpleStructWidget;
}

class AddNewSimpleStructWidget : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewSimpleStructWidget(QTreeWidgetItemSMBXAddress* editToItem = 0, QWidget *parent = 0);
    ~AddNewSimpleStructWidget();

    QPair<QTreeWidgetItemSMBXAddress *, QList<QTreeWidgetItem *> > generateNewEntry();
private slots:
    void updateGuiState(bool _1);
    void on_buttonHeaderPathLookup_clicked();
protected:

    void accept();
private:
    Ui::AddNewSimpleStructWidget *ui;
    QTreeWidgetItemSMBXAddress* m_editItem;
    QList<QTreeWidgetItemSMBXAddress* > m_subItems;
};

#endif // ADDNEWSIMPLESTRUCTWIDGET_H
