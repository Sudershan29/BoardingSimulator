
#include <iostream>
#include "flight.h"

using std::cout;

int main(){

    /*
        1. Create a flight first with seats, rows, and generate tickets
        2. Create passengers and assign them tickets and arrival times
        3. Then simulate the algorithm
    */

    Flight flight(BOEING_737_700);
    
    cout<<flight;

    return 0;
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