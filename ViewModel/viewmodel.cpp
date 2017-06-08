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
const int INIT_MEM_ROWS = 64;
const int INIT_MEM_COLUMNS = 100;
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
//    return m_instModel.setItem((r), (c), new QStandardItem(QString::fromStdString((stdStr))));
    m_instModel.setData(m_instModel.index(r, c), QString::fromStdString(stdStr));
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
    m_memModel(*new QStandardItemModel(INIT_MEM_ROWS, INIT_MEM_COLUMNS, this))
{
    initModel();
    connectActions();
    connectMem();
}

// 初始化
void ViewModel::initModel() {
    initInstModel();
    initRsModel();
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

void ViewModel::initMemModel() {
    for (int i = 0; i < INIT_MEM_COLUMNS; i++) {
        m_memModel.setHorizontalHeaderItem(i, new QStandardItem(QString("%1").arg(i)));
    }
    for (int i = 0; i < INIT_MEM_ROWS; i++) {
        m_memModel.setVerticalHeaderItem(i , new QStandardItem(QString("%1").arg(i * INIT_MEM_COLUMNS)));
    }
    m_memTab.setMemTableModel(m_memModel);
}

// 将所有action有关signal连接到slot
void ViewModel::connectActions() {
    connect(&m_mainView, &MainView::NotifyLoadInst, this, &ViewModel::onNotifyLoadInst);
    connect(this, &ViewModel::NotifyLoadInstError, &m_mainView, &MainView::onNotifyLoadInstError);
    connect(&m_mainView, &MainView::NotifyStep, this, &ViewModel::onNotifyStep);
    connect(&m_mainView, &MainView::NotifyClear, this, &ViewModel::onNotifyClear);
}

// 将memModel有关signal连接到slot
void ViewModel::connectMem() {
    connect(&m_memModel, &QStandardItemModel::itemChanged, this, &ViewModel::onNotifyMemChanged);
}

// 根据Tomasulo类更新前端数据
void ViewModel::updateView() {
    updateInst();
    updateStatus();
    updateRS();
    updateMem();
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

void ViewModel::updateStatus() {
    m_infoTab.setTimeText(QString::number(m_tomasulo.timeCounter));
    m_infoTab.setPCText(QString::number(m_tomasulo.pc));
    m_infoTab.setAddText(m_tomasulo.addNow == nullptr ? QString(""): QString::fromStdString(m_tomasulo.addNow->name));
    m_infoTab.setMulText(m_tomasulo.mulNow == nullptr ? QString(""): QString::fromStdString(m_tomasulo.mulNow->name));
    // TODO: CDB?
}

void ViewModel::updateMem() {
    for (size_t i = 0; i < m_tomasulo.mem.mem.size(); i++) {
        size_t row = i / INIT_MEM_COLUMNS;
        size_t col = i % INIT_MEM_COLUMNS;
        setInstStr(m_memModel, row, col, std::to_string(m_tomasulo.mem.get(i)));    // TODO: change to QStr API
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

void ViewModel::onNotifyMemChanged(QStandardItem *item) {
    qDebug() << item->data(Qt::UserRole);
    m_tomasulo.mem.set(item->row() * INIT_INST_COLUMNS + item->column(), item->data().toDouble());
}

