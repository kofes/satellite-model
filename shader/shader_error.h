#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"

#include <exception>

namespace shader {
class error {
public:
    error(GLint code, const std::string& what):
            m_code(code), m_what(what) {};
    virtual const char* what() const throw() {
        return m_what.c_str();
    }
    virtual GLint code() const throw() {
        return m_code;
    }
protected:
    GLint m_code;
    std::string m_what;
};
}