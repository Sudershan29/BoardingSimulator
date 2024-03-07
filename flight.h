#ifndef flight_h
#define flight_h

#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <string>
#include <utility>
#include "flight_model.h"
#include "ticket.h"
#include "strategy.h"
#include "passenger.h"
#include "seat.h"
#include "quantum.h"
#include "queue.h"
#include "config.h"

using std::string;
using std::vector;
using std::ostream;
using std::cout;
using std::endl;
using std::function;
using std::pair;

namespace flightSimulator
{
    template <GroupAllocStrategy T>
    class Flight
    {
    public:
        Flight(FlightModel const &m) : numberOfPassengers(0),
                                       model(m),
                                       strategy(T(m)),
                                       arrangement(m),
                                       flightQueue(FlightQ(
                                           m,
                                           [&](Passenger &p, int i) -> bool
                                           { return this->isMySeat(p, i); },
                                           [&](Passenger &p) -> TimeQuantum
                                           { return this->arrangement.waitTime(p); },
                                           [&](Passenger &p) -> void
                                           { this->sitDown(p); }))
        {
            generateTickets();
        }

        // Flight(Flight const &f) : numberOfPassengers(f.numberOfPassengers),
        //                          model(f.model),
        //                          strategy(f.strategy),
        //                          tickets(f.tickets),
        //                          arrangement(f.arrangement),
        //                          flightQueue(f.flightQueue) {}


        TimeQuantum board(PassengerList passengers, BoardingStrategy auto const &boardingStrat)
        {
            TimeQuantum total, iterationDuration(config.getInt("Quantum", "per_step"));
            sort(passengers.begin(), passengers.end(), [&boardingStrat](Passenger const &p1, Passenger const &p2)
                 { return boardingStrat.compare(p1, p2); });

            // Now you can start boarding
            int passengerIdx = 0, totalPassengers = passengers.size();
            auto allPassengersLoaded = [&passengerIdx, &totalPassengers]() -> bool
            { return passengerIdx == totalPassengers; };

            while (!allPassengersLoaded() || !flightQueue.Done())
            {
                // NOTE: You can add one more level of indirection here | Incorportaing with boarding group-wise
                if (!allPassengersLoaded() && flightQueue.Push(passengers[passengerIdx]))
                    passengerIdx += 1;

                flightQueue.Move();
                total += iterationDuration;
                if (config.getInt("Simulation", "show"))
                {
                    cout << "\033[2J\033[1;1H";
                    boardingSimulation();
                    std::this_thread::sleep_for(std::chrono::milliseconds(config.getInt("Simulation", "slowdown_in_ms")));
                    cout << total << endl;
                }
            }
            return total;
        }

        friend ostream &operator<<(ostream &os, Flight const &f)
        {
            os << f.model;
            return f.printTickets(os, f.strategy);
        }

        int totalPassengers() const { return model.totalPassengers; }

        /*
            NOTE: Since passengers have random arrival times, we do not need to generate random tickets
        */
        Ticket const &getTicket()
        {
            if (numberOfPassengers == model.totalPassengers)
                throw std::runtime_error("No more tickets available");

            return tickets[numberOfPassengers++];
        }

        void deplane() {
            numberOfPassengers = 0;
            arrangement.deplane();
        }

    private:
        int numberOfPassengers;
        FlightModel const &model;
        T strategy;
        TicketList tickets;
        SeatMap arrangement;
        FlightQ flightQueue;

        void generateTickets()
        {
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

            tickets = result;
        }

        bool isMySeat(Passenger const &p, int idx)
        {
            return p.ticket.matchSeat(idx);
        }

        void sitDown(Passenger const &p)
        {
            arrangement.sitDown(p.ticket.getRow(), p.ticket.getSeatIdx());
        }

        /*
            NOTE: Get format.h to fix this
        */
        ostream &printTickets(ostream &os, GroupAllocStrategy auto const &strategy) const
        {
            // business class
            int idx = 0;
            for (int row = 1; row <= model.business.rows; row++)
            {
                for (int seat = 1; seat <= model.business.perRow; seat++, idx++)
                    os << tickets[idx] << "(" + to_string(strategy.getGroupNumber(tickets[idx])) + ") ";
                os << "\n";
            }

            // economy class
            for (int row = model.business.rows + 1; row <= model.totalRows; row++)
            {
                for (int seat = 1; seat <= model.economy.perRow; seat++, idx++)
                    os << tickets[idx] << "(" + to_string(strategy.getGroupNumber(tickets[idx])) + ") ";
                os << "\n";
            }

            return os;
        }

        /*

            IDEA:

            Row 1 : 1A( X ) 1B ( X ) |  Q  | 1C ( X ) 1D ( X )
            ....

            Row 10 : 10A ( X ) 10B ( X ) 10C ( X ) |   Q  | ....

        */
        void boardingSimulation()
        {
            auto printQ = [&](ostream &os, int rowNumber)
            { flightQueue.printIdx(os, rowNumber); };

            arrangement.print(cout, printQ);
        }
    };
}
#endif