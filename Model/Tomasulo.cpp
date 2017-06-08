#include <iomanip>
#include <random>
#include "Tomasulo.h"

using namespace std;

void Tomasulo::issue() {
    if (pc >= inss.size()) return;

    Ins* ins = &inss[pc];
    Manager* manager = nullptr;
    if (isAdderIns(ins)){
        if (!addManager.isFull()) manager = &addManager;
    } else if (isMulerIns(ins)) {
        if (!mulManager.isFull()) manager = &mulManager;
    } else if (isLoadIns(ins)) {
        if (!ldManager.isFull()) manager = &ldManager;
    } else if (isStoreIns(ins)) {
        if (!stManager.isFull()) manager = &stManager;
    } else {
        assert(false);
    }

    if (manager == nullptr) return ;
    ReservationStation* rs = manager->getAFreeRS();
    assert(rs && !rs->isBusy);
    rs->reset();

    rs->isBusy = true;
    rs->ins = ins;

    ins->issueTime = timeCounter;
    ins->timeLeftToFinish = getCycleOfInsOp(ins->op);

    if (isArithmeticIns(ins)) {
        if (regs[ins->src1].srcRS != nullptr){
            rs->q1 = regs[ins->src1].srcRS;
        } else {
            rs->v1 = regs[ins->src1].value;
        }

        if (regs[ins->src2].srcRS != nullptr){
            rs->q2 = regs[ins->src2].srcRS;
        } else {
            rs->v2 = regs[ins->src2].value;
        }
        regs[ins->des].srcRS = rs;

        if (rs->isReady() && addNow == nullptr) {
            if (isAdderIns(ins)) addNow = rs;
            else {assert(isMulerIns(ins)); mulNow = rs;}
        }
    } else {
        assert(isMemIns(ins));
        if (isLoadIns(ins)) {
            rs->addr = ins->memAddr;
            regs[ins->memReg].srcRS = rs;
        } else {
            assert(isStoreIns(ins));
            rs->addr = ins->memAddr;
            if (regs[ins->memReg].srcRS != nullptr) {
                rs->q = regs[ins->memReg].srcRS;
            } else {
                rs->v = regs[ins->memReg].value;
                loadCycleZero = rs;
            }
        }
    }

    eventHappen(insIssueEvent(rs));
    ++pc;
}

