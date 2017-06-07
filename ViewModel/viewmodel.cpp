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
const int INIT_INST_COLUMNS = 8;        // 指令 TODO: 最终几列？
const int INIT_RS_ROWS = 5;
const int INIT_RS_COLUMNS = 7;          // 保留站
const int COL_OPR = 0;
const int COL_OP1 = COL_OPR + 1;
const int COL_OP2 = COL_OP1 + 1;
const int COL_OP3 = COL_OP2 + 1;
const int COL_ISSUE = COL_OP3 + 1;
const int COL_ExSt = COL_ISSUE + 1;
const int COL_ExFin = COL_ExSt + 1;
const int COL_Wt = COL_ExFin + 1;

inline void setInstStr(QStandardItemModel &m_instModel, int r, int c, const std::string &stdStr) {
    return m_instModel.setItem((r), (c), new QStandardItem(QString::fromStdString((stdStr))));
}

ViewModel::ViewModel(MainView &mainView, InfoTab &infoTab, std::vector<Ins> &inss, Tomasulo &tomasulo) :
    QObject(nullptr),
    m_mainView(mainView),
    m_infoTab(infoTab),
    m_inss(inss),
    m_tomasulo(tomasulo),
    m_instModel(*new QStandardItemModel(INIT_INST_ROWS, INIT_INST_COLUMNS, this)),
    m_rsModel(*new QStandardItemModel(INIT_RS_ROWS, INIT_RS_COLUMNS, this))
{
    initModel();
    ConnectActions();
}

// 初始化
void ViewModel::initModel() {
    initInstModel();
    initRsModel();
    updateView();
}

void ViewModel::initInstModel() {
    m_instModel.setHorizontalHeaderItem(0, new QStandardItem(QString("Type")));
    m_instModel.setHorizontalHeaderItem(1, new QStandardItem(QString("Dst/Reg")));
    m_instModel.setHorizontalHeaderItem(2, new QStandardItem(QString("Src1/Addr")));
    m_instModel.setHorizontalHeaderItem(3, new QStandardItem(QString("Src2")));
    m_instModel.setHorizontalHeaderItem(4, new QStandardItem(QString("Issue")));
    m_instModel.setHorizontalHeaderItem(5, new QStandardItem(QString("Ex/Start")));
    m_instModel.setHorizontalHeaderItem(6, new QStandardItem(QString("Ex/Finish")));
    m_instModel.setHorizontalHeaderItem(7, new QStandardItem(QString("Wt")));
    m_infoTab.setInstTableModel(m_instModel);
}

void ViewModel::initRsModel() {
    m_rsModel.setHorizontalHeaderItem(0, new QStandardItem(QString("Time")));
    m_rsModel.setHorizontalHeaderItem(1, new QStandardItem(QString("Name")));
    m_rsModel.setHorizontalHeaderItem(2, new QStandardItem(QString("Busy")));
    m_rsModel.setHorizontalHeaderItem(3, new QStandardItem(QString("V1")));
    m_rsModel.setHorizontalHeaderItem(4, new QStandardItem(QString("V2")));
    m_rsModel.setHorizontalHeaderItem(5, new QStandardItem(QString("Q1")));
    m_rsModel.setHorizontalHeaderItem(6, new QStandardItem(QString("Q2")));
    m_infoTab.setRsTableModel(m_rsModel);
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
    updateInst();
    updateStatus();
    updateRS();
}

void ViewModel::updateInst() {
    int row = 0;
    for (auto ins: m_tomasulo.inss) {
        setInstStr(m_instModel, row, COL_OPR, opToStr(ins.op));
        if (isArithmeticIns(&ins)) {
            setInstStr(m_instModel, row, COL_OP1, "F" + std::to_string(ins.des));
            setInstStr(m_instModel, row, COL_OP2, "F" + std::to_string(ins.src1));
            setInstStr(m_instModel, row, COL_OP3, "F" + std::to_string(ins.src2));
        } else if (isMemIns(&ins)) {
            setInstStr(m_instModel, row, COL_OP1, "F" + std::to_string(ins.memReg));
            setInstStr(m_instModel, row, COL_OP2, std::to_string(ins.memAddr));
        }
        auto issueStr = ins.issueTime != INVALID_TIME ? std::to_string(ins.issueTime) : "";
        auto execStartStr = ins.execStartTime != INVALID_TIME ? std::to_string(ins.execStartTime) : "";
        auto execFinishStr = ins.execFinishTime != INVALID_TIME ? std::to_string(ins.execFinishTime) : "";
        auto wtStr = ins.writeResultTime != INVALID_TIME ? std::to_string(ins.writeResultTime) : "";
        setInstStr(m_instModel, row, COL_ISSUE, issueStr);
        setInstStr(m_instModel, row, COL_ExSt, execStartStr);
        setInstStr(m_instModel, row, COL_ExFin, execFinishStr);
        setInstStr(m_instModel, row, COL_Wt, wtStr);
        row++;
    }

    for (; row < m_instModel.rowCount(); row++) {
        for (int col = 0; col < m_instModel.columnCount(); col++) {
            m_instModel.setItem(row, col, new QStandardItem(QString("")));
        }
    }
}

void ViewModel::updateRS() {
    // TODO NOW
}

void ViewModel::updateStatus() {
    m_infoTab.setTimeText(QString::number(m_tomasulo.timeCounter));
    m_infoTab.setPCText(QString::number(m_tomasulo.pc));
    m_infoTab.setAddText(m_tomasulo.addNow == nullptr ? QString(""): QString::fromStdString(m_tomasulo.addNow->name));
    m_infoTab.setMulText(m_tomasulo.mulNow == nullptr ? QString(""): QString::fromStdString(m_tomasulo.mulNow->name));
    // TODO: CDB?
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

