#include "PhongMaterial.h"

#include "Application/utils.h"
#include "spdlog/spdlog.h"

#include "3rdParty/stb/stb_image.h"

namespace xe {

    GLuint PhongMaterial::shader_ = 0u;
    GLuint PhongMaterial::material_uniform_buffer_ = 0u;
    GLint  PhongMaterial::uniform_map_Kd_location_ = 0;

    void PhongMaterial::bind() {
        glUseProgram(program());
        float use_map_Kd = 0.0f;
        if (map_Kd > 0) {
            OGL_CALL(glUniform1i(uniform_map_Kd_location_, map_Kd_unit));
            OGL_CALL(glActiveTexture(GL_TEXTURE0 + map_Kd_unit));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, map_Kd));
            use_map_Kd = 1.0f;
        }

        struct MaterialGpu {
            glm::vec4 Ka;
            glm::vec4 Kd;
            glm::vec4 Ks;
            glm::vec4 Ns_use_map;
        } gpu{ Ka, Kd, Ks, glm::vec4(Ns, use_map_Kd, 0.0f, 0.0f) };

        OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, material_uniform_buffer_));
        glBindBuffer(GL_UNIFORM_BUFFER, material_uniform_buffer_);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialGpu), &gpu);
        OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0u));

    }

    void PhongMaterial::unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
        glBindTexture(GL_TEXTURE_2D, 0u);
    }

    void PhongMaterial::init() {


        auto program = xe::utils::create_program(
            { {GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/phong_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/phong_fs.glsl"} });
        if (!program) {
            std::cerr << "Invalid program" << std::endl;
            exit(-1);
        }

        shader_ = program;

        glGenBuffers(1, &material_uniform_buffer_);

        glBindBuffer(GL_UNIFORM_BUFFER, material_uniform_buffer_);
        glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
#if __APPLE__
        auto u_modifiers_index = glGetUniformBlockIndex(shader_, "Material");
        if (u_modifiers_index == -1) {
            spdlog::warn("Cannot find  {} uniform block in program", "Material");
        }
        else {
            glUniformBlockBinding(program, u_modifiers_index, 0);
        }
#endif

#if __APPLE__
        auto u_transformations_index = glGetUniformBlockIndex(shader_, "Transformations");
        if (u_transformations_index == -1) {
            spdlog::warn("Cannot find  {} uniform block in program", "Transformation");
        }
        else {
            glUniformBlockBinding(program, u_transformations_index, 1);
        }
#endif


        uniform_map_Kd_location_ = glGetUniformLocation(shader_, "map_Kd");
        if (uniform_map_Kd_location_ == -1) {
            spdlog::warn("Cannot get uniform {} location", "map_Kd");
        }

    }
}