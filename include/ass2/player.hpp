#ifndef COMP3421_OPENGL_PRACTICE_EULER_CAMERA_HPP
#define COMP3421_OPENGL_PRACTICE_EULER_CAMERA_HPP

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace player {

    struct playerPOV {
        glm::vec3 pos;
        float yaw;
        float pitch;
        float yVelocity;


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
     *
     * @param pos - position of the camera
     * @param target - point in word space the camera is looking at
     * @return initialised camera_t struct
     */
    playerPOV make_camera(glm::vec3 pos, glm::vec3 target);

    /**
     * Get the view matrix of the camera (aka inverse transformation of cam)
     * @param cam - camera data
     * @return - view matrix
     */
    glm::mat4 get_view(const playerPOV &cam);

    /**
     * Update the camera's position on wasd keys as well as the direction the camera's facing based on cursor movement
     * @param cam - cam info
     * @param window - glfw window
     * @param dt - delta time
     */
    void update_cam_angles(playerPOV &cam, GLFWwindow *window, float dt);

    /**
     * Gets a vector that is in the direction of where the player is facing.
     * Divides the direction by the given increments
     */
    glm::vec3 getLookingDirection(playerPOV &cam, int increments);

}

#endif //COMP3421_OPENGL_PRACTICE_EULER_CAMERA_HPP
