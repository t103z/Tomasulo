#include "memtab.h"
#include "ui_memtab.h"
#include <QStandardItemModel>

MemTab::MemTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemTab)
{
    ui->setupUi(this);
}

MemTab::~MemTab()
{
    delete ui;
}

void MemTab::setMemTableModel(QStandardItemModel &model) {
    ui->tableMem->setModel(&model);
    // TODO: 目前是等宽设置
//    for (size_t c = 0; c < ui->tableMem->horizontalHeader()->count(); ++c)
//    {
//        ui->tableMem->horizontalHeader()->setSectionResizeMode(
//            c, QHeaderView::Stretch);
//    }
    for (size_t c = 0; c < ui->tableMem->horizontalHeader()->count(); c++) {
        ui->tableMem->setColumnWidth(c, 60);
    }
}
