#ifndef TOMASULO_TOMASULO_H
#define TOMASULO_TOMASULO_H

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include "Tomasulo/defs.h"
#include "Tomasulo/Ins.h"
#include "Tomasulo/Mem.h"
#include "Tomasulo/ReservationStation.h"
#include "Tomasulo/Register.h"
#include "Tomasulo/Event.h"

class Tomasulo;

using calcFn = std::function<double(InsOp, double, double)>;

/*!
 * 保留站管理器基类
 */
class Manager{
public:
    Manager(std::size_t n, const std::string& name){
        rss.reserve(n);
        for (int i = 1;i <= n;++i) rss.emplace_back(name + std::to_string(i));
    }

    /*!
     * 重置
     */
    void reset(){
        for (auto && r : rss) r.reset();
    }

    /*!
     * 判断是否有空闲保留站
     * @return
     */
    bool isFull(){
        for (auto &&r : rss) {
            if (!r.isBusy) return false;
        }
        return true;
    }

    /*!
     * 取得一个就绪保留站
     * @return
     */
    virtual ReservationStation* getAReadyRS() = 0;

    /*!
     * 取得一个空闲保留站
     * @return
     */
    ReservationStation* getAFreeRS() {
        for (ReservationStation& r : rss) {
            if (!r.isBusy) return &r;
        }
        return nullptr;
    }

    /*!
     * 广播信息处理
     * @param rs 保留站
     */
    void accept(ReservationStation* rs, EventCallBack callBack) {
        for (auto&& r : rss) r.accept(rs, callBack);
    }

    std::vector<ReservationStation> rss;            // 保留站
};

/*!
 * 加法、乘法保留站管理器
 */
class RSManager : public Manager{
public:
    RSManager(std::size_t n, const std::string& name, calcFn& fn) : Manager(n, name), fn(fn) {}

    /*!
     * 取得一个就绪保留站
     * @return 所有就绪保留站中具有指令id最小的保留站
     */
    ReservationStation *getAReadyRS() override {
        int insIndx = -1;
        ReservationStation* rs = nullptr;
        for (ReservationStation& r : rss) {
            if (r.isReady()) {
                if (insIndx == -1) {
                    insIndx = r.ins->index;
                    rs = &r;
                } else if (r.ins->index < insIndx) {
                    rs = &r;
                    insIndx = r.ins->index;
                }
            }
        }

        return rs;
    }

    calcFn& fn;
};

using lsFn = std::function<double(InsOp, int, double)>;

/*!
 * load、store缓冲管理器
 */
class LSBManager : public Manager{
public:
    LSBManager(std::size_t n, const std::string& name, lsFn& fn) : Manager(n, name), fn(fn) {}

    /*!
     * 取得一个就绪保留站
     * 不允许调用该函数
     * @return 错误
     */
    ReservationStation *getAReadyRS() override {
        assert(false); // not allowed to call LSBManager::getAReadyRS
        return nullptr;
    }

    /*!
     * 取得一个执行到cycle周期的保留站
     * @param op 指令类型
     * @param cycle 周期[0, getCycleOfOp(op))
     * @return 满足上述条件且在所有满足条件保留站中指令id最小的保留站
     */
    ReservationStation* getARSForCycle(InsOp op, size_t cycle) {
        assert(op == InsOp::LD || op == InsOp::ST);

        int insIndx = -1;
        ReservationStation* rs = nullptr;
        for (ReservationStation& r : rss) {
            if (r.isReady() && r.ins->timeLeftToFinish == getCycleOfInsOp(op) - cycle) {
                if (insIndx == -1) {
                    insIndx = r.ins->index;
                    rs = &r;
                } else if (r.ins->index < insIndx) {
                    rs = &r;
                    insIndx = r.ins->index;
                }
            }
        }

        return rs;
    }

    /*!
     * 取得计算地址的保留站
     * 别名函数：取得第0阶段保留站
     * @param op
     * @return
     */
    ReservationStation* getARSToCalcAddr(InsOp op) {
        if (op == InsOp::LD) return getARSForCycle(op, 0);

        assert(op != InsOp::LD);
        return nullptr;
    }

