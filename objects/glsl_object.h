#pragma once

#include <GL/gl.h>
#include <inc/Matrix.h>
#include <map>
#include <GL/glew.h>
#include "objects.h"

namespace glsl {
class object: public draw::object {
public:
    object();
    virtual ~object();

    object& vertex(GLuint index);
    object& model(GLuint index);
    object& mvp(GLuint index);
    object& normal(GLuint index);
    object& tangent(GLuint index);
    object& bitangent(GLuint index);
    object& texture(GLuint index);
    object& normal_sampler(GLuint index);
    object& diffuse_sampler(GLuint index);
    object& specular_sampler(GLuint index);
    object& material_ambient(GLuint index);
    object& material_diffuse(GLuint index);
    object& material_specular(GLuint index);
    object& material_emission(GLuint index);
    object& material_shininess(GLuint index);
    object& sampler_selector(GLuint index);

    virtual object& show_normals(const linear_algebra::Matrix& vp);
    virtual object& render(const linear_algebra::Matrix& vp);

    GLuint vao() const;
protected:
    std::map<std::string, GLuint> m_vbo;
    std::map<std::string, GLuint> m_attr;

    GLuint m_vao;
};
}
