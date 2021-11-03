#ifndef COMP3421_RENDERER_HPP
#define COMP3421_RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <ass2/scene.hpp>
#include <iostream>

namespace renderer {
	struct renderer_t {
		glm::mat4 projection;
		glm::vec3 sun_light_dir = glm::normalize(glm::vec3(0) - glm::vec3(0, 10, 0));
		glm::vec3 sun_light_color = glm::vec3(1.1, 1.1, 1.1);
		float sun_light_ambient = 0.5f;

		glm::vec3 sunriseColor = glm::vec3((float)247/255, (float)205/255, (float)93/255);
		glm::vec3 dayColor = glm::vec3(1, 1, 1);
		glm::vec3 dayBlueColor = glm::vec3((float)135/255, (float)206/255, (float)235/255);
		glm::vec3 sunsetColor = glm::vec3((float)250/255, (float)214/255, (float)165/255);
		glm::vec3 nightColor = glm::vec3((float)25/255, (float)25/255, (float)112/255);

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

		void changeSunlight(float degree) {

			if (degree >= 0.0f && degree < 90.0f) {
				sun_light_color = sunriseColor + ((dayColor - sunriseColor) * glm::vec3(degree / 90.0f)); 
			} else if (degree >= 90.0f && degree < 180.0f) {
				sun_light_color = dayColor + ((sunsetColor - dayColor) * glm::vec3((degree - 90) / 90.0f)); 
			} else if (degree >= 180.0f && degree < 270.0f) {
				sun_light_color = sunsetColor + ((nightColor - sunsetColor) * glm::vec3((degree - 180) / 90.0f)); 
			} else {
				sun_light_color = nightColor + ((sunriseColor - nightColor) * glm::vec3((degree - 270) / 90.0f)); 
			}
			if (degree >= 180.0f && degree < 360.0f) {
				sun_light_ambient = -glm::sin(glm::radians(degree)) * 0.4f + 0.25f;
			} else {
				sun_light_ambient = 0.25f;
			}
		}

		glm::vec3 getSkyColor(float degree) {

			if (degree >= 0.0f && degree < 90.0f) {
				return sunriseColor + ((dayBlueColor - sunriseColor) * glm::vec3(degree / 90.0f)); 
			} else if (degree >= 90.0f && degree < 180.0f) {
				return dayBlueColor + ((sunsetColor - dayBlueColor) * glm::vec3((degree - 90) / 90.0f)); 
			} else if (degree >= 180.0f && degree < 270.0f) {
				return sunsetColor + ((nightColor - sunsetColor) * glm::vec3((degree - 180) / 90.0f)); 
			} else {
				return nightColor + ((sunriseColor - nightColor) * glm::vec3((degree - 270) / 90.0f)); 
			}
		}
	};

	/*
	renderer_t init(const glm::mat4& projection);

	void draw(const scene::node_t* node, const renderer_t& renderer, glm::mat4 model);

	void render(const renderer_t& renderer, const scene::scene_t& scene);
	*/
}

#endif // COMP3421_RENDERER_HPP
