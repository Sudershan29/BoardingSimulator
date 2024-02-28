#ifndef flight_model_h
#define flight_model_h

#include <iostream>
#include <string>

using std::ostream;

using std::string;

/*
    This class has properties of the flight based on the model
*/
struct FlightModel
{
    struct flightClass { 
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
        economy  = {_economyRows, _economyPerRow};
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

    bool isBusinessClass(int row) const { 
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

FlightModel const BOEING_737_700{"Boeing-737", 160, 30, 10, 4, 20, 6};

#endif