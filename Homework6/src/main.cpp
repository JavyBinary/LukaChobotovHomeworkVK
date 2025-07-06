#include "Formater.hpp"

#include <iostream>
#include <cassert>

int main(int, char**) {
    assert(Format("{1} {0}", "world", "Hello") == "Hello world");
    const std::string s = "string";
    assert(Format("{0}, {1}, {2}, {3}", 42, 3.14, s, 'A') == "42, 3.14, string, A");
    assert(Format("{0}{0}{1}{1}", "a", "b") == "aabb");
    assert(Format("text") == "text");
    assert(Format("{0} and {1}", "Start", "End") == "Start and End");

    try {
        Format("{2}", "arg0", "arg1");
    } catch(const UserException& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        Format("{}", 1);
    } catch(const UserException& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        Format("{abc}", 1);
    } catch(const UserException& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        Format("a } b", 1);
    } catch(const UserException& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        Format("a { b", 1);
    } catch(const UserException& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        Format("{0oops}", "hello");
    } catch(const UserException& e) {
       std::cout << e.what() << std::endl;
    }

    return 0;
}
