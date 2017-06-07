#include "viewmodel.h"
#include "View/mainview.h"
#include "View/infotab.h"
#include "View/memtab.h"
#include "Model/Tomasulo.h"
#include "Model/Tomasulo/Ins.h"
#include "Model/Tomasulo/defs.h"
#include <QStandardItemModel>
#include <QString>
#include <QFile>
#include <QDebug>

/*********** checklist************
 * 1. 完成单向Model到View的映射   On-going
 * 2. 完成双向Model和View的映射   TODO
 * 3. 完成Action的映射            Finish (for now)
 * *******************************/

const int INIT_INST_ROWS = 5;
const int INIT_INST_COLUMNS = 8;        // 指令 TODO: 最终几列？
const int INIT_RS_ROWS = 5;
const int INIT_RS_COLUMNS = 7;          // 保留站
const int INIT_REGS_ROWS = 16;
const int INIT_REGS_COLUMNS = 3;        // 寄存器信息
const int INIT_LOAD_ROWS = 3;
const int INIT_LOAD_COLUMNS = 4;        // Load/Store缓冲
const int INIT_MEM_ROWS = 20;
const int INIT_MEM_COLUMNS = 320;
// 指令序列列号
const int COL_OPR = 0;
const int COL_OP1 = COL_OPR + 1;
const int COL_OP2 = COL_OP1 + 1;
const int COL_OP3 = COL_OP2 + 1;
const int COL_ISSUE = COL_OP3 + 1;
const int COL_ExSt = COL_ISSUE + 1;
const int COL_ExFin = COL_ExSt + 1;
const int COL_Wt = COL_ExFin + 1;
// 保留站列号
const int COL_TIME = 0;
const int COL_NAME = COL_TIME + 1;
const int COL_BUSY = COL_NAME + 1;
const int COL_V1 = COL_BUSY + 1;
const int COL_V2 = COL_V1 + 1;
const int COL_Q1 = COL_V2 + 1;
const int COL_Q2 = COL_Q1 + 1;

inline void setInstStr(QStandardItemModel &m_instModel, int r, int c, const std::string &stdStr) {
    m_instModel.setData(m_instModel.index(r, c), QString::fromStdString(stdStr));
}

inline void setInstQStr(QStandardItemModel &m_instModel, int r, int c, const QString &qStr) {
    m_instModel.setData(m_instModel.index(r, c), qStr);
}

ViewModel::ViewModel(MainView &mainView, InfoTab &infoTab, MemTab &memTab, std::vector<Ins> &inss, Tomasulo &tomasulo) :
    QObject(nullptr),
    m_mainView(mainView),
    m_infoTab(infoTab),
    m_memTab(memTab),
    m_inss(inss),
    m_tomasulo(tomasulo),
    m_instModel(*new QStandardItemModel(INIT_INST_ROWS, INIT_INST_COLUMNS, this)),
    m_rsModel(*new QStandardItemModel(INIT_RS_ROWS, INIT_RS_COLUMNS, this)),
    m_regsModel(*new QStandardItemModel(INIT_REGS_ROWS, INIT_REGS_COLUMNS, this)),
    m_loadModel(*new QStandardItemModel(INIT_LOAD_ROWS, INIT_LOAD_COLUMNS, this)),
    m_storeModel(*new QStandardItemModel(INIT_LOAD_ROWS, INIT_LOAD_COLUMNS, this)),
    m_memModel(*new QStandardItemModel(INIT_MEM_ROWS, INIT_MEM_COLUMNS, this))
{
    initModel();
    ConnectActions();
}

