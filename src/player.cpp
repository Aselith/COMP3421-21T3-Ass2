#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ass2/player.hpp>

namespace player {

    const float MOUSE_SENSITIVITY = 20.0;

    playerPOV createCamera(glm::vec3 pos, glm::vec3 target) {
        glm::vec3 front = glm::normalize(target - pos);
        // calculate the yaw and pitch from the front vector
        float yaw = glm::degrees(std::atan2(front.x, -front.z));
        float pitch = glm::degrees(std::asin(front.y));
        return {pos, yaw, pitch, 0};
    }

    void updateCameraAngle(playerPOV &cam, GLFWwindow *win, float dt) {
        double mx, my;
        glfwGetCursorPos(win, &mx, &my);
        static glm::vec2 mouse_prev = {mx, my};
        glm::vec2 mouse_pos = {mx, my};

        glm::vec2 offset = MOUSE_SENSITIVITY * dt * (mouse_pos - mouse_prev);
        offset.y *= -1;
        mouse_prev = mouse_pos;

        cam.yaw += offset.x;
        if (cam.yaw > 360) {
            cam.yaw -= float{360.0};
        } else if (cam.yaw < 0) {
            cam.yaw = float{360.0} + cam.yaw;
        }

        cam.pitch += offset.y;

        if(cam.pitch > float{89.0}) {
            cam.pitch = float{89.0};
        } else if(cam.pitch < float{-89.0}) {
            cam.pitch = float{-89.0};
        }
    }

    glm::vec3 getLookingDirection(playerPOV *cam, int increments) {
        return glm::vec3 (
            glm::sin(glm::radians(90 - cam->pitch)) * glm::sin(glm::radians(cam->yaw)) / increments,
            glm::cos(glm::radians(90 - cam->pitch)) / increments,
            -glm::sin(glm::radians(90 - cam->pitch)) * glm::cos(glm::radians(cam->yaw)) / increments
        );
    }
}
