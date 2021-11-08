#ifndef COMP3421_RENDERER_HPP
#define COMP3421_RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <chicken3421/chicken3421.hpp>

#include <ass2/scene.hpp>
#include <iostream>
#include <string>

#define MAX_LIGHTS 101

namespace renderer {

	struct lightSource {
		glm::vec3 position;
		glm::vec3 diffuse;
		glm::vec3 ambient;
		glm::vec3 specular = glm::vec3(1.1f);
		float intensity = 1.0f;

		GLint position_loc;
		GLint diffuse_loc;
		GLint ambient_loc;
		GLint specular_loc;
		GLint intensity_loc;

		bool occupied = false;
	};

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

		GLint sun_direction_loc;
		GLint sun_color_loc;
		GLint sun_ambient_loc;

		GLint uTex_loc;
		GLint uSpec_loc;

		GLint mat_tex_factor_loc;
		GLint mat_specular_factor_loc;
		GLint mat_specular_loc;
		GLint mat_color_loc;
		GLint mat_diffuse_loc;
		GLint mat_ambient_loc;
		GLint phong_exponent_loc;
		GLint camera_loc;
		std::vector<lightSource> all_lights_sources;

		void initialise(int height, int width) {
			auto vs = chicken3421::make_shader("res/shaders/default.vert", GL_VERTEX_SHADER);
			auto fs = chicken3421::make_shader("res/shaders/default.frag", GL_FRAGMENT_SHADER);
			program = chicken3421::make_program(vs, fs);
			chicken3421::delete_shader(vs);
			chicken3421::delete_shader(fs);

			uTex_loc = chicken3421::get_uniform_location(program, "uTex");
			uSpec_loc = chicken3421::get_uniform_location(program, "uSpec");

			// Gets MVP_Loc
			view_proj_loc = chicken3421::get_uniform_location(program, "uViewProj");
			model_loc = chicken3421::get_uniform_location(program, "uModel");

			// Get projection
			projection = glm::perspective(glm::radians(60.0), (double) height / (double) width, 0.1, 50.0);
			// sunlight uniform locations
			sun_direction_loc = chicken3421::get_uniform_location(program, "uSun.direction");
			sun_color_loc = chicken3421::get_uniform_location(program, "uSun.color");
			sun_ambient_loc = chicken3421::get_uniform_location(program, "uSun.ambient");

			// material uniform locations
			mat_ambient_loc = chicken3421::get_uniform_location(program, "uMat.ambient");

			mat_tex_factor_loc = chicken3421::get_uniform_location(program, "uMat.texFactor");
			mat_color_loc = chicken3421::get_uniform_location(program, "uMat.color");
			mat_diffuse_loc = chicken3421::get_uniform_location(program, "uMat.diffuse");
			camera_loc =  chicken3421::get_uniform_location(program, "uCameraPos");
			phong_exponent_loc = chicken3421::get_uniform_location(program, "uMat.phongExp");
			mat_specular_factor_loc = chicken3421::get_uniform_location(program, "uMat.specularFactor");
			mat_specular_loc = chicken3421::get_uniform_location(program, "uMat.specular");

			for (int i = 0; i < MAX_LIGHTS; i++) {
				lightSource light;
				light.ambient_loc = chicken3421::get_uniform_location(program, "allLights[" + std::to_string(i) + "].ambient");
				light.diffuse_loc = chicken3421::get_uniform_location(program, "allLights[" + std::to_string(i) + "].diffuse");
				light.position_loc = chicken3421::get_uniform_location(program, "allLights[" + std::to_string(i) + "].position");
				light.specular_loc = chicken3421::get_uniform_location(program, "allLights[" + std::to_string(i) + "].specular");
				light.intensity_loc = chicken3421::get_uniform_location(program, "allLights[" + std::to_string(i) + "].intensity");
				light.position = {-1, -1, -1};
				all_lights_sources.push_back(light);
			}
		}

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

		int getMaxLights() {
			return MAX_LIGHTS;
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

		void setBasePters(glm::vec3 pos) {
			glUniform3fv(sun_direction_loc, 1, glm::value_ptr(sun_light_dir));
            glUniform3fv(sun_color_loc, 1, glm::value_ptr(sun_light_color));
            glUniform3fv(camera_loc, 1, glm::value_ptr(pos));
            glUniform1f(sun_ambient_loc, sun_light_ambient);
			for (auto light : all_lights_sources) {
				glUniform3fv(light.ambient_loc, 1, glm::value_ptr(light.ambient));
				glUniform3fv(light.diffuse_loc, 1, glm::value_ptr(light.diffuse));
				glUniform3fv(light.specular_loc, 1, glm::value_ptr(light.specular));
				glUniform3fv(light.position_loc, 1, glm::value_ptr(light.position));
				glUniform1f(light.intensity_loc, light.intensity);
			}
			glUniform1i(uTex_loc, 0);
			glUniform1i(uSpec_loc, 1);
		}


		int addLightSource(glm::vec3 pos, glm::vec3 color, float intensity) {
			for (size_t i = 0; i < all_lights_sources.size(); i++) {
				if (!all_lights_sources[i].occupied) {
					std::cout << "Light added\n";
					all_lights_sources[i].occupied = true;
					all_lights_sources[i].position = pos;
					all_lights_sources[i].diffuse = color;
					all_lights_sources[i].ambient = color;
					all_lights_sources[i].intensity = intensity;
					all_lights_sources[i].specular = color * 1.5f;
					if (all_lights_sources[i].specular.r >= 1.0f) {
						all_lights_sources[i].specular.r = 1.0f;
					}
					if (all_lights_sources[i].specular.g >= 1.0f) {
						all_lights_sources[i].specular.g = 1.0f;
					}
					if (all_lights_sources[i].specular.b >= 1.0f) {
						all_lights_sources[i].specular.b = 1.0f;
					}
					return (int)i;
				}
			}
			return -1;
		}

		void removeLightSource(int lightID) {
			if (lightID < 0) return;
			std::cout << "Light removed\n";
			all_lights_sources[lightID].occupied = false;
			all_lights_sources[lightID].position = {-1, -1, 1};
			return;
		}
	};

	/*
	renderer_t init(const glm::mat4& projection);

	void draw(const scene::node_t* node, const renderer_t& renderer, glm::mat4 model);

	void render(const renderer_t& renderer, const scene::scene_t& scene);
	*/
}

#endif // COMP3421_RENDERER_HPP
