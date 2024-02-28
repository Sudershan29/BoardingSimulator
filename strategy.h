#ifndef boarding_strategy_h
#define boarding_strategy_h

#include <cstddef>
#include <concepts>
#include <functional>
#include <string>
#include "ticket.h"
#include "flight_model.h"

using std::string_view;

template <typename T>
concept BoardingStrategy = requires(T t, Ticket const &ticket) {
    { t.getGroupNumber(ticket) } -> std::convertible_to<int>;
};

/*
    Assuming there are MAX of 4 groups,

    Business - Group 1
    Window   - Group 2
    Middle   - Group 3
    Aisle    - Group 4
 
*/
struct WindowMiddleAisle {
    int getGroupNumber(Ticket const &ticket) const {
        if(ticket.isBusinessClass())
            return 1;

        if (ticket.isWindowSeat())
            return 2;
        else if (ticket.isMiddleSeat())
            return 3;

        return 4;
    }
};

/* 
    Assuming there are MAX of 5 groups,

    Business - Group 1
    Economy  - Group (2 - 5) based on the row number

 */
struct FrontToBack {
    int const MAX_GROUPS = 5;

    FlightModel const &model;
    FrontToBack(FlightModel const &m): model(m){ }

    int getGroupNumber(Ticket const &ticket) const {
        if (ticket.isBusinessClass())
            return 1;

        int averageRowsInOneGroup = model.economy.rows / (MAX_GROUPS - 1);
        int trueRowNumber = ticket.getRow() - model.business.rows - 1;
        return 2 + trueRowNumber / averageRowsInOneGroup;
    }
};

#endif