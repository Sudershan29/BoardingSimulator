#ifndef flight_h
#define flight_h

#include <iostream>
#include <memory>
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
using std::make_unique;
using std::unique_ptr;
using std::vector;
using std::ostream;
using std::cout;
using std::endl;
using std::function;
using std::pair;

namespace flightSimulator
{
    using TicketList = std::vector<unique_ptr<Ticket>>;
    template <GroupAllocStrategy T>
    class Flight
    {
    public:
        Flight(FlightModel const &m) : numberOfPassengers(0),
                                       model(make_unique<FlightModel>(m)),
                                       strategy(T(m)),
                                       arrangement(make_unique<SeatMap>(m))
        {
            flightQueue = make_unique<FlightQ>(
                m,
                [&](Passenger &p, int i) -> bool
                { return this->isMySeat(p, i); },
                [&](Passenger &p) -> TimeQuantum
                { return this->arrangement->waitTime(p); },
                [&](Passenger &p) -> void
                { this->sitDown(p); });

            generateTickets();
        }

        Flight(Flight const &f) : numberOfPassengers(f.numberOfPassengers),
                                  model(make_unique<FlightModel>(*f.model)),
                                  strategy(f.strategy),
                                  arrangement(make_unique<SeatMap>(*f.arrangement)),
                                  flightQueue(make_unique<FlightQ>(*f.flightQueue)) 
        {
            for (auto const &t : f.tickets)
                tickets.push_back(make_unique<Ticket>(*t));

        }

        Flight(Flight &&f){
            numberOfPassengers = f.numberOfPassengers;
            model = std::move(f.model);
            strategy = f.strategy;
            tickets = std::move(f.tickets);
            arrangement = std::move(f.arrangement);
            flightQueue = std::move(f.flightQueue);
        }

        TimeQuantum board(PassengerList passengers, BoardingStrategy auto const &boardingStrat)
        {
            TimeQuantum total, iterationDuration(config.getInt("Quantum", "per_step"));
            sort(passengers.begin(), passengers.end(), [&boardingStrat](Passenger const &p1, Passenger const &p2)
                 { return boardingStrat.compare(p1, p2); });

            // Now you can start boarding
            int passengerIdx = 0, totalPassengers = passengers.size();
            auto allPassengersLoaded = [&passengerIdx, &totalPassengers]() -> bool
            { return passengerIdx == totalPassengers; };

            while (!allPassengersLoaded() || !flightQueue->Done())
            {
                // NOTE: You can add one more level of indirection here | Incorportaing with boarding group-wise
                if (!allPassengersLoaded() && flightQueue->Push(passengers[passengerIdx]))
                    passengerIdx += 1;

                flightQueue->Move();
                total += iterationDuration;
                if (config.getInt("Simulation", "show"))
                {
                    cout << "\033[2J\033[1;1H"; // TODO: Change this
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

        int totalPassengers() const { return model->totalPassengers; }

        /*
            NOTE: Since passengers have random arrival times, we do not need to generate random tickets
        */
        Ticket const &getTicket()
        {
            if (numberOfPassengers == model->totalPassengers)
                throw std::runtime_error("No more tickets available");

            return *tickets[numberOfPassengers++];
        }

        void deplane() {
            numberOfPassengers = 0;
            arrangement->deplane();
        }

    private:
        int numberOfPassengers;
        unique_ptr<FlightModel> model;
        T strategy;
        TicketList tickets;
        unique_ptr<SeatMap> arrangement;
        unique_ptr<FlightQ> flightQueue;

        void generateTickets()
        {
            // business class
            for (int row = 1; row <= model->business.rows; row++)
                for (int seat = 1; seat <= model->business.perRow; seat++)
                {
                    char seatLetter = seat + 'A' - 1;
                    unique_ptr<Ticket> t = make_unique<Ticket>(seatLetter, row, *model);
                    t->setBoardingGroup(strategy.getGroupNumber(*t));
                    tickets.push_back(std::move(t));
                }

            // economy class
            for (int row = model->business.rows + 1; row <= model->totalRows; row++)
                for (int seat = 1; seat <= model->economy.perRow; seat++)
                {
                    char seatLetter = seat + 'A' - 1;
                    unique_ptr<Ticket> t = make_unique<Ticket>(seatLetter, row, *model);
                    t->setBoardingGroup(strategy.getGroupNumber(*t));
                    tickets.push_back(std::move(t));
                }
        }

        bool isMySeat(Passenger const &p, int idx)
        {
            return p.ticket->matchSeat(idx);
        }

        void sitDown(Passenger const &p)
        {
            arrangement->sitDown(p.ticket->getRow(), p.ticket->getSeatIdx());
        }

        /*
            NOTE: Get format.h to fix this
        */
        ostream &printTickets(ostream &os, GroupAllocStrategy auto const &strategy) const
        {
            // business class
            int idx = 0;
            for (int row = 1; row <= model->business.rows; row++)
            {
                for (int seat = 1; seat <= model->business.perRow; seat++, idx++)
                    os << *tickets[idx] << "(" + to_string(strategy.getGroupNumber(*tickets[idx])) + ") ";
                os << "\n";
            }

            // economy class
            for (int row = model->business.rows + 1; row <= model->totalRows; row++)
            {
                for (int seat = 1; seat <= model->economy.perRow; seat++, idx++)
                    os << *tickets[idx] << "(" + to_string(strategy.getGroupNumber(*tickets[idx])) + ") ";
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
            { flightQueue->printIdx(os, rowNumber); };

            arrangement->print(cout, printQ);
        }
    };
}
#endif