void Tomasulo::exec() {
    // adder exec
    if (addNow != nullptr) {
        Ins* ins = addNow->ins;
        if (ins->timeLeftToFinish == getCycleOfInsOp(ins->op)) {
            ins->execStartTime = timeCounter;
            eventHappen(insExecStartEvent(addNow));
        }

        assert(ins->timeLeftToFinish > 0);
        if (--ins->timeLeftToFinish == 0) {
            ins->execFinishTime = timeCounter;
            addNow->desValue = (addManager.fn)(ins->op, addNow->v1, addNow->v2);
            pendingWrite.push_back(addNow);

            eventHappen(insExecFinishEvent(addNow));
            addNow = nullptr;
        }
    } else {
        addNow = addManager.getAReadyRS();
    }

    // muler exec
    if (mulNow != nullptr) {
        Ins* ins = mulNow->ins;
        if (ins->timeLeftToFinish == getCycleOfInsOp(ins->op)) {
            ins->execStartTime = timeCounter;
            eventHappen(insExecStartEvent(mulNow));
        }

        assert(ins->timeLeftToFinish > 0);
        if (--ins->timeLeftToFinish == 0){
            ins->execFinishTime = timeCounter;
            mulNow->desValue = (mulManager.fn)(ins->op, mulNow->v1, mulNow->v2);
            pendingWrite.push_back(mulNow);

            eventHappen(insExecFinishEvent(mulNow));
            mulNow = nullptr;
        }
    } else {
        mulNow = mulManager.getAReadyRS();
    }

    // load exec
    ReservationStation* mrs = ldManager.getARSToAccessMem(InsOp::LD);
//    for (ReservationStation& rs : ldManager.rss) {
//        Ins* ins = rs.ins;
//        if (ins != nullptr && ins->timeLeftToFinish == getCycleOfInsOp(InsOp::LD)) {
//            --ins->timeLeftToFinish;
//        }
//    }
    ReservationStation* crs = ldManager.getARSToCalcAddr(InsOp::LD);
    if (crs != nullptr) {
        Ins* ins = crs->ins;

        ins->execStartTime = timeCounter;
        --ins->timeLeftToFinish;
        eventHappen(insExecStartEvent(crs));
    }

    if (!isMemUsed && mrs && checkLDSTSafe(mrs->ins)) {
        assert(crs != mrs);
        Ins* ins = mrs->ins;
        if (ins->timeLeftToFinish > 0) {
            --ins->timeLeftToFinish;
            assert(ins->timeLeftToFinish == 0);

            ins->execFinishTime = timeCounter;
            mrs->desValue = (ldManager.fn)(ins->op, mrs->addr, 0);
            pendingWrite.push_back(mrs);

            eventHappen(ldInsLoadEvent(mrs));
            eventHappen(insExecFinishEvent(mrs));
            isMemUsed = true;
        }
    }

    // store exec
    // Cycle 1
    ReservationStation* rs = stManager.getARSForCycle(InsOp::ST, 1);

    if (rs) {
        Ins* ins = rs->ins;
        --ins->timeLeftToFinish;
        assert(ins->timeLeftToFinish == 0);

        ins->execFinishTime = timeCounter;
        eventHappen(insExecFinishEvent(rs));
    }

    // Cycle 0
    if (loadCycleZero) {
        Ins* ins = loadCycleZero->ins;

        ins->execStartTime = timeCounter;
        --ins->timeLeftToFinish;

        eventHappen(insExecStartEvent(loadCycleZero));
        loadCycleZero = nullptr;
    } else {
        loadCycleZero = stManager.getARSForCycle(InsOp::ST, 0);
    }

//    for (ReservationStation& rs : stManager.rss) {
//        Ins* ins = rs.ins;
//        if (ins != nullptr && ins->timeLeftToFinish > 0) {
//            if (ins->timeLeftToFinish == getCycleOfInsOp(ins->op)) {
//                ins->execStartTime = timeCounter;
//            }
//            if (--ins->timeLeftToFinish == 0){
//                ins->execFinishTime = timeCounter;
//            }
//        }
//    }
}

void Tomasulo::write() {
    // mem
    ReservationStation* rs = stManager.getARSToAccessMem(InsOp::ST);
    if (rs != nullptr && checkLDSTSafe(rs->ins)) {
        Ins* ins = rs->ins;

        ins->writeResultTime = timeCounter;
        (stManager.fn)(ins->op, rs->addr, rs->v);

        eventHappen(stInsWriteEvent(rs));

        rs->reset();
        isMemUsed = true;
    }

    if (pendingWrite.empty()) return ;

    // bus arbitration
    static random_device rd;
    assert(pendingWrite.size() != 0);
    uniform_int_distribution<size_t> uid{0, pendingWrite.size() - 1};
    size_t choseInd = uid(rd);

    // broadcast
    rs = pendingWrite[choseInd];
    eventHappen(insWriteBackEvent(rs));
    addManager.accept(rs, eventCallBack);
    mulManager.accept(rs, eventCallBack);
    for (auto &&r : regs) {
        r.accept(rs, eventCallBack);
    }
    stManager.accept(rs, eventCallBack);

    rs->ins->writeResultTime = timeCounter;
    rs->reset();

    pendingWrite.erase(pendingWrite.begin() + choseInd);
}

void Tomasulo::nextTime() {
    ++timeCounter;
    isMemUsed = false;
    events.clear();
    write();
    exec();
    issue();
}

