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
#include <unordered_map>
#include <unordered_set>

namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace beast = boost::beast;
namespace http = boost::beast::http;

const static std::string API = "api.data.gov.sg";
const static std::string API_PATH = "/v1/environment/air-temperature";



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

    static std::unordered_map<std::string, float> json_parser ( std::string& json, std::unordered_set<std::string> needed_values){
        boost::property_tree::ptree pt;
        std::istringstream json_stream(json);
        std::unordered_map<std::string, float> values;
        try {
            boost::property_tree::read_json(json_stream, pt);

            boost::property_tree::ptree readings = pt.get_child("items").front().second.get_child("readings");

            for (const auto& reading : readings) {
                std::string station_id = reading.second.get<std::string>("station_id");
                double value = reading.second.get<double>("value");
                if( needed_values.find(station_id) != needed_values.end())
                values[station_id] = value;
            }
            return values;
        } catch (const std::exception& e) {
            throw;
        }
    }

};


#endif //NIIS_API_READER_HPP
