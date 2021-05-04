#ifndef __ROUTE_HPP__
#define __ROUTE_HPP__

#include <vector>

#include "../constants.hpp"
#include "station.hpp"

class Route
{
    std::string number;
    RouteType type;
public:
    std::vector<Station> stations;

    Route() {}
    Route(std::string _number, const char * _type): number(_number) {
        if (!strcmp(_type, ROUTE_TYPE_FLAG_BUS)) {
            type = BUS;
        } else if (!strcmp(_type, ROUTE_TYPE_FLAG_TRAM)) {
            type = TRAM;
        } else {
            type = TROLLEY;
        }
    }

    std::string get_number() { return number; }
    RouteType get_type() {return type; }
};

#endif
