#pragma once
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
public:

    void look_at(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
        z_ = glm::normalize(eye - center);
        x_ = glm::normalize(glm::cross(up, z_));
        y_ = glm::normalize(glm::cross(z_, x_));

        position_ = eye;
        center_ = center;
    }

    void perspective(float fov, float aspect, float near, float far) {
        fov_ = fov;
        aspect_ = aspect;
        near_ = near;
        far_ = far;
    }

    void set_aspect(float aspect) {
        aspect_ = aspect;
    }

    glm::mat4 projection() const { return glm::perspective(fov_, aspect_, near_, far_); }
    glm::vec3 x() const { return x_; }
    glm::vec3 y() const { return y_; }
    glm::vec3 z() const { return z_; }
    glm::vec3 position() const { return position_; }
    glm::vec3 center() const { return center_; }

    void zoom(float y_offset);
    glm::mat4 view() const;
    void rotate_around_center(float angle, const glm::vec3& axis);

private:
    float Camera::logistic(float y);
    float Camera::inverse_logistics(float x);
    glm::mat3 Camera::rotation(float angle, const glm::vec3& axis);
    void Camera::rotate_around_point(float angle, const glm::vec3& axis, const glm::vec3& c);

    float fov_;
    float aspect_;
    float near_;
    float far_;


    glm::vec3 position_;
    glm::vec3 center_;
    glm::vec3 x_;
    glm::vec3 y_;
    glm::vec3 z_;
};