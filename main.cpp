#include <iostream>

#include "Api_reader.hpp"



int main() {
    try {
        std::unordered_set<std::string> needed_values;
        needed_values.insert("S50");
        needed_values.insert("S60");
        needed_values.insert("S107");
        std::string json = Api_reader::get();

        for ( auto map : Api_reader::json_parser(json, needed_values)){
            std::cout << map.first << " "<< map.second << std::endl;
        }
        return 0;
    } catch (const std::exception& e){
        std::cerr << "Ошибка при разборе JSON: " << e.what() << std::endl;
        return 0;
    }
}
