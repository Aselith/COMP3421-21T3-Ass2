#ifndef COMP3421_SCENE_OBJECT_HPP
#define COMP3421_SCENE_OBJECT_HPP

#include "static_mesh.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <math.h>

#include <ass2/shapes.hpp>
#include <ass2/player.hpp>
#include <ass2/texture_2d.hpp>
#include <ass2/utility.hpp>
#include <ass2/renderer.hpp>

#include <vector>
#include <iostream>

namespace scene {
    const size_t WORLD_WIDTH = 125;
    const size_t WORLD_HEIGHT = 50;

    const float GRAVITY = 1;
    const float CAMERA_SPEED = 5;
    const float JUMP_POWER = 0.25;
    const float EYE_LEVEL = 1.0f;
    const float PLAYER_RADIUS = 0.35f;
    const float SCREEN_DISTANCE = 0.25f;
    const int RENDER_DISTANCE = 15;

    struct node_t {
        static_mesh::mesh_t mesh;
        GLuint texture = 0;
        glm::vec4 color = glm::vec4(1.0f);
		glm::vec3 diffuse = glm::vec4(1.0f);

        glm::vec3 translation = glm::vec3(0.0);
        glm::vec3 rotation = glm::vec3(0.0);
        glm::vec3 scale = glm::vec3(1.0);
        std::vector<node_t> children;

        int x = 0, y = 0, z = 0;
        bool air = true;
        bool transparent = true;
        bool illuminating = false;
    };
    
    /**
     * Render the given node and its children recursively
     * @param node - the node to render
     * @param parent_mvp - the parent's model view transform
     * @param mvp_loc - the location of the uniform mvp variable in the render program
     */
    void drawBlock(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo, std::vector<bool> faces);
    void drawElement(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo);

    node_t createBlock(int x, int y, int z, GLuint texID, bool invertNormals, bool affectedByLight);
    node_t createSkySphere(GLuint texID, float radius, int tesselation);
    node_t createFlatSquare(GLuint texID, bool invert);
    void destroy(const node_t *node);

    // WORLD = Everything that shows up on the screen is controlled from here

    struct world {

        float walkingMultiplier = 0.5f;
        player::playerPOV playerCamera;
        int increments = 200;
        int playerReachRange = 4 * increments;
        int groundLevel = -99999, aboveLevel = 99999;

        std::vector<std::vector<std::vector<node_t>>> terrain = {WORLD_WIDTH , std::vector< std::vector<node_t> > (WORLD_HEIGHT, std::vector<node_t> (WORLD_WIDTH) ) };
        std::vector<std::vector<std::vector<GLuint>>> blockMap = {WORLD_WIDTH , std::vector< std::vector<GLuint> > (WORLD_HEIGHT, std::vector<GLuint> (WORLD_WIDTH) ) };
        node_t screen;
        node_t centreOfWorldNode;
        node_t highlightedBlock;
        
        std::vector<GLuint> hotbar;
        std::vector<GLuint> hotbarSecondary;

        std::vector<int> hotbarTextureIndex;
        std::vector<GLuint> transparentTextures;
        std::vector<GLuint> illuminatingTextures;

        GLuint flyingIcon;

        int hotbarIndex = 0;
        int walkCycle = 0;
        int swingCycle = -1;
        int handIndex = 0, hotbarHUDIndex = 0, flyingIconIndex = 0, skySphereIndex = 0, skyIndex = 0;
        bool flyingMode = false;

