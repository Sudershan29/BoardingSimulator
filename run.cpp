
#include <iostream>
#include <string>
#include "flight.h"
#include "passenger.h"
#include "rng.h"
#include "strategy.h"

using std::cout;
using std::string;
using std::to_string;

template <GroupAllocStrategy T>
void createPassenger(Flight<T> &flight, PassengerList &passengers, RNGenerator auto &rngInstance);

int main(){

    /*
        1. Create a flight first with seats, rows, and generate tickets
        2. Create passengers and assign them tickets and arrival times
        3. Then simulate the algorithm
    */

    // Step 1
    Flight<FrontToBack> flight(BOEING_737_700);
    /*
        TODO: Determine the range of arrival times for passengers after boarding starts

        (80% there when boarding starts + 20% after) or something similar

        Switch to NormDistribution based on FACTS
    */
    RNGenerator auto arrivalTimeGenerator  = RandomNumber(0, 100);

    // Step 2
    PassengerList passengers;
    createPassenger(flight, passengers, arrivalTimeGenerator);

    // Step 3
    flight.board(passengers);

    return 0;
}

template <GroupAllocStrategy T>
void createPassenger(Flight<T> &flight, PassengerList &passengers, RNGenerator auto &rngInstance)
{
    for (int i = 0; i < flight.totalPassengers(); i++)
    {
        string name = to_string(i);
        Ticket const &ticket = flight.getTicket();
        int arrivalTime = rngInstance.getNumber();

        passengers.push_back(Passenger(name, ticket, arrivalTime));
    }
}

/*

Step 1:

    Flight with seats: 
        i)  Boeing 737 : 10 rows for business class with 2 - 2 configuration + 20 rows for economy class with 3 - 3 configuration = 10 * 4 + 20 * 6 = 40 + 120 = 160 seats
        ii) Then creates tickets, and assigns them to passengers
        iii) Flight uses a boarding group now but we have to decide the number of passengers in each boarding group


Ideas:

    1) Represent 1 unit of time as 1 minute, and then use a bell curve to represent the arrival times of passengers
    2) Then represent queue moving time as x, seat shuffling time (going from aisle to window) as y and bag stow time as z
    3) Then we can simulate the boarding process and see how long it takes

*/