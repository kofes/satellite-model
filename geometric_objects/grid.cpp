#include "grid.h"

namespace geometry {
class grid::core {
public:
    core() = default;

    std::size_t count = 0;
    float vertex[6] = {0};
};

grid::grid(std::size_t count):
        object(),
        m_core(new grid::core) {
    m_core->count = count;
    float bound = (count - (count % 2 == 1)) / 20.f;
    m_core->vertex[0] =
    m_core->vertex[2] =
    m_core->vertex[3] =
            - (m_core->vertex[5] = bound);
}

grid& grid::count(std::size_t count) {
    m_core->count = count;
    float bound = (count - (count % 2 == 1)) / 20.f;
    m_core->vertex[0] =
    m_core->vertex[2] =
    m_core->vertex[3] =
            - (m_core->vertex[5] = bound);
    return *this;
}

grid& grid::recompute() {
    return *this;
}

grid& grid::render() {
    if (m_core->count == 0) return *this;
    float buffer[2 * 3* (m_core->count * 2)];

    const float* vertex = m_core->vertex;
    const std::size_t& count = m_core->count;
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
        buffer[(count + i) * 6 + 5] = vertex[5] - 1.f + .1f * i;
    }

    glBindVertexArray(m_vao);

    glDeleteBuffers(1, &vbo["vertex"]);

    glGenBuffers(1, &vbo["vertex"]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo["vertex"]);

    glBufferData(GL_ARRAY_BUFFER, (2 * 3 * count * 2) * sizeof(float), buffer, GL_STATIC_DRAW);
    glVertexAttribPointer(attr["vertex"], 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(attr["vertex"]);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    return *this;
}

grid& grid::vertex(GLuint index) {
    attr["vertex"] = index;
    return *this;
}

grid& grid::normal(GLuint) {
    return *this;
}

grid& grid::tangent(GLuint) {
    return *this;
}

grid& grid::bitangent(GLuint) {
    return *this;
}

grid& grid::texture(GLuint) {
    return *this;
}
}
