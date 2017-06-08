#ifndef TOMASULO_RESERVATIONSTATION_H
#define TOMASULO_RESERVATIONSTATION_H

#include "defs.h"
#include "Ins.h"
#include "Event.h"
#include <string>

/*!
 * 保留站
 */
class ReservationStation {
public:
    std::string name;                       // 保留站名称
    bool isBusy = false;                    // 是否繁忙
    Ins* ins = nullptr;                     // 存储的指令
    union {                                 // 操作数1值
        double v1 = INVALID_VALUE;
        double v;
    };
    union {                                 // 操作数2值
        double v2 = INVALID_VALUE;
        int addr;
    };
    union {                                 // 操作数1保留站指针
        ReservationStation* q1 = nullptr;
        ReservationStation* q;
    };
    ReservationStation* q2 = nullptr;       // 操作数2保留站指针

    double desValue = INVALID_VALUE;        // 执行结果

    ReservationStation(const std::string& name) : name(name) {}

    void reset() {
        isBusy = false;
        ins = nullptr;
        v1 = v2 = INVALID_VALUE;
        q1 = q2 = nullptr;
        desValue = INVALID_VALUE;
    }

    /*!
     * 广播信息处理
     * @param rs 保留站
     */
    void accept(ReservationStation* rs, EventCallBack callBack) {
        if (ins == nullptr) return;

        bool isGetData = false;
        if (isArithmeticIns(ins)) {
            if (q1 == rs) {
                v1 = rs->desValue;
                q1 = nullptr;
                isGetData = true;
            }
            if (q2 == rs) {
                v2 = rs->desValue;
                q2 = nullptr;
                isGetData = true;
            }
        } else {
            assert(isMemIns(ins));
            if (isStoreIns(ins)) {
                if (q == rs){
                    v = rs->desValue;
                    q = nullptr;
                    isGetData = true;
                }
            }
        }

        if (isGetData) callBack(broadcastDataReceived(this, rs));
    }

    /*!
     * 判断保留站内指令是否就绪
     * 就绪：源操作数准备完毕
     * @return
     */
    bool isReady() {
        if (ins == nullptr) return false;
        if (isArithmeticIns(ins)) {
            return q1 == nullptr && q2 == nullptr;
        } else {
            assert(isMemIns(ins));
            if (isLoadIns(ins)) return true;
            if (isStoreIns(ins)) return q == nullptr;
        }
        assert(false);
        return false;
    }
};

#endif //TOMASULO_RESERVATIONSTATION_H
