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

const char *VERT_PATH = "res/shaders/default.vert";
const char *FRAG_PATH = "res/shaders/default.frag";
const int WIN_HEIGHT = 1280;
const int WIN_WIDTH = 720;


int main() {

    chicken3421::enable_debug_output();
    GLFWwindow *window = marcify(chicken3421::make_opengl_window(WIN_HEIGHT, WIN_WIDTH, "Assignment 2"));

    scene::world gameWorld;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        // Close program if esc is pressed
        if (action != GLFW_PRESS) return;
        scene::world *gameWorld = (scene::world *) glfwGetWindowUserPointer(win);
        switch(key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(win, GLFW_TRUE);
                break;
            case GLFW_KEY_E:
                gameWorld->switchHotbars();
                break;
            case GLFW_KEY_F:
                gameWorld->toggleMode();
                break;
            case GLFW_KEY_G:
                std::cout << "X: " << gameWorld->playerCamera.pos.x << " ";
                std::cout << "Y: " << gameWorld->playerCamera.pos.y << " ";
                std::cout << "Z: " << gameWorld->playerCamera.pos.z;
                std::cout << "\n";
                break;
        }
    });

    // Setting up all the render informations
    renderer::renderer_t renderInfo;
    auto vs = chicken3421::make_shader(VERT_PATH, GL_VERTEX_SHADER);
    auto fs = chicken3421::make_shader(FRAG_PATH, GL_FRAGMENT_SHADER);
    renderInfo.program = chicken3421::make_program(vs, fs);
    chicken3421::delete_shader(vs);
    chicken3421::delete_shader(fs);

    // Gets MVP_Loc
    renderInfo.view_proj_loc = chicken3421::get_uniform_location(renderInfo.program, "uViewProj");
	renderInfo.model_loc = chicken3421::get_uniform_location(renderInfo.program, "uModel");

    // Get projection
    renderInfo.projection = glm::perspective(glm::radians(60.0), (double) WIN_HEIGHT / (double) WIN_WIDTH, 0.1, 50.0);
    // sunlight uniform locations
    renderInfo.sun_direction_loc = chicken3421::get_uniform_location(renderInfo.program, "uSun.direction");
    renderInfo.sun_color_loc = chicken3421::get_uniform_location(renderInfo.program, "uSun.color");
    renderInfo.sun_ambient_loc = chicken3421::get_uniform_location(renderInfo.program, "uSun.ambient");

    // material uniform locations
    renderInfo.mat_tex_factor_loc =
        chicken3421::get_uniform_location(renderInfo.program, "uMat.texFactor");
    renderInfo.mat_color_loc = chicken3421::get_uniform_location(renderInfo.program, "uMat.color");
    renderInfo.mat_diffuse_loc = chicken3421::get_uniform_location(renderInfo.program, "uMat.diffuse");

    gameWorld.playerCamera = player::make_camera(glm::vec3(gameWorld.terrain.size() / 2, 5, gameWorld.terrain[0][0].size() / 2), glm::vec3(4));

    // gameWorld.updateAllBlocksCulling();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // TODO - turn this on or off?
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /**
     * Setting up clicks
     */
    glfwSetWindowUserPointer(window, &gameWorld);
    glfwSetMouseButtonCallback(window, [](GLFWwindow *win, int button, int action, int mods) {
        scene::world *gameWorld = (scene::world *) glfwGetWindowUserPointer(win);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            gameWorld->leftClickDestroy();
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            gameWorld->rightClickPlace();
            
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow *win, double xoffset, double yoffset) {
        scene::world *gameWorld = (scene::world *) glfwGetWindowUserPointer(win);

        if (yoffset > 0) {
            gameWorld->scrollHotbar(1);
        } else {
            gameWorld->scrollHotbar(-1);
        }
    });


    glm::vec3 sunPosition;
    float degrees = 90;

    while (!glfwWindowShouldClose(window)) {
        float dt = utility::time_delta();

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            degrees += 1.0f;
        }
        degrees += 0.02f;
        if (degrees >= 360) {
            degrees -= 360.0f;
        }
        sunPosition = glm::vec3(gameWorld.playerCamera.pos.x + (gameWorld.getSunDistance() - 10) * glm::cos(glm::radians(degrees)), gameWorld.playerCamera.pos.y + (gameWorld.getSunDistance() - 10) * glm::sin(glm::radians(degrees)), gameWorld.playerCamera.pos.z);

        renderInfo.sun_light_dir = glm::normalize(gameWorld.playerCamera.pos - sunPosition);
        // renderInfo.sun_light_dir = glm::normalize(sunPosition * glm::vec3(-1, -1, 1) - sunPosition);
        renderInfo.changeSunlight(degrees);
        
        gameWorld.updatePlayerPositions(window, dt);
        gameWorld.updateSunPosition(degrees, renderInfo.getSkyColor(degrees));
    
        gameWorld.drawWorld(renderInfo.projection * player::get_view(gameWorld.playerCamera), renderInfo);
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
