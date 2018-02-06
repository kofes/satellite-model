#include <iostream>
#include "args_serializer.h"

using namespace std;

int main(int argc, char* argv[]) {

    serialize::args(argc, argv)
    .handle("argument", [&] (const serialize::values& values, const std::string& error) {
        for (const std::string& value: values)
            cout << value << endl;
    });

    return 0;
}