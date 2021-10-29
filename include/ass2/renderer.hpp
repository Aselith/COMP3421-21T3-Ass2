#ifndef COMP3421_RENDERER_HPP
#define COMP3421_RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <ass2/scene.hpp>

namespace renderer {
	struct renderer_t {
		glm::mat4 projection;
		glm::vec3 sun_light_dir = glm::normalize(glm::vec3(0) - glm::vec3(50, 50, 0));
		glm::vec3 sun_light_color = glm::vec3(1, 1, 1);
		float sun_light_ambient = 0.1f;

		GLuint program;

		// vertex shader uniforms
		GLint view_proj_loc;
		GLint model_loc;

		// TODO: fragment directional light uniforms
		GLint sun_direction_loc;
		GLint sun_color_loc;
		GLint sun_ambient_loc;

		// TODO: add more fragment material uniforms
		GLint mat_tex_factor_loc;
		GLint mat_color_loc;
		GLint mat_diffuse_loc;
	};

	renderer_t init(const glm::mat4& projection);

	void draw(const scene::node_t* node, const renderer_t& renderer, glm::mat4 model);

	void render(const renderer_t& renderer, const scene::scene_t& scene);
}

#endif // COMP3421_RENDERER_HPP