        world() {
            
            texture_2d::params_t parameters;
            parameters.filter_min = GL_NEAREST;
            parameters.filter_max = GL_NEAREST;

            flyingIcon = texture_2d::init("./res/textures/flying_mode.png", parameters);

            highlightedBlock = scene::createBlock(0, 0, 0, texture_2d::init("./res/textures/highlight.png", parameters), false, true);
            highlightedBlock.scale = glm::vec3(1.001, 1.001, 1.001);
            // Setting up Sun
            node_t sun = scene::createBlock(0, 0, 0, texture_2d::init("./res/textures/sun.png", parameters), true, false);
            sun.scale = glm::vec3(4.0, 4.0, 4.0);
            sun.translation.x += (float)getSunDistance() - 15;
            node_t sunAura = scene::createBlock(0, 0, 0, texture_2d::init("./res/textures/sun_aura.png", parameters), true, false);
            sunAura.scale = glm::vec3(1.2, 1.2, 1.2);
            sun.children.push_back(sunAura);
            // Setting up Moon
            node_t moon = scene::createBlock(0, 0, 0, texture_2d::init("./res/textures/moon.png", parameters), false, false);
            moon.scale = glm::vec3(4.0, 4.0, 4.0);
            moon.translation.x -= (float)getSunDistance();

            // Generating random stars
            srand(time(0));
            GLuint starTexID = texture_2d::init("./res/textures/star.png", parameters);
            for (int i = 0; i < 777; i++) {
                node_t star = scene::createBlock(0, 0, 0, starTexID, false, false);
                int rng = rand() % 6;
                if (rng == 5) {
                    rng = 0;
                }
                switch (rng) {
                    case 0:
                        star.translation.x += 3;
                
                        star.translation.y += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f);
                        if (rand() % 2 == 0) {
                            star.translation.y *= -1;
                        }
                        star.translation.z += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f);
                        if (rand() % 2 == 0) {
                            star.translation.z *= -1;
                        }
                        break;
                    case 1:
                        star.translation.x += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f) - 1;
                        star.translation.y += RENDER_DISTANCE / 3;
                        star.translation.z += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f);
                        if (rand() % 2 == 0) {
                            star.translation.z *= -1;
                        }
                        break;
                    case 2:
                        star.translation.x += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f) - 1;
                        star.translation.y -= RENDER_DISTANCE / 3;
                        star.translation.z += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f);
                        if (rand() % 2 == 0) {
                            star.translation.z *= -1;
                        }
                        break;
                    case 3:
                        star.translation.x += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f) - 1;
                        star.translation.y += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f);
                        star.translation.z += RENDER_DISTANCE / 3;
                        if (rand() % 2 == 0) {
                            star.translation.y *= -1;
                        }
                        break;
                    case 4:
                        star.translation.x += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f) - 1;
                        star.translation.y += (float)((rand() % RENDER_DISTANCE) / 2) + (float)((rand() % 10) / 10.0f);
                        star.translation.z -= RENDER_DISTANCE / 3;
                        if (rand() % 2 == 0) {
                            star.translation.y *= -1;
                        }
                        break;
                }
                
                star.scale *= ((rand() % 6) + 2) / 100.0f;
                star.rotation.x += (rand() % 10) / 10.0f;
                star.rotation.y += (rand() % 10) / 10.0f;
                star.rotation.z += (rand() % 10) / 10.0f;
                moon.children.push_back(star);
            }

            node_t centreOfWorld;
            node_t skySphere = createSkySphere(0, (float)getSunDistance(), 256);
            // skySphere.texture = texture_2d::init("./res/textures/sky.png", parameters);
            skySphere.rotation = glm::vec3(0, 0, 90);
            skySphere.translation.x -= 15;
            skySphere.diffuse = glm::vec4((float)173/255, (float)216/255, (float)230/255, 1.0f);

            centreOfWorld.children.push_back(skySphere);
            skySphereIndex = centreOfWorld.children.size() - 1;
            centreOfWorld.children.push_back(sun);
            centreOfWorld.children.push_back(moon);
            centreOfWorldNode.children.push_back(centreOfWorld);
            skyIndex = centreOfWorldNode.children.size() - 1;

            // Setting up HUD

            // Crosshair
            node_t crosshair = scene::createFlatSquare(texture_2d::init("./res/textures/crosshair.png", parameters), false);
            crosshair.translation.z = -1 * SCREEN_DISTANCE;
            crosshair.scale = glm::vec3(0.02, 0.02, 0.02);
            screen.children.push_back(crosshair);

            // ADD BLOCKS HERE
            // First hotbar
            hotbar.push_back(texture_2d::init("./res/textures/dirt.png", parameters));
            GLuint dirtblockTexID = hotbar.back();
            hotbar.push_back(texture_2d::init("./res/textures/grass_block.png", parameters));
            GLuint grassblockTexID = hotbar.back();
            hotbar.push_back(texture_2d::init("./res/textures/oak_planks.png", parameters));
            hotbar.push_back(texture_2d::init("./res/textures/oak_log.png", parameters));
            hotbar.push_back(texture_2d::init("./res/textures/oak_leaves.png", parameters));
            hotbar.push_back(texture_2d::init("./res/textures/cobblestone.png", parameters));
            hotbar.push_back(texture_2d::init("./res/textures/mossy_cobblestone.png", parameters));
            hotbar.push_back(texture_2d::init("./res/textures/stone.png", parameters));
            hotbar.push_back(texture_2d::init("./res/textures/stone_bricks.png", parameters));
            hotbar.push_back(texture_2d::init("./res/textures/glass.png", parameters));
            transparentTextures.push_back(hotbar.back());
            hotbar.push_back(texture_2d::init("./res/textures/sea_lantern.png", parameters));
            illuminatingTextures.push_back(hotbar.back());
            hotbar.push_back(texture_2d::init("./res/textures/tnt.png", parameters));
            hotbar.push_back(texture_2d::init("./res/textures/bedrock.png", parameters));
            GLuint bedrockTexID = hotbar.back();
            // Second hotbar
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/white.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/orange.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/magenta.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/light_blue.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/yellow.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/lime.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/pink.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/gray.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/light_gray.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/cyan.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/purple.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/blue.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/brown.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/green.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/red.png", parameters));
            hotbarSecondary.push_back(texture_2d::init("./res/textures/blocks/wool/black.png", parameters));

            // Hand
            node_t blockHand = scene::createBlock(0, 0, 0, hotbar[0], false, true);
            blockHand.translation.z = -1 * SCREEN_DISTANCE;
            blockHand.translation.y -= 0.15;
            blockHand.translation.x += 0.25;
            blockHand.scale = glm::vec3(0.15, 0.15, 0.15);

            screen.children.push_back(blockHand);
            handIndex = screen.children.size() - 1;

            node_t flyingIconNode = scene::createFlatSquare(flyingIcon, false);
            flyingIconNode.translation.z = -2.8 * SCREEN_DISTANCE;
            flyingIconNode.translation.y += 0.089f;
            flyingIconNode.translation.x -= 0.17f;
            flyingIconNode.scale = glm::vec3(0.03, 0.03, 1);
            flyingIconNode.air = true;
            screen.children.push_back(flyingIconNode);
            flyingIconIndex = screen.children.size() - 1;

            node_t hotbarTexture = scene::createFlatSquare(texture_2d::init("./res/textures/hotbar.png", parameters), false);
            hotbarTexture.translation.z = -2.8 * SCREEN_DISTANCE;
            hotbarTexture.translation.y -= 0.03f;
            hotbarTexture.scale = glm::vec3(0.3, 0.3, 1);
            screen.children.push_back(hotbarTexture);
            hotbarHUDIndex = screen.children.size() - 1;

            // Filling up the hotbar
            float xPos = -0.3;
            for (int i = 0; i < 9; i++) {
                node_t hudHotbar = scene::createBlock(0, 0, 0, hotbar[1], false, false);
                hudHotbar.translation.z += 0.55;
                hudHotbar.translation.y += 0.321;
                hudHotbar.translation.x = xPos;
                xPos += 0.075;
                hudHotbar.rotation = glm::vec3(45, 31, 30);
                hudHotbar.scale = glm::vec3(0.035, 0.035, 0.0001);
                screen.children[hotbarHUDIndex].children.push_back(hudHotbar);
                hotbarTextureIndex.push_back(screen.children[hotbarHUDIndex].children.size() - 1);
            }

            // Update the textures of the hotbar
            scrollHotbar(1);
            scrollHotbar(-1);

            for (int x = 0; x < blockMap.size(); x++) {
                for (int z = 0; z < blockMap[0][0].size(); z++) {
                    for (int y = 0; y < blockMap[0].size(); y++) {
                        blockMap[x][y][z] = -1;
                    }
                }
            }

            // Generating the new world
            for (int x = 0; x < blockMap.size(); x++) {
                for (int z = 0; z < blockMap[0][0].size(); z++) {
                    blockMap[x][0][z] = bedrockTexID;
                    blockMap[x][1][z] = grassblockTexID;
                }
            }

            std::cout << "World Created\n";
        }

        void switchHotbars() {
            
            std::vector<GLuint> tempHotbar = hotbar;
            hotbar.clear();
            for (auto i : hotbarSecondary) {
                hotbar.push_back(i);
            }
            hotbarSecondary.clear();
            for (auto i : tempHotbar) {
                hotbarSecondary.push_back(i);
            }
            hotbarIndex = 0;
            scrollHotbar(-1);
            scrollHotbar(1);
        }

        void updateSunPosition(float degree, glm::vec3 skyColor) {
            centreOfWorldNode.children[skyIndex].translation = playerCamera.pos;
            centreOfWorldNode.children[skyIndex].translation.y -= EYE_LEVEL;
            centreOfWorldNode.children[skyIndex].rotation = glm::vec3(0,0,degree);
            centreOfWorldNode.children[skyIndex].children[skySphereIndex].diffuse = skyColor;

        }

        void bobHand() {
            // Only bob hand if the player is on the ground
            if (playerCamera.pos.y == (float)groundLevel + EYE_LEVEL) {
                screen.children[handIndex].translation.y += glm::sin(walkCycle * M_PI / 4) / 150;
                walkCycle += 1;
                walkCycle %= 8;
            }
        }

        void swingHand() {
            if (swingCycle >= 0) {

                screen.children[handIndex].rotation.x -= 10 * glm::sin(swingCycle * M_PI / 3);
                screen.children[handIndex].translation.y -= glm::sin(swingCycle * M_PI / 3) / 50;
                screen.children[handIndex].translation.z -= glm::sin(swingCycle * M_PI / 3) / 30;
                swingCycle += 1;
                swingCycle %= 6;
                if (swingCycle == 0) {
                    swingCycle = -1;
                }
            }
        }

        bool isTransparent(GLuint texID) {
            for (GLuint i : transparentTextures) {
                if (i == texID) {
                    return true;
                }
            }
            return false;
        }

        bool isIlluminating(GLuint texID) {
            for (GLuint i : illuminatingTextures) {
                if (i == texID) {
                    return true;
                }
            }
            return false;
        }

        void toggleMode() {
            flyingMode = !flyingMode;
            screen.children[flyingIconIndex].air = !flyingMode;
        }

        void scrollHotbar(int direction) {
            hotbarIndex += direction;

            if (hotbarIndex >= (int)hotbar.size()) {
                hotbarIndex = 0;
            } else if (hotbarIndex < 0) {
                hotbarIndex = (int)hotbar.size() + hotbarIndex;
            }
            screen.children[handIndex].texture = hotbar[hotbarIndex];

            int tempIndex = hotbarIndex - 4;
            if (tempIndex < 0) {
                tempIndex = hotbar.size() + tempIndex;
            }
            for (int i : hotbarTextureIndex) {
                screen.children[hotbarHUDIndex].children[i].texture = hotbar[tempIndex];
                tempIndex++;
                tempIndex %= hotbar.size();
            }

        }
    
        glm::vec3 findCursorBlock(bool giveBlockBefore) {
            glm::vec3 lookingDirection = player::getLookingDirection(playerCamera, increments);
            float rayX = playerCamera.pos.x, rayY = playerCamera.pos.y, rayZ = playerCamera.pos.z;

            if (isCoordOutBoundaries((int)round(rayX), (int)round(rayY), (int)round(rayZ))) {
                 return glm::vec3(-1, -1, -1);
            }
            int limit = 0;
            while (terrain[(int)round(rayX)][(int)round(rayY)][(int)round(rayZ)].air && limit < playerReachRange) {
                
                rayX += lookingDirection.x;
                rayY += lookingDirection.y;
                rayZ += lookingDirection.z;
                limit++;
                
                if (isCoordOutBoundaries((int)round(rayX), (int)round(rayY), (int)round(rayZ))) {
                    return glm::vec3(-1, -1, -1);
                }
            }
            if (limit >= playerReachRange) {
                return glm::vec3(-1, -1, -1);
            } else if (giveBlockBefore) {
                return glm::vec3((int)round(rayX - lookingDirection.x), (int)round(rayY - lookingDirection.y), (int)round(rayZ - lookingDirection.z));
            } else {
                return glm::vec3((int)round(rayX), (int)round(rayY), (int)round(rayZ));
            }
        }

        void rightClickPlace() {
            if (swingCycle == -1) swingCycle = 0;
            
            auto placeBlockVector = findCursorBlock(true);
            auto placeX = placeBlockVector.x, placeY = placeBlockVector.y, placeZ = placeBlockVector.z;
            
            if (isCoordOutBoundaries(placeX, placeY, placeZ)) {
                return;
            }
            
            if (terrain[placeX][placeY][placeZ].air) {
                placeBlock(scene::createBlock(placeX, placeY, placeZ, hotbar[hotbarIndex], false, !isIlluminating(hotbar[hotbarIndex])));

                terrain[placeX][placeY][placeZ].transparent = isTransparent(hotbar[hotbarIndex]);
                terrain[placeX][placeY][placeZ].illuminating = isIlluminating(hotbar[hotbarIndex]);

                if (checkInsideBlock()) {
                    terrain[placeX][placeY][placeZ].air = true;
                    terrain[placeX][placeY][placeZ].transparent = true;
                }
            }
        }

        void leftClickDestroy() {
            if (swingCycle == -1) swingCycle = 0;
            
            auto placeBlockVector = findCursorBlock(false);
            auto placeX = placeBlockVector.x, placeY = placeBlockVector.y, placeZ = placeBlockVector.z;
            
            if (isCoordOutBoundaries(placeX, placeY, placeZ)) {
                return;
            }

            if (!terrain[placeX][placeY][placeZ].air) {
                terrain[placeX][placeY][placeZ].air = true;
                terrain[placeX][placeY][placeZ].transparent = true;
            }
        }

        void placeBlock(node_t block) {
            int blockX = block.x, blockY = block.y, blockZ = blockY;

            block.transparent = isTransparent(block.texture);
            block.illuminating = isIlluminating(block.texture);

            terrain[blockX][block.y][block.z] = block;
            return;
        }

        int getSunDistance() {
            return 2 * RENDER_DISTANCE;
        }

        /**
         * Function that controls how the player moves and obstructions by
         * blocks
         */
        void updatePlayerPositions(GLFWwindow *window, float dt) {
            swingHand();
            player::update_player_camera(playerCamera, window, dt);

            if (flyingMode) {
                playerCamera.yVelocity = 0;
            }

            float step = dt * CAMERA_SPEED * walkingMultiplier;
            // Controls running
            if (playerCamera.yVelocity == 0 && playerCamera.pos.y == (float)groundLevel + EYE_LEVEL) {
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                    walkingMultiplier = 1.0f;
                } else {
                    walkingMultiplier = 0.5f;
                }
            }
            
            glm::mat4 trans = glm::translate(glm::mat4(1.0), -playerCamera.pos);
            trans *= glm::rotate(glm::mat4(1.0), -glm::radians(playerCamera.yaw), glm::vec3(0, 1, 0));
            trans *= glm::rotate(glm::mat4(1.0), glm::radians(playerCamera.pitch), glm::vec3(1, 0, 0));

            auto right = glm::vec3(trans[0]);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                playerCamera.pos.z += step * -glm::cos(glm::radians(playerCamera.yaw));
                if (checkInsideBlock()) {
                    playerCamera.pos.z -= step * -glm::cos(glm::radians(playerCamera.yaw));
                }
                playerCamera.pos.x += step * glm::sin(glm::radians(playerCamera.yaw));
                if (checkInsideBlock()) {
                    playerCamera.pos.x -= step * glm::sin(glm::radians(playerCamera.yaw));
                }
                bobHand();
                if (walkingMultiplier == 1.0f) {
                    bobHand();
                }
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                playerCamera.pos.z -= step * -glm::cos(glm::radians(playerCamera.yaw));
                if (checkInsideBlock()) {
                    playerCamera.pos.z += step * -glm::cos(glm::radians(playerCamera.yaw));
                }
                playerCamera.pos.x -= step * glm::sin(glm::radians(playerCamera.yaw));
                if (checkInsideBlock()) {
                    playerCamera.pos.x += step * glm::sin(glm::radians(playerCamera.yaw));
                }
                bobHand();
                if (walkingMultiplier == 1.0f) {
                    bobHand();
                }
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                playerCamera.pos.z += right.z * -step;
                if (checkInsideBlock()) {
                    playerCamera.pos.z -= right.z * -step;
                }
                playerCamera.pos.x += right.x * -step;
                if (checkInsideBlock()) {
                    playerCamera.pos.x -= right.x * -step;
                }
                bobHand();
                if (walkingMultiplier == 1.0f) {
                    bobHand();
                }
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                playerCamera.pos.z += right.z * step;
                if (checkInsideBlock()) {
                    playerCamera.pos.z -= right.z * step;
                }
                playerCamera.pos.x += right.x * step;
                if (checkInsideBlock()) {
                    playerCamera.pos.x -= right.x * step;
                }
                bobHand();
                if (walkingMultiplier == 1.0f) {
                    bobHand();
                }
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && playerCamera.yVelocity == 0) {
                if (!flyingMode) {
                    playerCamera.yVelocity = JUMP_POWER;
                } else {
                    playerCamera.pos.y += step;
                    if (checkInsideBlock()) {
                        playerCamera.pos.y -= step;
                    }
                }
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && playerCamera.pos.y != (float)groundLevel + EYE_LEVEL) {
                if (flyingMode) {
                    playerCamera.pos.y -= step;
                    if (playerCamera.pos.y < (float)groundLevel + EYE_LEVEL) {
                        playerCamera.pos.y = groundLevel + EYE_LEVEL;
                    }
                }
            }
            
            // Enacting gravity onto the camera
            if (!flyingMode) {
                playerCamera.pos.y += playerCamera.yVelocity;
                playerCamera.yVelocity = playerCamera.yVelocity - GRAVITY * 0.06;

                if (playerCamera.pos.y < groundLevel) {
                    playerCamera.pos.y -= playerCamera.yVelocity;
                    playerCamera.yVelocity = 0;
                }
                if (playerCamera.yVelocity > 0 && checkInsideBlock()) {
                    playerCamera.pos.y -= 5 * playerCamera.yVelocity;
                    playerCamera.yVelocity = -JUMP_POWER;
                }
            }

            // World boundaries. Respawns when the y co-ordinate goes too low or too high
            if (playerCamera.pos.y - EYE_LEVEL < -100 || playerCamera.pos.y > WORLD_HEIGHT - 1) {
                std::cout << "Respawned, you went too high or too low!\n";
                flyingMode = false;
                toggleMode();
                if (groundLevel > 0) {
                    playerCamera.pos.y = groundLevel;
                } else {
                    playerCamera.pos.y = 4;
                }

                if (checkInsideBlock()) {
                    terrain[(size_t)playerCamera.pos.x][(size_t)playerCamera.pos.y][(size_t)playerCamera.pos.z].air = true;
                    terrain[(size_t)playerCamera.pos.x][(size_t)playerCamera.pos.y - 1][(size_t)playerCamera.pos.z].air = true;
                }
                playerCamera.pos.x = terrain.size() / 2;
                playerCamera.pos.z = terrain[0][0].size() / 2;

                playerCamera.yVelocity = JUMP_POWER;
            }

            groundLevel = findClosestBlockAboveBelow(-1);

            if (playerCamera.yVelocity < 0) {

                // Below keeps player steady on ground
                if (playerCamera.pos.y - EYE_LEVEL < groundLevel) {
                    playerCamera.pos.y = groundLevel + EYE_LEVEL;
                    playerCamera.yVelocity = 0;
                }
            }
        
        }

        bool isCoordOutBoundaries(int x, int y, int z) {
            if (x < 0 || x > terrain.size() - 1) {
                return true;
            }
            if (z < 0 || z > terrain[0][0].size() - 1) {
                return true;
            }
            if (y < 0 || y > terrain[0].size() - 1) {
                return true;
            }
            return false;
        }

        bool checkInsideBlock() {
            float playerPosY = playerCamera.pos.y - EYE_LEVEL;
            float playerPosX = playerCamera.pos.x;
            float playerPosZ = playerCamera.pos.z;

            int xPosRd = (int)round(playerPosX);
            int yPosRd = (int)round(playerPosY);
            int zPosRd = (int)round(playerPosZ);


            // Checks around the player in a circle if they are touching a block or not
            for (float degree = 0; degree < 360.0f; degree += 5.0f) {
                xPosRd = (int)round(playerPosX + PLAYER_RADIUS * (float)glm::sin(glm::radians(degree)));
                zPosRd = (int)round(playerPosZ + PLAYER_RADIUS * (float)-glm::cos(glm::radians(degree)));
                if (isCoordOutBoundaries(xPosRd, yPosRd, zPosRd)) {
                    continue;
                }
                if (!terrain[xPosRd][yPosRd][zPosRd].air || !terrain[xPosRd][yPosRd + 1][zPosRd].air) {
                    return true;
                }
            }
            return false;
            
        }
         
        int findClosestBlockAboveBelow(int direction) {
            
            float playerPosY = playerCamera.pos.y - EYE_LEVEL;
            float playerPosX = playerCamera.pos.x;
            float playerPosZ = playerCamera.pos.z;

            for (int i = 0; abs(i) < 100; i += direction) {
                int xPosRd = (int)round(playerPosX);
                int yPosRd = (int)round(playerPosY) + i;
                int zPosRd = (int)round(playerPosZ);

                // Checks around the player in a circle if they are touching a block or not
                for (float degree = 0; degree < 360.0f; degree += 5.0f) {
                    xPosRd = (int)round(playerPosX + PLAYER_RADIUS * (float)glm::sin(glm::radians(degree)));
                    zPosRd = (int)round(playerPosZ + PLAYER_RADIUS * (float)-glm::cos(glm::radians(degree)));
                    if (isCoordOutBoundaries(xPosRd, yPosRd, zPosRd) || isCoordOutBoundaries(xPosRd, yPosRd + 1, zPosRd)) {
                        continue;
                    }
                    if (!terrain[xPosRd][yPosRd][zPosRd].air || !terrain[xPosRd][yPosRd + 1][zPosRd].air) {
                        return yPosRd - direction;
                    }
                }
                
            }
            return direction * 999999;
        }

        void drawWorld(const glm::mat4 &parent_mvp, renderer::renderer_t renderInfo) {

            glUseProgram(renderInfo.program);
            glClearColor(0.f, 0.f, 0.2f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUniform3fv(renderInfo.sun_direction_loc, 1, glm::value_ptr(renderInfo.sun_light_dir));
            glUniform3fv(renderInfo.sun_color_loc, 1, glm::value_ptr(renderInfo.sun_light_color));
            glUniform1f(renderInfo.sun_ambient_loc, renderInfo.sun_light_ambient);

            auto view_proj = renderInfo.projection * player::get_view(playerCamera);
		    glUniformMatrix4fv(renderInfo.view_proj_loc, 1, GL_FALSE, glm::value_ptr(view_proj));

            drawElement(&centreOfWorldNode, glm::mat4(1.0f), renderInfo);
            drawTerrain(glm::mat4(1.0f), renderInfo);

            highlightedBlock.translation = findCursorBlock(false);
            if (!isCoordOutBoundaries(highlightedBlock.translation.x, highlightedBlock.translation.y, highlightedBlock.translation.z)) {
                drawElement(&highlightedBlock, glm::mat4(1.0f), renderInfo);
            }

            drawscreen(glm::mat4(1.0f), renderInfo);

            
        }

        void drawTerrain(const glm::mat4 &parent_mvp, renderer::renderer_t renderInfo) {

            std::vector<glm::vec3> transparentBlocks;

            glm::vec3 pos = playerCamera.pos;

            pos.x -= glm::sin(glm::radians(90.0f)) * glm::sin(glm::radians(playerCamera.yaw));
            pos.z -= -glm::sin(glm::radians(90.0f)) * glm::cos(glm::radians(playerCamera.yaw));
            glm::vec2 yRange = glm::vec2((int)(pos.y - RENDER_DISTANCE), (int)(pos.y + RENDER_DISTANCE));
            glm::vec2 xRange = glm::vec2((int)(pos.x - RENDER_DISTANCE), (int)(pos.x + RENDER_DISTANCE));
            glm::vec2 zRange = glm::vec2((int)(pos.z - RENDER_DISTANCE), (int)(pos.z + RENDER_DISTANCE));

            std::vector<glm::vec3> returnVector;

            auto pointA = glm::vec3 (
                pos.x + glm::sin(glm::radians(90.0f)) * glm::sin(glm::radians(playerCamera.yaw - 90.0f)) * RENDER_DISTANCE,
                0,
                pos.z + -glm::sin(glm::radians(90.0f)) * glm::cos(glm::radians(playerCamera.yaw - 90.0f)) * RENDER_DISTANCE
            );

            auto pointB = glm::vec3 (
                pos.x + glm::sin(glm::radians(90.0f)) * glm::sin(glm::radians(playerCamera.yaw - 90.0f)) * RENDER_DISTANCE,
                0,
                pos.z + -glm::sin(glm::radians(90.0f)) * glm::cos(glm::radians(playerCamera.yaw - 90.0f)) * RENDER_DISTANCE
            );

            for (int y = yRange.x; y < yRange.y; y++) {
                for (int z = zRange.x; z < zRange.y; z++) {
                    for (int x = xRange.x; x < xRange.y; x++) {
    
                        bool renderBlock = false;

                        if (isCoordOutBoundaries(x, y, z)) {
                            continue;
                        } else if (utility::calculateDistance(glm::vec3(x, y, z), glm::vec3(pos.x, pos.y, pos.z)) <= RENDER_DISTANCE) {

                            if (abs(playerCamera.pitch) > 50.0f) {
                                renderBlock = true;
                            } else {
                                glm::vec3 vec1 = pointB - glm::vec3(pos.x, 0, pos.z);
                                glm::vec3 vec2 = glm::vec3(x, 0, z) - glm::vec3(pos.x, 0, pos.z);
                                auto product = glm::cross(vec1, vec2);
                                if (product.y <= 0) {
                                    renderBlock = true;
                                }
                            }

                            if (renderBlock && !terrain[x][y][z].air) {
                                // Figuring out which sides should be rendered or not.
                                // Only render side if it has air next to it
                                std::vector<bool> faces(6, true);

                                if (terrain[x][y][z].transparent) {
                                    transparentBlocks.push_back(glm::vec3(x, y, z));
                                    continue;
                                }

                                getHiddenFaces(x, y, z, faces, true);

                                if (utility::countFalses(faces) < 6) {
                                    drawBlock(&terrain[x][y][z], parent_mvp, renderInfo, faces);
                                }
                                
                            } else if (blockMap[x][y][z] != -1) {
                                placeBlock(scene::createBlock(x, y, z, blockMap[x][y][z], false, true));
                                blockMap[x][y][z] = -1;
                            }

                        }

                    }
                }
            }


            for (auto i : transparentBlocks) {
                std::vector<bool> faces(6, true);
                getHiddenFaces(i.x, i.y, i.z, faces, false);
                if (utility::countFalses(faces) < 6) {
                    drawBlock(&terrain[i.x][i.y][i.z], parent_mvp, renderInfo, faces);
                }
            }


        }

        void getHiddenFaces(int x, int y, int z, std::vector<bool> &faces, bool glassIncluded) {
            if (!isCoordOutBoundaries(x, y - 1, z) && !(terrain[x][y - 1][z].air || glassIncluded * terrain[x][y - 1][z].transparent)) {
                faces[0] = false; // 0 bottom
            }
            if (!isCoordOutBoundaries(x, y + 1, z) && !(terrain[x][y + 1][z].air || glassIncluded * terrain[x][y + 1][z].transparent)) {
                faces[1] = false; // 0 Is top
            }
            if (!isCoordOutBoundaries(x, y, z + 1) && !(terrain[x][y][z + 1].air || glassIncluded * terrain[x][y][z + 1].transparent)) {
                faces[2] = false;
            }
            if (!isCoordOutBoundaries(x, y, z - 1) && !(terrain[x][y][z - 1].air || glassIncluded * terrain[x][y][z - 1].transparent)) {
                faces[3] = false;
            }
            if (!isCoordOutBoundaries(x + 1, y, z) && !(terrain[x + 1][y][z].air || glassIncluded * terrain[x + 1][y][z].transparent)) {
                faces[4] = false;
            }
            if (!isCoordOutBoundaries(x - 1, y, z) && !(terrain[x - 1][y][z].air || glassIncluded * terrain[x - 1][y][z].transparent)) {
                faces[5] = false;
            }
        }

        void drawscreen(const glm::mat4 &parent_mvp, renderer::renderer_t renderInfo) {

            screen.translation = playerCamera.pos;
            screen.rotation.x = playerCamera.pitch;
            screen.rotation.y = -playerCamera.yaw;

            drawElement(&screen, parent_mvp, renderInfo);
        }
    };

}
#endif //COMP3421_SCENE_OBJECT_HPP
