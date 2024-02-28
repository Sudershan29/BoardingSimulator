#ifndef ticket_h
#define ticket_h

#include <vector>
#include <iostream>
#include <string>
#include "flight_model.h"

using std::ostream;
using std::string;


class Ticket;
typedef std::vector<Ticket> TicketList;

class Ticket
{
public:
    Ticket(char _seat, int _row, FlightModel const &m) : seat(_seat), row(_row) {
        isBusiness = m.isBusinessClass(row);
        isWindow   = m.isWindow(seat, isBusiness);
        isMiddle   = m.isMiddle(seat, isBusiness);
    }

    void setBoardingGroup(int _boardingGroup) { boardingGroup = _boardingGroup; }

    char getSeat() const { return seat; }

    int getRow() const { return row; }

    bool isBusinessClass() const { return isBusiness; }

    bool isWindowSeat() const { return isWindow; }

    bool isMiddleSeat() const { return isMiddle; }

    bool isAisleSeat() const {  return !isWindow && !isMiddle; }

    int getBoardingGroup() const { return boardingGroup; }

    friend ostream& operator<<(ostream& os, Ticket const &t){
        os << std::to_string(t.row) + t.seat;
        return os;
    }

private:
    char seat;
    bool isBusiness, isWindow, isMiddle;
    int  row, boardingGroup;
};

#endif