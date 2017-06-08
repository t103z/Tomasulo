#include "viewmodel.h"
#include "View/mainview.h"
#include "View/infotab.h"
#include "View/memtab.h"
#include "View/addinstdialog.h"
#include "Model/Tomasulo.h"
#include "Model/Tomasulo/Ins.h"
#include "Model/Tomasulo/defs.h"
#include <QStandardItemModel>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QTimer>

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
const int INIT_MEM_ROWS = 410;
const int INIT_MEM_COLUMNS = 10;
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

void ViewModel::setInstStr(QStandardItemModel &m_instModel, int r, int c, const std::string &stdStr) {
    auto oldVal = m_instModel.data(m_instModel.index(r, c), Qt::DisplayRole).toString();
    auto newVal = QString::fromStdString(stdStr);
    if (m_running && newVal != oldVal) {
        if (m_instModel.item(r, c)) m_instModel.item(r, c)->setBackground(Qt::yellow);
    } else {
        if (m_instModel.item(r, c)) m_instModel.item(r, c)->setBackground(Qt::transparent);
    }
    if (!m_running && m_instModel.item(r, c)) {
        m_instModel.item(r, c)->setBackground(Qt::transparent);
    }
    m_instModel.setData(m_instModel.index(r, c), QString::fromStdString(stdStr));

}

void ViewModel::setInstQStr(QStandardItemModel &m_instModel, int r, int c, const QString &qStr) {
    auto oldVal = m_instModel.data(m_instModel.index(r, c), Qt::DisplayRole).toString();
    if (qStr != oldVal) {
        if (m_instModel.item(r, c)) m_instModel.item(r, c)->setBackground(Qt::yellow);
    } else {
        if (m_instModel.item(r, c)) m_instModel.item(r, c)->setBackground(Qt::transparent);
    }
    if (!m_running && m_instModel.item(r, c)) {
        m_instModel.item(r, c)->setBackground(Qt::transparent);
    }
    m_instModel.setData(m_instModel.index(r, c), qStr);
}

ViewModel::ViewModel(MainView &mainView, InfoTab &infoTab, MemTab &memTab,
                     std::vector<Ins> &inss, Tomasulo &tomasulo) :
    QObject(nullptr),
    m_mainView(mainView),
    m_infoTab(infoTab),
    m_memTab(memTab),
    m_addInstDialog(nullptr),
    m_inss(inss),
    m_tomasulo(tomasulo),
    m_instModel(*new QStandardItemModel(INIT_INST_ROWS, INIT_INST_COLUMNS, this)),
    m_rsModel(*new QStandardItemModel(INIT_RS_ROWS, INIT_RS_COLUMNS, this)),
    m_regsModel(*new QStandardItemModel(INIT_REGS_ROWS, INIT_REGS_COLUMNS, this)),
    m_loadModel(*new QStandardItemModel(INIT_LOAD_ROWS, INIT_LOAD_COLUMNS, this)),
    m_storeModel(*new QStandardItemModel(INIT_LOAD_ROWS, INIT_LOAD_COLUMNS, this)),
    m_memModel(*new QStandardItemModel(INIT_MEM_ROWS, INIT_MEM_COLUMNS, this)),
    m_updatingView(false),
    m_running(false)
{
    initModel();
    connectActions();
    connectMem();
    connectRegs();
    setTimer();
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
    connect(&m_mainView, &MainView::NotifyAddInst, this, &ViewModel::onNotifyAddInst);
    connect(&m_mainView, &MainView::NotifyPlay, this, &ViewModel::onNotifyPlay);
    connect(&m_mainView, &MainView::NotifyFastFoward, this, &ViewModel::onNotifyFastFoward);
    connect(&m_mainView, &MainView::NotifyBack, this, &ViewModel::onNotifyBack);
}

// 将memModel有关signal连接到slot
void ViewModel::connectMem() {
    connect(&m_memModel, &QStandardItemModel::itemChanged, this, &ViewModel::onNotifyMemChanged);
    connect(&m_memTab, &MemTab::NotifyCheckMem, this, &ViewModel::onNotifyCheckMem);
    connect(&m_memTab, &MemTab::NotifyModifyMem, this, &ViewModel::onNotifyModifyMem);
}

// 将regsModel有关signal连接到slot
void ViewModel::connectRegs() {
    connect(&m_regsModel, &QStandardItemModel::itemChanged, this, &ViewModel::onNotifyRegsChanged);
}

// 设置计时器
void ViewModel::setTimer() {
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &ViewModel::onNotifyStep);
}