std::ostream& operator<<(std::ostream& out, const Tomasulo& t) {
    out << "====================================================================" << endl;
    out << "Time : " << t.timeCounter << endl;
    out << "PC : " << t.pc << endl << endl;
    out << "Number Of Instructions : " << t.inss.size() << endl;

    out << "--------------------------------------------------------------------" << endl;
    out << setw(21) << "Instructions";
    out.setf(ios::right);
    out << setw(8) << "Issue" << setw(8) << "Ex St" << setw(8) << "Ex Cmp" << setw(8) << "Wt" << endl;
    for (int i = 0; i < t.inss.size(); ++i) {
        const Ins& ins = t.inss[i];
        out << ((i == t.pc) ? '*' : ' ') << setw(20) << ins.toString();
        out << setw(8) << (ins.issueTime != INVALID_TIME ? to_string(ins.issueTime) : "")
            << setw(8) << (ins.execStartTime != INVALID_TIME ? to_string(ins.execStartTime) : "")
            << setw(8) << (ins.execFinishTime != INVALID_TIME ? to_string(ins.execFinishTime) : "")
            << setw(8) << (ins.writeResultTime != INVALID_TIME ? to_string(ins.writeResultTime) : "") << endl;
    }

    out << "--------------------------------------------------------------------" << endl;
    out << "Registers : " << endl;
    for (int i = 0; i < t.regs.size(); ++i) {
        out << setw(8) << i;
    }
    out << endl;
    for (int i = 0; i < t.regs.size(); ++i) {
        out << setw(8) << t.regs[i].value;
    }
    out << endl;
    for (int i = 0; i < t.regs.size(); ++i){
        out << setw(8) << (t.regs[i].srcRS == nullptr ? "-" : t.regs[i].srcRS->name);
    }
    out << endl;

    constexpr int sp = 8;
    out << "--------------------------------------------------------------------" << endl;
    auto aManagerOut = [&out](const RSManager& manager) {
        for (auto&& rs : manager.rss) {
            Ins* ins = rs.ins;
            bool hasIns = (rs.ins != nullptr);
            out << setw(sp) << (hasIns ? to_string(ins->timeLeftToFinish) : "")
                << setw(sp) << rs.name
                << setw(sp) << hasIns;
            if (!hasIns)
                out << endl;
            else {
                out << setw(sp); if (rs.q1 != nullptr) out << ""; else out << rs.v1;
                out << setw(sp); if (rs.q2 != nullptr) out << ""; else out << rs.v2;
                out << setw(sp) << ((rs.q1 == nullptr) ? "" : rs.q1->name)
                    << setw(sp) << ((rs.q2 == nullptr) ? "" : rs.q2->name)
                    << endl;
            }
        }
    };
    out << setw(sp) << "Time" << setw(sp) << "Name" << setw(sp) << "Busy" << setw(sp) << "V1" << setw(sp) << "V2" << setw(sp) << "Q1" << setw(sp) << "Q2" << endl;
    aManagerOut(t.addManager);
    aManagerOut(t.mulManager);

    out << endl;

    auto mManagerOut = [&out](const LSBManager& manager) {
        for (auto&& rs : manager.rss) {
            Ins* ins = rs.ins;
            bool hasIns = (rs.ins != nullptr);
            out << setw(sp) << (hasIns ? to_string(ins->timeLeftToFinish) : "")
                << setw(sp) << rs.name;
            if (!hasIns)
                out << endl;
            else {
                out << setw(sp) << rs.addr;
                if (ins->op == InsOp::LD) {
                    if (ins->timeLeftToFinish == 0) out << setw(sp) << rs.desValue;
                    else out << setw(sp) << "";
                } else {
                    if (rs.q) out << setw(sp) << rs.q->name;
                    else out << setw(sp) << rs.v;
                }
                out << endl;
            }

        }
    };
    out << setw(sp) << "Time" << setw(sp) << "Name" << setw(sp) << "ADDR" << setw(sp) << "VALUE" << endl;
    mManagerOut(t.ldManager);
    mManagerOut(t.stManager);

    out << endl;
    out << "Adder : " << (t.addNow == nullptr ? "" : t.addNow->name) << endl;
    out << "Muler : " << (t.mulNow == nullptr ? "" : t.mulNow->name) << endl;
    out << "Event : " << endl;
    for (auto&& event : t.events) {
        out << "        " << event.description << endl;
    }

    out << "====================================================================" << endl;

    return out;
}
