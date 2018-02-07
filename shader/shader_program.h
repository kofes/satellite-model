#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include <memory>
#include <functional>

#include "shader.h"
#include "shader_error.h"

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
    program(const program& src);
    program& operator=(const program& src);
    ~program();

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