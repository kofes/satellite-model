#pragma once

#include <LinearAlgebra.h>
#include <map>
#include <GL/glew.h>
#include "draw_object.h"

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

    object& update_material_ambient(const helper::color& color);
    object& update_material_diffuse(const helper::color& color);
    object& update_material_specular(const helper::color& color);
    object& update_material_emission(const helper::color& color);
    object& update_material_shininess(double degree);

    object& material_ambient(helper::color& color);
    object& material_diffuse(helper::color& color);
    object& material_specular(helper::color& color);
    object& material_emission(helper::color& color);
    object& material_shininess(double& degree);

    virtual object& show_normals(const linear_algebra::Matrix& vp);
    virtual object& render(const linear_algebra::Matrix& vp);

    GLuint vao() const;
protected:
    std::map<std::string, GLuint> m_vbo;
    std::map<std::string, GLuint> m_attr;

    helper::color m_material_ambient;
    helper::color m_material_diffuse;
    helper::color m_material_specular;
    helper::color m_material_emission;
    double m_material_shininess;

    GLuint m_vao;
};
}
