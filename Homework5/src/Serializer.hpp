#pragma once

#include <sstream>

enum class Error {
    NoError,
    CorruptedArchive
};

struct Data {
    uint64_t a{};
    bool b{};
    uint64_t c{};

    template<typename Serializer>
    Error Serialize(Serializer& serializer) {
        return serializer(a, b, c);
    }
};

class Serializer {
private:
    static constexpr char kSeparator = ' ';
    std::ostream& os;

    Error Process(bool arg) {
        os << (arg ? "true" : "false");
        return Error::NoError;
    }

    Error Process(uint64_t arg) {
        os << arg;
        return Error::NoError;
    }

    template<typename T, typename... ArgsT>
    Error Process(T&& arg, ArgsT&&... args) {
        Error error = Process(std::forward<T>(arg));
        if(error != Error::NoError) {
            return Error::CorruptedArchive;
        }

        if constexpr (sizeof...(args) > 0) {
            os << kSeparator;
            error = Process(std::forward<ArgsT>(args)...);
        }

        return error;
    }

public:
    explicit Serializer(std::ostream& out) : os(out) {}

    template<typename T>
    Error Save(T& object) {
        return object.Serialize(*this);
    }

    template<typename... ArgsT>
    Error operator()(ArgsT... args) {
        return Process(args...);
    }
};
