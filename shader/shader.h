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
    shader(GLuint id);
    shader(GLuint id, std::ifstream& src);
    shader(GLuint id, const std::string& src);
    ~shader();

    shader& exec(std::ifstream& src);
    shader& exec(const std::string& src);

    shader& compile();

    GLuint id();
    GLuint type();
private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}