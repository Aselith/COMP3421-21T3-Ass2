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

#include <vector>
#include <iostream>

namespace scene {
    const size_t WORLD_WIDTH = 150;
    const size_t WORLD_HEIGHT = 50;

    const float GRAVITY = 1;
    const float CAMERA_SPEED = 5;
    const float JUMP_POWER = 0.25;
    const float EYE_LEVEL = 1.0f;
    const float PLAYER_RADIUS = 0.25f;
    const int RENDER_DISTANCE = 25;
    

    struct node_t {
        static_mesh::mesh_t mesh;
        GLuint texture = 0;
        glm::vec3 translation = glm::vec3(0.0);
        glm::vec3 rotation = glm::vec3(0.0);
        glm::vec3 scale = glm::vec3(1.0);
        std::vector<node_t> children;

        int x = 0, y = 0, z = 0;
        bool noCollisionBox = true;
        bool air = true;
    };
    
    /**
     * Render the given node and its children recursively
     * @param node - the node to render
     * @param parent_mvp - the parent's model view transform
     * @param mvp_loc - the location of the uniform mvp variable in the render program
     */
    void drawBlock(const node_t *node, const glm::mat4 &parent_mvp, GLint mvp_loc, std::vector<bool> faces);
    void drawElement(const node_t *node, const glm::mat4 &parent_mvp, GLint mvp_loc);

    node_t createBlock(int x, int y, int z, GLuint texID);
    node_t createFlatSquare(GLuint texID);
    void destroy(const node_t *node);

    // WORLD = Everything that shows up on the screen is controlled from here

    struct world {

        float walkingMultiplier = 0.5f;
        player::playerPOV playerCamera;
        int increments = 150;
        int playerReachRange = 4 * increments;
        int groundLevel = -99999;
        int aboveLevel = 99999;

        std::vector<std::vector<std::vector<node_t>>> terrain = {WORLD_WIDTH , std::vector< std::vector<node_t> > (WORLD_HEIGHT, std::vector<node_t> (WORLD_WIDTH) ) };
        node_t screen;
    
        world() {
            std::cout << "World Created\n";
            texture_2d::params_t parameters;
            parameters.filter_min = GL_NEAREST;
            parameters.filter_max = GL_NEAREST;

            node_t crosshair = scene::createFlatSquare(texture_2d::init("./res/textures/crosshair.png", parameters));
            crosshair.translation.z = -1 * PLAYER_RADIUS;
            crosshair.scale = glm::vec3(0.02, 0.02, 0.02);

            screen.children.push_back(crosshair);
        }
    
        void rightClickPlace(GLuint texID) {
            glm::vec3 lookingDirection = player::getLookingDirection(playerCamera, increments);
            float rayX = playerCamera.pos.x, rayY = playerCamera.pos.y, rayZ = playerCamera.pos.z;

            if (isCoordOutBoundaries((int)round(rayX), (int)round(rayY), (int)round(rayZ))) {
                return;
            }
            int limit = 0;
            while (terrain[(int)round(rayX)][(int)round(rayY)][(int)round(rayZ)].air && limit < playerReachRange) {
                
                rayX += lookingDirection.x;
                rayY += lookingDirection.y;
                rayZ += lookingDirection.z;
                limit++;
                
                if (isCoordOutBoundaries((int)round(rayX), (int)round(rayY), (int)round(rayZ))) {
                    return;
                }
            }

            int placeX = (int)round(rayX - lookingDirection.x);
            int placeY = (int)round(rayY - lookingDirection.y);
            int placeZ = (int)round(rayZ - lookingDirection.z);
            
            if (terrain[placeX][placeY][placeZ].air && limit < playerReachRange) {
                placeBlock(scene::createBlock(placeX, placeY, placeZ, texID));
                if (checkInsideBlock()) {
                    terrain[placeX][placeY][placeZ].air = true;
                }
            }
        }

        void leftClickDestroy() {

            glm::vec3 lookingDirection = player::getLookingDirection(playerCamera, increments);

            float rayX = playerCamera.pos.x, rayY = playerCamera.pos.y, rayZ = playerCamera.pos.z;

            if (isCoordOutBoundaries((int)round(rayX), (int)round(rayY), (int)round(rayZ))) {
                return;
            }
            int limit = 0;
            while (terrain[(int)round(rayX)][(int)round(rayY)][(int)round(rayZ)].air && limit < playerReachRange) {
                
                rayX += lookingDirection.x;
                rayY += lookingDirection.y;
                rayZ += lookingDirection.z;
                limit++;
                
                if (isCoordOutBoundaries((int)round(rayX), (int)round(rayY), (int)round(rayZ))) {
                    return;
                }
            }

            if (!terrain[(int)round(rayX)][(int)round(rayY)][(int)round(rayZ)].air) {
                terrain[(int)round(rayX)][(int)round(rayY)][(int)round(rayZ)].air = true;
            }
        }

