#ifndef MEMTAB_H
#define MEMTAB_H

#include <QWidget>

class QStandardItemModel;
class QModelIndex;

namespace Ui {
class MemTab;
}

class MemTab : public QWidget
{
    Q_OBJECT

public:
    explicit MemTab(QWidget *parent = 0);
    ~MemTab();
    void setMemTableModel(QStandardItemModel &model);
    void setTableFocus(const QModelIndex &index);

signals:
    void NotifyCheckMem(int addr);
    void NotifyModifyMem(int addr, double val);

private slots:
    void onCheckMem();
    void onModifyMem();

private:
    Ui::MemTab *ui;
};

#endif // MEMTAB_H
