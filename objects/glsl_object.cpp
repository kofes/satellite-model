#include <GL/glew.h>
#include "glsl_object.h"

namespace glsl {
object::object():
    m_attr({
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
    })
{
    glGenVertexArrays(1, &m_vao);
}

object::~object() {
    glDeleteVertexArrays(1, &m_vao);
}

object &object::vertex(GLuint index) {
    m_attr["vertex"] = index;
    return *this;
}

object &object::model(GLuint index) {
    m_attr["model"] = index;
    return *this;
}

object &object::mvp(GLuint index) {
    m_attr["mvp"] = index;
    return *this;
}

object &object::normal(GLuint index) {
    m_attr["normal"] = index;
    return *this;
}

object &object::tangent(GLuint index) {
    m_attr["tangent"] = index;
    return *this;
}

object &object::bitangent(GLuint index) {
    m_attr["bitangent"] = index;
    return *this;
}

object &object::texture(GLuint index) {
    m_attr["texture"] = index;
    return *this;
}

object &object::normal_sampler(GLuint index) {
    m_attr["normal_sampler"] = index;
    return *this;
}

object &object::diffuse_sampler(GLuint index) {
    m_attr["diffuse_sampler"] = index;
    return *this;
}

object &object::specular_sampler(GLuint index) {
    m_attr["specular_sampler"] = index;
    return *this;
}

object &object::material_ambient(GLuint index) {
    m_attr["material_ambient"] = index;
    return *this;
}

object &object::material_diffuse(GLuint index) {
    m_attr["material_diffuse"] = index;
    return *this;
}

object &object::material_specular(GLuint index) {
    m_attr["material_specular"] = index;
    return *this;
}

object &object::material_emission(GLuint index) {
    m_attr["material_emission"] = index;
    return *this;
}

object &object::material_shininess(GLuint index) {
    m_attr["material_shininess"] = index;
    return *this;
}

object &object::sampler_selector(GLuint index) {
    m_attr["sampler_selector"] = index;
    return *this;
}

virtual object &object::show_normals(const linear_algebra::Matrix &vp) { return *this; }

virtual object &object::render(const linear_algebra::Matrix &vp) { return *this; }

GLuint object::vao() const {
    return m_vao;
}
}