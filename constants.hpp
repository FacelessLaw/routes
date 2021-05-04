#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#include "pugixml-1.11/src/pugixml.hpp"

const char TABLE_FILE_PATH[]{"../data_set.xml"};
const pugi::char_t LOAD_ERRORS[]{"There are some errors with file loading"};
const pugi::char_t STATION_TYPE_FLAG[]{"Остановка"};
const pugi::char_t ROUTE_TYPE_FLAG_TRAM[]{"Трамвай"};
const pugi::char_t ROUTE_TYPE_FLAG_TROLLEY[]{"Троллуйбус"};
const pugi::char_t ROUTE_TYPE_FLAG_BUS[]{"Автобус"};

enum RouteType {
    BUS,
    TRAM,
    TROLLEY
};

#endif
