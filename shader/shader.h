#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include <fstream>
#include <memory>

#include "shader_error.h"

namespace glsl {
namespace shader {
class shader {
public:
    explicit shader(GLenum type);
    explicit shader(GLenum type, std::ifstream& src);
    explicit shader(GLenum type, const std::string& src);
    ~shader();

    shader& exec(std::ifstream& src);
    shader& exec(const std::string& src);

    shader& compile();

    GLuint id() const;
    GLuint type() const;
private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}