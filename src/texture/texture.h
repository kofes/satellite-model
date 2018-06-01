#pragma once

#include <memory>
#include "SOIL/SOIL.h"

class texture {
public:
    texture(const std::string& src = "");
    ~texture();

    texture& exec(const std::string& src);

    const unsigned char* img() const;

    int width() const;
    int height() const;
private:
    class core;

    std::shared_ptr<core> m_core;
};
