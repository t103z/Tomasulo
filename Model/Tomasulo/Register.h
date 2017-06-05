#ifndef TOMASULO_REGISTER_H
#define TOMASULO_REGISTER_H

#include "ReservationStation.h"

/*!
 * 寄存器
 */
class Register {
public:
    double value = 0.0;                     // 寄存器值
    ReservationStation* srcRS = nullptr;    // 保留站表

    void reset(){
        value = 0.0;
        srcRS = nullptr;
    }

    /*!
     * 广播信息处理
     * @param rs 保留站
     */
    void accept(ReservationStation* rs) {
        if (srcRS == rs) {
            value = rs->desValue;
            srcRS = nullptr;
        }
    }
};


#endif //TOMASULO_REGISTER_H
