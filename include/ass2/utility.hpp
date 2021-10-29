#ifndef COMP3421_ASS2_UTILITY_HPP
#define COMP3421_ASS2_UTILITY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

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
}

#endif //COMP3421_ASS2_MEMES_HPP
