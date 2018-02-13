#include "shader_program.h"

namespace glsl {
namespace shader {
class program::core {
public:
    core() = default;
    core(GLuint id) : id(id) {};

    GLuint id;
};

program::program(): m_core(new program::core) {
    m_core->id = glCreateProgram();
}

program &program::add(const shader &src) {
    glAttachShader(m_core->id, src.id());
    return *this;
}

program &program::link() {
    glLinkProgram(m_core->id);

    GLint error_code;
    glGetProgramiv(m_core->id, GL_LINK_STATUS, &error_code);
    if (error_code == GL_FALSE) {
        GLsizei len;
        glGetProgramiv(m_core->id, GL_INFO_LOG_LENGTH, &len);
        char error_msg[len];
        glGetProgramInfoLog(m_core->id, len, NULL, error_msg);
        throw (error(error_code, error_msg));
    }

    return *this;
}

GLuint program::id() const {
    return m_core->id;
}

GLint program::get(const std::string &name, field type) {
    GLint id;
    switch (type) {
        case field::attribute: id = glGetAttribLocation(m_core->id, name.c_str()); break;
        case field::uniform: id = glGetUniformLocation(m_core->id, name.c_str()); break;
    }
    if (id == -1)
        SHADER_PROGRAM_LOGout << "bad shader field name " << name << '\n';
    return id;
}

program& program::link(const std::string& name, field type, GLuint& id) {
    id = get(name, type);
    return *this;
}
}
}