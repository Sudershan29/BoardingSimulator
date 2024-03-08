#ifndef seat_h
#define seat_h

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <functional>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include "quantum.h"
#include "flight_model.h"

using std::vector;
using std::runtime_error;
using std::ostream;
using std::function;
using std::pair;
using std::string;
using std::unique_ptr;
using std::make_unique;

namespace flightSimulator
{
    /*
        Assumptions
        i) No of corridors = 1
    */
    class Seat
    {
        vector<bool> row;
        int middle, totalSeats, rowNum;

    public:
        Seat(int total, int _rowNum, int _middle = -1) : totalSeats(total), rowNum(_rowNum)
        {
            middle = _middle == -1 ? totalSeats / 2 : _middle;
            row.resize(totalSeats, false);
        }

        void sitDown(int seat)
        {
            if (seat >= totalSeats)
                throw runtime_error("Invalid seat number");

            if (row[seat])
                throw runtime_error("Seat already taken");

            row[seat] = true;
        }

        // 0 1 2 | 3 4 5
        int aheadOfMe(int seat)
        {
            if (seat >= totalSeats)
                throw runtime_error("Invalid seat number");

            int count = 0;
            if (seat < middle)
            {
                for (int i = middle - 1; i > seat; i--)
                    if (row[i])
                        count += 1;

                return count;
            }

            for (int i = middle; i < seat; i++)
                if (row[i])
                    count += 1;

            return true;
        }

        void deplane(){
            for(int i = 0; i < totalSeats; i++)
                row[i] = false;
        }

        friend ostream &operator<<(ostream &os, pair<Seat const &, function<void(ostream &, int)> const &> seatWithQFunction)
        {
            Seat const &s = seatWithQFunction.first;
            auto printFunc = [&](int i) -> string
            { return s.row[i] ? " X " : "   "; };

            os << fmt::format("Row {:^{}} |", s.rowNum + 1, 6);

            string result;
            for (int i = 0; i < s.middle; i++)
                result += printFunc(i);

            os << fmt::format("{:^{}} |", result, 9);
            seatWithQFunction.second(os, s.rowNum);
            os << " | ";

            result.clear();
            for (int i = s.middle; i < s.totalSeats; i++)
                result += printFunc(i);

            os << fmt::format("{:^{}} |", result, 9);

            return os;
        }
    };

    class SeatMap
    {
        int totalRows;
        vector<unique_ptr<Seat>> seats;

    public:
        SeatMap(FlightModel const &model)
        {
            totalRows = model.totalRows;
            for (int i = 0; i < model.totalRows; i++)
                if (i < model.business.rows)
                    seats.push_back(make_unique<Seat>(model.business.perRow, i));
                else
                    seats.push_back(make_unique<Seat>(model.economy.perRow, i));
        }

        SeatMap(SeatMap const &s) {
            totalRows = s.totalRows;
            for(int i = 0; i < totalRows; i++)
                seats.push_back(make_unique<Seat>(*s.seats[i]));
        }

        SeatMap(SeatMap &&s) {
            totalRows = s.totalRows;
            seats = std::move(s.seats);
        }

        /*
            This function returns the number of people sitting ahead of me
        */
        int peopleInMyWay(int row, int seat)
        {
            return seats[row]->aheadOfMe(seat);
        }

        void sitDown(int row, int seat)
        {
            seats[row - 1]->sitDown(seat);
        }

        void print(ostream &os, function<void(ostream &, int)> const qPrintFunction)
        {
            for (auto const &seat : seats)
                os << std::pair(*seat, qPrintFunction) << "\n";
        }

        TimeQuantum waitTime(Passenger const &p)
        {
            TimeQuantum result = p.UnloadingTime();
            result += TimeQuantum(peopleInMyWay(p.ticket->getRow() - 1, p.ticket->getSeatIdx())) * config.getInt("Quantum", "people_wait_time");
            return result;
        }

        void deplane(){
            for(auto &seat : seats)
                seat->deplane();
        }
    };
}
#endif