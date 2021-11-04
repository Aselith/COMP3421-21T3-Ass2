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
		}
		// normalise all the normals
		for (auto i = size_t{0}; i < mesh_template.normals.size(); i++) {
			mesh_template.normals[i] = glm::normalize(mesh_template.normals[i]);
		}
	}

	void calcTrianglePos(static_mesh::mesh_template_t& mesh_template) {
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
		}
		// normalise all the normals
		for (auto i = size_t{0}; i < mesh_template.normals.size(); i++) {
			mesh_template.normals[i] = glm::normalize(mesh_template.normals[i]);
		}
	}

	void invertHalfNormals(static_mesh::mesh_template_t& mesh_template) {
		for (auto i = size_t{mesh_template.normals.size() / 2}; i < mesh_template.normals.size(); i++) {
			mesh_template.normals[i] *= -1;
		}
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


}
