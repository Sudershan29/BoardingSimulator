
#include <iostream>
#include <string>
#include "flight.h"
#include "passenger.h"
#include "rng.h"
#include "strategy.h"
#include "quantum.h"
#include "simulation.h"

using std::cout;
using std::string;
using std::to_string;
using std::endl;

using namespace flightSimulator;

// Replace with your own boarding strategy
struct MonkeyStrat
{
    FlightModel model;
    MonkeyStrat(FlightModel const &m) : model(m) {}

    int getGroupNumber(Ticket const &ticket) const
    {
        // I hate people who pay extra for tickets, jk 
        if (ticket.isBusinessClass())
            return 4;

        // More likely to have paid extra
        if(ticket.isWindowSeat())
            return 3;

        else if (ticket.isMiddleSeat())
            return 1;
        
        else
            return 2;
    }

    string_view getName() const
    {
        return "MonkeyStrat";
    }
};

/*

    USAGE:

    // 1. Testing with default configurations

    ./bin/run

    // 2. Testing with custom configurations

    ./bin/run custom privatejet.ini


*/

int main(int argc, char **argv)
{
    /*
        Style 1 : Getting default strategy
    */

   if(argc >= 3) {
        config.extend("config/" + string(argv[2]));
        BOEING_737_700.refresh();
   }

   if (argc <= 1 || (argc >= 2 && string(argv[1]) != "custom"))
   {
       string strategy = config.get("Flight", "boarding_strategy");
       unique_ptr<SimulationVisitor> sim = getSimulation(strategy, BOEING_737_700);
       sim->simulate(1);
       return 0;
   }

    /*

        Style 2 : Testing with your own strategy

    */

    BoardingSimulator<MonkeyStrat> monkeyStrat(BOEING_737_700);
    monkeyStrat.simulate(1);    // iterations

    return 0;
}

/*

TODO: Left flight
*/