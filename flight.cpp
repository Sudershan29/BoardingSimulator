
#include <iostream>
#include <string>
#include "flight.h"
#include "strategy.h"

using std::to_string;

void Flight::generateTickets() {
    TicketList result;

    // business class
    for(int row = 1; row <= model.business.rows; row++)
        for(int seat = 1; seat <= model.business.perRow; seat++){
            char seatLetter = seat + 'A' - 1;
            result.push_back(Ticket(seatLetter, row, model));
        }

    // economy class
    for(int row = model.business.rows + 1; row <= model.totalRows; row++)
        for(int seat = 1; seat <= model.economy.perRow; seat++){
            char seatLetter = seat + 'A' - 1;
            result.push_back(Ticket(seatLetter, row, model));
        }

    seats = result;
}

void Flight::board(BoardingStrategy auto const &strategy){
    
}

ostream& operator<<(ostream &os, Flight const &f){
    os << f.model;
    return f.printSeats(os, FrontToBack(f.model));
}


/*
    NOTE: Get format.h to fix this 
*/
ostream &Flight::printSeats(ostream &os, BoardingStrategy auto const &strategy) const
{
    // business class
    int idx = 0;
    for (int row = 1; row <= model.business.rows; row++){
        for (int seat = 1; seat <= model.business.perRow; seat++, idx++)
            os << seats[idx] << "(" + to_string(strategy.getGroupNumber(seats[idx])) + ") ";
        os << "\n";
    }

    // economy class
    for (int row = model.business.rows + 1; row <= model.totalRows; row++){
        for (int seat = 1; seat <= model.economy.perRow; seat++, idx++)
            os << seats[idx] << "(" + to_string(strategy.getGroupNumber(seats[idx])) + ") ";
        os << "\n";
    }

    return os;
}