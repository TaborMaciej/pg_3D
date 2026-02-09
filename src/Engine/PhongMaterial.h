#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Application/utils.h"
#include "Engine/Material.h"

namespace xe {
	class PhongMaterial : public Material {
	public:

		static void init();

		static GLuint program() { return shader_; }

		PhongMaterial(const glm::vec4 color, GLuint texture)
			: Ka(0.0f, 0.0f, 0.0f, 1.0f),
			  Kd(color),
			  Ks(0.0f, 0.0f, 0.0f, 1.0f),
			  Ns(0.0f),
			  map_Kd(texture),
			  map_Kd_unit(0u) {}

		PhongMaterial(const glm::vec4 color) : PhongMaterial(color, 0) {}

		void set_map_Kd(GLuint tex) { map_Kd = tex; }
		GLuint get_map_Kd() const { return map_Kd; }

		glm::vec4 Ka;
		glm::vec4 Kd;
		glm::vec4 Ks;
		float Ns;
		GLuint map_Kd;
		const GLuint map_Kd_unit;

		void bind() override;

		void unbind() override;

	private:

		static GLuint shader_;
		static GLuint material_uniform_buffer_;
		static GLint uniform_map_Kd_location_;
	};
}


