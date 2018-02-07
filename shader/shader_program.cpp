#include "shader_program.h"

namespace glsl {
namespace shader {
class program::core {
public:
    core() = default;
    core(GLuint id) : id(id) {};

    GLuint id;
    size_t *ptr_counter;
};

program::program(): m_core(new program::core) {
    m_core->id = glCreateProgram();
    m_core->ptr_counter = new size_t;
}

program::program(const program &src): m_core(new program::core) {
    m_core->id = src.id();
    m_core->ptr_counter = src.m_core->ptr_counter;
    ++*m_core->ptr_counter;
}

program &program::operator=(const program &src) {
    if (!*m_core->ptr_counter) {
        glDeleteProgram(m_core->id);
        delete m_core->ptr_counter;
    } else --*m_core->ptr_counter;

    m_core->id = src.id();
    m_core->ptr_counter = src.m_core->ptr_counter;
    ++*m_core->ptr_counter;

    return *this;
}

program::~program() {
    if (!*m_core->ptr_counter) {
        glDeleteProgram(m_core->id);
        delete m_core->ptr_counter;
    } else --*m_core->ptr_counter;
}

program &program::add(const shader &src) {
    glAttachShader(m_core->id, src.id());

    return *this;
}

program &program::link() {
    glLinkProgram(m_core->id);

    GLint error_code;
    glGetProgramiv(m_core->id, GL_LINK_STATUS, &error_code);
    if (error_code) {
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
    switch (type) {
        case field::attribute: return glGetAttribLocation(m_core->id, name.c_str());
        case field::uniform: return glGetUniformLocation(m_core->id, name.c_str());
    }
}

program &program::operator()(const std::string &name, field type, field_handler handler) {
    handler(get(name, type));
    return *this;
}
}
}