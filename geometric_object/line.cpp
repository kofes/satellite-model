#include <mvp/actions/actions.h>
#include "line.h"

namespace geometry {
class line::core {
public:
    core() = default;

    float vertices[3 * 2];

    linear_algebra::Vector position = {0, 0, 0, 1};
    linear_algebra::Vector scale = {1, 1, 1, 1};
    linear_algebra::Matrix rotate = linear_algebra::Matrix::eye(4, 1);

    float ambient[3] = {1, 1, 1};
};

line::line(
        const linear_algebra::Vector& A,
        const linear_algebra::Vector& B
) : m_core(new line::core) {
    for (size_t i = 0; i < 3; ++i) {
        m_core->vertices[i + 0] = A[i];
        m_core->vertices[i + 3] = B[i];
    }
}

line& line::color(uint8_t r, uint8_t g, uint8_t b) {
    float rgb[] = {r / 255.f, g / 255.f, b / 255.f};
    for (uint8_t i = 0; i < 3; ++i)
        m_core->ambient[i] *= rgb[i];
    return *this;
}

line& line::scale(const linear_algebra::Vector& scale) {
    m_core->scale = mvp::action::scale(scale) * m_core->scale;
    return *this;
}
line& line::translate(const linear_algebra::Vector& move) {
    m_core->position = mvp::action::translate(move) * m_core->position;
    return *this;
}
line& line::rotate(const linear_algebra::Vector& axis, double degree) {
    m_core->rotate = mvp::action::rotate(axis, degree) * m_core->rotate;
    return *this;
}

line& line::render(const linear_algebra::Matrix& vp) {
    glBindVertexArray(m_vao);

    glDeleteBuffers(1, &vbo["vertex"]);
    glGenBuffers(1, &vbo["vertex"]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo["vertex"]);
    glBufferData(GL_ARRAY_BUFFER, (3 * 2) * sizeof(float), m_core->vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attr["vertex"], 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attr["vertex"]);
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

    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    return *this;
}
}