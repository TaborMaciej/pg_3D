//
// Created by pbialas on 25.09.2020.
//

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


void SimpleShapeApplication::init() {
	// A utility function that reads the shader sources, compiles them and creates the program object
	// As everything in OpenGL we reference program by an integer "handle".
	auto program = xe::utils::create_program(
		{ {GL_VERTEX_SHADER, std::string(ROOT_DIR) + "/src/Engine/shaders/color_vs.glsl"},
		 {GL_FRAGMENT_SHADER, std::string(ROOT_DIR) + "/src/Engine/shaders/color_fs.glsl"} });

	if (!program) {
		std::cerr << "Invalid program" << std::endl;
		exit(-1);
	}
	xe::ColorMaterial::init();

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
	camera_->look_at(glm::vec3(0.0f, -0.5f, -1.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	camera_->perspective(glm::pi<float>() / 2.0, (float)w / h, 0.1f, 100.0f);

	// Generating the uniform buffer (transform)
	glGenBuffers(1, &u_pvm_buffer_handle);
	OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_handle));
	glBufferData(GL_UNIFORM_BUFFER, 64, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto pyramid = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/pyramid.obj", std::string(ROOT_DIR) + "/Models");
	//auto pyramid = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/blue_marble.obj", std::string(ROOT_DIR) + "/Models");


	add_submesh(pyramid);

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
	glm::mat4 PVM = camera_->projection() * camera_->view() * M_;

	// Loading the data for transformation uniform buffer
	// PVM is a mat4 so its loaded as 4 vec4 each 16 bytes long (std140)
	glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_handle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
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