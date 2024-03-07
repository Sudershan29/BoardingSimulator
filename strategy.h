#ifndef boarding_strategy_h
#define boarding_strategy_h

#include <cstddef>
#include <concepts>
#include <functional>
#include <string>
#include "passenger.h"
#include "ticket.h"
#include "flight_model.h"
#include "config.h"

using std::string_view;
namespace flightSimulator
{
    int const MAX_GROUPS = config.getInt("Flight", "max_boarding_group");

    template <typename T>
    concept GroupAllocStrategy = requires(T t, Ticket const &ticket, FlightModel const &model, T const &t2) {
        {
            t.getGroupNumber(ticket)
        } -> std::convertible_to<int>;
        {
            t.getName()
        } -> std::convertible_to<string_view>;
        {
            T(model)
        } -> std::convertible_to<T>;
        {
            T(t2)
        } -> std::convertible_to<T>;
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

        WindowMiddleAisle(WindowMiddleAisle const &) { }

        int getGroupNumber(Ticket const &ticket) const {
            if(ticket.isBusinessClass())
                return 1;

            if (ticket.isWindowSeat())
                return 2;
            else if (ticket.isMiddleSeat())
                return 3;

            return 4;
        }

        string_view getName() const {
            return "WindowMiddleAisle";
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
        FrontToBack(FrontToBack const &f): model(f.model){ }

        int getGroupNumber(Ticket const &ticket) const {
            if (ticket.isBusinessClass())
                return 1;

            int averageRowsInOneGroup = model.economy.rows / (MAX_GROUPS - 1);
            int trueRowNumber = ticket.getRow() - model.business.rows - 1;
            return 2 + trueRowNumber / averageRowsInOneGroup;
        }

        string_view getName() const {
            return "FrontToBack";
        }
    };

    /* 
        Assuming there are MAX of 5 groups,

        Business - Group 5
        Economy  - Group (4 - 1) based on the row number

    */
    struct BackToFront {
        FlightModel model;
        BackToFront(FlightModel const &m) : model(m) {}
        BackToFront(BackToFront const &f) : model(f.model) {}

        int getGroupNumber(Ticket const &ticket) const {
            if (ticket.isBusinessClass())
                return MAX_GROUPS;

            int averageRowsInOneGroup = model.economy.rows / (MAX_GROUPS - 1);
            int trueRowNumber = ticket.getRow() - model.business.rows - 1;
            return MAX_GROUPS - 1 - trueRowNumber / averageRowsInOneGroup;
        }

        string_view getName() const {
            return "BackToFront";
        }
    };

    /*
        This arrangement would use arrival_times to sort everything
    */
    struct RandomBoarding {
        RandomBoarding(FlightModel const &) {}
        RandomBoarding(RandomBoarding const &) {}

        int getGroupNumber(Ticket const &) const {
            return MAX_GROUPS;
        }

        string_view getName() const {
            return "RandomBoarding";
        }
    };

    struct SteffenModified{
        FlightModel model;
        SteffenModified(FlightModel const &m) : model(m) {}
        SteffenModified(SteffenModified const &s) : model(s.model) {}

        int getGroupNumber(Ticket const &ticket) const
        {
            // if (ticket.isBusinessClass())
            //     return 1;

            if(ticket.getRow()%2)
                return ticket.rightSideOfThePlane() ? 2 : 3;

            return ticket.rightSideOfThePlane() ? 4 : 5;
        }

        string_view getName() const {
            return "SteffenModified";
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
}

#endif