#include <mvp/actions/actions.h>
#include "solid_sphere.h"

namespace geometry {
namespace solid {
class sphere::core {
public:
    core() = default;

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> texture;
    std::vector<GLuint> indices;

    size_t slices;
    size_t stacks;
    float radius;

    linear_algebra::Vector position = {0.5,0.25,0.5,1};
};

sphere::sphere(float radius, size_t slices, size_t stacks): m_core(new sphere::core) {
    m_core->slices = slices;
    m_core->stacks = stacks;
    m_core->radius = radius;

    std::vector<float>& positions = m_core->positions;
    std::vector<float>& normals = m_core->normals;
    std::vector<float>& texture = m_core->texture;

    for(size_t i = 0; i <= stacks; ++i) {
        float V = i / (float)stacks;
        float phi = V * M_PI;
        for (size_t j = 0; j <= slices; ++j) {
            float U = j / (float)slices;
            float theta = U * 2 * M_PI;

            float X = std::sin(phi) * std::cos(theta);
            float Z = std::sin(phi) * std::sin(theta);
            float Y = std::cos(phi);

            positions.push_back(X * radius);
            positions.push_back(Y * radius);
            positions.push_back(Z * radius);

            normals.push_back(X);
            normals.push_back(Y);
            normals.push_back(Z);

            texture.push_back(U);
            texture.push_back(V);
        }
    }

    std::vector<GLuint>& indices = m_core->indices;

    for (size_t i = 0; i < slices * (stacks+1); ++i) {
        indices.push_back(i);
        indices.push_back(i + slices + 1);
        indices.push_back(i + slices);

        indices.push_back(i + slices + 1);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
}

sphere &sphere::render(const linear_algebra::Matrix &vp) {
    const std::vector<float> &positions = m_core->positions;
    const std::vector<float> &normals = m_core->normals;
    const std::vector<float> &texture = m_core->texture;

    const float &radius = m_core->radius;

    const std::vector<GLuint>& indices = m_core->indices;

    glBindVertexArray(m_vao);

    glDeleteBuffers(1, &vbo["vertex"]);
    glGenBuffers(1, &vbo["vertex"]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo["vertex"]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(attr["vertex"], 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attr["vertex"]);

    glDeleteBuffers(1, &vbo["index"]);
    glGenBuffers(1, &vbo["index"]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo["index"]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    if (attr["normal"] != GL_MAX_VERTEX_ATTRIBS) {
        glDeleteBuffers(1, &vbo["normal"]);
        glGenBuffers(1, &vbo["normal"]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo["normal"]);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(attr["normal"], 3, GL_FLOAT, GL_TRUE, 0, nullptr);
        glEnableVertexAttribArray(attr["normal"]);
        glUniform3f(attr["material_diffuse"], 0.5, 0.5, 0.5);
        glUniform3f(attr["material_specular"], 1.f, 1.f, 1.f);
    }

    if (attr["texture"] != GL_MAX_VERTEX_ATTRIBS) {
        glDeleteBuffers(1, &vbo["texture"]);
        glGenBuffers(1, &vbo["texture"]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo["texture"]);
        glBufferData(GL_ARRAY_BUFFER, texture.size() * sizeof(float), texture.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(attr["texture"], 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(attr["texture"]);
        glUniform1i(attr["sampler_selector"], true);
    } else
        glUniform1i(attr["sampler_selector"], false);

 	linear_algebra::Matrix model =
            mvp::action::scale({m_core->radius, m_core->radius, m_core->radius}) *
            mvp::action::translate(m_core->position).T();
    linear_algebra::Matrix mvp = model * vp;

    std::cout << model << std::endl;
    std::cout << mvp << std::endl;

    float flat_matrix[16] = {0};

 	for (size_t i = 0; i < 4; ++i)
 		for (size_t j = 0; j < 4; ++j)
            flat_matrix[i*4 + j] = mvp[i][j];
 	glUniformMatrix4fv(attr["mvp"], 1, GL_FALSE, flat_matrix);

 	for (size_t i = 0; i < 4; ++i)
 		for (size_t j = 0; j < 4; ++j)
            flat_matrix[i*4 + j] = model[i][j];
 	glUniformMatrix4fv(attr["model"], 1, GL_FALSE, flat_matrix);

    glUniform3f(attr["material_ambient"], 1, 1, 1);
    glUniform3f(attr["material_emission"], .5, .5, .5);
    glUniform1f(attr["material_shininess"], 3.f);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
//    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
}
}