#pragma once
#include <QObject>

class MainView;
class InfoTab;
class MemTab;
class AddInstDialog;
class MultiStepDialog;
class Ins;
class Tomasulo;
class QStandardItemModel;
class QStandardItem;
class QStringListModel;
class QFile;

class ViewModel : public QObject
{
    Q_OBJECT
public:
    explicit ViewModel(MainView &mainView, InfoTab &infoTab, MemTab &memTab,
                       std::vector<Ins> &inss, Tomasulo &tomasulo);

public slots:
    void onNotifyLoadInst(const QFile &fileName);
    void onNotifyAppendInst(const std::string &insStr);
    void onNotifyStep();
    void onNotifyClear();
    void onNotifyAddInst();
    void onNotifyMultiStep();
    void onNotifyMultiNext(int nIns);
    void onNotifyMemChanged(QStandardItem *item);
    void onNotifyRegsChanged(QStandardItem *item);
    void onNotifyCheckMem(int addr);
    void onNotifyModifyMem(int addr, double val);

signals:
    void NotifyLoadInstError(const std::vector<int> &lineNums);

private:
    MainView &m_mainView;
    InfoTab &m_infoTab;
    MemTab &m_memTab;
    AddInstDialog &m_addInstDialog;
    MultiStepDialog &m_multiStepDialog;
    std::vector<Ins> &m_inss;
    Tomasulo &m_tomasulo;

    bool m_updatingView;
    bool m_running;

    QStandardItemModel &m_instModel;    // 指令序列
    QStandardItemModel &m_rsModel;      // 保留站
    QStandardItemModel &m_regsModel;    // 寄存器信息
    QStandardItemModel &m_loadModel;      // Load缓冲
    QStandardItemModel &m_storeModel;      // Store缓冲
    QStandardItemModel &m_memModel;     // 内存
    QStringListModel &m_eventsModel;      // 事件列表

    explicit ViewModel(const ViewModel& rhs) = delete;
    ViewModel& operator= (const ViewModel& rhs) = delete;
    void initModel();
    void initInstModel();
    void initRsModel();
    void initRegsModel();
    void initLoadModel();
    void initStoreModel();
    void initMemModel();
    void initEventsModel();
    void connectActions();
    void connectMem();
    void connectRegs();
    void updateView();
    void updateInst();
    void updateStatus();
    void updateRS();
    void updateMem();
    void updateRegs();
    void updateLoad();
    void updateStore();
    void updateAction();
    void updateEvents();
};
