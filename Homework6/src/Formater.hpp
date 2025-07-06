#pragma once

#include <stdexcept>
#include <concepts>
#include <sstream>
#include <string>
#include <vector>
#include <functional>

class UserException : public std::runtime_error {
public:
    explicit UserException(const std::string& message) : std::runtime_error("Exception: " + message) {}
};

template<typename T>
concept Streamable = requires(std::ostream& os, const T& value ) {
    {os << value} -> std::same_as<std::ostream&>;
};

template<Streamable... ArgsT>
std::string Format(const std::string& str, ArgsT&&... args) {
    const size_t argsCount = sizeof...(args);

    std::vector<std::function<void(std::stringstream&)>> argsPrinter{};
    argsPrinter.reserve(argsCount);
    (argsPrinter.emplace_back([arg = std::forward<ArgsT>(args)](std::stringstream& str) { str << arg; }), ...);

    std::stringstream result{};
    size_t i{};
    while(i < str.length()) {
        size_t openPos = str.find('{', i);
        size_t closePos = str.find('}', i);

        if(openPos > closePos) {
            throw UserException("Invalid bracket.");
        } else if(openPos == std::string::npos && closePos == std::string::npos) {
            result << str.substr(i);
            break;
        } else if(!(openPos == std::string::npos ^ closePos != std::string::npos)) {
            throw UserException("Invalid bracket.");
        }
        result << str.substr(i, openPos - i);

        std::string indexStr = str.substr(openPos + 1, closePos - openPos - 1);
        if(indexStr.empty()) {
            throw UserException("Invalid index.");
        }

        size_t index{};
        size_t pos{};
        try {
            index = std::stoul(indexStr, &pos);
        } catch(...) {
            throw UserException("Invalid index.");
        }
        if(pos != indexStr.length() || index >= argsCount) {
            throw UserException("Invalid index.");
        }

        argsPrinter[index](result);
        i = closePos + 1;
    }
    return result.str();
}
