#include "Event.h"
#include "ReservationStation.h"
#include "Register.h"

Event insIssueEvent(const ReservationStation* rs) {
    std::stringstream ss;
    ss << "指令 " << rs->ins->index << " : " << rs->ins->toString() << " 发射到 "<< rs->name;
    return Event(ss.str());
}

Event insExecStartEvent(const ReservationStation* rs) {
    std::stringstream ss;
    ss << "指令 " << rs->ins->index << " [保留站 " << rs->name << "] 开始执行";
    return Event(ss.str());
}

Event insExecFinishEvent(const ReservationStation* rs) {
    std::stringstream ss;
    ss << "指令 " << rs->ins->index << " [保留站 " << rs->name << "] 执行完成";
    return Event(ss.str());
}

Event ldInsLoadEvent(const ReservationStation* rs) {
    std::stringstream ss;
    ss << "指令 " << rs->ins->index << " [Load缓冲 " << rs->name << "] 读取Mem[" << rs->addr << "]" ;
    return Event(ss.str());
}

Event insWriteBackEvent(const ReservationStation* rs) {
    std::stringstream ss;
    ss << "指令 " << rs->ins->index << " [保留站 " << rs->name << "] 广播数据";
    return Event(ss.str());
}

Event stInsWriteEvent(const ReservationStation* rs) {
    std::stringstream ss;
    ss << "指令 " << rs->ins->index << " [Store缓冲 " << rs->name << " ] 写入Mem[" << rs->addr << "]" ;
    return Event(ss.str());
}

Event broadcastDataReceived(const ReservationStation* me, const ReservationStation* src) {
    std::stringstream ss;
    ss << "保留站 " << me->name << " 接收保留站 " << src->name << " 广播数据";
    return Event(ss.str());
}

Event broadcastDataReceived(const Register* me, const ReservationStation* src) {
    std::stringstream ss;
    ss << "寄存器 " << me->index << " 接收保留站 " << src->name << " 广播数据";
    return Event(ss.str());
}

