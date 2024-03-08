#ifndef flight_model_h
#define flight_model_h

#include <iostream>
#include <string>
#include "config.h"

using std::ostream;

using std::string;

namespace flightSimulator
{
    /*
        This class has properties of the flight based on the model
    */
    struct FlightModel
    {
        struct flightClass
        {
            int rows, perRow;
            friend ostream &operator<<(ostream &os, flightClass const &f)
            {
                os << "Rows: " << f.rows << " Per Row: " << f.perRow;
                return os;
            }
        };

        string name;
        int totalPassengers, totalRows;
        flightClass business, economy;

        FlightModel(string _name, int _totalPassengers, int _totalRows, int _businessRows, int _businessPerRow, int _economyRows, int _economyPerRow) : name(_name), totalPassengers(_totalPassengers), totalRows(_totalRows)
        {
            business = {_businessRows, _businessPerRow};
            economy = {_economyRows, _economyPerRow};
        }

        FlightModel(FlightModel const &m) : name(m.name), totalPassengers(m.totalPassengers), totalRows(m.totalRows), business(m.business), economy(m.economy) {}

        FlightModel(FlightModel &&m) : name(std::move(m.name)), totalPassengers(m.totalPassengers), totalRows(m.totalRows), business(m.business), economy(m.economy) {}

        FlightModel &operator=(FlightModel const &m)
        {
            name = m.name;
            totalPassengers = m.totalPassengers;
            totalRows = m.totalRows;
            business = m.business;
            economy = m.economy;
            return *this;
        }

        ~FlightModel() = default;

        void refresh(){
            name = config.get("Flight", "name"),
            totalPassengers = config.getInt("Flight", "total_seats"),
            totalRows = config.getInt("Flight", "total_rows"),
            business = {config.getInt("Flight", "business_class_rows"), config.getInt("Flight", "business_class_per_row")};
            economy = {config.getInt("Flight", "economy_class_rows"), config.getInt("Flight", "economy_class_per_row")};
        }

        bool isWindow(char const &seat, bool businessClass) const
        {
            int seatNumber = seat - 'A' + 1;
            if (businessClass)
                return seatNumber == 1 || seatNumber == business.perRow;

            return seatNumber == 1 || seatNumber == economy.perRow;
        }

        bool isMiddle(char const &seat, bool businessClass) const
        {
            int seatNumber = seat - 'A' + 1;
            if (businessClass)
                return seatNumber == 2 || seatNumber == business.perRow - 1;

            return seatNumber == 2 || seatNumber == economy.perRow - 1;
        }

        bool isBusinessClass(int row) const
        {
            return row <= business.rows;
        }

        friend ostream &operator<<(ostream &os, FlightModel const &m)
        {
            string const separator = "\n" + std::string(10, '-') + "\n";
            os << separator + "Flight Model: " + m.name + separator;
            os << separator + "Total Passengers: " << m.totalPassengers << separator;

            // TODO: Use format
            os << "Business Class : " << m.business << "\n";
            os << "Economy Class : " << m.economy << separator;

            return os;
        }
    };

    FlightModel BOEING_737_700{
        config.get("Flight", "name"),
        config.getInt("Flight", "total_seats"),
        config.getInt("Flight", "total_rows"),
        config.getInt("Flight", "business_class_rows"),
        config.getInt("Flight", "business_class_per_row"),
        config.getInt("Flight", "economy_class_rows"),
        config.getInt("Flight", "economy_class_per_row")};
}
#endif