    /*!
     * 取得进行访存的保留站
     * LD指令执行阶段第1周期访存
     * ST指令写回阶段访存
     * @param op
     * @return 满足上述条件且在所有满足条件保留站中指令id最小的保留站
     */
    ReservationStation* getARSToAccessMem(InsOp op) {
        assert(op == InsOp::LD || op == InsOp::ST);
        static auto checkStore = [](ReservationStation& r) -> bool {
            return r.isReady() && r.ins->timeLeftToFinish == 0;
        };

        static auto checkLoad = [](ReservationStation& r) -> bool {
            return r.isReady() && r.ins->timeLeftToFinish == 1;
        };

        std::function<bool(ReservationStation&)> check = checkLoad;
        if (op == InsOp::ST) check = checkStore;

        int insIndx = -1;
        ReservationStation* rs = nullptr;
        for (ReservationStation& r : rss) {
            if (check(r)) {
                if (insIndx == -1) {
                    insIndx = r.ins->index;
                    rs = &r;
                } else if (r.ins->index < insIndx) {
                    rs = &r;
                    insIndx = r.ins->index;
                }
            }
        }

        return rs;
    }

    lsFn& fn;
};

class Tomasulo {
public:
    Tomasulo(const std::vector<Ins>& inss = std::vector<Ins>{}) :
            addManager(ADDER_SIZE, "Add", addFn),
            mulManager(MULER_SIZE, "Mult", mulFn),
            ldManager(LDBUF_SIZE, "Load", loadFn),
            stManager(STBUF_SIZE, "Store", storeFn),
            mem(MEM_SIZE),
            regs(REG_SIZE),
            inss(inss) {
        for (int i = 0; i < this->inss.size(); ++i) {
            this->inss[i].index = i + 1;
        }

        for (int i = 0; i < this->regs.size(); ++i) {
            this->regs[i].index = i;
        }
    }

    /*!
     * 重置
     */
    void reset(){
        inss.clear();
        for(auto&& r : regs) r.reset();
        mem.reset();
        addManager.reset();
        addNow = nullptr;
        mulManager.reset();
        mulNow = nullptr;
        ldManager.reset();
        stManager.reset();
        loadCycleZero = nullptr;

        pendingWrite.clear();
        isMemUsed = false;

        timeCounter = 0;
        pc = 0;
        events.clear();
    }

    /*!
     * 添加指令
     * @param ins
     */
    void pushIns(const Ins& ins){
        inss.push_back(ins);
        inss.back().index = inss.size();
    }

    /*!
     * 运行一个周期
     */
    void nextTime();

    std::vector<Ins> inss;                              // 指令序列
    std::vector<Register> regs;                         // 寄存器堆
    Mem mem;                                            // 存储器

    RSManager addManager;                               // 加法保留站
    ReservationStation* addNow = nullptr;               // 加法器

    RSManager mulManager;                               // 乘法保留站
    ReservationStation* mulNow = nullptr;               // 乘法器

    LSBManager ldManager;                               // LD缓冲
    LSBManager stManager;                               // ST缓冲

    int timeCounter = 0;                                // 计时器
    int pc = 0;                                         // 程序计数器
    std::vector<Event> events;                          // 事件

    bool isEventHappened() const {
        return !events.empty();
    }

private:
    void issue();
    void exec();
    void write();

    bool isMemUsed = false;
    ReservationStation* loadCycleZero = nullptr;
    std::vector<ReservationStation*> pendingWrite;

    calcFn addFn = [](InsOp op, double a, double b) -> double{
        switch (op){
            case InsOp::ADDD: return a+b;
            case InsOp::SUBD: return a-b;
            default:
                assert(false); // addFn
                return 0;
        }
    };

    calcFn mulFn = [](InsOp op, double a, double b) -> double{
        switch (op){
            case InsOp::MULTD: return a*b;
            case InsOp::DIVD: return a/b;
            default:
                assert(false); // mulFn
                return 0;
        }
    };

    lsFn loadFn = [this](InsOp op, int addr, double value) -> double {
        assert(op == InsOp::LD);
        return mem.get(addr);
    };

    lsFn storeFn = [this](InsOp op, int addr, double value) -> double {
        assert(op == InsOp::ST);
        mem.set(addr, value);
        assert(mem.get(addr) == value);
        return value;
    };

    bool checkLDSTSafe(Ins* ins) {
        Manager *manager = &ldManager;
        if (ins->op == InsOp::LD) manager = &stManager;
        for (auto &&r : manager->rss) {
            if (r.ins && r.ins->index < ins->index && r.ins->memAddr == ins->memAddr) {
                return false;
            }
        }
        return true;
    }

    void eventHappen(Event&& event = Event()){
        events.push_back(std::move(event));
    }

    EventCallBack eventCallBack = [this](Event&& event) -> void {
        eventHappen(std::move(event));
    };
};

std::ostream& operator<<(std::ostream& out, const Tomasulo& t);

#endif //TOMASULO_TOMASULO_H
