#ifndef flight_h
#define flight_h

#include <iostream>
#include <vector>
#include <string>
#include "flight_model.h"
#include "ticket.h"
#include "strategy.h"
#include "passenger.h"

using std::string;
using std::vector;
using std::ostream;

template <GroupAllocStrategy T>
class Flight
{
public:
    Flight(FlightModel const &m) : numberOfPassengers(m.totalPassengers), model(m), strategy(T(m))
    {
        generateTickets();
    }

    void board(PassengerList const p){
        // sort(passengers.begin(), passengers.end());
    }

    friend ostream& operator<<(ostream& os, Flight const &f){
        os << f.model;
        return f.printSeats(os, f.strategy);
    }

    int totalPassengers() const { return model.totalPassengers; }

    /*
        NOTE: Since passengers have random arrival times, we do not need to generate random tickets
    */
    Ticket const &getTicket()
    {
        if (numberOfPassengers == model.totalPassengers)
            throw std::runtime_error("No more tickets available");

        return seats[numberOfPassengers++];
    }

private:
    int                numberOfPassengers;
    FlightModel const &model;
    T                  strategy;
    TicketList         seats;

    void generateTickets(){
        TicketList result;

        // business class
        for (int row = 1; row <= model.business.rows; row++)
            for (int seat = 1; seat <= model.business.perRow; seat++)
            {
                char seatLetter = seat + 'A' - 1;
                Ticket t(seatLetter, row, model);
                t.setBoardingGroup(strategy.getGroupNumber(t));
                result.push_back(t);
            }

        // economy class
        for (int row = model.business.rows + 1; row <= model.totalRows; row++)
            for (int seat = 1; seat <= model.economy.perRow; seat++)
            {
                char seatLetter = seat + 'A' - 1;
                Ticket t(seatLetter, row, model);
                t.setBoardingGroup(strategy.getGroupNumber(t));
                result.push_back(t);
            }

        seats = result;
    }

    /*
        NOTE: Get format.h to fix this
    */
    ostream &printSeats(ostream &os, GroupAllocStrategy auto const &strategy) const
    {
        // business class
        int idx = 0;
        for (int row = 1; row <= model.business.rows; row++)
        {
            for (int seat = 1; seat <= model.business.perRow; seat++, idx++)
                os << seats[idx] << "(" + to_string(strategy.getGroupNumber(seats[idx])) + ") ";
            os << "\n";
        }

        // economy class
        for (int row = model.business.rows + 1; row <= model.totalRows; row++)
        {
            for (int seat = 1; seat <= model.economy.perRow; seat++, idx++)
                os << seats[idx] << "(" + to_string(strategy.getGroupNumber(seats[idx])) + ") ";
            os << "\n";
        }

        return os;
    }
};

#endif