#include "addinstdialog.h"
#include "ui_addinstdialog.h"
#include "Model/Tomasulo/Ins.h"
#include "ViewModel/viewmodel.h"

AddInstDialog::AddInstDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddInstDialog)
{
    ui->setupUi(this);
}

AddInstDialog::~AddInstDialog()
{
    delete ui;
}

void AddInstDialog::on_buttonBox_accepted()
{
    emit NotifyAppendInst(ui->plainTextEdit->toPlainText().toStdString());
}

void AddInstDialog::clearText()
{
    ui->plainTextEdit->clear();
}
