#include <ass2/renderer.hpp>
#include <ass2/texture_2d.hpp>
#include <ass2/player.hpp>
#include <ass2/static_mesh.hpp>

#include "chicken3421/chicken3421.hpp"

const char* VERT_PATH = "res/shaders/shader.vert";
const char* FRAG_PATH = "res/shaders/shader.frag";

namespace renderer {
	renderer_t init(const glm::mat4& projection) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		auto renderer = renderer_t{};
		renderer.projection = projection;

		// make the render program
		auto vs = chicken3421::make_shader(VERT_PATH, GL_VERTEX_SHADER);
		auto fs = chicken3421::make_shader(FRAG_PATH, GL_FRAGMENT_SHADER);
		renderer.program = chicken3421::make_program(vs, fs);
		chicken3421::delete_shader(vs);
		chicken3421::delete_shader(fs);

		renderer.view_proj_loc = chicken3421::get_uniform_location(renderer.program, "uViewProj");
		renderer.model_loc = chicken3421::get_uniform_location(renderer.program, "uModel");

		// TODO: sunlight uniform locations
		renderer.sun_direction_loc = chicken3421::get_uniform_location(renderer.program, "uSun.direction");
		renderer.sun_color_loc = chicken3421::get_uniform_location(renderer.program, "uSun.color");
		renderer.sun_ambient_loc = chicken3421::get_uniform_location(renderer.program, "uSun.ambient");

		// TODO: material uniform locations
		renderer.mat_tex_factor_loc =
		   chicken3421::get_uniform_location(renderer.program, "uMat.texFactor");
		renderer.mat_color_loc = chicken3421::get_uniform_location(renderer.program, "uMat.color");
		renderer.mat_diffuse_loc = chicken3421::get_uniform_location(renderer.program, "uMat.diffuse");

		return renderer;
	}

	void draw(const scene::node_t* node, const renderer_t& renderer, glm::mat4 model) {
		model *= glm::translate(glm::mat4(1.0), node->translation);
		model *= glm::rotate(glm::mat4(1.0), node->rotation.z, glm::vec3(0, 0, 1));
		model *= glm::rotate(glm::mat4(1.0), node->rotation.y, glm::vec3(0, 1, 0));
		model *= glm::rotate(glm::mat4(1.0), node->rotation.x, glm::vec3(1, 0, 0));
		model *= glm::scale(glm::mat4(1.0), node->scale);

		glUniformMatrix4fv(renderer.model_loc, 1, GL_FALSE, glm::value_ptr(model));

		for (auto i = size_t{0}; i < node->model.meshes.size(); ++i) {
			GLuint tex = node->model.materials[i].texture;
			glUniform1f(renderer.mat_tex_factor_loc, tex ? 1.0f : 0.0f);
			glUniform4fv(renderer.mat_color_loc, 1, glm::value_ptr(node->model.materials[i].color));
			// TODO: set more material uniforms
			glUniform3fv(renderer.mat_diffuse_loc, 1, glm::value_ptr(node->model.materials[i].diffuse));
			texture_2d::bind(node->model.materials[i].texture);
			mesh::draw(node->model.meshes[i]);
		}

		for (auto const& child : node->children) {
			draw(child, renderer, model);
		}
	}

	void render(const renderer_t& renderer, const scene::world& scene) {
		glUseProgram(renderer.program);
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO: set sunlight uniform variables
		glUniform3fv(renderer.sun_direction_loc, 1, glm::value_ptr(renderer.sun_light_dir));
		glUniform3fv(renderer.sun_color_loc, 1, glm::value_ptr(renderer.sun_light_color));
		glUniform1f(renderer.sun_ambient_loc, renderer.sun_light_ambient);

		auto view_proj = renderer.projection * player::get_view(scene.playerCamera);
		glUniformMatrix4fv(renderer.view_proj_loc, 1, GL_FALSE, glm::value_ptr(view_proj));

		// draw(scene.root, renderer, glm::mat4(1.0f));
	}
} // namespace renderer
