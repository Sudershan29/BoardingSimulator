#ifndef quantum_h
#define quantum_h

#include <iostream>
#include "config.h"

namespace flightSimulator
{
    float const MULTIPIER = config.getFloat("Quantum", "minutes_conversion");

    struct TimeQuantum
    {
        int quantum;

        TimeQuantum() : quantum(0) {}

        TimeQuantum(int q) : quantum(q) {}

        TimeQuantum(TimeQuantum const &t) = default;

        int minutes() const
        {
            return quantum * MULTIPIER;
        }

        bool isDone()
        {
            return quantum == 0;
        }

        void reduce()
        {
            quantum -= 1;
        }

        void reset()
        {
            quantum = 0;
        }

        friend TimeQuantum operator+(TimeQuantum const &t1, TimeQuantum const &t2)
        {
            TimeQuantum t;
            t.quantum = t1.quantum + t2.quantum;
            return t;
        }

        friend bool operator<(TimeQuantum const &t1, TimeQuantum const &t2)
        {
            return t1.quantum < t2.quantum;
        }

        friend bool operator>(TimeQuantum const &t1, TimeQuantum const &t2)
        {
            return t1.quantum > t2.quantum;
        }

        TimeQuantum &operator*(int const &t)
        {
            quantum *= t;
            return *this;
        }

        TimeQuantum &operator+=(TimeQuantum const &t)
        {
            quantum = quantum + t.quantum;
            return *this;
        }

        TimeQuantum &operator=(TimeQuantum const &t)
        {
            quantum = t.quantum;
            return *this;
        }

        friend std::ostream &operator<<(std::ostream &os, TimeQuantum const &t)
        {
            os << t.minutes() << " minutes";
            return os;
        }

        TimeQuantum &operator/=(int const &t)
        {
            quantum /= t;
            return *this;
        }
    };
}

#endif