#pragma once

#include <cstdlib>
#include <string>
#include <map>
#include <list>
#include <functional>
#include <iostream>

//#define SERIALIZE_DEBUG

#ifdef SERIALIZE_DEBUG
#define serial_dbg (std::cout << "Serialize debug: ")
#define serial_out (std::cout)
#else
#define serial_dbg (std::ostream(0))
#define serial_out (std::ostream(0))
#endif

namespace helper {
namespace serialize {
typedef std::list<std::string> values;

class map : public std::map<std::string, std::list<std::string>> {
public:
    typedef std::function<void(const serialize::values &values, const std::string &error)> callback;
    helper::serialize::map &handle(const std::string &keyword, callback handler);
    bool has(const std::string &keyword);
};

map args(int argc, char *argv[], map &&smap = {});
}
}