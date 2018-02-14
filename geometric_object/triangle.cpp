#include <mvp/actions/actions.h>
#include "triangle.h"
#include "line.h"

namespace geometry {
class triangle::core {
public:
    core() = default;

    float vertices[3 * 3];
    float normals[3 * 3];

    linear_algebra::Vector position = {0, 0, 0, 1};
    linear_algebra::Vector scale = {1, 1, 1, 1};
    linear_algebra::Matrix rotate = linear_algebra::Matrix::eye(4, 1);

    float diffuse[3] = {0.5, 0.5, 0.5};
    float specular[3] = {1.f, 1.f, 1.f};
    float ambient[3] = {1, 1, 1};
    float emission[3] = {.5, .5, .5};
    float shininess = 3.f;
};

triangle::triangle(
        const linear_algebra::Vector& A,
        const linear_algebra::Vector& B,
        const linear_algebra::Vector& C
) : m_core(new triangle::core) {
    for (size_t i = 0; i < 3; ++i) {
        m_core->vertices[i + 0] = A[i];
        m_core->vertices[i + 3] = B[i];
        m_core->vertices[i + 6] = C[i];
    }
    linear_algebra::Vector normA = linear_algebra::vectorMultiply({B - A, C - A}).normalize();
    linear_algebra::Vector normB = linear_algebra::vectorMultiply({C - B, A - B}).normalize();
    linear_algebra::Vector normC = linear_algebra::vectorMultiply({A - C, B - C}).normalize();
    for (size_t i = 0; i < 3; ++i) {
        m_core->normals[i + 0] = normA[i];
        m_core->normals[i + 3] = normB[i];
        m_core->normals[i + 6] = normC[i];
    }
}

triangle& triangle::scale(const linear_algebra::Vector& scale) {
    m_core->scale = mvp::action::scale(scale) * m_core->scale;
    return *this;
}
triangle& triangle::translate(const linear_algebra::Vector& move) {
    m_core->position = mvp::action::translate(move) * m_core->position;
    return *this;
}
triangle& triangle::rotate(const linear_algebra::Vector& axis, double degree) {
    m_core->rotate = mvp::action::rotate(axis, degree) * m_core->rotate;
    return *this;
}

triangle& triangle::show_normals(const linear_algebra::Matrix& vp) {
    linear_algebra::Matrix model =
            m_core->rotate.inv() *
            mvp::action::scale(m_core->scale) *
            mvp::action::translate(m_core->position).T();
    for (uint8_t i = 0; i < 3; ++i)
        geometry::line(linear_algebra::Vector {
                m_core->position[0],
                m_core->position[1],
                m_core->position[2]
        }, linear_algebra::Vector {
                model[0][i] + m_core->position[0],
                model[1][i] + m_core->position[1],
                model[2][i] + m_core->position[2],
        })
                .vertex(attr["vertex"])
                .model(attr["model"])
                .mvp(attr["mvp"])
                .sampler_selector(attr["sampler_selector"])
                .material_ambient(attr["material_ambient"])
                .material_diffuse(attr["material_diffuse"])
                .material_specular(attr["material_specular"])
                .material_emission(attr["material_emission"])
                .material_shininess(attr["material_shininess"])
                .color(
                        (i == 0)*255,
                        (i == 1)*255,
                        (i == 2)*255
                )
                .render(vp);
    return *this;
}

triangle& triangle::render(const linear_algebra::Matrix& vp) {
    glBindVertexArray(m_vao);

    glDeleteBuffers(1, &vbo["vertex"]);

    glGenBuffers(1, &vbo["vertex"]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo["vertex"]);

    glBufferData(GL_ARRAY_BUFFER, (3 * 3) * sizeof(float), m_core->vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attr["vertex"], 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attr["vertex"]);

    if (attr["normal"] != GL_MAX_VERTEX_ATTRIBS) {
        glDeleteBuffers(1, &vbo["normal"]);
        glGenBuffers(1, &vbo["normal"]);
        glBindBuffer(GL_ARRAY_BUFFER, vbo["normal"]);
        glBufferData(GL_ARRAY_BUFFER, (3 * 3) * sizeof(float), m_core->normals, GL_STATIC_DRAW);
        glVertexAttribPointer(attr["normal"], 3, GL_FLOAT, GL_TRUE, 0, nullptr);
        glEnableVertexAttribArray(attr["normal"]);
        glUniform3f(attr["material_diffuse"],
                    m_core->diffuse[0],
                    m_core->diffuse[1],
                    m_core->diffuse[2]);
        glUniform3f(attr["material_specular"],
                    m_core->specular[0],
                    m_core->specular[1],
                    m_core->specular[2]);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUniform1i(attr["sampler_selector"], false);

    linear_algebra::Matrix model =
            m_core->rotate *
            mvp::action::scale(m_core->scale) *
            mvp::action::translate(m_core->position).T();

    linear_algebra::Matrix mvp = model * vp;

    float flat_matrix[16] = {0};

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            flat_matrix[i*4 + j] = mvp[i][j];
    glUniformMatrix4fv(attr["mvp"], 1, GL_FALSE, flat_matrix);

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            flat_matrix[i*4 + j] = model[i][j];
    glUniformMatrix4fv(attr["model"], 1, GL_FALSE, flat_matrix);

    glUniform3f(attr["material_ambient"],
                m_core->ambient[0],
                m_core->ambient[1],
                m_core->ambient[2]);
    glUniform3f(attr["material_emission"],
                m_core->emission[0],
                m_core->emission[1],
                m_core->emission[2]);
    glUniform1f(attr["material_shininess"], m_core->shininess);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    return *this;
}
}