#ifndef __STATION_HPP__
#define __STATION_HPP__

#include <string>

#include "../constants.hpp"

class Station
{
    std::string name;
    std::string official_name;
    std::string location;
    
    double x, y;

    RouteType type;
public:
    Station(
        std::string _name,
        std::string _official_name,
        std::string _location,
        double _x,
        double _y,
        const char * _type
    ): 
    name(_name), 
    official_name(_official_name),
    location(_location), x(_x), y(_y) {
        if (!strcmp(_type, ROUTE_TYPE_FLAG_BUS)) {
            type = BUS;
        } else if (!strcmp(_type, ROUTE_TYPE_FLAG_TRAM)) {
            type = TRAM;
        } else {
            type = TROLLEY;
        }
    }

    std::string get_name() const { return name; }
    std::string get_official_name() const { return official_name; }
    std::string get_location() const { return location; }
    double get_x() const { return x; }
    double get_y() const { return y; }
    RouteType get_type() const {return type; }
};

#endif
