#include "infotab.h"
#include "ui_infotab.h"
#include "Viewmodel/columnnoedit.h"
#include <QStandardItemModel>
#include <QStringListModel>

InfoTab::InfoTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoTab)
{
    ui->setupUi(this);
}

InfoTab::~InfoTab()
{
    delete ui;
}

void InfoTab::setInstTableModel(QStandardItemModel &model) {
    ui->tableInst->setModel(&model);
    ui->tableInst->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // TODO: 目前是等宽设置
    for (size_t c = 0; c < ui->tableInst->horizontalHeader()->count(); ++c)
    {
        ui->tableInst->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

void InfoTab::setRsTableModel(QStandardItemModel &model) {
    ui->tableRS->setModel(&model);
    ui->tableRS->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for (size_t c = 0; c < ui->tableRS->horizontalHeader()->count(); ++c)
    {
        ui->tableRS->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

void InfoTab::setRegsTableModel(QStandardItemModel &model) {
    ui->tableRegs->setModel(&model);
    //ui->tableRegs->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for (size_t c = 0; c < ui->tableRegs->horizontalHeader()->count(); ++c)
    {
        ui->tableRegs->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
    for (size_t c = 0; c < model.columnCount(); ++c)
        if (c != 1)
            ui->tableRegs->setItemDelegateForColumn(c, new NoEditableDelegate(ui->tableRegs));
}

void InfoTab::setLoadTableModel(QStandardItemModel &model) {
    ui->tableLQ->setModel(&model);
    ui->tableLQ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for (size_t c = 0; c < ui->tableLQ->horizontalHeader()->count(); ++c)
    {
        ui->tableLQ->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

void InfoTab::setStoreTableModel(QStandardItemModel &model) {
    ui->tableSQ->setModel(&model);
    ui->tableSQ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    for (size_t c = 0; c < ui->tableSQ->horizontalHeader()->count(); ++c)
    {
        ui->tableSQ->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

void InfoTab::setEventsTableModel(QStringListModel &model) {
    ui->listEvents->setModel(&model);
    ui->listEvents->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void InfoTab::setTimeText(const QString &timeText) {
    ui->textTime->setText(timeText);
}

void InfoTab::setPCText(const QString &pcText) {
    ui->textPC->setText(pcText);
}

void InfoTab::setAddText(const QString &addText) {
    ui->textAdd->setText(addText);
}

void InfoTab::setMulText(const QString &mulText) {
    ui->textMul->setText(mulText);
}

void InfoTab::setScrollBar() {
    ui->listEvents->scrollToBottom();
}
