#include "multistepdialog.h"
#include "ui_multistepdialog.h"

MultiStepDialog::MultiStepDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MultiStepDialog)
{
    ui->setupUi(this);
}

MultiStepDialog::~MultiStepDialog()
{
    delete ui;
}

void MultiStepDialog::on_buttonBox_accepted()
{
    emit NotifyMultiNext(ui->spinBox->text().toInt());
}
