#pragma once

#include "GL/glew.h"
#include <map>
#include <LinearAlgebra.h>

namespace geometry {
class object {
public:
    object() {
        glGenVertexArrays(1, &m_vao);
    }
    virtual ~object() {
        glDeleteVertexArrays(1, &m_vao);
    }

    object& vertex(GLuint index) {
      attr["vertex"] = index;
      return *this;
    }
    object& model(GLuint index) {
      attr["model"] = index;
      return *this;
    }
    object& mvp(GLuint index) {
      attr["mvp"] = index;
      return *this;
    }
    object& normal(GLuint index) {
      attr["normal"] = index;
      return *this;
    }
    object& tangent(GLuint index) {
      attr["tangent"] = index;
      return *this;
    }
    object& bitangent(GLuint index) {
      attr["bitangent"] = index;
      return *this;
    }
    object& texture(GLuint index) {
      attr["texture"] = index;
      return *this;
    }
    object& normal_sampler(GLuint index) {
      attr["normal_sampler"] = index;
      return *this;
    }
    object& diffuse_sampler(GLuint index) {
      attr["diffuse_sampler"] = index;
      return *this;
    }
    object& specular_sampler(GLuint index) {
      attr["specular_sampler"] = index;
      return *this;
    }
    object& material_ambient(GLuint index) {
      attr["material_ambient"] = index;
      return *this;
    }
    object& material_diffuse(GLuint index) {
      attr["material_diffuse"] = index;
      return *this;
    }
    object& material_specular(GLuint index) {
      attr["material_specular"] = index;
      return *this;
    }
    object& material_emission(GLuint index) {
      attr["material_emission"] = index;
      return *this;
    }
    object& material_shininess(GLuint index) {
      attr["material_shininess"] = index;
      return *this;
    }
    object& sampler_selector(GLuint index) {
        attr["sampler_selector"] = index;
        return *this;
    }

    virtual object& color(uint8_t r, uint8_t g, uint8_t b) {return *this;}

    virtual object& show_normals(const linear_algebra::Matrix& vp) {return *this;}

    virtual object& recompute() {return *this;}

    virtual object& render(const linear_algebra::Matrix& vp) {return *this;}

    GLuint vao() const {
        return m_vao;
    }
protected:
    std::map<std::string, GLuint> vbo;
    std::map<std::string, GLuint> attr = {
        {"vertex", GL_MAX_VERTEX_ATTRIBS},
        {"model", GL_MAX_VERTEX_ATTRIBS},
        {"mvp", GL_MAX_VERTEX_ATTRIBS},
        {"normal", GL_MAX_VERTEX_ATTRIBS},
        {"tangent", GL_MAX_VERTEX_ATTRIBS},
        {"bitangent", GL_MAX_VERTEX_ATTRIBS},
        {"texture", GL_MAX_VERTEX_ATTRIBS},
        {"normal_sampler", GL_MAX_VERTEX_ATTRIBS},
        {"diffuse_sampler", GL_MAX_VERTEX_ATTRIBS},
        {"specular_sampler", GL_MAX_VERTEX_ATTRIBS},
        {"material_ambient", GL_MAX_VERTEX_ATTRIBS},
        {"material_diffuse", GL_MAX_VERTEX_ATTRIBS},
        {"material_specular", GL_MAX_VERTEX_ATTRIBS},
        {"material_emission", GL_MAX_VERTEX_ATTRIBS},
        {"material_shininess", GL_MAX_VERTEX_ATTRIBS}
    };

    GLuint m_vao;
};
}
