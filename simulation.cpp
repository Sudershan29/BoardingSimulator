
#include <iostream>
#include <vector>
#include "simulation.h"

using namespace flightSimulator;

using std::cout;
int ITER = 100;

/*

    USAGE:

    ./bin/simulation                    // DEFAULT

    ./bin/simulation 100 privatejet.ini // CUSTOM

*/

int main(int argc, char **argv)
{
    if(argc >= 2)
        ITER = std::stoi(argv[1]);

    if (argc >= 3)
    {
        config.extend("config/" + string(argv[2]));
        BOEING_737_700.refresh();
    }

    // Turning the graphics off
    config.set("Simulation", "show", 0);

    vector<unique_ptr<SimulationVisitor>> simulations;

    for (auto strategy : {"WindowMiddleAisle", "SteffenModified", "FrontToBack", "BackToFront", "RandomBoarding"})
        simulations.push_back(getSimulation(strategy, BOEING_737_700));

    /*

        Feel free to test your own strategy and compare with other strategies

        unique_ptr<SimulationVisitor> yourStrategy = Simulation<MonkeyStrat>(BOEING_737_700);
        simulation.push_back(yourStrategy);
    */

    for(auto &simulation: simulations){
        simulation->simulate(ITER);
        simulation->printResults(cout);
    }

    return 0;
}

/*

10 s


*/