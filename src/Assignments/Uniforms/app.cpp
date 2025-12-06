//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Application/utils.h"

void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    // A vector containing the x,y,z vertex coordinates for the triangle.
    std::vector<GLfloat> vertices = {
            //Triangle vertices
            -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
            //Rectangle vertices
            -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f };

    std::vector<GLushort> indices = {
        0, 1, 2, 3, 4, 5, 6
    };

    float strength = 0.4f;
    glm::vec3 color = { 1.0f, 1.0f, 1.0f };

    float theta = 1.0 * glm::pi<float>() / 6.0f;
    auto cs = std::cos(theta);
    auto ss = std::sin(theta);
    glm::mat2 rot{ cs,ss,-ss,cs };
    glm::vec2 transform{ 0.0,  -0.25 };
    glm::vec2 scale{ 0.5, 0.5 };


    // Generating the buffer and loading the vertex data into it.
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Indices buffer
    GLuint i_buffer_handle;
    glGenBuffers(1, &i_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Uniform buffer (color)
    GLuint u_color_buffer_handle;
    glGenBuffers(1, &u_color_buffer_handle);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_color_buffer_handle));
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_color_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Uniform buffer (transform)
    GLuint u_transform_buffer_handle;
    glGenBuffers(1, &u_transform_buffer_handle);
    OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_transform_buffer_handle));
    glBufferData(GL_UNIFORM_BUFFER, 48, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_transform_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);



    // This setups a Vertex Array Object (VAO) that  encapsulates
    // the state of all vertex buffers needed for rendering
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, u_color_buffer_handle);

    // This indicates that the data for attribute 0 should be read from a vertex buffer.
    glEnableVertexAttribArray(0);
    // and this specifies how the data is layout in the buffer.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_UNIFORM_BUFFER, u_color_buffer_handle);

    // Upload strength at offset 0
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);

    // Upload vec3 starting at offset 16
    glBufferSubData(GL_UNIFORM_BUFFER, 16, 3 * sizeof(float), &color);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    glBindBuffer(GL_UNIFORM_BUFFER, u_transform_buffer_handle);

    // scale — two floats at offset 0
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec2), &scale);

    // translation — two floats at offset 8
    glBufferSubData(GL_UNIFORM_BUFFER, 8, sizeof(glm::vec2), &transform);

    // First column → offset 16
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec2), &rot[0]);

    // Second column → offset 32
    glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(glm::vec2), &rot[1]);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //end of vao "recording"

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    auto[w, h] = frame_buffer_size();
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will setup all the required vertex buffers.
    glBindVertexArray(vao_);  
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (void*)0);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)(3 * sizeof(GLushort)));
    glBindVertexArray(0);
}
