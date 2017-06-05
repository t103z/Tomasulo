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

void InfoTab::SetInstTableModel(QStandardItemModel &model) {
    ui->tableInst->setModel(&model);
    for (size_t c = 0; c < ui->tableInst->horizontalHeader()->count(); ++c)
    {
        ui->tableInst->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}
