#ifndef INFOTAB_H
#define INFOTAB_H

#include <QWidget>

class QStandardItemModel;

namespace Ui {
class InfoTab;
}

class InfoTab : public QWidget
{
    Q_OBJECT

public:
    explicit InfoTab(QWidget *parent = 0);
    ~InfoTab();

    void SetInstTableModel(QStandardItemModel &model);
private:
    Ui::InfoTab *ui;
};

#endif // INFOTAB_H
