#include <iostream>

#include "Api_reader.hpp"
#include "Mosquitto_mqtt.hpp"



int main() {
    try {

        std::string json = Api_reader::get();

        Mqtt_broker::send_values(Api_reader::json_parser(json));
        return 0;
    } catch (const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 0;
    }
}
