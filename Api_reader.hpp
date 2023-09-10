//
// Created by kotozavr on 06.09.23.
//

#ifndef NIIS_API_READER_HPP
#define NIIS_API_READER_HPP

#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>
#include "MyException.hpp"

namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace beast = boost::beast;
namespace http = boost::beast::http;

const static std::string API = "api.data.gov.sg";
const static std::string API_PATH = "/v1/environment/air-temperature";

#define TOPIC_VALUES "/api/temperature/"
#define TOPIC_INFO "/api/status"



class Api_reader {
public:
    static std::string get() {
        asio::io_context io;
        ssl::context ctx(ssl::context::sslv23_client);

        ctx.set_default_verify_paths();

        ssl::stream<asio::ip::tcp::socket> socket(io, ctx);
        asio::ip::tcp::resolver resolver(io);

        asio::connect(socket.lowest_layer(), resolver.resolve(API, "https"));


        socket.handshake(ssl::stream_base::client);

        http::request<http::string_body> request(http::verb::get, API_PATH, 11);
        request.set(http::field::host, API);
        request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, request);

        std::string response;
        {
            boost::beast::flat_buffer buffer;
            http::response<http::dynamic_body> res;
            http::read(socket, buffer, res);
            response = boost::beast::buffers_to_string(res.body().data());
        }

        beast::error_code ec;
        socket.lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);

        return response;
    }

    static std::vector<std::pair<std::string,std::string>> json_parser ( std::string& json){
        boost::property_tree::ptree pt;
        std::istringstream json_stream(json);
        std::vector<std::pair<std::string,std::string>> values;

            boost::property_tree::read_json(json_stream, pt);

            boost::property_tree::ptree readings = pt.get_child("items").front().second.get_child("readings");
            if (readings.empty()){
                throw (MyException::ExceptionType::JSONParseError);
            }
            for (const auto& reading : readings) {
                auto station_id = reading.second.get<std::string>("station_id");
                auto value = reading.second.get<std::string>("value");
                if( station_id == "S50" || station_id == "S60" || station_id == "S107" )
                values.emplace_back(TOPIC_VALUES +station_id,value);
            }
            boost::property_tree::ptree api_info = pt.get_child("api_info");
            auto status = api_info.get<std::string>("status");
            values.emplace_back(TOPIC_INFO,status);
            return values;
    }

};


#endif //NIIS_API_READER_HPP
