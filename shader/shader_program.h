#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include <memory>
#include <functional>

#include "shader.h"
#include "shader_error.h"

#define SHADER_PROGRAM_LOG

#ifdef SHADER_PROGRAM_LOG
#include <iostream>
#define SHADER_PROGRAM_LOGout (std::cout << "SHADER PROGRAM: ")
#else
#define SHADER_PROGRAM_LOGout (std::ostream(0))
#endif

namespace glsl {
namespace shader {
enum class field {
    attribute,
    uniform
};

typedef std::function<void(GLint id)> field_handler;

class program {
public:
    program();

    program& add(const shader& src);

    program& link();

    GLuint id() const;

    GLint get(const std::string& name, field type);

    program& operator()(const std::string& name, field type, field_handler handler);
private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}