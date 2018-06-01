#include "args_serializer.h"

namespace helper {
namespace serialize {
map& helper::serialize::map::handle(const std::string &keyword, callback handler) {
    auto key_iter = find(keyword);
    bool keyword_exits = key_iter != end();
    if (!keyword_exits)
        handler(serialize::values(), "keyword " + keyword + " not exists");
    else
        handler(key_iter->second, "");
    return *this;
}

bool map::has(const std::string &keyword) {
    return find(keyword) != end();
}

map args(int argc, char *argv[], map &&smap) {
    serial_dbg << "argc = " << argc << "; argv = [" << argv[0];
    for (size_t i = 1; i < argc; ++i)
        serial_out << ", " << argv[i];
    serial_out << ']' << std::endl;

    smap.clear();
    for (size_t i = 1; i < argc; ++i) {
        std::string argue = argv[i];

        size_t eqpos = argue.find('=');
        if (eqpos == std::string::npos) {
            smap[argue].emplace_front("true");
            continue;
        }
        if (argue.length() == eqpos + 1) continue;

        std::string key = argue.substr(0, eqpos);
        std::string value = argue.substr(eqpos + 1);

        smap[key].emplace_front(value);

        serial_out << "{key: " << key << ", value: " << value << "}" << std::endl;

    }

    return std::move(smap);
}
}
}