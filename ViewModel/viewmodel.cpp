#include "viewmodel.h"
#include "View/mainview.h"
#include "View/infotab.h"
#include "Model/Tomasulo.h"
#include "Model/Tomasulo/Ins.h"
#include "Model/Tomasulo/defs.h"
#include <QStandardItemModel>
#include <QString>
#include <QFile>

/*********** checklist************
 * 1. 完成单向Model到View的映射   On-going
 * 2. 完成双向Model和View的映射   TODO
 * 3. 完成Action的映射            Finish (for now)
 * *******************************/

const int INIT_INST_ROWS = 5;
const int INIT_INST_COLUMNS = 8;
const int COL_OPR = 0;
const int COL_OP1 = 1;
const int COL_OP2 = 2;
const int COL_OP3 = 3;


ViewModel::ViewModel(MainView &mainView, InfoTab &infoTab, std::vector<Ins> &inss, Tomasulo &tomasulo) :
    QObject(nullptr),
    m_mainView(mainView),
    m_infoTab(infoTab),
    m_inss(inss),
    m_tomasulo(tomasulo),
    m_instModel(*new QStandardItemModel(INIT_INST_ROWS, INIT_INST_COLUMNS, this))
{
    initModel();
    ConnectActions();
}

// 初始化
void ViewModel::initModel() {
    m_infoTab.SetInstTableModel(m_instModel);
    updateView();
}

// 将所有action有关signal连接到slot
void ViewModel::ConnectActions() {
    connect(&m_mainView, &MainView::NotifyLoadInst, this, &ViewModel::onNotifyLoadInst);
    connect(this, &ViewModel::NotifyLoadInstError, &m_mainView, &MainView::onNotifyLoadInstError);
    connect(&m_mainView, &MainView::NotifyStep, this, &ViewModel::onNotifyStep);
    connect(&m_mainView, &MainView::NotifyClear, this, &ViewModel::onNotifyClear);
}

// 根据Tomasulo类更新前端数据
void ViewModel::updateView() {
    // TODO: 需要完善指令映射
    int row = 0;
    for (auto ins: m_tomasulo.inss) {
        QStandardItem *item = new QStandardItem(QString::fromStdString(opToStr(ins.op)));
        m_instModel.setItem(row, COL_OPR, item);
        row++;
    }

    for (; row < m_instModel.rowCount(); row++) {
        auto emptyItem = new QStandardItem(QString(""));
        m_instModel.setItem(row, COL_OPR, emptyItem);
    }
}

// slots
void ViewModel::onNotifyLoadInst(const QFile &fileName) {
    auto r = Ins::loadInsFromFile(fileName.fileName().toStdString());
    if (r.first.size()) {       // 发生错误
        emit NotifyLoadInstError(r.first);
    }
    // 将未出错的指令读入
    m_tomasulo.reset();
    for (size_t i = 0; i < r.second.size(); i++) {
        m_tomasulo.pushIns(r.second[i]);
    }
    updateView();
}

void ViewModel::onNotifyStep() {
    m_tomasulo.nextTime();
    updateView();
}

void ViewModel::onNotifyClear() {
    m_tomasulo.reset();
    updateView();
}

