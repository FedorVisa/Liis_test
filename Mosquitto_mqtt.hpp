//
// Created by kotozavr on 07.09.23.
//

#ifndef NIIS_MOSQUITTO_MQTT_HPP
#define NIIS_MOSQUITTO_MQTT_HPP

#include <mosquitto.h>
#include <string>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <thread>
#include "MyException.hpp"
#include <fstream>


const char * MQTT_USERNAME = "rw";
const char * MQTT_PASSWORD = "readwrite";
const char * MQTT_HOST = "test.mosquitto.org";
const char *cafile = "./mqtt_certificates/mosquitto.org.crt";



class Mqtt_broker{
public:

    static void on_connect(struct mosquitto *mosq, void *userdata, int rc) {

        if (rc == 0) {
            std::cout << "Успешное подключение к брокеру MQTT" << '\n';


            std::vector<std::pair<std::string, std::string>> *data = static_cast<std::vector<std::pair<std::string, std::string>> *>(userdata);

            int qos = 1;
            for (const auto &entry : *data) {
                const std::string &topic = entry.first;
                const std::string &payload = entry.second;
                mosquitto_subscribe(mosq, NULL, topic.c_str(), qos);
                mosquitto_publish(mosq, NULL, topic.c_str(), payload.length(), payload.c_str(), qos, false);
            }

        } else {
            throw (MyException::ExceptionType::NoConnection);
        }
    }

    static void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *msg) {
        std::cout << static_cast<char*>(msg->payload)<< '\n';
    }

    static void send_values(std::vector<std::pair<std::string,std::string>> values){
        struct mosquitto *mosq = NULL;

        mosquitto_lib_init();

        mosq = mosquitto_new(NULL, true, &values);
        if (!mosq) {
            throw (MyException::ExceptionType::CantCreateValue);
        }

        mosquitto_username_pw_set(mosq, MQTT_USERNAME, MQTT_PASSWORD);
        mosquitto_tls_set(mosq, cafile, NULL, NULL, NULL, NULL);
        mosquitto_message_callback_set(mosq,on_message);

        mosquitto_connect_callback_set(mosq, on_connect);


        int rc = mosquitto_connect(mosq, MQTT_HOST, 8885, 60);
        std::cout << rc;
        if (rc != MOSQ_ERR_SUCCESS) {
            throw (MyException::ExceptionType::NoConnection);
        }

        mosquitto_loop_start(mosq);
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        mosquitto_disconnect(mosq);
        mosquitto_loop_stop(mosq, true);

        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
    }
};

#endif //NIIS_MOSQUITTO_MQTT_HPP
