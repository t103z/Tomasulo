#include "mainview.h"
#include "ui_mainview.h"
#include "View/infotab.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QMessageBox>

MainView::MainView(QWidget *parent, InfoTab &info) :
    QMainWindow(parent),
    m_infoTab(info),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    m_infoTab.setParent(this);
    ui->layoutInfo->addWidget(&m_infoTab);
}

MainView::~MainView()
{
    delete ui;
}

// slots
void MainView::on_actionLoadInst_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Instruction FIle"), QDir::currentPath());
    emit NotifyLoadInst(fileName);
}

void MainView::onNotifyLoadInstError(const std::vector<int> &lineNums) {
    QMessageBox msgBox;
    msgBox.setText("文件中有非法指令！");
    QString lineNumString("");
    for (auto lineNum: lineNums) {
        lineNumString.append(QString::number(lineNum) + QString(" "));
    }
    msgBox.setInformativeText(QString("错误行号: ") + lineNumString + QString("\n所有其他指令已读取"));
    msgBox.setStandardButtons(QMessageBox::Close);
    msgBox.exec();
}

void MainView::on_actionStep_triggered()
{
    emit NotifyStep();
}

void MainView::on_actionClear_triggered()
{
    emit NotifyClear();
}
