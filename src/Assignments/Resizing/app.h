//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/gl.h"
#include <glm/glm.hpp>

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug) {}

    void init() override;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;

private:
    GLuint vao_;
    GLuint u_pvm_buffer_handle;
    float angle_ = 0.0f;

    float fov_;
    float aspect_;
    float near_;
    float far_;

    glm::vec3 axis_;
    glm::vec3 scale_;
    glm::vec3 translate_;

    glm::mat4 M_;
    glm::mat4 P_;
    glm::mat4 V_;
};