#pragma once

#include "GL/glew.h"
#include <unordered_map>

namespace geometry {
class object {
public:
    object() {
        glGenVertexArrays(1, &m_vao);
    }

    virtual ~object() {
        glDeleteVertexArrays(1, &m_vao);
    }

    virtual object& recompute() = 0;

    virtual object& render() = 0;

    virtual object& vertex(GLuint) = 0;
    virtual object& normal(GLuint) = 0;
    virtual object& tangent(GLuint) = 0;

    virtual object& bitangent(GLuint) = 0;

    virtual object& texture(GLuint) = 0;

    virtual GLuint vao() const {
        return m_vao;
    }
protected:
    std::unordered_map<std::string, GLuint> vbo = {
            {"index",     0},
            {"vertex",    0},
            {"normal",    0},
            {"tangent",   0},
            {"bitangent", 0},
            {"texture",   0}
    };
    std::unordered_map<std::string, GLuint> attr = {
            {"vertex",    GL_MAX_VERTEX_ATTRIBS},
            {"normal",    GL_MAX_VERTEX_ATTRIBS},
            {"tangent",   GL_MAX_VERTEX_ATTRIBS},
            {"bitangent", GL_MAX_VERTEX_ATTRIBS},
            {"texture",   GL_MAX_VERTEX_ATTRIBS}
    };

    GLuint m_vao;
};
}