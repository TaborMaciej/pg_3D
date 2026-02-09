#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "spdlog/spdlog.h"

#include "Application/utils.h"

#define STB_IMAGE_IMPLEMENTATION  1

#include "3rdParty/stb/stb_image.h"
#include <Engine/mesh_loader.h>

namespace {
	constexpr glm::uint kMaxPointLights = 24;

	struct PointLightGpu {
		alignas(16) glm::vec4 position_in_view_space; // xyz used
		alignas(16) glm::vec4 color;                  // xyz used
		alignas(16) glm::vec4 intensity_radius;       // x=intensity, y=radius
	};
}

void SimpleShapeApplication::init() {
	// A utility function that reads the shader sources, compiles them and creates the program object
	// As everything in OpenGL we reference program by an integer "handle".
	auto program = xe::utils::create_program(
		{ {GL_VERTEX_SHADER, std::string(ROOT_DIR) + "/src/Engine/shaders/phong_vs.glsl"},
		 {GL_FRAGMENT_SHADER, std::string(ROOT_DIR) + "/src/Engine/shaders/phong_fs.glsl"} });

	if (!program) {
		std::cerr << "Invalid program" << std::endl;
		exit(-1);
	}
	xe::ColorMaterial::init();
	xe::PhongMaterial::init();

	// Creating the model parameters 
	axis_ = { 0.0f, 1.0f, 0.0f };
	scale_ = { 1.0f, 1.0f, 1.0f };
	translate_ = { 0.0f, 0.0f, 0.0f };

	// Model definition
	M_ = glm::mat4(1.0f);

	// Transforming the model
	M_ = glm::scale(M_, scale_);
	M_ = glm::rotate(M_, glm::radians(angle_), axis_);
	M_ = glm::translate(M_, translate_);

	int w, h;
	std::tie(w, h) = frame_buffer_size();

	set_camera(new Camera);
	set_controler(new CameraControler(camera()));
	camera_->look_at(glm::vec3(0.0f, 0.0f, 1.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	camera_->perspective(glm::pi<float>() / 2.0, (float)w / h, 0.1f, 100.0f);

	// Generating the uniform buffer (transform)
	glGenBuffers(1, &u_pvm_buffer_handle);
	OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_handle));
	glBufferData(GL_UNIFORM_BUFFER, 176, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &u_light_buffer_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, u_light_buffer_handle);
	glBufferData(GL_UNIFORM_BUFFER, 32 + sizeof(PointLightGpu) * kMaxPointLights, nullptr, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 2, u_light_buffer_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto pyramid = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/square.obj", std::string(ROOT_DIR) + "/Models");

	add_submesh(pyramid);

	add_ambient(glm::vec3(1.0f, 0.0f, 0.0f));
	auto light = new xe::PointLight(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), 2.0f, 45.0f);
	add_light(*light);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// Setting the background color of the rendering window.
	glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

	// This setups an OpenGL vieport of the size of the whole rendering window.
	glViewport(0, 0, w, h);

	glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
	// Creating the PVM for later use with uniform
	glm::mat4 VM  = camera_->view() * M_;
	glm::mat4 PVM = camera_->projection() * VM;

	auto R = glm::mat3(VM);
	auto N = glm::mat3(glm::cross(R[1], R[2]), glm::cross(R[2], R[0]), glm::cross(R[0], R[1]));

	// Loading the data for transformation uniform buffer
	// PVM is a mat4 so its loaded as 4 vec4 each 16 bytes long (std140)
	glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_handle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(glm::mat4), &VM[0]);;
	glm::vec4 N_col0(N[0], 0.0f);
	glm::vec4 N_col1(N[1], 0.0f);
	glm::vec4 N_col2(N[2], 0.0f);
	glBufferSubData(GL_UNIFORM_BUFFER, 128, sizeof(glm::vec4), &N_col0);
	glBufferSubData(GL_UNIFORM_BUFFER, 144, sizeof(glm::vec4), &N_col1);
	glBufferSubData(GL_UNIFORM_BUFFER, 160, sizeof(glm::vec4), &N_col2);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for (auto& light : p_lights_) {
		glm::vec4 pos_vs = VM * glm::vec4(light.position_in_ws, 1.0f);
		light.position_in_vs = glm::vec3(pos_vs);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, u_light_buffer_handle);

	glm::vec4 ambient4(ambient_, 0.0f);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), &ambient4);

	glm::uint n = static_cast<glm::uint>(p_lights_.size());
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::uint), &n);

	std::vector<PointLightGpu> gpu_lights;
	gpu_lights.reserve(p_lights_.size());
	for (const auto& light : p_lights_) {
		PointLightGpu gpu{};
		gpu.position_in_view_space = glm::vec4(light.position_in_vs, 0.0f);
		gpu.color = glm::vec4(light.color, 0.0f);
		gpu.intensity_radius = glm::vec4(light.intensity, light.radius, 0.0f, 0.0f);
		gpu_lights.push_back(gpu);
	}

	if (!gpu_lights.empty()) {
		glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(PointLightGpu) * gpu_lights.size(), gpu_lights.data());
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for (auto m : meshes_)
		m->draw();
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
	Application::framebuffer_resize_callback(w, h);
	glViewport(0, 0, w, h);
	camera_->set_aspect((float)w / h);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
	Application::mouse_button_callback(button, action, mods);

	if (controler()) {
		double x, y;
		glfwGetCursorPos(window_, &x, &y);

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			controler()->LMB_pressed(x, y);

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			controler()->LMB_released(x, y);
	}

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
	Application::cursor_position_callback(x, y);
	if (controler()) {
		controler()->mouse_moved(x, y);
	}
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
	Application::scroll_callback(xoffset, yoffset);
	camera()->zoom(yoffset / 30.0f);
}