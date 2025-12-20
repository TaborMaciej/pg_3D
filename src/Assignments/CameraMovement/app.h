//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/gl.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include "CameraControler.h"

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug) {}

    void init() override;

    void frame() override;

    void mouse_button_callback(int button, int action, int mods) override;

    void cursor_position_callback(double x, double y) override;

    void framebuffer_resize_callback(int w, int h) override;

    void set_camera(Camera* camera) { camera_ = camera; }

    void set_controler(CameraControler* controler) { controler_ = controler; }

    void scroll_callback(double xoffset, double yoffset) override {
        Application::scroll_callback(xoffset, yoffset);
        camera()->zoom(yoffset / 30.0f);
    }

    Camera* camera() { return camera_; }
    CameraControler* controler() { return controler_; }

    ~SimpleShapeApplication() {
        if (camera_) {
            delete camera_;
        }
    }

private:
    Camera* camera_;
    CameraControler* controler_;

    GLuint vao_;
    GLuint u_pvm_buffer_handle;
    float angle_ = 0.0f;

    glm::vec3 axis_;
    glm::vec3 scale_;
    glm::vec3 translate_;

    glm::mat4 M_;
};