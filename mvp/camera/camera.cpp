#include "camera.h"
#include <cmath>

namespace mvp {
struct Camera::Core {
    Core():
            m_fovy(75.f),
            m_as(1.4f),
            m_near(.5f),
            m_far(100.f)
    {}

    Core(double fovy, double as, double near, double far):
            m_fovy(fovy),
            m_as(as),
            m_near(near),
            m_far(far)
    {};

    linear_algebra::Vector m_pos = {0, .5, -1.5};
    double m_fovy, m_as, m_near, m_far;
    double m_speed = .1, m_yaw = 0, m_pitch = 180;
};

Camera::Camera(): m_core(new Core) {};

Camera::Camera(double fovy, double as, double near, double far):
        m_core(new Core(fovy, as, near, far))
{}

linear_algebra::Matrix Camera::model() {
    linear_algebra::Vector up = this->up();
    linear_algebra::Vector target = this->target();
    linear_algebra::Vector side = this->side();

    double f = std::cos(m_core->m_fovy / 2) / std::sin(m_core->m_fovy / 2);
    double as = m_core->m_as;
    double far = m_core->m_far;
    double near = m_core->m_near;

    linear_algebra::Matrix perspective = {
            {f / as, 0,               0              ,               0              },
            {  0   , f,               0              ,               0              },
            {  0   , 0,  (far + near) / (near - far) , 2 * far * near / (near - far)},
            {  0   , 0,              -1              ,               0              }
    };
    return linear_algebra::Matrix {
            {side[0], up[2], target[0], 0},
            {side[1], up[1], target[1], 0},
            {side[2], up[3], target[2], 0},
            {- m_core->m_pos * side, - m_core->m_pos * up, - m_core->m_pos * target, 1}
    } * perspective;
}

Camera& Camera::move(const linear_algebra::Vector&& vec) {
    m_core->m_pos += vec;
    return *this;
}

linear_algebra::Vector Camera::target() {
    double yaw = m_core->m_yaw * M_PI / 180;
    double pitch = m_core->m_pitch * M_PI / 180;
    return {
            std::sin(yaw) * std::cos(pitch),
            std::sin(- pitch),
            std::cos(yaw) * std::cos(pitch)
    };
}

linear_algebra::Vector Camera::up() {
    double yaw = m_core->m_yaw * M_PI / 180;
    double pitch = m_core->m_pitch * M_PI / 180;
    return {
            std::sin(yaw) * std::sin(pitch),
            std::cos(pitch),
            std::cos(yaw) * std::sin(pitch)
    };
}

linear_algebra::Vector Camera::side() {
    double yaw = m_core->m_yaw * M_PI / 180;
    return {
            std::cos(yaw),
            0,
            std::sin(- yaw)
    };
}

Camera& Camera::pitch(double degree) {
    degree += m_core->m_pitch + 360;
    degree -= ((int)degree / 360) * 360;
    if (90 < degree && degree < 180 || 180 < degree && degree < 270)
        degree = degree > 180 ? 270 : 90;
    std::cout << "PITCH: " << degree << std::endl;
    m_core->m_pitch = degree;
}

Camera& Camera::yaw(double degree) {
    degree += m_core->m_yaw + 360;
    degree -= ((int)degree / 360) * 360;
    m_core->m_yaw = degree;
}

Camera& Camera::zoom(double px) {
    m_core->m_fovy += px;
    if (m_core->m_fovy < 73.2) m_core->m_fovy = 73.2;
    if (m_core->m_fovy > 75.3) m_core->m_fovy = 75.3;
}

const linear_algebra::Vector& Camera::position() const {
    return m_core->m_pos;
}

double Camera::speed() const {
    return m_core->m_speed;
}

void Camera::speed(double speed) {
    m_core->m_speed = speed;
}
}