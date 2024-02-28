#ifndef flight_h
#define flight_h

#include <iostream>
#include <string>
#include "flight_model.h"
#include "ticket.h"
#include "strategy.h"

using std::string;
using std::ostream;

class Flight {
public:
    Flight(FlightModel const &m): model(m) {
        generateTickets();
    }

    void board(BoardingStrategy auto const &strategy);

    friend ostream& operator<<(ostream& os, Flight const &f);

private:
    FlightModel const &model;
    TicketList         seats;

    void generateTickets();
    ostream &printSeats(ostream &os, BoardingStrategy auto const &strategy) const;
};

#endif