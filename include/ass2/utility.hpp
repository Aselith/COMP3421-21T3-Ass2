#ifndef COMP3421_ASS2_UTILITY_HPP
#define COMP3421_ASS2_UTILITY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <ass2/static_mesh.hpp>

namespace utility {
    float calculateDistance(glm::vec3 posA, glm::vec3 posB);

    /**
     * Returns the current time in seconds.
     * @return Returns the current time in seconds.
     */
    float time_now();

    /**
     * Returns the difference in time between when this function was previously called and this call.
     * @return A float representing the difference between function calls in seconds.
     */
    float time_delta();


    void calcVertNormals(static_mesh::mesh_template_t& mesh_template);

    void calcTrianglePos(static_mesh::mesh_template_t& mesh_template);

    void invertHalfNormals(static_mesh::mesh_template_t& mesh_template);

    void invertShape(static_mesh::mesh_template_t &meshTemplate);

    int countFalses(std::vector<bool> vector);

    bool isPointInHemisphere(glm::vec3 centre, glm::vec3 tip, glm::vec3 point, int renderDist);

    glm::vec3 cubicBezier(const std::vector<glm::vec3> &control_points, float t);


}

#endif //COMP3421_ASS2_MEMES_HPP
