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

#include "Application/utils.h"

void SimpleShapeApplication::init() {
	// A utility function that reads the shader sources, compiles them and creates the program object
	// As everything in OpenGL we reference program by an integer "handle".
	auto program = xe::utils::create_program(
		{ {GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
		 {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"} });

	if (!program) {
		std::cerr << "Invalid program" << std::endl;
		exit(-1);
	}

	// A vector containing the x,y,z vertex coordinates for the triangle.
	std::vector<GLfloat> vertices = {
		// Base
		-0.5f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f, //A
		-0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, //B
		0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, //C
		0.5f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f, //D

		// Front triangle
		0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, //C
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, //E
		-0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, //B

		// Left triangle
		0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, //D
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //E
		0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, //C

		// Right triangle
		-0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 1.0f, //B
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //E
		-0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 1.0f, //A

		// Back triangle
		-0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 0.0f, //A
		0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, //E
		0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 0.0f //D
	};

	std::vector<GLushort> indices = {
		0, 2, 1,
		0, 3, 2,
		4, 5, 6,
		7, 8, 9,
		10, 11, 12,
		13, 14, 15
	};

	float strength = 0.9f;
	glm::vec3 color = { 1.0f, 1.0f, 1.0f };

	// Creating the model parameters 
	axis_ = { 0.0f, 1.0f, 0.0f };
	scale_ = { 1.0f, 1.0f, 1.0f };
	translate_ = { 0.0f, -0.25f, 0.0f };

	int w, h;
	std::tie(w, h) = frame_buffer_size();

	set_camera(new Camera);
	camera_->look_at(glm::vec3(0.0f, -0.3f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	camera_->perspective((float)w / h, glm::pi<float>() / 2.0, 0.1f, 100.0f);

	// Generating the vertex buffer and loading the vertex data into it.
	GLuint v_buffer_handle;
	glGenBuffers(1, &v_buffer_handle);
	OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generating the indices buffer and loading the vertex data into it.
	GLuint i_buffer_handle;
	glGenBuffers(1, &i_buffer_handle);
	OGL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Generating the uniform buffer (color)
	GLuint u_color_buffer_handle;
	glGenBuffers(1, &u_color_buffer_handle);
	OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_color_buffer_handle));
	glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_color_buffer_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Generating the uniform buffer (transform)
	glGenBuffers(1, &u_pvm_buffer_handle);
	OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_handle));
	glBufferData(GL_UNIFORM_BUFFER, 64, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// This setups a Vertex Array Object (VAO) that encapsulates
	// the state of all vertex buffers needed for rendering
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
	glBindBuffer(GL_UNIFORM_BUFFER, u_color_buffer_handle);

	// This indicates that the data for attribute 0 should be read from a vertex buffer.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));

	// This indicates that the data for attribute 1 should be read from a vertex buffer offset by 3 float values
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));

	// Loading the data for color uniform buffer
	// Load strength at offset 0 and color at offset 16 (std140)
	glBindBuffer(GL_UNIFORM_BUFFER, u_color_buffer_handle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 3 * sizeof(float), &color);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);



	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//end of vao "recording"

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
	// Binding the VAO will setup all the required vertex buffers.
	glBindVertexArray(vao_);

	// Model definition
	M_ = glm::mat4(1.0f);

	// Transforming the model
	M_ = glm::scale(M_, scale_);
	M_ = glm::rotate(M_, glm::radians(angle_), axis_);
	M_ = glm::translate(M_, translate_);

	// Creating the PVM for later use with uniform
	glm::mat4 PVM = camera_->projection() * camera_->view() * M_;

	// Loading the data for transformation uniform buffer
	// PVM is a mat4 so its loaded as 4 vec4 each 16 bytes long (std140)
	glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_handle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, (void*)(0));
	glBindVertexArray(0);
	angle_ += 0.8f;
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
	Application::framebuffer_resize_callback(w, h);
	glViewport(0, 0, w, h);
	camera_->set_aspect((float)w / h);
}