#pragma once

#include "Serializer.hpp"

#include <sstream>
#include <string>

class Deserializer {
private:
    std::istream& is;

    Error Process(bool& arg) {
        std::string str{};
        is >> str;
        if(is.fail()) {
            return Error::CorruptedArchive;
        }

        if(str == "true") {
            arg = true;
        } else if(str == "false") {
            arg = false;
        } else {
            return Error::CorruptedArchive;
        }
        return Error::NoError;
    }

    Error Process(uint64_t& arg) {
        is >> arg;
        if(is.fail()) {
            return Error::CorruptedArchive;
        }
        return Error::NoError;
    }

    template<typename T, typename... ArgsT>
    Error Process(T& arg, ArgsT&... args) {
        Error error = Process(arg);
        if(error != Error::NoError) {
            return Error::CorruptedArchive;
        }

        if constexpr (sizeof...(args) > 0) {
            error = Process(args...);
        }

        return error;
    }

public:
    explicit Deserializer(std::istream& in) : is(in) {}

    template<typename T>
    Error Load(T& object) {
        return object.Serialize(*this);
    }

    template<typename... ArgsT>
    Error operator()(ArgsT&... args) {
        return Process(args...);
    }
};
