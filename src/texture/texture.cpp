#include "texture.h"

class texture::core {
public:
    core() = default;

    unsigned char* image = nullptr;
    int width = 0, height = 0;
};

texture::texture(const std::string& src): m_core(new texture::core) {
    m_core->image = SOIL_load_image(
            src.c_str(),
            &m_core->width,
            &m_core->height,
            nullptr,
            SOIL_LOAD_RGBA
    );
}

texture::~texture() {
    SOIL_free_image_data(m_core->image);
}

texture& texture::exec(const std::string& src) {
    SOIL_free_image_data(m_core->image);
    m_core->image = SOIL_load_image(
            src.c_str(),
            &m_core->width,
            &m_core->height,
            nullptr,
            SOIL_LOAD_RGBA
    );
}

const unsigned char* texture::img() const {
    return m_core->image;
}

int texture::width() const {
    return m_core->width;
}

int texture::height() const {
    return m_core->height;
}
