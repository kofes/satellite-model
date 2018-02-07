#include "shader.h"
#include <sstream>

namespace glsl {
namespace shader {
class shader::core {
public:
    core() = default;
    core(GLenum type, GLuint id = 0) : type(type), id(id) {};

    GLenum type;
    GLuint id;
};

shader::shader(GLenum type) : m_core(new shader::core(type)) {
    m_core->id = glCreateShader(type);
}

shader::shader(GLenum type, std::ifstream& src): m_core(new shader::core(type)) {
    m_core->id = glCreateShader(type);
    std::stringstream sstream;
    {
        std::string buff;
        while (getline(src, buff))
            sstream << buff << '\n';
    }
    const char* source = sstream.str().c_str();
    glShaderSource(m_core->id, 1, &source, NULL);
}

shader::shader(GLenum type, const std::string& src): m_core(new shader::core(type)) {
    m_core->id = glCreateShader(type);
    const char* source = src.c_str();
    glShaderSource(m_core->id, 1, &source, NULL);
}

shader::~shader() {
    glDeleteShader(m_core->id);
}

shader& shader::exec(std::ifstream& src) {
    std::stringstream sstream;
    {
        std::string buff;
        while (getline(src, buff))
            sstream << buff << '\n';
    }
    const char* source = sstream.str().c_str();
    glShaderSource(m_core->id, 1, &source, NULL);
    return *this;
}

shader& shader::exec(const std::string& src) {
    const char* source = src.c_str();
    glShaderSource(m_core->id, 1, &source, NULL);
    return *this;
}

shader& shader::compile() {
    glCompileShader(m_core->id);
    GLint error_code;
    glGetShaderiv(m_core->id, GL_COMPILE_STATUS, &error_code);
    if (error_code) {
        GLsizei len;
        glGetShaderiv(m_core->id, GL_INFO_LOG_LENGTH, &len);
        char error_msg[len];
        glGetShaderInfoLog(m_core->id, len, NULL, error_msg);
        throw (error(error_code, error_msg));
    }
    return *this;
}
GLuint shader::id() const {
    return m_core->id;
}

GLuint shader::type() const {
    return m_core->type;
}
}
}