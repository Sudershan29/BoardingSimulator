#ifndef passsenger_h
#define passsenger_h

#include <iostream>
#include <string>
#include <ticket.h>

using std::string;

class Passenger {

private:
    string name;
    Ticket ticket;
    int    arrivalTime; // This orders the users
};

#endif