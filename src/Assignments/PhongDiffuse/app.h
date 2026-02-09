#pragma once

#include <vector>

#include "Application/application.h"
#include "Application/utils.h"

#include "glad/gl.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include "CameraControler.h"
#include "Engine/Mesh.h"
#include "Engine/ColorMaterial.h"
#include "Engine/PhongMaterial.h"
#include "Engine/Light.h"

class SimpleShapeApplication : public xe::Application
{
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug) : Application(width, height, title, debug) {}

    void init() override;

    void frame() override;

    void mouse_button_callback(int button, int action, int mods) override;

    void cursor_position_callback(double x, double y) override;

    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override;

    void set_camera(Camera* camera) { camera_ = camera; }

    void set_controler(CameraControler* controler) { controler_ = controler; }
    
    void add_submesh(xe::Mesh* mesh) {
        meshes_.push_back(mesh);
    }

    void add_light(const xe::PointLight& p_light) {
        p_lights_.push_back(p_light);
    }

    void add_ambient(glm::vec3 ambient) {
        ambient_ = ambient;
    }

    Camera* camera() { return camera_; }
    CameraControler* controler() { return controler_; }
    std::vector<xe::Mesh*> meshes() { return meshes_; }

    ~SimpleShapeApplication() {
        if (camera_) {
            delete camera_;
        }
    }

private:
    Camera* camera_;
    CameraControler* controler_;
    std::vector<xe::Mesh*> meshes_;

    GLuint u_pvm_buffer_handle;
    GLuint u_light_buffer_handle;
    float angle_ = 0.0f;

    glm::vec3 axis_;
    glm::vec3 scale_;
    glm::vec3 translate_;

    glm::mat4 M_;

    glm::vec3 ambient_;
    std::vector<xe::PointLight> p_lights_;
};