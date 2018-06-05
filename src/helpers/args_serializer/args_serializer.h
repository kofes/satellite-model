#pragma once

#include <cstdlib>
#include <string>
#include <map>
#include <list>
#include <functional>
#include <iostream>

// #define SERIALIZE_DEBUG

#ifdef SERIALIZE_DEBUG
#define serial_dbg (std::cout << "Serialize debug: ")
#define serial_out (std::cout)
#else
#define serial_dbg (std::ostream(nullptr))
#define serial_out (std::ostream(nullptr))
#endif

namespace helper {
namespace serialize {
enum class ERR_CODE: char {
    NONE,
    KEY_NOT_EXISTS
};

typedef std::list<std::string> values;
typedef std::function<void(
    const serialize::values &values,
    const serialize::ERR_CODE &error
)> callback;

class map : public std::map<std::string, std::list<std::string>> {
 public:
    static map& args(int argc, char *argv[]);
    helper::serialize::map &handle(const std::string &keyword, callback handler);
    bool has(const std::string &keyword);

 private:
    map() = default;
    map(const map& src) = delete;
    void operator=(const map& src) = delete;
};
}   // namespace serialize
}   // namespace helper
