#include "memtab.h"
#include "ui_memtab.h"
#include <QStandardItemModel>
#include <QRegExp>
#include <QMessageBox>
#include <Model/Tomasulo/defs.h>

MemTab::MemTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemTab)
{
    ui->setupUi(this);

    connect(ui->btnCheckMem, &QPushButton::clicked, this, &MemTab::onCheckMem);
    connect(ui->editCheckAddr, &QLineEdit::returnPressed, this, &MemTab::onCheckMem);
    connect(ui->btnModifyMem, &QPushButton::clicked, this, &MemTab::onModifyMem);
    connect(ui->editModifyAddr, &QLineEdit::returnPressed, this, &MemTab::onModifyMem);
    connect(ui->editModifyData, &QLineEdit::returnPressed, this, &MemTab::onModifyMem);
}

MemTab::~MemTab()
{
    delete ui;
}

void popUpMessage(const std::string &message) {
    QMessageBox msgBox;
    msgBox.setText(QString::fromStdString(message));
    msgBox.exec();
}

void MemTab::setMemTableModel(QStandardItemModel &model) {
    ui->tableMem->setModel(&model);
    // 等宽设置
    for (size_t c = 0; c < ui->tableMem->horizontalHeader()->count(); ++c)
    {
        ui->tableMem->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
//    for (size_t c = 0; c < ui->tableMem->horizontalHeader()->count(); c++) {
//        ui->tableMem->setColumnWidth(c, 60);
//    }
}

void MemTab::setTableFocus(const QModelIndex &index)
{
    ui->tableMem->setCurrentIndex(index);
}

void MemTab::onCheckMem()
{
    // validate line input value
    QRegExp re("\\d+");
    if (!re.exactMatch(ui->editCheckAddr->text())) {
        popUpMessage("输入的地址有误！");
        return;
    }
    int addr = ui->editCheckAddr->text().toInt();
    if (addr < 0 || addr >= MEM_SIZE) {
        popUpMessage("输入地址越界！");
        return;
    }
    emit NotifyCheckMem(addr);
}

void MemTab::onModifyMem()
{
    QRegExp re_int("\\d+");
    QRegExp re_double("\\d+\\.?\\d*");
    if (!re_int.exactMatch(ui->editModifyAddr->text()) || !re_double.exactMatch(ui->editModifyData->text())) {
        popUpMessage("输入的地址或数据有误！");
        return;
    }
    auto addr = ui->editModifyAddr->text().toInt();
    auto data = ui->editModifyData->text().toDouble();
    if (addr < 0 || addr >= MEM_SIZE) {
        popUpMessage("输入地址越界！");
        return;
    }
    emit NotifyModifyMem(addr, data);
}
