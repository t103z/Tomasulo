#ifndef TOMASULO_INS_H
#define TOMASULO_INS_H

#include "defs.h"
#include <assert.h>
#include <string>

class Ins;

// 指令类型判断
bool isAdderIns(const Ins* ins);
bool isMulerIns(const Ins* ins);
bool isLoadIns(const Ins* ins);
bool isStoreIns(const Ins* ins);

bool isArithmeticIns(const Ins* ins);
bool isMemIns(const Ins* ins);

class Ins {
public:
    int index = INVALID;                        // 指令id

    int timeLeftToFinish = INVALID_TIME;        // 剩余执行时间
    int issueTime = INVALID_TIME;               // 发射时间
    int execStartTime = INVALID_TIME;           // 开始执行时间
    int execFinishTime = INVALID_TIME;          // 执行完毕时间
    int writeResultTime = INVALID_TIME;         // 结果写回时间

    InsOp op = InsOp::UNINIT;                   // 操作数
    union {                                     // 源操作数1
        int src1 = 0;
        int memReg;
    };

    union {                                     // 源操作数2
        int src2 = 0;
        int memAddr;
    };

    int des = 0;                                // 目的操作数

    Ins() {}

    Ins(InsOp op, int des, int src1, int src2) : op(op), src1(src1), src2(src2), des(des) {}

    Ins(InsOp op, int reg, int addr) : op(op), memAddr(addr), memReg(reg), des(0) {
        assert(op == InsOp::LD || op == InsOp::ST);
    }

    void restart(){
        timeLeftToFinish = INVALID_TIME;
        issueTime = INVALID_TIME;
        execStartTime = INVALID_TIME;
        execFinishTime = INVALID_TIME;
        writeResultTime = INVALID_TIME;
    }

    /*!
     * 转字符串
     * @return 标准格式字符串
     */
    std::string toString() const {
        if (isArithmeticIns(this)) {
            return opToStr(op) + " F" + std::to_string(des) + ", F" +std::to_string(src1) + ", F" + std::to_string(src2);
        } else if (isMemIns(this)) {
            return opToStr(op) + " F" + std::to_string(memReg) + ", "+std::to_string(memAddr);
        }

        return "";
    }

    /*!
     * 从文件加载指令序列
     * @param fname 文件名
     * @return <发生错误的行号（从1开始），正确读入的指令序列>
     */
    static std::pair<std::vector<int>, std::vector<Ins>> loadInsFromFile(const std::string& fname);

    /*!
     * 从string加载指令序列
     * @param string
     * @return <发生错误的行号（从1开始），正确读入的指令序列>
     */
    static std::pair<std::vector<int>, std::vector<Ins>> loadInsFromString(const std::string& string);

private:
    static std::pair<std::vector<int>, std::vector<Ins>> loadIns(std::istream& in);
};

/*!
 * 获取指令执行周期
 * @param op
 * @return
 */
int getCycleOfInsOp(InsOp op);

inline bool isAdderIns(const Ins* ins) {
    return ins->op == InsOp::ADDD || ins->op == InsOp::SUBD;
}

inline bool isMulerIns(const Ins* ins){
    return ins->op == InsOp::MULTD || ins->op == InsOp::DIVD;
}

inline bool isLoadIns(const Ins* ins) {
    return ins->op == InsOp::LD;
}

inline bool isStoreIns(const Ins* ins){
    return ins->op == InsOp::ST;
}

inline bool isArithmeticIns(const Ins* ins){
    return isAdderIns(ins) || isMulerIns(ins);
}

inline bool isMemIns(const Ins* ins){
    return isLoadIns(ins) || isStoreIns(ins);
}

#endif //TOMASULO_INS_H
