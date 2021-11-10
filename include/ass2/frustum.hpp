
#ifndef COMP3421_ASS2_FRUSTUM_HPP
#define COMP3421_ASS2_FRUSTUM_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define VIEW_DEGREE 65

namespace frustum {

	/**
	 * Measures the degree between the vector from block to player camera and the vector
	 * in which the player is looking at.
	 * Returns true if the degree is less than VIEW_DEGREE, false if otherwise
	 */
	bool isBlockInView(glm::vec3 lookingDir, glm::vec3 blockPoint, glm::vec3 playerPos);
}

#endif //COMP3421_ASS2_FRUSTUM_HPP