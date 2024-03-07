
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

int main()
{
    string strategy = config.get("Flight", "boarding_strategy");
    // unique_ptr<SimulationVisitor> sim = getSimulation(strategy, BOEING_737_700);

    BoardingSimulator<SteffenModified> sim(BOEING_737_700);
    sim.simulate(1);
    cout << sim;

    // sim->simulate(1);
    // cout << *sim;

    return 0;
}