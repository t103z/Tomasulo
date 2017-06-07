#ifndef MEMTAB_H
#define MEMTAB_H

#include <QWidget>

class QStandardItemModel;

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

private:
    Ui::MemTab *ui;
};

#endif // MEMTAB_H
