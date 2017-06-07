#include "infotab.h"
#include "ui_infotab.h"
#include <QStandardItemModel>

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
    // TODO: 目前是等宽设置
    for (size_t c = 0; c < ui->tableInst->horizontalHeader()->count(); ++c)
    {
        ui->tableInst->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

void InfoTab::setRsTableModel(QStandardItemModel &model) {
    ui->tableRS->setModel(&model);
    for (size_t c = 0; c < ui->tableRS->horizontalHeader()->count(); ++c)
    {
        ui->tableRS->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
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

void InfoTab::setCDBText(const QString &cdbText) {
    ui->textCDB->setText(cdbText);
}
