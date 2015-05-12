#ifndef ADDNEWSIMPLESTRUCTWIDGET_H
#define ADDNEWSIMPLESTRUCTWIDGET_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
class AddNewSimpleStructWidget;
}

class AddNewSimpleStructWidget : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewSimpleStructWidget(QTreeWidgetItem* editToItem = 0, QWidget *parent = 0);
    ~AddNewSimpleStructWidget();

    QTreeWidgetItem *generateNewEntry();
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void updateGuiState(bool _1);
private:
    Ui::AddNewSimpleStructWidget *ui;
    QTreeWidgetItem* m_editItem;
};

#endif // ADDNEWSIMPLESTRUCTWIDGET_H
