
#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <string>
#include <future>
#include <variant>
#include <exception>
#include <memory>

#include "flight.h"
#include "passenger.h"
#include "rng.h"
#include "strategy.h"
#include "quantum.h"
#include "config.h"

using std::async;
using std::cout;
using std::endl;
using std::exception;
using std::future;
using std::string;
using std::string_view;
using std::to_string;
using std::variant;
using std::make_unique;
using std::unique_ptr;

using namespace flightSimulator;

template <GroupAllocStrategy T>
TimeQuantum assignPassengerTickets(Flight<T> flight);

template <GroupAllocStrategy T>
class BoardingSimulator
{
public:
    BoardingSimulator(FlightModel const &model) : strategy(T(model)), flight(make_unique<Flight<T>>(model)) {}

    BoardingSimulator(BoardingSimulator const &b) : strategy(b.strategy), flight(make_unique<Flight<T>>(*b.flight)) {}

    BoardingSimulator(BoardingSimulator &&b) : strategy(b.strategy), flight(std::move(b.flight)), running(b.running) {}    

    future<void> simulateFuture(int iterations)
    {
        return async([&]()
                     { simulate(iterations); });
    }

    void simulate(int iterations)
    {
        for (int i = 0; i < iterations; i++)
        {
            loadPassengers();
            total += runSingle();
            flight->deplane();
        }

        total /= iterations;
        running = false;
    }

    friend ostream &operator<<(ostream &os, ::BoardingSimulator<T> const &b)
    {
        os << b.getStrategyName() << " | Average time = " << b.total << endl;
        return os;
    }

    void operator()(int simulations)
    {
        simulate(simulations);
    }

private:
    T strategy;
    TimeQuantum total, minTime, maxTime;
    unique_ptr<Flight<T>> flight;
    PassengerList passengers;
    bool running = false;

    void loadPassengers()
    {
        // Step 1
        passengers.clear();

        // Step 2
        for (int i = 0; i < flight->totalPassengers(); i++)
        {
            string name = to_string(i);
            Ticket const &ticket = flight->getTicket();

            passengers.push_back(Passenger(name, ticket));
        }
    }

    TimeQuantum runSingle()
    {
        return flight->board(passengers, CompareBoardingGroups());
    }

    string_view getStrategyName() const
    {
        return strategy.getName();
    }
};

class SimulationVisitor
{
public:
    virtual void simulate(int)   = 0;
    virtual ~SimulationVisitor() = default;
    virtual void printResults(ostream &os) const = 0;
};

template <GroupAllocStrategy T>
class Simulation : public SimulationVisitor
{
    unique_ptr<BoardingSimulator<T>> simulator;

public:
    Simulation(FlightModel const &m) : simulator(make_unique<BoardingSimulator<T>>(m)) {}

    void simulate(int iter) override
    {
        return simulator->simulate(iter);
    }

    void printResults(ostream &os) const override
    {
        os << *simulator;
    }

    friend ostream &operator<<(ostream &os, Simulation<T> const &s)
    {
        os << *s.simulator;
        return os;
    }
};

unique_ptr<SimulationVisitor> getSimulation(string const &s, FlightModel const &model)
{
    unique_ptr<SimulationVisitor> result;
    if (s == "SteffenModified")
        result = make_unique<Simulation<SteffenModified>>(model);
    else if (s == "WindowMiddleAisle")
        result = make_unique<Simulation<WindowMiddleAisle>>(model);
    else if (s == "FrontToBack")
        result = make_unique<Simulation<FrontToBack>>(model);
    else if (s == "BackToFront")
        result = make_unique<Simulation<BackToFront>>(model);
    else if (s == "RandomBoarding")
        result = make_unique<Simulation<RandomBoarding>>(model);
    else
        throw std::invalid_argument("Invalid strategy name: " + s);

    return result;
}

#endif // SIMULATION_H