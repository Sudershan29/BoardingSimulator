#ifndef queue_h
#define queue_h

#include <iostream>
#include <vector>
#include <functional>
#include <optional>
#include "passenger.h"
#include "flight_model.h"
#include "quantum.h"

#define FMT_HEADER_ONLY
#include <fmt/format.h>

using std::vector;
using std::unique_ptr;
using std::function;
using std::runtime_error;
using std::ostream;

namespace flightSimulator
{
    enum FlightQStatus
    {
        empty,
        moving,
        found
    };

    class FlightQ
    {
        vector<unique_ptr<Passenger>> passengers;
        vector<FlightQStatus> statuses;
        vector<TimeQuantum> countdown; // Time till the spot becomes vacant
        function<bool(Passenger &, int)> isMySeat;
        function<TimeQuantum(Passenger &)> waitTime;
        function<void(Passenger &)> sitDown;
        int front, back, inQ;

        bool canMove(int idx)
        {
            if (idx > front)
                throw runtime_error("Seat index out of bounds");

            return statuses[idx] == FlightQStatus::empty;
        }

    public:
        FlightQ(FlightModel const &model,
                function<bool(Passenger &, int)> _isMySeat,
                function<TimeQuantum(Passenger &)> _waitTime,
                function<void(Passenger &)> _sitDown) : isMySeat(_isMySeat),
                                                        waitTime(_waitTime),
                                                        sitDown(_sitDown),
                                                        front(model.totalRows - 1),
                                                        back(0),
                                                        inQ(0)
        {
            statuses.resize(model.totalRows, FlightQStatus::empty);
            countdown.resize(model.totalRows, TimeQuantum(0));
            passengers.resize(model.totalRows);
        }

        FlightQ(FlightQ const &f){
            for(auto const &p : f.passengers)
                passengers.push_back(make_unique<Passenger>(*p));

            statuses = f.statuses;
            countdown = f.countdown;
            isMySeat = f.isMySeat;
            waitTime = f.waitTime;
            sitDown = f.sitDown;
            front = f.front;
            back = f.back;
            inQ = f.inQ;   
        }

        FlightQ(FlightQ &&f){
            passengers = std::move(f.passengers);
            statuses = std::move(f.statuses);
            countdown = std::move(f.countdown);
            isMySeat = f.isMySeat;
            waitTime = f.waitTime;
            sitDown = f.sitDown;
            front = f.front;
            back = f.back;
            inQ = f.inQ;   
        }

        // O(n)
        void Move()
        {
            for (int i = front; i >= back; i--)
            {
                // if no one is in the spot, continue
                if (statuses[i] == FlightQStatus::empty)
                    continue;

                if (statuses[i] == FlightQStatus::moving)
                {
                    // If I found my seat, then calculate countdown
                    if (isMySeat(*passengers[i], i))
                    {
                        statuses[i] = FlightQStatus::found;
                        countdown[i] = waitTime(*passengers[i]);
                        continue;
                    }
                    // If there is a spot ahead of me, then move ahead
                    else if (canMove(i + 1))
                    {
                        passengers[i + 1] = std::move(passengers[i]);
                        statuses[i + 1] = FlightQStatus::moving;

                        statuses[i] = FlightQStatus::empty;
                        passengers[i] = nullptr;
                        continue;
                    }

                    // cannot move, then wait
                    else
                        continue;
                }

                // if the countdown is not done, reduce it
                if (!countdown[i].isDone())
                {
                    countdown[i].reduce();
                    continue;
                }

                // Remove it from the queue
                inQ -= 1;
                sitDown(*passengers[i]);
                passengers[i] = nullptr;
                countdown[i].reset();
                statuses[i] = FlightQStatus::empty;
            }
        }

        bool Done()
        {
            return inQ == 0;
        }

        bool Push(Passenger p)
        {
            if (!canMove(back))
                return false;

            inQ += 1;
            passengers[back] = std::make_unique<Passenger>(p);
            statuses[back] = FlightQStatus::moving;
            return true;
        }

        ostream &printIdx(ostream &os, int idx) const
        {
            if (statuses[idx] == FlightQStatus::empty)
            {
                os << fmt::format("{:^{}}", "", 5);
                return os;
            }

            os << *passengers[idx]->ticket;
            return os;
        }

        friend ostream &operator<<(ostream &os, FlightQ const &f)
        {
            for (int i = f.back; i < f.front; i++)
            {
                f.printIdx(os, i);
                os << "\n";
            }

            return os;
        }
    };
}

#endif

 