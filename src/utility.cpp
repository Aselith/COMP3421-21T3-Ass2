#include <glad/glad.h>

#include <iostream>
#include <chicken3421/chicken3421.hpp>

#include <ass2/utility.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <algorithm>

namespace utility {

    float calculateDistance(glm::vec3 posA, glm::vec3 posB) {
        return sqrt( pow(posB.x - posA.x, 2) + pow(posB.y - posA.y, 2) + pow(posB.z - posA.z, 2) );
    }

    float time_delta() {
        static float then = time_now();
        float now = time_now();
        float dt = now - then;
        then = now;
        return dt;
    }

    float time_now() {
        return (float)glfwGetTime();
    }

    void calcVertNormals(static_mesh::mesh_template_t& mesh_template) {
		mesh_template.normals = std::vector<glm::vec3>(mesh_template.positions.size(), glm::vec3(0));

		const auto& pos = mesh_template.positions;
		for (auto i = size_t{0}; i < mesh_template.indices.size() - 2; i += 3) {
			GLuint i1 = mesh_template.indices[i];
			GLuint i2 = mesh_template.indices[i + 1];
			GLuint i3 = mesh_template.indices[i + 2];
			auto a = pos[i2] - pos[i1];
			auto b = pos[i3] - pos[i1];
			auto face_normal = glm::normalize(glm::cross(a, b));
			mesh_template.normals[i1] += face_normal;
			mesh_template.normals[i2] += face_normal;
			mesh_template.normals[i3] += face_normal;
			/*
			std::cout << "Normals: " <<  face_normal.x << " " << face_normal.y << " " << face_normal.z << "\n";
			std::cout <<  mesh_template.normals[i1].x << " " << mesh_template.normals[i1].y << " " << mesh_template.normals[i1].z << "\n";
			std::cout <<  mesh_template.normals[i2].x << " " << mesh_template.normals[i2].y << " " << mesh_template.normals[i2].z << "\n";
			std::cout <<  mesh_template.normals[i3].x << " " << mesh_template.normals[i3].y << " " << mesh_template.normals[i3].z << "\n";
			*/
		}
		// normalise all the normals
		// std::cout << "Normal size: " << mesh_template.normals.size() << "\n";
		for (auto i = size_t{0}; i < mesh_template.normals.size(); i++) {
			
			mesh_template.normals[i] = glm::normalize(mesh_template.normals[i]);
			// std::cout << mesh_template.normals[i].x << " " << mesh_template.normals[i].y << " " << mesh_template.normals[i].z << "\n";
		}
		// std::cout << "\n";
	}

	void calcFaceNormals(static_mesh::mesh_template_t& mesh_template) {
		chicken3421::expect(mesh_template.indices.size() == 0,
		                    "shapes::calc_face_normals requires the mesh_template to not use "
		                    "indices");
		mesh_template.normals =
		   std::vector<glm::vec3>(mesh_template.positions.size(), glm::vec3(1, 0, 0));
		const auto& pos = mesh_template.positions;
		for (auto i = size_t{0}; i < mesh_template.positions.size() - 2; i += 3) {
			auto a = pos[i + 1] - pos[i];
			auto b = pos[i + 2] - pos[i];
			auto face_normal = glm::normalize(glm::cross(a, b));
			for (auto j = size_t{0}; j < 3; ++j) {
				mesh_template.normals[i + j] = face_normal;
			}
		}
	}


	static_mesh::mesh_template_t expand_indices(const static_mesh::mesh_template_t& mesh_template) {
		chicken3421::expect(mesh_template.indices.size() != 0,
		                    "shapes::expand_indices requires the mesh_template to have indices to "
		                    "expand");
		auto new_mesh_template = static_mesh::mesh_template_t{};
		for (auto i : mesh_template.indices) {
			new_mesh_template.positions.push_back(mesh_template.positions[i]);
			if (!mesh_template.colors.empty()) {
				new_mesh_template.colors.push_back(mesh_template.colors[i]);
			}
			if (!mesh_template.tex_coords.empty()) {
				new_mesh_template.tex_coords.push_back(mesh_template.tex_coords[i]);
			}
			if (!mesh_template.normals.empty()) {
				new_mesh_template.normals.push_back(mesh_template.normals[i]);
			}
		}
		return new_mesh_template;
	}

	void invertShape(static_mesh::mesh_template_t &meshTemplate) {
		for (size_t i = 0; i < meshTemplate.indices.size(); i += 3) {
			int tempVariable = meshTemplate.indices[i + 1];
			meshTemplate.indices[i + 1] = meshTemplate.indices[i + 2];
			meshTemplate.indices[i + 2] = tempVariable;
		}
	}

	int countFalses(std::vector<bool> vector) {
		int total = 0;
		for (auto i : vector) {
			if (!i) {
				total++;
			}
		}
		return total;
	}

	bool isPointInHemisphere(glm::vec3 centre, glm::vec3 tip, glm::vec3 point, int renderDist) {
		if (calculateDistance(centre, point) > renderDist) {
			return false;
		}
		return glm::dot(tip, point - centre) > 0;	
	}

	glm::vec3 cubicBezier(const std::vector<glm::vec3> &control_points, float t) {
		glm::vec3 pos = (1 - t) * (1 - t) * (1 - t) * control_points[0]
						+ 3 * t * (1 - t) * (1 - t) * control_points[1]
						+ 3 * t * t * (1 - t) * control_points[2]
						+ t * t * t * control_points[3];
		return pos;
	}

	int obtainSignOfNumber(float number) {
		if (number < 0) {
			return -1;
		} else {
			return 1;
		}
	}

	int getDirection(float yaw) {
		yaw = fmod(yaw, 360.0f);
		if (yaw >= 315.0f || yaw < 45.0f) {
			return 0;
		} else if (yaw >= 45.0f && yaw < 135.0f) {
			return 1;
		} else if (yaw >= 135.0f && yaw < 225.0f) {
			return 2;
		} else {
			return 3;
		}
	}

}
