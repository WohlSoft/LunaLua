#ifndef QTREEWIDGETITEMSMBXADDRESS_H
#define QTREEWIDGETITEMSMBXADDRESS_H

#include <QTreeWidgetItem>

class QTreeWidgetItemSMBXAddress : public QTreeWidgetItem
{
public:
    explicit QTreeWidgetItemSMBXAddress(int type = Type) : QTreeWidgetItem(type) {}
    explicit QTreeWidgetItemSMBXAddress(const QStringList &strings, int type = Type) : QTreeWidgetItem(strings, type) {}
    explicit QTreeWidgetItemSMBXAddress(QTreeWidget *view, int type = Type) : QTreeWidgetItem(view, type) {}
    QTreeWidgetItemSMBXAddress(QTreeWidget *view, const QStringList &strings, int type = Type): QTreeWidgetItem(view, strings, type) {}
    QTreeWidgetItemSMBXAddress(QTreeWidget *view, QTreeWidgetItem *after, int type = Type) : QTreeWidgetItem(view, after, type) {}
    explicit QTreeWidgetItemSMBXAddress(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type) {}
    QTreeWidgetItemSMBXAddress(QTreeWidgetItem *parent, const QStringList &strings, int type = Type) : QTreeWidgetItem(parent, strings, type) {}
    QTreeWidgetItemSMBXAddress(QTreeWidgetItem *parent, QTreeWidgetItem *after, int type = Type) : QTreeWidgetItem(parent, after, type) {}
    QTreeWidgetItemSMBXAddress(const QTreeWidgetItem &other) : QTreeWidgetItem(other) {}
    bool operator < (const QTreeWidgetItem &other) const {
        int column = treeWidget()->sortColumn();
        bool successFirst;
        bool successSecond;
        int first = text(column).toInt(&successFirst, 16);
        int second = other.text(column).toInt(&successSecond, 16);
        if(successFirst && successSecond){
            return first < second;
        }else{
            return QTreeWidgetItem::operator <(other);
        }
  }

};

#endif // QTREEWIDGETITEMSMBXADDRESS_H
