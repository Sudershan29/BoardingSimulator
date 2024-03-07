
#include <iostream>
#include "simulation.h"

using namespace flightSimulator;

using std::cout;
int constexpr ITER = 100;

int main()
{
    config.set("Simulation", "show", 0);

    BoardingSimulator<WindowMiddleAisle> wMa(BOEING_737_700);
    BoardingSimulator<SteffenModified> sM(BOEING_737_700);
    BoardingSimulator<FrontToBack> fTb(BOEING_737_700);
    BoardingSimulator<BackToFront> bTf(BOEING_737_700);
    BoardingSimulator<RandomBoarding> rB(BOEING_737_700);

    wMa(ITER);
    sM(ITER);
    fTb(ITER);
    bTf(ITER);
    rB(ITER);

    cout << wMa << sM << fTb << bTf << rB;

    return 0;
}

/*

10 s


*/