// 根据Tomasulo类更新前端数据
void ViewModel::updateView() {
    m_updatingView = true;
    updateInst();
    updateStatus();
    updateRS();
    updateMem();
    updateRegs();
    updateLoad();
    updateStore();
    updateAction();
    m_updatingView = false;
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
void ViewModel::updateRSManager(QStandardItemModel &model, const RSManager &manager, int &row) {

    for (auto && rs : manager.rss) {
        Ins* ins = rs.ins;
        bool hasIns = (rs.ins != nullptr);
        setInstStr(model, row, COL_TIME, hasIns ? std::to_string(ins->timeLeftToFinish): "");
        setInstStr(model, row, COL_NAME, rs.name);
        setInstStr(model, row, COL_BUSY, hasIns ? "True" : "False");

        auto formatDouble = [&hasIns](double n) {
            if (!hasIns) {
                return QString("");
            } else if (n == 0.0) {
                return QString("0");
            } else {
                return QString::number(n, 'g', 4);
            }
        };

        setInstQStr(model, row, COL_V1, rs.q1 == nullptr ? formatDouble(rs.v1) : QString(""));
        setInstQStr(model, row, COL_V2, rs.q2 == nullptr ? formatDouble(rs.v2) : QString(""));
        setInstStr(model, row, COL_Q1, hasIns ? (rs.q1 == nullptr ? "": rs.q1->name): "");
        setInstStr(model, row, COL_Q2, hasIns ? (rs.q2 == nullptr ? "": rs.q2->name): "");
        row++;
    }
}

void ViewModel::updateRS() {
    int row = 0;
    updateRSManager(m_rsModel, m_tomasulo.addManager, row);
    updateRSManager(m_rsModel, m_tomasulo.mulManager, row);
}

void ViewModel::updateRegs() {
    int row = 0;
    for (auto reg: m_tomasulo.regs) {
        setInstQStr(m_regsModel, row, 1, QString::number(reg.value, 'g', 4));
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
                    QString::number(rs.desValue, 'g', 4));
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
                    QString::number(rs.v, 'g', 4)));
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

void ViewModel::updateMem() {
    for (size_t i = 0; i < m_tomasulo.mem.mem.size(); i++) {
        size_t row = i / INIT_MEM_COLUMNS;
        size_t col = i % INIT_MEM_COLUMNS;
        setInstStr(m_memModel, row, col, std::to_string(m_tomasulo.mem.get(i)));    // TODO: change to QStr API
    }
}

void ViewModel::updateAction() {
    if (m_running) {
        m_mainView.disableAddInst();
    }
    else {
        m_mainView.enableAddInst();
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
    m_running = false;
    updateView();
}

void ViewModel::onNotifyAppendInst(const std::string &insStr) {
    auto r = Ins::loadInsFromString(insStr);
    if (r.first.size()) {
        emit NotifyLoadInstError(r.first);
    }
    for (size_t i = 0; i < r.second.size(); ++i) {
        m_tomasulo.pushIns(r.second[i]);
    }
    updateView();
}

void ViewModel::onNotifyStep() {
    m_tomasulo.nextTime();
    m_running = true;
    updateView();
    if (m_tomasulo.isEventHappened()) m_timer.stop();
}

void ViewModel::onNotifyPlay() {
    if (m_tomasulo.isAllFinished()) return;
    m_running = true;
    m_timer.start();
}

void ViewModel::onNotifyFastFoward(int steps) {
    m_running = true;
    while (steps--) {
        m_tomasulo.nextTime();
        if (m_tomasulo.isAllFinished()) break;
    }
    updateView();
}

void ViewModel::onNotifyClear() {
    m_tomasulo.reset();
    m_running = false;
    updateView();
}

void ViewModel::onNotifyAddInst() {
    m_addInstDialog = new AddInstDialog(&m_mainView);
    connect(m_addInstDialog, &AddInstDialog::NotifyAppendInst, this, &ViewModel::onNotifyAppendInst);
    m_addInstDialog->exec();

    delete m_addInstDialog;
    m_addInstDialog = nullptr;
    updateView();
}

void ViewModel::onNotifyMemChanged(QStandardItem *item) {
    if (m_updatingView) return;
    size_t addr = item->row() * INIT_MEM_COLUMNS + item->column();
    double value = item->data(Qt::DisplayRole).toDouble();
    m_tomasulo.mem.set(addr, value);
}

void ViewModel::onNotifyRegsChanged(QStandardItem *item) {
    if (m_updatingView) return;
    m_tomasulo.regs[item->row()].value = item->data(Qt::DisplayRole).toDouble();
}

void ViewModel::onNotifyCheckMem(int addr) {
    m_memTab.setTableFocus(m_memModel.index(addr / INIT_MEM_COLUMNS, addr % INIT_MEM_COLUMNS));
}

void ViewModel::onNotifyModifyMem(int addr, double val) {
    m_tomasulo.mem.set(addr, val);
    m_updatingView = true;
    setInstQStr(m_memModel, addr / INIT_MEM_COLUMNS, addr % INIT_MEM_COLUMNS, QString::number(val, 'g', 4));
    m_updatingView = false;
    m_memTab.setTableFocus(m_memModel.index(addr / INIT_MEM_COLUMNS, addr % INIT_MEM_COLUMNS));
}

void ViewModel::onNotifyBack() {
    int current = m_tomasulo.timeCounter;
    if (current == 0) {
        m_running = false;
    } else {
        m_running = true;
        m_tomasulo.restart();
        for (int i = 0; i < current - 1; i++) {
            m_tomasulo.nextTime();
        }
    }
    updateView();
}
