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
const float W_PRESS_SPACE = 0.4f;

// 0 -> Basic flat dirt world
// 1 -> Wooly world
// 2 -> Iron world
// 3 -> Classic Sky Block

struct pointerInformation {
    scene::world *gameWorld;
    renderer::renderer_t *renderInfo;
    float frameRate = 0, lastWPressed = 0;
};

int main() {

    // Printing welcome message
    int worldType = 0, renderDistance = 0, worldWidth = 0, frameLimiter = 1;
    std::cout << "\n\u001B[34mWelcome to a clone of Minecraft, created by z5309206 for COMP3421 ASS2 21T3 UNSW!\n\n\u001B[0m";
    std::cout << "Presets:\n0 -> Basic Super Flat World\n1 -> Wooly World\n2 -> Iron World\n3 -> Classic Sky Block\n4 -> Parkour Course\n";
    std::cout << "Enter your desired preset world [If not recognised, Basic Super Flat World is used]: ";
    std::cin >> worldType;
    std::cout << "\n";
    std::cout << "Enter your world's width [Minimum 50 | Maximum 500]: ";
    std::cin >> worldWidth;
    if (worldWidth < 50) {
        worldWidth = 50;
    } else if (worldWidth > 500) {
        worldWidth = 500;
    }
    std::cout << "Enter your desired render distance [Minimum 15 | Recommended 30]: ";
    std::cin >> renderDistance;
    if (renderDistance < 15) {
        renderDistance = 15;
    }
    std::cout << "Enable frame limiter? [0 for \"No\" | Anything else for \"Yes\"]: ";
    std::cin >> frameLimiter;

    GLFWwindow *window = chicken3421::make_opengl_window(WIN_HEIGHT, WIN_WIDTH, "COMP3421 21T3 Assignment 2 [Minecraft: Clone Simulator]");
    chicken3421::image_t faviconImage = chicken3421::load_image("./res/textures/favicon.png", false);
    GLFWimage favicon = {faviconImage.width, faviconImage.height, (unsigned char *) faviconImage.data};
    glfwSetWindowIcon(window, 1, &favicon);
    chicken3421::delete_image(faviconImage);

    std::vector<scene::miniBlockData> listOfBlocks;

    std::vector<glm::vec2> listOfPositions;
    std::cout << "\n";
    switch (worldType) {
        case 1:
            std::cout << "Wooly World selected\n";
            listOfBlocks.emplace_back(scene::miniBlockData("gray", glm::vec3(0,0,0), false, true));
            listOfBlocks.emplace_back(scene::miniBlockData("white", glm::vec3(0,1,0), false, true));
            break;
        case 2:
            std::cout << "Iron World selected\n";
            listOfBlocks.emplace_back(scene::miniBlockData("raw_iron", glm::vec3(0,0,0), false, true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(0,1,0), false, true));
            break;
        case 3:
            std::cout << "Classic Sky Block selected\n";
            listOfBlocks.emplace_back(scene::miniBlockData("barrel", glm::vec3(0,3,0), true));
            // Generating land
            listOfPositions = {
                {-1, 0},
                { 0, 0},
                { 1, 0},
                {-1, 1},
                { 0, 1},
                { 1, 1},
                {-1, 2},
                { 0, 2},
                { 1, 2},
                {-1, 3},
                { 0, 3},
                { 1, 3},
                {-1, 4},
                { 0, 4},
                { 1, 4},
                {-1, 5},
                { 0, 5},
                { 1, 5},
                { 1, 5},
                { 2, 3},
                { 2, 4},
                { 2, 5},
                { 3, 3},
                { 3, 4},
                { 3, 5},
                { 4, 3},
                { 4, 4},
                { 4, 5},
            };
            for (auto i : listOfPositions) {
                listOfBlocks.emplace_back(scene::miniBlockData("dirt", glm::vec3(i.x,0,i.y), true));
                listOfBlocks.emplace_back(scene::miniBlockData("dirt", glm::vec3(i.x,1,i.y), true));
                if (i.x == 4 && i.y == 5) {
                    listOfBlocks.emplace_back(scene::miniBlockData("dirt", glm::vec3(i.x,2,i.y), true));

                    // Creating the tree
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_log", glm::vec3(i.x,3,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_log", glm::vec3(i.x,4,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_log", glm::vec3(i.x,5,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_log", glm::vec3(i.x,6,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_log", glm::vec3(i.x,7,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_log", glm::vec3(i.x,8,i.y), true));
                    // Creating the leaves
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,9,i.y), true)); // +
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,9,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,9,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,9,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,9,i.y + 1), true));

                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,8,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,8,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,8,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,8,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,8,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,8,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,8,i.y + 1), true));

                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,7,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,7,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,7,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,7,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,7,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,7,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,7,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,7,i.y - 1), true));

                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 2,7,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 2,7,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 2,7,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 2,7,i.y + 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,7,i.y + 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,7,i.y + 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,7,i.y + 2), true));
                    //
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 2,7,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 2,7,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 2,7,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 2,7,i.y - 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,7,i.y - 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,7,i.y - 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,7,i.y - 2), true));

                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,6,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,6,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,6,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,6,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,6,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,6,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,6,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,6,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 2,6,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 2,6,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 2,6,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 2,6,i.y + 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,6,i.y + 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,6,i.y + 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,6,i.y + 2), true));
                    //
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 2,6,i.y), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 2,6,i.y + 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 2,6,i.y - 1), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 2,6,i.y - 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x - 1,6,i.y - 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x,6,i.y - 2), true));
                    listOfBlocks.emplace_back(scene::miniBlockData("oak_leaves", glm::vec3(i.x + 1,6,i.y - 2), true));
                } else {
                    listOfBlocks.emplace_back(scene::miniBlockData("grass_block", glm::vec3(i.x,2,i.y), true));
                }
            }
            break;
        case 4:
            std::cout << "Parkour Course selected\n";
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-13, 11, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-13, 12, -6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-13, 12, -5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-13, 12, 0), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-13, 15, -5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-12, 0, 10), true));
            listOfBlocks.emplace_back(scene::miniBlockData("copper_block", glm::vec3(-12, 0, 11), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-12, 3, 10), true));
            listOfBlocks.emplace_back(scene::miniBlockData("copper_block", glm::vec3(-11, 0, 10), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-11, 0, 11), true));
            listOfBlocks.emplace_back(scene::miniBlockData("copper_block", glm::vec3(-11, 0, 12), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-11, 3, 11), true));
            listOfBlocks.emplace_back(scene::miniBlockData("copper_block", glm::vec3(-10, 0, 11), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-10, 0, 12), true));
            listOfBlocks.emplace_back(scene::miniBlockData("copper_block", glm::vec3(-10, 0, 13), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-10, 3, 12), true));
            listOfBlocks.emplace_back(scene::miniBlockData("copper_block", glm::vec3(-9, 0, 12), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-9, 0, 13), true));
            listOfBlocks.emplace_back(scene::miniBlockData("copper_block", glm::vec3(-9, 0, 14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-9, 3, 13), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-9, 9, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-9, 10, -6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("copper_block", glm::vec3(-8, 0, 13), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-8, 0, 14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-8, 3, 14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-8, 7, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-8, 8, -6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-7, 5, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-7, 6, -6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-6, 3, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-6, 4, -6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(-3, 0, 19), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(-2, 3, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(0, 0, 19), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(0, 2, 1), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(0, 2, 2), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(0, 2, 3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(0, 2, 4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(0, 2, 5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glowstone", glm::vec3(0, 3, 0), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glowstone", glm::vec3(0, 3, 6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(1, 2, 0), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(1, 2, 1), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(1, 2, 2), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(1, 2, 3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(1, 2, 4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(1, 2, 5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(1, 2, 6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(2, 0, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(2, 1, -13), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(2, 2, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(2, 2, 0), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(2, 2, 1), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(2, 2, 2), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(2, 2, 3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(2, 2, 4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(2, 2, 5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(2, 2, 6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("magma", glm::vec3(3, 0, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(3, 2, -12), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(3, 2, 0), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(3, 2, 1), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(3, 2, 2), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(3, 2, 3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(3, 2, 4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(3, 2, 5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(3, 2, 6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(4, 2, 0), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(4, 2, 1), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(4, 2, 2), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(4, 2, 3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(4, 2, 4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(4, 2, 5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(4, 2, 6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("yellow", glm::vec3(4, 3, 8), true));
            listOfBlocks.emplace_back(scene::miniBlockData("yellow", glm::vec3(4, 3, 10), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glowstone", glm::vec3(4, 6, 8), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(5, 2, 0), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(5, 2, 1), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(5, 2, 2), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(5, 2, 3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("obsidian", glm::vec3(5, 2, 4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("iron_block", glm::vec3(5, 2, 5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(5, 2, 6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("yellow", glm::vec3(5, 2, 12), true));
            listOfBlocks.emplace_back(scene::miniBlockData("sea_lantern", glm::vec3(5, 4, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(6, 0, 20), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(6, 0, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(6, 0, 22), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(6, 2, 1), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(6, 2, 2), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(6, 2, 3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(6, 2, 4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(6, 2, 5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glowstone", glm::vec3(6, 3, 0), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glowstone", glm::vec3(6, 3, 6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(7, 0, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(7, 0, 20), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(7, 0, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(7, 0, 22), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glowstone", glm::vec3(7, 1, 19), true));
            listOfBlocks.emplace_back(scene::miniBlockData("marccoin_block", glm::vec3(7, 1, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glowstone", glm::vec3(7, 1, 23), true));
            listOfBlocks.emplace_back(scene::miniBlockData("marccoin_block", glm::vec3(7, 2, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(7, 3, -15), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(7, 3, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(7, 3, -13), true));
            listOfBlocks.emplace_back(scene::miniBlockData("marccoin_block", glm::vec3(7, 3, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("marccoin_block", glm::vec3(7, 4, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("marccoin_block", glm::vec3(7, 5, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("marccoin_block", glm::vec3(7, 6, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("marccoin_block", glm::vec3(7, 7, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("marccoin_block", glm::vec3(7, 8, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 0, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(8, 0, 20), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(8, 0, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("diamond_ore", glm::vec3(8, 0, 22), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 3, -15), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 3, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 3, -13), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 3, -9), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 3, -8), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 3, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 6, -9), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 6, -8), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(8, 6, -7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(9, 0, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glowstone", glm::vec3(9, 1, 21), true));
            listOfBlocks.emplace_back(scene::miniBlockData("yellow", glm::vec3(9, 2, 12), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(9, 3, -15), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(9, 3, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(9, 3, -13), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(10, 0, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(14, 2, 14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glass", glm::vec3(14, 8, -6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glass", glm::vec3(14, 8, -5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glass", glm::vec3(14, 8, -4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glass", glm::vec3(14, 8, -3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glass", glm::vec3(14, 9, -6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glass", glm::vec3(14, 9, -5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glass", glm::vec3(14, 9, -4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("glass", glm::vec3(14, 9, -3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("red", glm::vec3(15, 0, -14), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(15, 3, 10), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(15, 4, 7), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(15, 5, 4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(15, 7, -6), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(15, 7, -5), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(15, 7, -4), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(15, 7, -3), true));
            listOfBlocks.emplace_back(scene::miniBlockData("orange", glm::vec3(17, 6, 1), true));
            break;
        default:
            std::cout << "Basic Super Flat World selected\n";
            listOfBlocks.emplace_back(scene::miniBlockData("bedrock", glm::vec3(0,0,0), false, true));
            listOfBlocks.emplace_back(scene::miniBlockData("dirt", glm::vec3(0,1,0), false, true));
            listOfBlocks.emplace_back(scene::miniBlockData("dirt", glm::vec3(0,2,0), false, true));
            listOfBlocks.emplace_back(scene::miniBlockData("grass_block", glm::vec3(0,3,0), false, true));
            break;
    }

    // Setting up all the render informations
    renderer::renderer_t renderInfo;
    renderInfo.initialise(WIN_HEIGHT, WIN_WIDTH);
    scene::world gameWorld(listOfBlocks, renderDistance, worldWidth, &renderInfo);


    // SETTING UP ALL CALLBACKS
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        // Close program if esc is pressed
        if (action != GLFW_PRESS) return;
        pointerInformation *info = (pointerInformation *) glfwGetWindowUserPointer(win);
        if (key != GLFW_KEY_I && key != GLFW_KEY_TAB) info->gameWorld->toggleInstructions(true);
        switch(key) {
            case GLFW_KEY_W:
                // Double tap to run
                if ((float)glfwGetTime() - info->lastWPressed <= W_PRESS_SPACE) {
                    info->gameWorld->runningMode = true;
                } else {
                    info->lastWPressed = glfwGetTime();
                }
                break;
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
                // Printing debug information
                std::cout << "X: " << info->gameWorld->playerCamera.pos.x << " ";
                std::cout << "Y: " << info->gameWorld->playerCamera.pos.y << " ";
                std::cout << "Z: " << info->gameWorld->playerCamera.pos.z << "\n";
                std::cout << "Yaw: " << info->gameWorld->playerCamera.yaw << "\n";
                std::cout << "Pitch: " << info->gameWorld->playerCamera.pitch << "\n";
                std::cout << "Player Vertical Velocity: " << info->gameWorld->playerCamera.yVelocity << "\n";
                std::cout << "Current frame rate: " << info->frameRate << " frames per second\n\n";
                break;
            case GLFW_KEY_C:
                info->gameWorld->toggleCutscene();
                break;
            case GLFW_KEY_O:
                info->gameWorld->convertCurrWorldIntoData();
                break;
            case GLFW_KEY_I:
                info->gameWorld->toggleInstructions(!(info->gameWorld->getInstructionStatus()));
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

    glfwSetScrollCallback(window, [](GLFWwindow *win, double xoffset, double yoffset) {
        pointerInformation *info = (pointerInformation *) glfwGetWindowUserPointer(win);

        if (yoffset > 0) {
            info->gameWorld->scrollHotbar(1);
        } else {
            info->gameWorld->scrollHotbar(-1);
        }
    });

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


    auto xPos = gameWorld.terrain.size() / 2;
    auto zPos = gameWorld.terrain[0][0].size() / 2;
    gameWorld.playerCamera = player::createCamera(glm::vec3(xPos, 5, zPos), glm::vec3(xPos, -10.0f, zPos));
    gameWorld.cutsceneCamera = player::createCamera(glm::vec3(xPos, 5, zPos), glm::vec3(xPos, -10.0f, zPos));

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

    glm::vec3 sunPosition;
    float degrees = 90;

    glUseProgram(renderInfo.program);

    gameWorld.updateBlocksToRender(true);

    float totalTime = 0;
    float totalFrames = 0;

    glfwShowWindow(window);
    glfwFocusWindow(window);
    // RENDER LOOP
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
            degrees += 20.0f * dt;
        } else {
            degrees += 0.25f * dt;
        }
        
        if (degrees >= 360) {
            gameWorld.updateMoonPhase();
            degrees -= 360.0f;
        }

        // Changing and updating where the sun will be
        sunPosition = glm::vec3(gameWorld.getCurrCamera()->pos.x + (renderDistance) * glm::cos(glm::radians(degrees)), gameWorld.getCurrCamera()->pos.y + (renderDistance - 10) * glm::sin(glm::radians(degrees)), gameWorld.getCurrCamera()->pos.z);
        renderInfo.sun_light_dir = glm::normalize(gameWorld.getCurrCamera()->pos - sunPosition);
        renderInfo.changeSunlight(degrees);

        if (gameWorld.getCutsceneStatus()) {
            gameWorld.animateCutscene();
        } else {
            gameWorld.updatePlayerPositions(window, dt, &renderInfo);
        }

        gameWorld.updateSunPosition(degrees, renderInfo.getSkyColor(degrees), dt);
        gameWorld.drawWorld(renderInfo);
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Not using frame limiter
        // not entirely correct as a frame limiter, but close enough
        // it would be more correct if we knew how much time this frame took to render
        // and calculated the distance to the next "ideal" time to render and only slept that long
        // the current way just always sleeps for 16.67ms, so in theory we'd drop frames
        if (frameLimiter != 0) std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(1000.f / 60));
    }
    
    std::cout << "Terminating program, please standby as everything gets wiped out...\n";
    // deleting the whole window also removes the opengl context, freeing all our memory in one fell swoop.
    renderInfo.deleteProgram();
    gameWorld.destroyEverthing();
    chicken3421::delete_opengl_window(window);

    std::cout << "Good bye!\n";

    return EXIT_SUCCESS;
}
