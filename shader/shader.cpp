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

shader::shader(GLenum type, std::ifstream&& src): m_core(new shader::core(type)) {
    m_core->id = glCreateShader(type);
    if (!src.is_open()) {
        SHADER_LOGout << "file not opened!" << '\n';
        throw (error((GLint)shader::ErrorCode::FILE_NOT_OPEN, "file not opened"));
    }
    std::stringstream sstream;
    {
        std::string buff;
        while (getline(src, buff))
            sstream << buff << '\n';
    }

    SHADER_LOGout << '\n' << sstream.str() << '\n';

    std::string str = sstream.str();
    const char* source = str.c_str();
    GLint len = str.length();
    glShaderSource(m_core->id, 1, &source, &len);
}

shader::shader(GLenum type, const std::string& src): m_core(new shader::core(type)) {
    m_core->id = glCreateShader(type);
    const char* source = src.c_str();
    GLint len = src.length();
    glShaderSource(m_core->id, 1, &source, &len);
}

shader::~shader() {
    glDeleteShader(m_core->id);
}

shader& shader::exec(std::ifstream&& src) {
    if (!src.is_open())
        SHADER_LOGout << "file not open!" << '\n';

    std::stringstream sstream;
    {
        std::string buff;
        while (getline(src, buff))
            sstream << buff << '\n';
    }

    SHADER_LOGout << '\n' << sstream.str() << '\n';

    std::string str = sstream.str();
    const char* source = str.c_str();
    GLint len = str.length();
    glShaderSource(m_core->id, 1, &source, &len);
    return *this;
}

shader& shader::exec(const std::string& src) {
    const char* source = src.c_str();
    GLint len = src.length();
    glShaderSource(m_core->id, 1, &source, &len);
    return *this;
}

shader& shader::compile() {
    glCompileShader(m_core->id);
    GLint error_code;
    glGetShaderiv(m_core->id, GL_COMPILE_STATUS, &error_code);
    if (error_code == GL_FALSE) {
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