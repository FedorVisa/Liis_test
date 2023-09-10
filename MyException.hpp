//
// Created by kotozavr on 08.09.23.
//

#ifndef NIIS_MYEXCEPTION_HPP
#define NIIS_MYEXCEPTION_HPP

#include <exception>


class MyException : public std::exception {
public:
public:
    enum class ExceptionType {
        CantCreateValue,
        NoConnection,
        JSONParseError
    };

    explicit MyException(ExceptionType type)
            : m_type(type) {}

    const char* what() const noexcept override {
        switch (m_type) {
            case ExceptionType::CantCreateValue:
                return "Cant set up MQTT broker.";
            case ExceptionType::NoConnection:
                return "Cant start mqtt section.";
            case ExceptionType::JSONParseError:
                return "Error occurred while parsing JSON.";

            default:
                return "Unknown exception";
        }
    }

private:
    ExceptionType m_type;
};

#endif //NIIS_MYEXCEPTION_HPP
