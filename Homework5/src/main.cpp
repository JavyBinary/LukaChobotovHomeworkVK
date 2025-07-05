#include "Serializer.hpp"
#include "Deserializer.hpp"

#include <iostream>
#include <cassert>
#include <sstream>

int main(int, char**) {
    Data x { 1, true, 2 };

    std::stringstream stream;

    Serializer serializer(stream);
    serializer.Save(x);

    Data y { 0, false, 0 };

    Deserializer deserializer(stream);
    const Error err = deserializer.Load(y);

    assert(err == Error::NoError);

    assert(x.a == y.a);
    assert(x.b == y.b);
    assert(x.c == y.c);
    return 0;
}
