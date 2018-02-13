#include "grid.h"

namespace geometry {
class grid::core {
public:
    core() = default;

    std::size_t count = 0;
    float vertex[6] = {
            -1.f, 0.f, -1.f,
            -1.f, 0.f, +1.f
    };
};

grid::grid(std::size_t count):
        object(),
        m_core(new grid::core) {
    m_core->count = count;
    float bound = (count - (count % 2 == 1)) / 20.f;
    m_core->vertex[0] *= bound / m_core->vertex[5];
    m_core->vertex[2] *= bound / m_core->vertex[5];
    m_core->vertex[3] *= bound / m_core->vertex[5];
    m_core->vertex[5] *= bound / m_core->vertex[5];
}

grid& grid::count(std::size_t count) {
    m_core->count = count;
    float bound = (count - (count % 2 == 1)) / 20.f;
    m_core->vertex[0] *= bound / m_core->vertex[5];
    m_core->vertex[2] *= bound / m_core->vertex[5];
    m_core->vertex[3] *= bound / m_core->vertex[5];
    m_core->vertex[5] *= bound / m_core->vertex[5];
    return *this;
}

grid& grid::render(const linear_algebra::Matrix& vp) {
    if (m_core->count == 0) return *this;
    const std::size_t& count = m_core->count;
    float buffer[2 * 3 * count * 2];

    const float* vertex = m_core->vertex;
    for (std::size_t i = 0; i < count; ++i) {
        buffer[i * 6 + 0] = vertex[0] + .1f * i;
        buffer[i * 6 + 1] = vertex[1];
        buffer[i * 6 + 2] = vertex[2] - 1.f;
        buffer[i * 6 + 3] = vertex[3] + .1f * i;
        buffer[i * 6 + 4] = vertex[4];
        buffer[i * 6 + 5] = vertex[5] - 1.f;

        buffer[(count + i) * 6 + 0] = vertex[0];
        buffer[(count + i) * 6 + 1] = vertex[1];
        buffer[(count + i) * 6 + 2] = vertex[2] - 1.f + .1f * i;
        buffer[(count + i) * 6 + 3] = - vertex[3];
        buffer[(count + i) * 6 + 4] = vertex[4];
        buffer[(count + i) * 6 + 5] = - vertex[5] - 1.f + .1f * i;
    }

    glBindVertexArray(m_vao);

    glDeleteBuffers(1, &vbo["vertex"]);

    glGenBuffers(1, &vbo["vertex"]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo["vertex"]);

    glBufferData(GL_ARRAY_BUFFER, (2 * 3 * count * 2) * sizeof(float), buffer, GL_STATIC_DRAW);
    glVertexAttribPointer(attr["vertex"], 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attr["vertex"]);

    float flat_matrix[16] = {0};

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            flat_matrix[i*4 + j] = vp[i][j];
    glUniformMatrix4fv(attr["mvp"], 1, GL_FALSE, flat_matrix);

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            flat_matrix[i*4 + j] = i == j;
    glUniformMatrix4fv(attr["model"], 1, GL_FALSE, flat_matrix);

    glUniform1i(attr["sampler_selector"], true);
    glUniform3f(attr["material_ambient"], 0, .5, .7);
    glUniform3f(attr["material_diffuse"], 0.5, 0.5, 0.5);
    glUniform3f(attr["material_specular"], 1.f, 1.f, 1.f);
    glUniform3f(attr["material_emission"], .5, .5, .5);
    glUniform1f(attr["material_shininess"], 3.f);

    glDrawArrays(GL_LINES, 0, 3 * 2 * count * 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return *this;
}
}
