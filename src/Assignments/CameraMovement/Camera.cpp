#include "Camera.h"
float Camera::logistic(float y) {
    return 1.0f / (1.0f + std::exp(-y));
}

float Camera::inverse_logistics(float x) {
    return std::log(x / (1.0f - x));
}

void Camera::zoom(float y_offset) {
    auto x = fov_ / glm::pi<float>();
    auto y = inverse_logistics(x);

    y += y_offset;
    x = logistic(y);
    fov_ = x * glm::pi<float>();
}

glm::mat4 Camera::view() const {
    glm::mat4 V(1.0f);
    for (int i = 0; i < 3; ++i) {
        V[i][0] = x_[i];
        V[i][1] = y_[i];
        V[i][2] = z_[i];
    }

    auto t = -glm::vec3{
            glm::dot(x_, position_),
            glm::dot(y_, position_),
            glm::dot(z_, position_),
    };
    V[3] = glm::vec4(t, 1.0f);

    return V;

}

glm::mat3 Camera::rotation(float angle, const glm::vec3& axis) {
    auto u = glm::normalize(axis);
    auto s = std::sin(angle);
    auto c = std::cos(angle);

    return glm::mat3(
        c + u.x * u.x * (1.0f - c),
        u.y * u.x * (1.0f - c) + u.z * s,
        u.z * u.x * (1.0f - c) - u.y * s,

        u.x * u.y * (1.0f - c) - u.z * s,
        c + u.y * u.y * (1.0f - c),
        u.z * u.y * (1.0f - c) + u.x * s,

        u.x * u.z * (1.0f - c) + u.y * s,
        u.y * u.z * (1.0f - c) - u.x * s,
        c + u.z * u.z * (1.0f - c)
    );
}

void Camera::rotate_around_point(float angle, const glm::vec3& axis, const glm::vec3& c) {
    auto R = rotation(angle, axis);
    x_ = R * x_;
    y_ = R * y_;
    z_ = R * z_;

    auto t = position_ - c;
    t = R * t;
    position_ = c + t;

}

void Camera::rotate_around_center(float angle, const glm::vec3& axis) {
    rotate_around_point(angle, axis, center_);
}