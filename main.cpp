#include <iostream>
#include <map>

#include "constants.hpp"
#include "models/route.hpp"

double parse_coord(std::string src, bool first = true) {
    int it_comm;
    for (size_t i = 0; i < src.size(); ++i) {
        if (src[i] == ',') {
            it_comm = i;
            break;
        }
    }

    double res = 0;

    if (first) {
        int it_dot;
        for (int i = 0; i < it_comm; ++i) {
            if (src[i] == '.') {
                it_dot = i;
                break;
            }
        }

        double pw = 1;
        for (int i = it_dot - 1; i >= 0; --i) {
            res += (src[i] - '0') * pw;
            pw *= 10;
        }
        pw = 0.1;
        for (int i = it_dot + 1; i < it_comm; ++i) {
            res += (src[i] - '0') * pw;
            pw /= 10;
        }
    } else {
        int it_dot;
        for (size_t i = it_comm + 1; i < src.size(); ++i) {
            if (src[i] == '.') {
                it_dot = i;
                break;
            }
        }

        double pw = 1;
        for (int i = it_dot - 1; i > it_comm; --i) {
            res += (src[i] - '0') * pw;
            pw *= 10;
        }
        pw = 0.1;
        for (size_t i = it_dot + 1; i < src.size(); ++i) {
            res += (src[i] - '0') * pw;
            pw /= 10;
        }
    }
    return res;
}

void parse_routes(std::string src, std::vector<std::string> &dest) {
    size_t tmp = 0;
    size_t pt = 0;
    while(tmp < src.size()) {
        if (src[tmp] == ',' || src[tmp] == '.') {
            std::string curr = "";
            while (pt != tmp) {
                curr += src[pt];
                ++pt;
            }
            ++pt;
            dest.push_back(curr);
        }
        ++tmp;
    }
    std::string curr = "";
    while (pt != tmp) {
        curr += src[pt];
        ++pt;
    }
    dest.push_back(curr);
}

void parse_streets(std::string src, std::vector<std::string> &dest) {
    size_t tmp = 0;
    size_t pt = 0;
    while(tmp < src.size()) {
        if (src[tmp] == ',') {
            std::string curr = "";
            while (pt != tmp) {
                curr += src[pt];
                ++pt;
            }
            pt += 2; // Skip space and comma
            ++tmp; // Skip space
            dest.push_back(curr);
        }
        ++tmp;
    }
    std::string curr = "";
    while (pt != tmp) {
        curr += src[pt];
        ++pt;
    }
    dest.push_back(curr);
}

