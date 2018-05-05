#include "camera.h"
#include <cmath>
#include <mvp/actions/actions.h>

namespace mvp {
struct Camera::Core {
    Core():
            fovy(75.f),
            as(1.4f),
            near(.5f),
            far(100.f)
    {}

    Core(double fovy, double as, double near, double far):
            fovy(fovy),
            as(as),
            near(near),
            far(far)
    {};

    linear_algebra::Vector position = {0, 0, 0};
    double fovy, as, near, far;
    double speed = .1, yaw = 180, pitch = 0;
};

Camera::Camera(): m_core(new Core) {};

Camera::Camera(double fovy, double as, double near, double far):
        m_core(new Core(fovy, as, near, far))
{}

linear_algebra::Matrix Camera::model() {
    linear_algebra::Vector up = -this->up();
    linear_algebra::Vector target = -this->target();
    linear_algebra::Vector side = this->side();
    linear_algebra::Vector pos = this->position();

    double f = std::cos(m_core->fovy / 2) / std::sin(m_core->fovy / 2);
    double as = m_core->as;
    double far = m_core->far;
    double near = m_core->near;

    linear_algebra::Matrix perspective = {
            {f / as, 0,               0              ,               0              },
            {  0   , f,               0              ,               0              },
            {  0   , 0,  (far + near) / (near - far) , 2 * far * near / (near - far)},
            {  0   , 0,              -1              ,               0              }
    };
    linear_algebra::Matrix view = {
            {side[0], up[0], target[0], 0},
            {side[1], up[1], target[1], 0},
            {side[2], up[2], target[2], 0},
            {- pos * side, - pos * up, - pos * target, 1}
    };
    return view * perspective;
}

Camera& Camera::move(const linear_algebra::Vector&& vec) {
    m_core->position += vec;
    return *this;
}

linear_algebra::Vector Camera::target() {
    double yaw = m_core->yaw * M_PI / 180;
    double pitch = m_core->pitch * M_PI / 180;
    return {
            std::cos(yaw) * std::cos(pitch),
            std::sin(yaw) * std::cos(pitch),
            -std::sin(pitch)
    };
}

linear_algebra::Vector Camera::up() {
    double yaw = m_core->yaw * M_PI / 180;
    double pitch = m_core->pitch * M_PI / 180;
    return {
            std::cos(yaw) * std::sin(pitch),
            std::sin(yaw) * std::sin(pitch),
            std::cos(pitch)
    };
}

linear_algebra::Vector Camera::side() {
    double yaw = m_core->yaw * M_PI / 180;
    return {
            -std::sin(yaw),
            std::cos(yaw),
            0
    };
}

Camera& Camera::pitch(double degree) {
    degree += m_core->pitch + 360;
    degree -= ((int)degree / 360) * 360;
    if (90 < degree && degree < 270)
        degree = (degree < 180) ? 90 : 270;
    m_core->pitch = degree;
    return *this;
}

Camera& Camera::yaw(double degree) {
    degree = -degree;
    degree += m_core->yaw + 360;
    degree -= ((int)degree / 360) * 360;
    m_core->yaw = degree;
    return *this;
}

Camera& Camera::zoom(double px) {
    m_core->fovy += px;
    if (m_core->fovy < 73.2) m_core->fovy = 73.2;
    if (m_core->fovy > 75.3) m_core->fovy = 75.3;
    return *this;
}

const linear_algebra::Vector& Camera::position() const {
    return m_core->position;
}

double Camera::speed() const {
    return m_core->speed;
}

void Camera::speed(double speed) {
    m_core->speed = speed;
}
}