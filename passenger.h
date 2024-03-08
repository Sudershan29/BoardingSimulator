#ifndef passsenger_h
#define passsenger_h

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "rng.h"
#include "config.h"
#include "ticket.h"
#include "quantum.h"

using std::string;
using std::unique_ptr;
using std::make_unique;

namespace flightSimulator
{
    /*
    Numbers of bags :
        0 Bag - 5%
        1 Bag - 80%
        2 Bags - 10%
        3 Bags - 5%
    */
    RNGenerator auto passengerBags = LotteryGame({5, 80, 10, 5});
    RNGenerator auto arrivalTimeGenerator = RandomNumber(0, 20);

    struct Passenger
    {
        Passenger(string name, Ticket const &t) : name(name), ticket(make_unique<Ticket>(t))
        {
            arrivalTime = arrivalTimeGenerator.getNumber();
        }

        string name;
        unique_ptr<Ticket> ticket;
        int arrivalTime; // This orders the users
        int bags;

        Passenger(Passenger const &p) : name(p.name), ticket(make_unique<Ticket>(*p.ticket)), arrivalTime(p.arrivalTime)
        {
            bags = passengerBags.getNumber();
        }

        Passenger &operator=(Passenger const &p)
        {
            name = p.name;
            ticket = make_unique<Ticket>(*p.ticket);
            arrivalTime = p.arrivalTime;
            return *this;
        }

        Passenger(Passenger &&p) {
            ticket = std::move(p.ticket);
            name = p.name;
            arrivalTime = p.arrivalTime;
            bags = p.bags;
        }

        TimeQuantum UnloadingTime() const
        {
            return TimeQuantum(config.getInt("Quantum", "bag_stow_time")) * bags;
        }

        friend ostream &operator<<(ostream &os, Passenger const &p)
        {
            os << p.name << " at " << p.arrivalTime << " ( " << p.ticket << " )";
            return os;
        }
    };

    typedef std::vector<Passenger> PassengerList;
}

#endif