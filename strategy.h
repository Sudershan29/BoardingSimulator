#ifndef boarding_strategy_h
#define boarding_strategy_h

#include <cstddef>
#include <concepts>
#include <functional>
#include <string>
#include "passenger.h"
#include "ticket.h"
#include "flight_model.h"

using std::string_view;
int const MAX_GROUPS = 5;

template <typename T>
concept GroupAllocStrategy = requires(T t, Ticket const &ticket, FlightModel const &model) {
    { t.getGroupNumber(ticket) } -> std::convertible_to<int>;
    { T(model) } -> std::convertible_to<T>;
};

template <typename T>
concept BoardingStrategy = requires(T t, Passenger const &p1, Passenger const &p2) {
    {
        t.compare(p1, p2)
    } -> std::convertible_to<bool>;
};

/*
    Assuming there are MAX of 4 groups,

    Business - Group 1
    Window   - Group 2
    Middle   - Group 3
    Aisle    - Group 4
 
*/
struct WindowMiddleAisle {
    WindowMiddleAisle(FlightModel const &) { }

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
    FlightModel model;
    FrontToBack(FlightModel const &m): model(m){ }

    int getGroupNumber(Ticket const &ticket) const {
        if (ticket.isBusinessClass())
            return 1;

        int averageRowsInOneGroup = model.economy.rows / (MAX_GROUPS - 1);
        int trueRowNumber = ticket.getRow() - model.business.rows - 1;
        return 2 + trueRowNumber / averageRowsInOneGroup;
    }
};

struct CompareArrivalTimes{
    bool compare(Passenger const &p1, Passenger const &p2) const {
        return p1.arrivalTime < p2.arrivalTime;
    }
};

/*
    TODO: Need fixes because this does not account for 
*/
struct CompareBoardingGroups{
    bool compare(Passenger const &p1, Passenger const &p2) const {
        if(p1.ticket.getBoardingGroup() == p2.ticket.getBoardingGroup())
            return p1.arrivalTime < p2.arrivalTime;

        return p1.ticket.getBoardingGroup() < p2.ticket.getBoardingGroup();
    }
};

#endif