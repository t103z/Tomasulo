#ifndef INFOTAB_H
#define INFOTAB_H

#include <QWidget>

class QStandardItemModel;
class QStringListModel;

namespace Ui {
class InfoTab;
}

class InfoTab : public QWidget
{
    Q_OBJECT

public:
    explicit InfoTab(QWidget *parent = 0);
    ~InfoTab();

    void setInstTableModel(QStandardItemModel &model);
    void setRsTableModel(QStandardItemModel &model);
    void setRegsTableModel(QStandardItemModel &model);
    void setLoadTableModel(QStandardItemModel &model);
    void setStoreTableModel(QStandardItemModel &model);
    void setEventsTableModel(QStringListModel &model);
    void setTimeText(const QString &timeText);
    void setPCText(const QString &pcText);
    void setAddText(const QString &addText);
    void setMulText(const QString &mulText);
    void setCDBText(const QString &cdbText);

private:
    Ui::InfoTab *ui;
};

#endif // INFOTAB_H
