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

#include <iostream>

const char *VERT_PATH = "res/shaders/default.vert";
const char *FRAG_PATH = "res/shaders/default.frag";

/**
 * Returns the difference in time between when this function was previously called and this call.
 * @return A float representing the difference between function calls in seconds.
 */
float time_delta();

/**
 * Returns the current time in seconds.
 * @return Returns the current time in seconds.
 */
float time_now();

int main() {
    GLFWwindow *window = marcify(chicken3421::make_opengl_window(1280, 720, "Assignment 2"));

    scene::world gameWorld;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        // Close program if esc is pressed
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(win, GLFW_TRUE);
        }
    });

    // make the render program
    auto vs = chicken3421::make_shader(VERT_PATH, GL_VERTEX_SHADER);
    auto fs = chicken3421::make_shader(FRAG_PATH, GL_FRAGMENT_SHADER);
    auto render_program = chicken3421::make_program(vs, fs);
    chicken3421::delete_shader(vs);
    chicken3421::delete_shader(fs);

    GLint mvp_loc = chicken3421::get_uniform_location(render_program, "uMVP");
    glm::mat4 proj = glm::perspective(glm::radians(60.0), (double) 1280 / (double) 720, 0.1, 50.0);
    gameWorld.playerCamera = player::make_camera(glm::vec3(gameWorld.terrain.size() / 2, 5, gameWorld.terrain[0][0].size() / 2), glm::vec3(4));

    texture_2d::params_t parameters;
    parameters.filter_min = GL_NEAREST;
    parameters.filter_max = GL_NEAREST;

    GLuint bedrockTexID = texture_2d::init("./res/textures/bedrock.png", parameters);
    GLuint grassblockTexID = texture_2d::init("./res/textures/grass_block.png", parameters);

    for (int x = 0; x < gameWorld.terrain.size(); x++) {
        for (int z = 0; z < gameWorld.terrain[0][0].size(); z++) {
            gameWorld.placeBlock(scene::createBlock(x, 0, z, bedrockTexID));
        }
    }

    // gameWorld.updateAllBlocksCulling();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // TODO - turn this on or off?
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    /**
     * Setting up clicks
     */
    glfwSetWindowUserPointer(window, &gameWorld);
    glfwSetMouseButtonCallback(window, [](GLFWwindow *win, int button, int action, int mods) {
        scene::world *gameWorld = (scene::world *) glfwGetWindowUserPointer(win);
        texture_2d::params_t parameters;
        parameters.filter_min = GL_NEAREST;
        parameters.filter_max = GL_NEAREST;
        auto tntTexID = texture_2d::init("res/textures/tnt.png", parameters);
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            gameWorld->leftClickDestroy();
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            gameWorld->rightClickPlace(tntTexID);
            
        }
    });


    glUseProgram(render_program);
    while (!glfwWindowShouldClose(window)) {
        float dt = time_delta();
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            std::cout << "X: " << gameWorld.playerCamera.pos.x << " ";
            std::cout << "Y: " << gameWorld.playerCamera.pos.y << " ";
            std::cout << "Z: " << gameWorld.playerCamera.pos.z;
            std::cout << "\n";
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.f, 0.f, 0.2f, 1.f);

        gameWorld.updatePlayerPositions(window, dt);
    
        gameWorld.drawWorld(proj * player::get_view(gameWorld.playerCamera), mvp_loc);
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


float time_delta() {
    static float then = time_now();
    float now = time_now();
    float dt = now - then;
    then = now;
    return dt;
}

float time_now() {
    return (float)glfwGetTime();
}