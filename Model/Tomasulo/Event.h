#ifndef TOMASULO_EVENT_H
#define TOMASULO_EVENT_H

#include <string>
#include <sstream>

class ReservationStation;
class Register;

class Event{
public:
    Event(const std::string& description = "") : description(description) {}

    std::string description;
};

Event insIssueEvent(const ReservationStation* rs);
Event insExecStartEvent(const ReservationStation* rs);
Event insExecFinishEvent(const ReservationStation* rs);
Event ldInsLoadEvent(const ReservationStation* rs);
Event insWriteBackEvent(const ReservationStation* rs);
Event stInsWriteEvent(const ReservationStation* rs);
Event broadcastDataReceived(const ReservationStation* me, const ReservationStation* src);
Event broadcastDataReceived(const Register* me, const ReservationStation* src);


#endif //TOMASULO_EVENT_H
