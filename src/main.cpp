#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <chicken3421/chicken3421.hpp>

#include <ass2/memes.hpp>
#include <ass2/player.hpp>
#include <ass2/shapes.hpp>
#include <ass2/texture_2d.hpp>
#include <ass2/static_mesh.hpp>
#include <ass2/scene.hpp>
#include <ass2/utility.hpp>
#include <ass2/renderer.hpp>

#include <iostream>

const int WIN_HEIGHT = 1280;
const int WIN_WIDTH = 720;

struct pointerInformation {
    scene::world *gameWorld;
    renderer::renderer_t *renderInfo;
};

int main() {

    chicken3421::enable_debug_output();
    GLFWwindow *window = marcify(chicken3421::make_opengl_window(WIN_HEIGHT, WIN_WIDTH, "Assignment 2"));

    scene::world gameWorld;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        // Close program if esc is pressed
        if (action != GLFW_PRESS) return;
        pointerInformation *info = (pointerInformation *) glfwGetWindowUserPointer(win);
        switch(key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(win, GLFW_TRUE);
                break;
            case GLFW_KEY_E:
                info->gameWorld->switchHotbars();
                break;
            case GLFW_KEY_F:
                info->gameWorld->toggleMode();
                break;
            case GLFW_KEY_G:
                std::cout << "X: " << info->gameWorld->playerCamera.pos.x << " ";
                std::cout << "Y: " << info->gameWorld->playerCamera.pos.y << " ";
                std::cout << "Z: " << info->gameWorld->playerCamera.pos.z << "\n";
                std::cout << "Yaw: " << info->gameWorld->playerCamera.yaw << "\n";
                std::cout << "Pitch: " << info->gameWorld->playerCamera.pitch << "\n";
                break;
            case GLFW_KEY_C:
                info->gameWorld->toggleCutscene();
                break;
        }
    });

    // Setting up all the render informations
    renderer::renderer_t renderInfo;
    renderInfo.initialise(WIN_HEIGHT, WIN_WIDTH);
    

    gameWorld.playerCamera = player::make_camera(glm::vec3(gameWorld.terrain.size() / 2, 5, gameWorld.terrain[0][0].size() / 2), glm::vec3(4));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /**
     * Setting up clicks
     */
    pointerInformation info;
    info.gameWorld = &gameWorld;
    info.renderInfo = &renderInfo;
    glfwSetWindowUserPointer(window, &info);

    glfwSetMouseButtonCallback(window, [](GLFWwindow *win, int button, int action, int mods) {

        if (action != GLFW_PRESS) return;

        pointerInformation *info = (pointerInformation *) glfwGetWindowUserPointer(win);

        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                info->gameWorld->leftClickDestroy(info->renderInfo);
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                info->gameWorld->rightClickPlace(info->renderInfo);
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                info->gameWorld->middleClickPick();
                break;
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow *win, double xoffset, double yoffset) {
        pointerInformation *info = (pointerInformation *) glfwGetWindowUserPointer(win);

        if (yoffset > 0) {
            info->gameWorld->scrollHotbar(1);
        } else {
            info->gameWorld->scrollHotbar(-1);
        }
    });


    glm::vec3 sunPosition;
    float degrees = 90;

    glUseProgram(renderInfo.program);
    while (!glfwWindowShouldClose(window)) {
        float dt = utility::time_delta();

        if (gameWorld.cutsceneEnabled) {
            degrees += 2.50f;
        } else {
            degrees += 0.02f;
        }
        
        if (degrees >= 360) {
            degrees -= 360.0f;
        }
        sunPosition = glm::vec3(gameWorld.playerCamera.pos.x + (gameWorld.getSunDistance() - 10) * glm::cos(glm::radians(degrees)), gameWorld.playerCamera.pos.y + (gameWorld.getSunDistance() - 10) * glm::sin(glm::radians(degrees)), gameWorld.playerCamera.pos.z);

        renderInfo.sun_light_dir = glm::normalize(gameWorld.playerCamera.pos - sunPosition);
        renderInfo.changeSunlight(degrees);

        if (gameWorld.getCutsceneStatus()) {
            gameWorld.animateCutscene();
        } else {
            gameWorld.updatePlayerPositions(window, dt);
        }

        gameWorld.updateSunPosition(degrees, renderInfo.getSkyColor(degrees));
        gameWorld.drawWorld(renderInfo.projection * gameWorld.getCurrCamera().get_view(), renderInfo);
        glfwSwapBuffers(window);
        glfwPollEvents();

        // not entirely correct as a frame limiter, but close enough
        // it would be more correct if we knew how much time this frame took to render
        // and calculated the distance to the next "ideal" time to render and only slept that long
        // the current way just always sleeps for 16.67ms, so in theory we'd drop frames
        std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000.f / 60));
    }

    // deleting the whole window also removes the opengl context, freeing all our memory in one fell swoop.
    chicken3421::delete_opengl_window(window);

    return EXIT_SUCCESS;
}