// 初始化
void ViewModel::initModel() {
    initInstModel();
    initRsModel();
    initRegsModel();
    initLoadModel();
    initStoreModel();
    initMemModel();
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

void ViewModel::initRegsModel() {
    m_regsModel.setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
    m_regsModel.setHorizontalHeaderItem(1, new QStandardItem(QString("Value")));
    m_regsModel.setHorizontalHeaderItem(2, new QStandardItem(QString("RS")));
    for (int i = 0; i < INIT_REGS_ROWS; ++i) {
        m_regsModel.setItem(i, 0, new QStandardItem(QString("F%1").arg(i)));
        m_regsModel.setItem(i, 1, new QStandardItem(QString("0")));
    }
    m_infoTab.setRegsTableModel(m_regsModel);
}

void ViewModel::initLoadModel() {
    m_loadModel.setHorizontalHeaderItem(0, new QStandardItem(QString("Time")));
    m_loadModel.setHorizontalHeaderItem(1, new QStandardItem(QString("Name")));
    m_loadModel.setHorizontalHeaderItem(2, new QStandardItem(QString("Address")));
    m_loadModel.setHorizontalHeaderItem(3, new QStandardItem(QString("Value")));
    m_infoTab.setLoadTableModel(m_loadModel);
}

void ViewModel::initStoreModel() {
    m_storeModel.setHorizontalHeaderItem(0, new QStandardItem(QString("Time")));
    m_storeModel.setHorizontalHeaderItem(1, new QStandardItem(QString("Name")));
    m_storeModel.setHorizontalHeaderItem(2, new QStandardItem(QString("Address")));
    m_storeModel.setHorizontalHeaderItem(3, new QStandardItem(QString("Value")));
    m_infoTab.setStoreTableModel(m_storeModel);
}

void ViewModel::initMemModel() {
    m_memTab.setMemTableModel(m_memModel);
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
    updateRegs();
    updateLoad();
    updateStore();
}

void ViewModel::updateInst() {
    int row = m_instModel.rowCount();
    while (row < m_tomasulo.inss.size()) {
        m_instModel.setItem(m_instModel.rowCount(), 0, new QStandardItem(""));
        row++;
    }
    row = 0;
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
            setInstStr(m_instModel, row, col, "");
        }
    }
}

// 更新保留站的helper function
void updateRSManager(QStandardItemModel &model, const RSManager &manager, int &row) {

    for (auto && rs : manager.rss) {
        Ins* ins = rs.ins;
        bool hasIns = (rs.ins != nullptr);
        setInstStr(model, row, COL_TIME, hasIns ? std::to_string(ins->timeLeftToFinish): "");
        setInstStr(model, row, COL_NAME, rs.name);
        setInstStr(model, row, COL_BUSY, std::to_string(hasIns));

        auto formatDouble = [&hasIns](double n) {
            if (!hasIns) {
                return QString("");
            } else if (n == 0.0) {
                return QString("0");
            } else {
                return QString("%1").arg(n);
            }
        };

        model.setItem(row, COL_V1, new QStandardItem(rs.q1 == nullptr ? formatDouble(rs.v1): QString("")));
        model.setItem(row, COL_V2, new QStandardItem(rs.q2 == nullptr ? formatDouble(rs.v2): QString("")));
        setInstStr(model, row, COL_Q1, hasIns ? (rs.q1 == nullptr ? "": rs.q1->name): "");
        setInstStr(model, row, COL_Q2, hasIns ? (rs.q2 == nullptr ? "": rs.q2->name): "");
        row++;
    }
}

void ViewModel::updateRS() {
    // TODO NOW
    int row = 0;
    updateRSManager(m_rsModel, m_tomasulo.addManager, row);
    updateRSManager(m_rsModel, m_tomasulo.mulManager, row);
}

void ViewModel::updateRegs() {
    int row = 0;
    for (auto reg: m_tomasulo.regs) {
        setInstQStr(m_regsModel, row, 1, QString::number(reg.value, 'e', 4));
        setInstStr(m_regsModel, row, 2, reg.srcRS == nullptr ? "": reg.srcRS->name);
        row++;
    }
}

void ViewModel::updateLoad() {
    int row = 0;
    for (auto&& rs: m_tomasulo.ldManager.rss) {
        bool noIns = (rs.ins == nullptr);
        setInstStr(m_loadModel, row, 0, noIns ? "":
                   std::to_string(rs.ins->timeLeftToFinish));
        setInstStr(m_loadModel, row, 1, rs.name);
        setInstStr(m_loadModel, row, 2, noIns ? "": std::to_string(rs.addr));
        setInstQStr(m_loadModel, row, 3, noIns ? QString(""):
                    QString::number(rs.desValue, 'e', 4));
        row++;
    }
}

void ViewModel::updateStore() {
    int row = 0;
    for (auto&& rs: m_tomasulo.stManager.rss) {
        bool noIns = (rs.ins == nullptr);
        setInstStr(m_storeModel, row, 0, noIns ? "":
                   std::to_string(rs.ins->timeLeftToFinish));
        setInstStr(m_storeModel, row, 1, rs.name);
        setInstStr(m_storeModel, row, 2, noIns ? "": std::to_string(rs.addr));
        setInstQStr(m_storeModel, row, 3, noIns ? QString(""):
                    (rs.q ? QString::fromStdString(rs.q->name):
                    QString::number(rs.v, 'e', 4)));
        row++;
    }
    assert(row == INIT_LOAD_ROWS);
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

