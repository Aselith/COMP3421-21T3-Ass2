#ifndef COMP3421_ASS2_PLAYER_HPP
#define COMP3421_ASS2_PLAYER_HPP

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace player {

    struct playerPOV {
        glm::vec3 pos;
        float yaw;
        float pitch;
        float yVelocity;

        /**
         * @brief Returns the view matrix of this point of view
         * 
         * @return glm::mat4 
         */
        glm::mat4 get_view() {
            // calculate the rotated coordinate frame
            glm::mat4 yawed = glm::rotate(glm::mat4(1.0), -glm::radians(yaw), glm::vec3(0, 1, 0));
            glm::mat4 pitched = glm::rotate(glm::mat4(1.0), glm::radians(pitch), glm::vec3(1, 0, 0));
            // transpose for inverted transformation
            glm::mat4 view = glm::transpose(yawed * pitched);
            view *= glm::translate(glm::mat4(1.0), -pos);
            return view;
        }
    };

    /**
     * @brief Creates a camera at the given pos looking at the given target location
     * 
     * @param pos 
     * @param target 
     * @return playerPOV 
     */
    playerPOV createCamera(glm::vec3 pos, glm::vec3 target);

    /**
     * @brief Update the camera's position on wasd keys as well as the direction the camera's facing based on cursor movement
     * 
     * @param cam
     * @param window
     * @param dt
     */
    void updateCameraAngle(playerPOV &cam, GLFWwindow *window, float dt);

    /**
     * @brief Get the Looking Vector of the given camera, and the increments for its length
     * 
     * @param cam 
     * @param increments 
     * @return glm::vec3 
     */
    glm::vec3 getLookingDirection(playerPOV *cam, int increments);

}

#endif //COMP3421_ASS2_PLAYER_HPP
