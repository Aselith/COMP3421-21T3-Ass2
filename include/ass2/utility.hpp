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
     * @brief Returns the current time in seconds.
     * @return Returns the current time in seconds.
     */
    float time_now();

    /**
     * @brief Returns the difference in time between when this function was previously called and this call.
     * @return A float representing the difference between function calls in seconds.
     */
    float time_delta();

    /**
     * @brief Calculates the normals to each triangle in the given mesh_template
     * 
     * @param mesh_template 
     */
    void calcVertNormals(static_mesh::mesh_template_t& mesh_template);

    /**
     * @brief Invert the shape by swapping the order of indices. This will turn the mesh inside out, meaning
     * it will only be visible from inside of the mesh
     * 
     * @param meshTemplate 
     */
    void invertShape(static_mesh::mesh_template_t &meshTemplate);

    /**
     * @brief Count the total falses ionside the given std::vector of bools
     * and returns it
     * 
     * @param vector 
     * @return int 
     */
    int countFalses(std::vector<bool> vector);

    /**
     * @brief Calculates the position on a bezier curve given the control points and t value
     * @warning t must be between 0.0f and 1.0f, 1.0f for being at the end of the curve
     * 
     * @param control_points 
     * @param t 
     * @return glm::vec3 
     */
    glm::vec3 cubicBezier(const std::vector<glm::vec3> &controlPoints, float t);

    /**
     * @brief Get the direction in which the player is facing.
     * Returned value will be 0, 1, 2, 3. These represents North East South West
     * 
     * @param yaw 
     * @return int 
     */
    int getDirection(float yaw);
}

#endif //COMP3421_ASS2_UTILITY_HPP
