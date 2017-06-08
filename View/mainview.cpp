#include "mainview.h"
#include "ui_mainview.h"
#include "View/infotab.h"
#include "View/memtab.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>

MainView::MainView(QWidget *parent, InfoTab &info, MemTab &mem) :
    QMainWindow(parent),
    m_infoTab(info),
    m_memTab(mem),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    m_infoTab.setParent(this);
    m_memTab.setParent(this);
    ui->layoutInfo->addWidget(&m_infoTab);
    ui->layoutMem->addWidget(&m_memTab);
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
    QMessageBox msgBox{QMessageBox::Critical,"错误", "文件中有非法指令！"};
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

void MainView::on_actionAddInst_triggered()
{
    emit NotifyAddInst();
}

void MainView::disableAddInst() {
    ui->actionAddInst->setEnabled(false);
}

void MainView::enableAddInst() {
    ui->actionAddInst->setEnabled(true);
}

void MainView::on_actionPlay_triggered()
{
    emit NotifyPlay();
}

void MainView::on_actionFastFoward_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("请输入要执行的步数"), tr("步数:"), QLineEdit::Normal, QString("0"), &ok);
    QRegExp re("\\d+");
    if (!re.exactMatch(text)) {
        QMessageBox msgBox{QMessageBox::Critical, "错误", "输入格式有误"};
        msgBox.exec();
        return;
    }
    emit NotifyFastFoward(text.toInt());
}

void MainView::on_actionBack_triggered()
{
    emit NotifyBack();
}
