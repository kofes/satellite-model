#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include <memory>

#include "shader.h"
#include "shader_error.h"

namespace glsl {
namespace shader {
class program {
public:
    program();
    program(const program& src);
    program& operator=(const program& src);
    ~program();

    program& add(const shader& src);

    program& link();

    GLuint id();

    GLint attribute(const std::string& name);
    GLint uniform(const std::string& name);
private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}