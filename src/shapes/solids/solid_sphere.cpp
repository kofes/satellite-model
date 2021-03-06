#include <mvp/actions/actions.h>
#include "shapes/shape_line.h"
#include "solid_sphere.h"

namespace shape {
namespace solid {

sphere::sphere(size_t slices, size_t stacks, float radius): m_radius(radius) {
    std::vector<float>& vertices = m_vertices;
    std::vector<float>& normals = m_normals;
    std::vector<float>& texture = m_texture;

    for(size_t i = 0; i <= stacks; ++i) {
        float V = i / (float)stacks;
        float phi = V * M_PI;
        for (size_t j = 0; j <= slices; ++j) {
            float U = j / (float)slices;
            float theta = U * 2 * M_PI;

            float X = std::sin(phi) * std::cos(theta);
            float Z = std::sin(phi) * std::sin(theta);
            float Y = std::cos(phi);

            vertices.push_back(X * radius);
            vertices.push_back(Y * radius);
            vertices.push_back(Z * radius);

            normals.push_back(X);
            normals.push_back(Y);
            normals.push_back(Z);

            texture.push_back(U);
            texture.push_back(V);
        }
    }

    std::vector<GLuint>& indices = m_indices;

    for (size_t i = 0; i < slices * (stacks+1); ++i) {
        indices.push_back(i);
        indices.push_back(i + slices + 1);
        indices.push_back(i + slices);

        indices.push_back(i + slices + 1);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
}

sphere& sphere::update_color(const helper::color& color) {
    glsl::object::update_color(color);
    float buff[3];
    std::tie(buff[0], buff[1], buff[2]) = color.f();
    for (uint8_t i = 0; i < 3; ++i) {
        m_material_ambient[i] *= color[i];
        m_material_diffuse[i] *= color[i];
        m_material_specular[i] *= color[i];
        m_material_emission[i] *= color[i];
    }
    return *this;
}

sphere& sphere::show_normals(const linear_algebra::Matrix& vp) {
    linear_algebra::Matrix model =
            m_orientation *
            mvp::action::scale(m_scale * m_radius * 1.5) *
            mvp::action::translate(m_position).T();
    for (uint8_t i = 0; i < 3; ++i) {
        shape::line line = shape::line({
                m_position[0]/2,
                m_position[1]/2,
                m_position[2]/2
        }, linear_algebra::Vector {
                model[0][i] + m_position[0]/2,
                model[1][i] + m_position[1]/2,
                model[2][i] + m_position[2]/2,
        });
        line.update_color(helper::color(
                (i == 0) * 255, // x - red
                (i == 1) * 255, // y - green
                (i == 2) * 255  // z - blue
                ))
            .vertex(m_attr["vertex"])
            .model(m_attr["model"])
            .mvp(m_attr["mvp"])
            .sampler_selector(m_attr["sampler_selector"])
            .material_ambient(m_attr["material_ambient"])
            .material_diffuse(m_attr["material_diffuse"])
            .material_specular(m_attr["material_specular"])
            .material_emission(m_attr["material_emission"])
            .material_shininess(m_attr["material_shininess"]);
        line.render(vp);
    }
    return *this;
}

sphere &sphere::render(const linear_algebra::Matrix &vp) {
    const std::vector<float> &vertices = m_vertices;
    const std::vector<float> &normals = m_normals;
    const std::vector<float> &texture = m_texture;

    const float &radius = m_radius;

    const std::vector<GLuint>& indices = m_indices;

    glBindVertexArray(m_vao);

    glDeleteBuffers(1, &m_vbo["vertex"]);
    glGenBuffers(1, &m_vbo["vertex"]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo["vertex"]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_attr["vertex"], 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_attr["vertex"]);

    glDeleteBuffers(1, &m_vbo["index"]);
    glGenBuffers(1, &m_vbo["index"]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo["index"]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    float red, green, blue;

    if (m_attr["normal"] != GL_MAX_VERTEX_ATTRIBS) {
        glDeleteBuffers(1, &m_vbo["normal"]);
        glGenBuffers(1, &m_vbo["normal"]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo["normal"]);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(m_attr["normal"], 3, GL_FLOAT, GL_TRUE, 0, nullptr);
        glEnableVertexAttribArray(m_attr["normal"]);

        std::tie(red, green, blue) = m_material_diffuse.f();
        glUniform3f(m_attr["material_diffuse"], red, green, blue);

        std::tie(red, green, blue) = m_material_specular.f();
        glUniform3f(m_attr["material_specular"], red, green, blue);
    }

    if (m_attr["texture"] != GL_MAX_VERTEX_ATTRIBS) {
        glDeleteBuffers(1, &m_vbo["texture"]);
        glGenBuffers(1, &m_vbo["texture"]);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo["texture"]);
        glBufferData(GL_ARRAY_BUFFER, texture.size() * sizeof(float), texture.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(m_attr["texture"], 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(m_attr["texture"]);
        glUniform1i(m_attr["sampler_selector"], true);
    } else
        glUniform1i(m_attr["sampler_selector"], false);

 	linear_algebra::Matrix model =
            m_orientation *
            mvp::action::scale(m_scale * m_radius) *
            mvp::action::translate(m_position).T();

    linear_algebra::Matrix mvp = model * vp;

    float flat_matrix[16] = {0};

 	for (size_t i = 0; i < 4; ++i)
 		for (size_t j = 0; j < 4; ++j)
            flat_matrix[i*4 + j] = mvp[i][j];
 	glUniformMatrix4fv(m_attr["mvp"], 1, GL_FALSE, flat_matrix);

 	for (size_t i = 0; i < 4; ++i)
 		for (size_t j = 0; j < 4; ++j)
            flat_matrix[i*4 + j] = model[i][j];
 	glUniformMatrix4fv(m_attr["model"], 1, GL_FALSE, flat_matrix);

    std::tie(red, green, blue) = m_material_ambient.f();
    glUniform3f(m_attr["material_ambient"], red, green, blue);
    std::tie(red, green, blue) = m_material_emission.f();
    glUniform3f(m_attr["material_emission"], red, green, blue);
    glUniform1f(m_attr["material_shininess"], m_material_shininess);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
//    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return *this;
}
}
}