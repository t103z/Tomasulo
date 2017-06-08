#ifndef TOMASULO_DEFS_H
#define TOMASULO_DEFS_H

#include <cstddef>
#include <string>
#include <algorithm>
#include <functional>

/*!
 * 操作数类型
 */
enum class InsOp {
    UNINIT,
    ADDD,
    SUBD,
    MULTD,
    DIVD,
    LD,
    ST
};

/*!
 * 字符串转操作数类型
 * @param str
 * @return 操作数类型，无效字符串返回InsOp::UNINIT
 */
inline InsOp getOpFromStr(const std::string& str){
    InsOp op = InsOp::UNINIT;
    std::string opstr = str;
    std::transform(opstr.begin(),opstr.end(),opstr.begin(),::toupper);
    if (opstr == "ADDD") {
        op = InsOp::ADDD;
    } else if (opstr == "SUBD") {
        op = InsOp::SUBD;
    } else if (opstr == "MULTD") {
        op = InsOp::MULTD;
    } else if (opstr == "DIVD") {
        op = InsOp::DIVD;
    } else if (opstr == "LD") {
        op = InsOp::LD;
    } else if (opstr == "ST") {
        op = InsOp::ST;
    }
    return op;
}

/*!
 * 操作数类型转字符串
 * @param op
 * @return 操作数类型对应的字符串
 */
inline std::string opToStr(InsOp op) {
    switch (op){
        case InsOp::ADDD:
            return "ADDD";
        case InsOp::SUBD:
            return "SUBD";
        case InsOp::LD:
            return "LD";
        case InsOp::ST:
            return "ST";
        case InsOp::MULTD:
            return "MULTD";
        case InsOp::DIVD:
            return "DIVD";
        default:
            return "";
    }
}

/*!
 * 无效值
 * 整型均为-1，浮点型无效值仅仅用于初始化
 */
constexpr int INVALID = -1;
constexpr int INVALID_TIME = INVALID;
constexpr double INVALID_VALUE = 0.0;

/*!
 * 内存、寄存器、保留站个数定义
 */
constexpr std::size_t MEM_SIZE = 4096;
constexpr std::size_t REG_SIZE = 16;
constexpr std::size_t ADDER_SIZE = 3;
constexpr std::size_t MULER_SIZE = 2;
constexpr std::size_t LDBUF_SIZE = 3;
constexpr std::size_t STBUF_SIZE = 3;

class Event;
using EventCallBack = std::function<void(Event&&)>;

#endif //TOMASULO_DEFS_H
