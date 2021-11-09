#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <chicken3421/chicken3421.hpp>

#include <ass2/player.hpp>
#include <ass2/shapes.hpp>
#include <ass2/texture_2d.hpp>
#include <ass2/static_mesh.hpp>
#include <ass2/scene.hpp>
#include <ass2/utility.hpp>
#include <ass2/renderer.hpp>
#include <ass2/frustum.hpp>

#include <iostream>

const int WIN_HEIGHT = 1280;
const int WIN_WIDTH = 720;

// 0 -> Basic flat dirt world
// 1 -> Wooly world
// 2 -> Iron world
const int WORLD_TYPE = 0;

struct pointerInformation {
    scene::world *gameWorld;
    renderer::renderer_t *renderInfo;
    float frameRate = 0;
};

int main() {

    GLFWwindow *window = chicken3421::make_opengl_window(WIN_HEIGHT, WIN_WIDTH, "Assignment 2 - A Minecraft Clone");
    chicken3421::image_t faviconImage = chicken3421::load_image("./res/textures/favicon.png", false);
    GLFWimage favicon = {faviconImage.width, faviconImage.height, (unsigned char *) faviconImage.data};
    glfwSetWindowIcon(window, 1, &favicon);
    chicken3421::delete_image(faviconImage);

    std::vector<scene::miniBlockData> listOfBlocks;

    switch (WORLD_TYPE) {
        case 1:
            listOfBlocks.emplace_back(scene::miniBlockData("gray", glm::vec3(0,0,0)));
            listOfBlocks.emplace_back("white");
            break;
        case 2:
            listOfBlocks.emplace_back("raw_iron");
            listOfBlocks.emplace_back("iron_block");
            break;
        default:
            listOfBlocks.emplace_back("bedrock");
            listOfBlocks.emplace_back("dirt");
            listOfBlocks.emplace_back("grass_block");
            break;
    }

    scene::world gameWorld(listOfBlocks);

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
            case GLFW_KEY_I:
                info->gameWorld->toggleInstructions();
                break;
            case GLFW_KEY_P:
                std::cout << "Current frame rate: " << info->frameRate << " frames per second\n";
                break;
            case GLFW_KEY_TAB:
                if (!glfwGetWindowAttrib(win, GLFW_MAXIMIZED)) {
                    glfwMaximizeWindow(win);
                } else {
                    glfwRestoreWindow(win);
                }  
                break;
        }
    });

    glfwSetWindowSizeCallback(window, [](GLFWwindow* win, int width, int height) {

        glViewport(0, 0, width, height);
    });


    // Setting up all the render informations
    renderer::renderer_t renderInfo;
    renderInfo.initialise(WIN_HEIGHT, WIN_WIDTH);
    

    gameWorld.playerCamera = player::make_camera(glm::vec3(gameWorld.terrain.size() / 2, 5, gameWorld.terrain[0][0].size() / 2), glm::vec3(4));
    gameWorld.cutsceneCamera = player::make_camera(glm::vec3(gameWorld.terrain.size() / 2, 5, gameWorld.terrain[0][0].size() / 2), glm::vec3(4));

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

    gameWorld.updateBlocksToRender(true);
    gameWorld.updateBlocksToRender(true);

    float totalTime = 0;
    float totalFrames = 0;

    while (!glfwWindowShouldClose(window)) {

        float dt = utility::time_delta();

        totalTime += dt;
        totalFrames++;

        if (totalTime >= 1) {
            info.frameRate = totalFrames / totalTime;
            totalFrames = 0;
            totalTime = 0;
        }

        if (gameWorld.cutsceneEnabled) {
            degrees += 15.0f * dt;
        } else {
            degrees += 0.5f * dt;
        }
        
        if (degrees >= 360) {
            gameWorld.updateMoonPhase();
            degrees -= 360.0f;
        }

        // Changing and updating where the sun will be
        sunPosition = glm::vec3(gameWorld.getCurrCamera()->pos.x + (gameWorld.getSunDistance()) * glm::cos(glm::radians(degrees)), gameWorld.getCurrCamera()->pos.y + (gameWorld.getSunDistance() - 10) * glm::sin(glm::radians(degrees)), gameWorld.getCurrCamera()->pos.z);
        renderInfo.sun_light_dir = glm::normalize(gameWorld.getCurrCamera()->pos - sunPosition);
        renderInfo.changeSunlight(degrees);

        if (gameWorld.getCutsceneStatus()) {
            gameWorld.animateCutscene();
        } else {
            gameWorld.updatePlayerPositions(window, dt);
        }

        gameWorld.updateSunPosition(degrees, renderInfo.getSkyColor(degrees));
        gameWorld.drawWorld(renderInfo);
        glfwSwapBuffers(window);
        glfwPollEvents();

        // not entirely correct as a frame limiter, but close enough
        // it would be more correct if we knew how much time this frame took to render
        // and calculated the distance to the next "ideal" time to render and only slept that long
        // the current way just always sleeps for 16.67ms, so in theory we'd drop frames
        // std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000.f / 60));
    }

    // deleting the whole window also removes the opengl context, freeing all our memory in one fell swoop.
    chicken3421::delete_opengl_window(window);

    return EXIT_SUCCESS;
}
