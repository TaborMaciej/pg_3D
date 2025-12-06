#include "camera.h"
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