        void placeBlock(node_t block) {
            int blockX = block.x, blockY = block.y, blockZ = blockY;
            terrain[blockX][block.y][block.z] = block;
            return;
        }

        /**
         * Function that controls how the player moves and obstructions by
         * blocks
         */
        void updatePlayerPositions(GLFWwindow *window, float dt) {
            player::update_player_camera(playerCamera, window, dt);

            float step = dt * CAMERA_SPEED * walkingMultiplier;
            if (playerCamera.yVelocity == 0) {
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
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && playerCamera.yVelocity == 0) {
                playerCamera.yVelocity = JUMP_POWER;
            }
            
            // Enacting gravity onto the camera
            playerCamera.pos.y += playerCamera.yVelocity;
            playerCamera.yVelocity = playerCamera.yVelocity - GRAVITY * dt;
            if (playerCamera.pos.y < groundLevel) {
                playerCamera.pos.y -= playerCamera.yVelocity;
                playerCamera.yVelocity = 0;
            }
            if (playerCamera.yVelocity > 0 && checkInsideBlock()) {
                playerCamera.pos.y -= 5 * playerCamera.yVelocity;
                playerCamera.yVelocity = -JUMP_POWER;
            }

            

            // World boundaries. REspawns when the y co-ordinate goes too low
            if (playerCamera.pos.y - EYE_LEVEL < -100) {
                std::cout << "Respawned\n";
                playerCamera.pos.y = 5;
                playerCamera.pos.x = terrain.size() / 2;
                playerCamera.pos.z = terrain[0][0].size() / 2;

                // playerCamera.yVelocity = 0;
            }

            if (playerCamera.yVelocity < 0) {
                groundLevel = findClosestBlockAboveBelow(-1);

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

        void drawWorld(const glm::mat4 &parent_mvp, GLint mvp_loc) {
            drawTerrain(parent_mvp, mvp_loc);
            drawscreen(parent_mvp, mvp_loc);
        }

        void drawTerrain(const glm::mat4 &parent_mvp, GLint mvp_loc) {
            

            glm::vec2 yRange = glm::vec2((int)(playerCamera.pos.y - RENDER_DISTANCE), (int)(playerCamera.pos.y + RENDER_DISTANCE));
            if (yRange.x < 0) {
                yRange.x = 0;
            }
            glm::vec2 xRange = glm::vec2((int)(playerCamera.pos.x - RENDER_DISTANCE), (int)(playerCamera.pos.x + RENDER_DISTANCE));
            if (xRange.x < 0) {
                xRange.x = 0;
            }
            glm::vec2 zRange = glm::vec2((int)(playerCamera.pos.z - RENDER_DISTANCE), (int)(playerCamera.pos.z + RENDER_DISTANCE));
            if (zRange.x < 0) {
                zRange.x = 0;
            }

            for (int y = yRange.x; y < yRange.y; y++) {
                for (int z = zRange.x; z < zRange.y; z++) {
                    for (int x = xRange.x; x < xRange.y; x++) {

                        if (!terrain[x][y][z].air && utility::calculateDistance(playerCamera.pos, glm::vec3(x, y, z)) < RENDER_DISTANCE) {
                            
                            // Figuring out which sides should be rendered or not.
                            // Only render side if it has air next to it
                            std::vector<bool> faces(6, true);
                            int totalHidden = 0;

                            if (!isCoordOutBoundaries(x, y - 1, z) && !terrain[x][y - 1][z].air) {
                                faces[0] = false; // 0 bottom
                                totalHidden++;
                            }
                            if (!isCoordOutBoundaries(x, y + 1, z) && !terrain[x][y + 1][z].air) {
                                faces[1] = false; // 0 Is top
                                totalHidden++;
                            }
                            if (!isCoordOutBoundaries(x, y, z + 1) && !terrain[x][y][z + 1].air) {
                                faces[2] = false;
                                totalHidden++;
                            }
                            if (!isCoordOutBoundaries(x, y, z - 1) && !terrain[x][y][z - 1].air) {
                                faces[3] = false;
                                totalHidden++;
                            }
                            if (!isCoordOutBoundaries(x + 1, y, z) && !terrain[x + 1][y][z].air) {
                                faces[4] = false;
                                totalHidden++;
                            }
                            if (!isCoordOutBoundaries(x - 1, y, z) && !terrain[x - 1][y][z].air) {
                                faces[5] = false;
                                totalHidden++;
                            }
                            if (totalHidden < 6) {
                                drawBlock(&terrain[x][y][z], parent_mvp, mvp_loc, faces);
                            }
                            
                        }
                    }
                }
            }

        }

        void drawscreen(const glm::mat4 &parent_mvp, GLint mvp_loc) {

            screen.translation = playerCamera.pos;
            screen.rotation.x = playerCamera.pitch;
            screen.rotation.y = -playerCamera.yaw;

            drawElement(&screen, parent_mvp, mvp_loc);
        }
    };

}
#endif //COMP3421_SCENE_OBJECT_HPP
