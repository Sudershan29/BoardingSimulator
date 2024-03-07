#ifndef rng_h
#define rng_h

#include <iostream>
#include <random>
#include <concepts>
#include <vector>

using std::vector;

namespace flightSimulator
{
    template <typename T>
    concept RNGenerator = requires(T t) {
        {
            t.getNumber()
        } -> std::convertible_to<int>;
    };

    struct NormDistribution
    {
        NormDistribution(int mean, int stddev, int min, int max) : gen(rd()), d(mean, stddev), min(min), max(max) {}

        int getNumber()
        {
            int num = d(gen);
            return std::clamp(num, min, max);
        }

        std::random_device rd;
        std::mt19937 gen;
        std::normal_distribution<> d;
        int min;
        int max;
    };

    struct RandomNumber
    {
        RandomNumber(int min, int max) : gen(rd()), d(min, max) {}

        int getNumber()
        {
            return d(gen);
        }

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_int_distribution<> d;
    };

    struct LotteryGame
    {
        LotteryGame(vector<int> const &numbers) : gen(rd()), d(numbers.begin(), numbers.end()) {}

        int getNumber()
        {
            return d(gen);
        }

        std::random_device rd;
        std::mt19937 gen;
        std::discrete_distribution<> d;
    };
}
#endif