int main()
{
    pugi::xml_document table; // Whole xml document init
    pugi::xml_parse_result result = table.load_file(TABLE_FILE_PATH); // Read xml in 'table'

    if (!result.description())
        std::cout << LOAD_ERRORS << std::endl;

    pugi::xml_node stations = table.child("dataset"); // Get part of the xml with data
    std::vector<Station> processed_stations; // Container of all stations
    std::map<std::string, Route> routes_from_numbers; // Map from name of route to Route object
    
    // Read the data, saving in data structures
    for (pugi::xml_node station: stations.children("transport_station"))
    {   
        if (!strcmp(station.child("object_type").child_value(), STATION_TYPE_FLAG)) {
            double x = parse_coord(station.child("coordinates").child_value(), true);
            double y = parse_coord(station.child("coordinates").child_value(), false);

            Station curr_st = Station(
                station.child("name_stopping").child_value(),
                station.child("the_official_name").child_value(),
                station.child("location").child_value(),
                x,
                y,
                station.child("type_of_vehicle").child_value()
            );
            processed_stations.push_back(curr_st);

            std::vector<std::string> parsed_routes;
            parse_routes(station.child("routes").child_value(), parsed_routes);
            for (size_t i = 0; i < parsed_routes.size(); ++i) {
                Route curr_rt = Route(
                    parsed_routes[i],
                    station.child("type_of_vehicle").child_value()
                );
                
                if (routes_from_numbers[parsed_routes[i]].stations.size() == 0) { // Init if needed
                    routes_from_numbers[parsed_routes[i]] = curr_rt;
                }
                // Push back current station to each route
                routes_from_numbers[parsed_routes[i]].stations.push_back(processed_stations.back());
            }
        }
    }

    std::string tram_name;
    std::string trolley_name;
    std::string bus_name;

    // Solve the 1 problem. Count stations for each type of route
    unsigned long long tram_count = 0;
    unsigned long long trolley_count = 0;
    unsigned long long bus_count = 0;

    for (auto el: routes_from_numbers) {
        if (el.second.get_type() == TRAM) {
            if (tram_count < el.second.stations.size()) {
                tram_count = el.second.stations.size();
                tram_name = el.first;
            }
        } else if (el.second.get_type() == TROLLEY) {
            if (trolley_count < el.second.stations.size()) {
                trolley_count = el.second.stations.size();
                trolley_name = el.first;
            }
        } else if (el.second.get_type() == BUS) {
            if (bus_count < el.second.stations.size()) {
                bus_count = el.second.stations.size();
                bus_name = el.first;
            }
        }
    }
    std::cout << "*-------------------------------------------------*"<< std::endl;
    std::cout << "1 part of analysys:" << std::endl;
    std::cout << "The biggest count of stations per type:" << std::endl;
    std::cout << "Bus: " << bus_name << "; Count: " << bus_count << ";" << std::endl;
    std::cout << "Tram: " << tram_name << "; Count: " << tram_count << ";" << std::endl;
    std::cout << "Trolley: " << trolley_name << "; Count: " << trolley_count << ";" << std::endl;
    std::cout << "*-------------------------------------------------*"<< std::endl;

    // Solve the 2 problem. Count the longest route per type
    // Assuming that all stations are 'well' sorted in raw table
    // Also assume that all the routes are not circled
    double tram_length = -1;
    double trolley_length = -1;
    double bus_length = -1;

    for (auto el: routes_from_numbers) {
        double curr_length = 0;
        double prev_x = el.second.stations[0].get_x(); // Always exists
        double prev_y = el.second.stations[0].get_y(); // Always exists
        for (size_t i = 1; i < el.second.stations.size(); ++i) {
            double delta_x = abs(el.second.stations[i].get_x() - prev_x);
            double delta_y = abs(el.second.stations[i].get_y() - prev_y);
            // Approximate by line between two stations
            curr_length += sqrt(delta_x * delta_x + delta_y * delta_y);
        }

        if (el.second.get_type() == TRAM) {
            if (tram_length < curr_length) {
                tram_length = curr_length;
                tram_name = el.first;
            }
        } else if (el.second.get_type() == TROLLEY) {
            if (trolley_length < curr_length) {
                trolley_length = curr_length;
                trolley_name = el.first;
            }
        } else if (el.second.get_type() == BUS) {
            if (bus_length < curr_length) {
                bus_length = curr_length;
                bus_name = el.first;
            }
        }
    }

    std::cout << "*-------------------------------------------------*"<< std::endl;
    std::cout << "2 part of analysys:" << std::endl;
    std::cout << "The longest route per type:" << std::endl;
    std::cout << "Bus: " << bus_name << "; Length: " << bus_length << ";" << std::endl;
    std::cout << "Tram: " << tram_name << "; Length: " << tram_length << ";" << std::endl;
    std::cout << "Trolley: " << trolley_name << "; Length: " << trolley_length << ";" << std::endl;
    std::cout << "*-------------------------------------------------*"<< std::endl;

    // Solve the 3 problem. Find the street with the biggest count of stations
    // Assuming that all the streets like "ul. Ulichnaya", "Ulichnaya ul.", "Ulichnaya", etc, are different
    // There are empty locations in the source table. Because of that we throw away empty value
    std::map<std::string,unsigned long long> streets; // Map name of street to count of stations

    for (size_t i = 0; i < processed_stations.size(); ++i) {
        std::vector<std::string> current_streets;
        parse_streets(processed_stations[i].get_location(), current_streets);

        for (size_t j = 0; j < current_streets.size(); ++j) {
            ++streets[current_streets[j]];
        }
    }

    unsigned long long count_of_stations = 0;
    std::string street_name;
    for (auto street: streets) {
        if (street.second > count_of_stations && street.first != "") {
            count_of_stations = street.second;
            street_name = street.first;
        }
    }

    std::cout << "*-------------------------------------------------*"<< std::endl;
    std::cout << "3 part of analysys:" << std::endl;
    std::cout << "The street with biggest count of stations:" << std::endl;
    std::cout << "Street: " << street_name
    << "; Count of stations: " << count_of_stations << ";" << std::endl;
    std::cout << "*-------------------------------------------------*"<< std::endl;

    return 0;
}
