#pragma once

#include "LinearAlgebra.h"
#include <memory>

namespace mvp {
class Camera {
public:
    Camera();

    Camera(double fovy, double as, double near, double far);

    explicit operator linear_algebra::Matrix();

    Camera& move(const linear_algebra::Vector&& vec);

    linear_algebra::Vector target();

    linear_algebra::Vector up();

    linear_algebra::Vector side();

    Camera& pitch(double degree);

    Camera& yaw(double degree);

    Camera& zoom(double px);

    double speed() const;

    void speed(double speed);

private:
    class Core;

    std::shared_ptr<Core> m_core;
};
}