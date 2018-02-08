#pragma once

#include <cstdlib>
#include <memory>

#include "geometric_object.h"

namespace geometry {
class grid : public object {
public:
    grid(std::size_t count = 0);

    grid& count(std::size_t count);

    grid& recompute() override;

    grid& render() override;

    grid& vertex(GLuint) override;
    grid& normal(GLuint) override;
    grid& tangent(GLuint) override;

    grid& bitangent(GLuint) override;

    object& color(GLuint) override;
    grid& texture(GLuint) override;
private:
    class core;

    std::shared_ptr<core> m_core;
};
}