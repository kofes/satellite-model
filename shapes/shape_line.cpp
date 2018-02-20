#include <mvp/actions/actions.h>
#include <GL/glew.h>
#include "shape_line.h"

namespace shape {
class line::core {
public:
    core() = default;

    float vertices[3 * 2];
};

line::line(
        const linear_algebra::Vector& A,
        const linear_algebra::Vector& B
) : m_core(new line::core) {
    for (size_t i = 0; i < 3; ++i) {
        m_core->vertices[i + 0] = A[i];
        m_core->vertices[i + 3] = B[i];
        m_position[i] = A[i];
    }

    linear_algebra::Vector buff = (B - A).normalize();
    rotate({1,0,0}, buff[0]);
    rotate({0,1,0}, buff[1]);
    rotate({0,0,1}, buff[2]);
}

line& line::update_color(const helper::color &color) {
    glsl::object::update_color(color);
    float buff[3];
    std::tie(buff[0], buff[1], buff[2]) = color.f();
    for (uint8_t i = 0; i < 3; ++i)
        m_material_ambient[i] *= buff[i];
    return *this;
}

line& line::render(const linear_algebra::Matrix& vp) {
    glBindVertexArray(m_vao);

    glDeleteBuffers(1, &m_vbo["vertex"]);
    glGenBuffers(1, &m_vbo["vertex"]);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo["vertex"]);
    glBufferData(GL_ARRAY_BUFFER, (3 * 2) * sizeof(float), m_core->vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(m_attr["vertex"], 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_attr["vertex"]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUniform1i(m_attr["sampler_selector"], false);

    linear_algebra::Matrix model =
            m_orientation *
            mvp::action::scale(m_scale) *
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

    float red, green, blue;
    std::tie(red, green, blue) = m_material_ambient.f();
    glUniform3f(m_attr["material_ambient"], red, green, blue);

    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    return *this;
}
}