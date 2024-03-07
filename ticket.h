#ifndef ticket_h
#define ticket_h

#include <vector>
#include <iostream>
#include <string>
#include "flight_model.h"
#define FMT_HEADER_ONLY
#include <fmt/format.h>

using std::ostream;
using std::string;

namespace flightSimulator {
class Ticket;
typedef std::vector<Ticket> TicketList;

class Ticket
{
public:
    Ticket(char _seat, int _row, FlightModel const &m) : m(m), seat(_seat), row(_row)
    {
        isBusiness = m.isBusinessClass(row);
        isWindow   = m.isWindow(seat, isBusiness);
        isMiddle   = m.isMiddle(seat, isBusiness);
    }

    // Ticket(Ticket &&t) {
    //     m = std::move(t.m);
    //     seat = t.seat;
    //     row = t.row;
    //     isBusiness = t.isBusiness;
    //     isWindow = t.isWindow;
    //     isMiddle = t.isMiddle;
    // }

    void setBoardingGroup(int _boardingGroup) { boardingGroup = _boardingGroup; }

    char getSeat() const { return seat; }

    int getSeatIdx() const { return Ticket::convertSeatToIdx(seat); }

    int getRow() const { return row; }

    bool isBusinessClass() const { return isBusiness; }

    bool isWindowSeat() const { return isWindow; }

    bool isMiddleSeat() const { return isMiddle; }

    bool isAisleSeat() const {  return !isWindow && !isMiddle; }

    int getBoardingGroup() const { return boardingGroup; }

    bool rightSideOfThePlane() const { 
        if(isBusinessClass())
            return getSeatIdx() >= m.business.perRow / 2;

        return getSeatIdx() >= m.economy.perRow / 2;
     }

    friend ostream& operator<<(ostream& os, Ticket const &t){
        os << fmt::format("{:^{}}", " " + std::to_string(t.row) + t.seat, 5);
        return os;
    }

    static int  convertSeatToIdx(char seat) { return seat - 'A'; }

    static char convertIdxToSeat(int idx) { return idx + 'A'; }

    /*
        Assumes currRow is 0 indexed
    */
    bool matchSeat(int currRow) const { return row == currRow + 1; }

private:
    FlightModel m;
    char seat;
    bool isBusiness, isWindow, isMiddle;
    int  row, boardingGroup;
};
}
#endif