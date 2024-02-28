#ifndef passsenger_h
#define passsenger_h

#include <iostream>
#include <vector>
#include <string>
#include "ticket.h"

using std::string;

struct Passenger {
    Passenger(string name, Ticket const &t, int arrivalTime = 0) : name(name), ticket(t), arrivalTime(arrivalTime) { }
    string        name;
    Ticket const &ticket;
    int           arrivalTime; // This orders the users
};

typedef std::vector<Passenger> PassengerList;

#endif