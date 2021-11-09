#ifndef COMP3421_SCENE_OBJECT_HPP
#define COMP3421_SCENE_OBJECT_HPP

#include "static_mesh.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <ass2/frustum.hpp>
#include <ass2/shapes.hpp>
#include <ass2/player.hpp>
#include <ass2/texture_2d.hpp>
#include <ass2/utility.hpp>
#include <ass2/renderer.hpp>

#include <math.h>
#include <vector>
#include <iostream>

namespace scene {
    const size_t WORLD_WIDTH = 110;
    const size_t WORLD_HEIGHT = 50;

    const size_t MAX_STARS = 200;

    const float GRAVITY = -9.8f;
    const float CAMERA_SPEED = 8.0f;
    const float JUMP_POWER = 4.2f;
    const float PLAYER_RADIUS = 0.35f;
    const float SCREEN_DISTANCE = 0.25f;
    const int RENDER_DISTANCE = 20;

    struct node_t {
        static_mesh::mesh_t mesh;

        // Materials
        GLuint textureID = 0;
        GLuint specularID = 0;
        glm::vec4 color = glm::vec4(1.0f);
        glm::vec3 ambient = glm::vec3(1.0f);
		glm::vec3 diffuse = glm::vec3(1.0f);
		glm::vec4 specular = glm::vec4(1.0f);
		float phong_exp = 5.0f;

        glm::vec3 translation = glm::vec3(0.0);
        glm::vec3 rotation = glm::vec3(0.0);
        glm::vec3 scale = glm::vec3(1.0);
        std::vector<node_t> children;
        std::vector<bool> culledFaces = {true, true, true, true, true, true};

        int x = 0, y = 0, z = 0;
        int lightID = -1;
        bool air = true, transparent = true, illuminating = false, ignoreCulling = false;
    };

    struct blockData {
        GLuint texture = 0;
        GLuint specularMap = 0;
        glm::vec3 rgb = {0, 0, 0};
        bool transparent = true;
        bool illuminating = false;
        bool rotatable = false;
        float intensity = 1.0f;
        std::string blockName;
    };

    struct miniBlockData {
        std::string blockName;
        glm::vec3 position;
        miniBlockData(std::string name, glm::vec3 pos) {
            blockName = name;
            position = pos;
        }
    };
    
    /**
     * Render the given node and its children recursively
     * @param node - the node to render
     * @param parent_mvp - the parent's model view transform
     * @param mvp_loc - the location of the uniform mvp variable in the render program
     */
    void drawBlock(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo, GLuint defaultSpecular);
    void drawElement(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo, GLuint defaultSpecular);

    blockData combineBlockData(std::string stringName, bool transparent, bool illuminating, bool rotatable = false, glm::vec3 color = {0, 0, 0}, float intensity = 1.0f);

    node_t createBlock(int x, int y, int z, GLuint texID, GLuint specID, bool invertNormals, bool affectedByLight);
    node_t createBlock(int x, int y, int z, blockData data, bool invertNormals, bool affectedByLight);
    node_t createSkySphere(GLuint texID, float radius, int tesselation);
    node_t createFlatSquare(GLuint texID, bool invert);
    node_t createBedPlayer(GLuint bedTexID, GLuint playerTexID);
    void destroy(const node_t *node, bool destroyTexture);

    // WORLD = Everything that shows up on the screen is controlled from here

    struct world {

        float eyeLevel = 1.0f;
        bool shiftMode = false;
        float walkingMultiplier = 0.5f;
        bool cutsceneEnabled = false;
        player::playerPOV playerCamera, cutsceneCamera;
        glm::vec3 oldPos, oldHandPos, oldHandRotation, lastRenderedPos;
        int increments = 200;
        int playerReachRange = 4 * increments;
        int groundLevel = -99999;
        float cutsceneTick = 0;
        float swingCycle = -1.0f, walkCycle = 0.0f;
        GLuint defaultSpecular = texture_2d::init("./res/textures/blocks/default_specular.png");
        std::vector<GLuint> moonPhases;
        int moonPhase = 0;

        std::vector<std::vector<std::vector<node_t>>> terrain = {WORLD_WIDTH , std::vector< std::vector<node_t> > (WORLD_HEIGHT, std::vector<node_t> (WORLD_WIDTH) ) };
        std::vector<std::vector<std::vector<blockData>>> blockMap = {WORLD_WIDTH , std::vector< std::vector<blockData> > (WORLD_HEIGHT, std::vector<blockData> (WORLD_WIDTH) ) };
        std::vector<node_t *> listOfBlocksToRender;

        node_t screen;
        node_t centreOfWorldNode;
        node_t highlightedBlock;
        node_t bed;
        
        std::vector<blockData> hotbar;
        std::vector<blockData> hotbarSecondary;

        std::vector<int> hotbarTextureIndex;

        int hotbarIndex = 0;
        int handIndex = 0, hotbarHUDIndex = 0, flyingIconIndex = 0, skySphereIndex = 0, skyIndex = 0, moonIndex = 0, instructionIndex = 0;
        bool flyingMode = false, startSwingHandAnim = false;

        world(std::vector<std::string> listOfBlocks) {
            std::cout << "Generating world, please standby...\n";
            GLuint flyingIcon = texture_2d::init("./res/textures/flying_mode.png");

            bed = createBedPlayer(texture_2d::init("./res/textures/bed.png"), texture_2d::init("./res/textures/player.png"));

            // Setting up moon phases
            moonPhases.push_back(texture_2d::init("./res/textures/blocks/moon/moon_0.png"));
            moonPhases.push_back(texture_2d::init("./res/textures/blocks/moon/moon_1.png"));
            moonPhases.push_back(texture_2d::init("./res/textures/blocks/moon/moon_2.png"));
            moonPhases.push_back(texture_2d::init("./res/textures/blocks/moon/moon_3.png"));
            moonPhases.push_back(texture_2d::init("./res/textures/blocks/moon/moon_4.png"));
            moonPhases.push_back(texture_2d::init("./res/textures/blocks/moon/moon_5.png"));
            moonPhases.push_back(texture_2d::init("./res/textures/blocks/moon/moon_6.png"));
            moonPhases.push_back(texture_2d::init("./res/textures/blocks/moon/moon_7.png"));

            moonPhase = rand() % (int)moonPhases.size();

            highlightedBlock = scene::createBlock(0, 0, 0, texture_2d::init("./res/textures/blocks/highlight.png"), -1, false, true);
            highlightedBlock.scale = glm::vec3(1.001, 1.001, 1.001);
            // Setting up Sun
            node_t sun = scene::createBlock(0, 0, 0, texture_2d::init("./res/textures/blocks/sun.png"), -1, true, false);
            sun.scale = glm::vec3(0.001, 4.0, 4.0);
            sun.translation.x += RENDER_DISTANCE;
            GLuint auraTextureID = texture_2d::init("./res/textures/blocks/sun_aura.png");
            node_t sunAura = scene::createBlock(0, 0, 0, auraTextureID, -1, true, false);
            sunAura.scale = glm::vec3(3.0, 1.2, 1.2);
            sun.children.push_back(sunAura);
            // Setting up Moon
            node_t moonOrbit;
            node_t moon = scene::createBlock(0, 0, 0, moonPhases[moonPhase], -1, false, false);
            moon.scale = glm::vec3(0.001, 2.5, 2.5);
            moonOrbit.translation.x -= RENDER_DISTANCE;

            moonOrbit.children.push_back(moon);

            // Generating random stars
            srand(time(0));
            GLuint starTexBlueID = texture_2d::init("./res/textures/blocks/star_blue.png");
            GLuint starTexYellowID = texture_2d::init("./res/textures/blocks/star_yellow.png");
            
            for (int i = 0; i < MAX_STARS; i++) {
                
                float starX, starY, starZ;
                do {
                    // Randomly generates a Y and Z point, and then finds the X point
                    // via sphere general form
                    starY = rand() % RENDER_DISTANCE;
                    starY += (float)((rand() % 9) / 10.0f);
                    starY *= (rand() % 2 == 0) ? 1 : -1;
                    starZ = rand() % RENDER_DISTANCE;
                    starZ += (float)((rand() % 9) / 10.0f);
                    starZ *= (rand() % 2 == 0) ? 1 : -1;
                    starX = sqrt(pow(RENDER_DISTANCE, 2) - pow(starY, 2) - pow(starZ, 2));
                } while (utility::calculateDistance(glm::vec3(0, 0, 0), glm::vec3(-starX + RENDER_DISTANCE, starY, starZ)) <= 2.0f);

                node_t star = scene::createBlock(0, 0, 0, (rand() % 2) ? starTexBlueID : starTexYellowID, -1, false, false);
                star.translation = {-starX + RENDER_DISTANCE, starY, starZ};

                
                star.scale *= ((rand() % 6) + 2) / 40.0f;
                star.rotation.x += (rand() % 10) / 10.0f;
                star.rotation.y += (rand() % 10) / 10.0f;
                star.rotation.z += (rand() % 10) / 10.0f;

                if (rand() % 10 == 0) {
                    // 1 in 10 chance that the star has an aura around it
                    star.children.push_back(scene::createBlock(0, 0, 0, auraTextureID, -1, false, false));
                    star.children.back().scale = glm::vec3(2.5f, 2.5f, 2.5f);
                }

                moonOrbit.children.push_back(star);
            }

            node_t centreOfWorld;
            node_t skySphere = createSkySphere(0, RENDER_DISTANCE + 1, 256);
            // skySphere.texture = texture_2d::init("./res/textures/sky.png");
            skySphere.rotation = glm::vec3(0, 0, 90);
            skySphere.diffuse = glm::vec4((float)173/255, (float)216/255, (float)230/255, 1.0f);

            centreOfWorld.children.push_back(skySphere);
            skySphereIndex = centreOfWorld.children.size() - (size_t)1;
            centreOfWorld.children.push_back(sun);
            centreOfWorld.children.push_back(moonOrbit);
            moonIndex = centreOfWorld.children.size() - (size_t)1;
            centreOfWorldNode.children.push_back(centreOfWorld);
            skyIndex = centreOfWorldNode.children.size() - (size_t)1;

            // Setting up HUD


            // Crosshair
            node_t crosshair = scene::createFlatSquare(texture_2d::init("./res/textures/crosshair.png"), false);
            crosshair.translation.z = -1 * SCREEN_DISTANCE;
            crosshair.scale = glm::vec3(0.02, 0.02, 0.02);
            screen.children.push_back(crosshair);

            // ADD BLOCKS HERE
            // First hotbar
            hotbar.push_back(combineBlockData("dirt", -1, false, false));
            hotbar.push_back(combineBlockData("coarse_dirt", -1, false, false));
            hotbar.push_back(combineBlockData("grass_block", -1, false, false));
            hotbar.push_back(combineBlockData("barrel", false, false, true));
            hotbar.push_back(combineBlockData("crafting_table", false, false));
            hotbar.push_back(combineBlockData("oak_planks", false, false));
            hotbar.push_back(combineBlockData("oak_log", false, false, true));
            hotbar.push_back(combineBlockData("oak_leaves", true, false));
            hotbar.push_back(combineBlockData("cobblestone", false, false));
            hotbar.push_back(combineBlockData("mossy_cobblestone", false, false));
            hotbar.push_back(combineBlockData("stone", false, false));
            hotbar.push_back(combineBlockData("stone_bricks", false, false));
            hotbar.push_back(combineBlockData("mossy_stone_bricks", false, false));
            hotbar.push_back(combineBlockData("cracked_stone_bricks", false, false));
            hotbar.push_back(combineBlockData("glass", true, false));
            hotbar.push_back(combineBlockData("sea_lantern", false, true, false, glm::vec3(212.0f, 235.0f, 255.0f) * (1.0f / 255.0f), 1.6f));
            hotbar.push_back(combineBlockData("magma", false, true, false, glm::vec3(244.0f, 133.0f, 34.0f) * (1.0f / 255.0f), 1.5f));
            hotbar.push_back(combineBlockData("glowstone", false, true, false, glm::vec3(251.0f, 218.0f, 116.0f) * (1.0f / 255.0f), 1.6f));
            hotbar.push_back(combineBlockData("crying_obsidian", false, true, false, glm::vec3(131.0f, 8.0f, 228.0f) * (1.0f / 255.0f), 2.0f));
            hotbar.push_back(combineBlockData("obsidian", false, false));
            hotbar.push_back(combineBlockData("tnt", false, false));
            hotbar.push_back(combineBlockData("coal_ore", false, false));
            hotbar.push_back(combineBlockData("iron_ore", false, false));
            hotbar.push_back(combineBlockData("raw_iron", false, false));
            hotbar.push_back(combineBlockData("iron_block", false, false));
            hotbar.push_back(combineBlockData("copper_ore", false, false));
            hotbar.push_back(combineBlockData("raw_copper", false, false));
            hotbar.push_back(combineBlockData("copper_block", false, false));
            hotbar.push_back(combineBlockData("gold_ore", false, false));
            hotbar.push_back(combineBlockData("raw_gold", false, false));
            hotbar.push_back(combineBlockData("gold_block", false, false));
            hotbar.push_back(combineBlockData("lapis_ore", false, false));
            hotbar.push_back(combineBlockData("redstone_ore", false, true, false, glm::vec3(253.0f, 94.0f, 94.0f) * (1.0f / 255.0f), 0.5f));
            hotbar.push_back(combineBlockData("diamond_ore", false, false));
            hotbar.push_back(combineBlockData("marccoin_block", false, false, true));
            hotbar.push_back(combineBlockData("bedrock", false, false));
            // Second hotbar
            hotbarSecondary.push_back(combineBlockData("white", false, false));
            hotbarSecondary.push_back(combineBlockData("orange", false, false));
            hotbarSecondary.push_back(combineBlockData("magenta", false, false));
            hotbarSecondary.push_back(combineBlockData("light_blue", false, false));
            hotbarSecondary.push_back(combineBlockData("yellow", false, false));
            hotbarSecondary.push_back(combineBlockData("lime", false, false));
            hotbarSecondary.push_back(combineBlockData("pink", false, false));
            hotbarSecondary.push_back(combineBlockData("gray", false, false));
            hotbarSecondary.push_back(combineBlockData("light_gray", false, false));
            hotbarSecondary.push_back(combineBlockData("cyan", false, false));
            hotbarSecondary.push_back(combineBlockData("purple", false, false));
            hotbarSecondary.push_back(combineBlockData("blue", false, false));
            hotbarSecondary.push_back(combineBlockData("brown", false, false));
            hotbarSecondary.push_back(combineBlockData("green", false, false));
            hotbarSecondary.push_back(combineBlockData("red", false, false));
            hotbarSecondary.push_back(combineBlockData("black", false, false));

            // Hand
            node_t blockHand = scene::createBlock(0, 0, 0, hotbar[0].texture, -1, false, true);
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

            node_t hotbarTexture = scene::createFlatSquare(texture_2d::init("./res/textures/hotbar.png"), false);
            hotbarTexture.translation.z = -2.8 * SCREEN_DISTANCE;
            hotbarTexture.translation.y -= 0.03f;
            hotbarTexture.scale = glm::vec3(0.3, 0.3, 1);
            screen.children.push_back(hotbarTexture);
            hotbarHUDIndex = screen.children.size() - 1;

            // Filling up the hotbar and positioning it correctly onto the screen
            float xPos = -0.3f;
            for (int i = 0; i < 9; i++) {
                node_t hudHotbar = scene::createBlock(0, 0, 0, hotbar[1].texture, -1, false, false);
                hudHotbar.translation.z += 0.55f;
                hudHotbar.translation.y += 0.321f;
                hudHotbar.translation.x = xPos;
                xPos += 0.075f;
                hudHotbar.rotation = glm::vec3(45, 31, 30);
                hudHotbar.scale = glm::vec3(0.035, 0.035, 0.0001);
                screen.children[(size_t)hotbarHUDIndex].children.push_back(hudHotbar);
                hotbarTextureIndex.push_back(screen.children[(size_t)hotbarHUDIndex].children.size() - 1);
            }

            node_t instructions = scene::createFlatSquare(texture_2d::init("./res/textures/instructions.png"), false);
            instructions.translation.z = -2.78f * SCREEN_DISTANCE;
            instructions.translation.x -= 0.11f;
            instructions.translation.y -= 0.019f;
            instructions.scale = glm::vec3(0.18, 0.18, 1);
            screen.children.push_back(instructions);
            instructionIndex = screen.children.size() - 1;
            
            // SCENE GRAPHS
            // Screen node -> Hand object
            //             -> Flying icon
            //             -> Instructions
            //             -> Hotbar object -> All 9 items

            // Centre of the World Node -> Skybox
            //                          -> Sun -> Sun aura
            //                          -> Moon -> All stars -> Some stars having another layer around it

            // Bed -> Centre Of Player Node -> Head
            //                              -> Torso
            //                              -> Left Arm
            //                              -> Right Arm
            //                              -> Left leg
            //                              -> Right leg

            // Update the textures of the hotbar
            scrollHotbar(1);
            scrollHotbar(-1);

            blockData emptyBlock;
            emptyBlock.texture = -1;
            emptyBlock.specularMap = -1;
            for (int x = 0; x < blockMap.size(); x++) {
                for (int z = 0; z < blockMap[0][0].size(); z++) {
                    for (int y = 0; y < blockMap[0].size(); y++) {
                        blockMap[x][y][z] = emptyBlock;
                    }
                }
            }

            // Generating the new world
            size_t layer = 0;
            for (int x = 0; x < blockMap.size(); x++) {
                for (int z = 0; z < blockMap[0][0].size(); z++) {
                    layer = 0;
                    for (std::string i : listOfBlocks) {
                        auto generatingBlock = findDataBlockName(i);
                        placeBlock(scene::createBlock(x, layer, z, generatingBlock, false, !generatingBlock.illuminating));
                        layer += 1;
                    }
                }
            }

            // Keeping track of where the hand and rotation is
            oldHandPos = screen.children[handIndex].translation;
            oldHandRotation = screen.children[handIndex].rotation;
            std::cout << "World Created\n";
        }

        player::playerPOV *getCurrCamera() {
            if (cutsceneEnabled) {
                return &cutsceneCamera;
            }
            return &playerCamera;
        }

        blockData findDataBlockName(std::string blockName) {
            for (auto i : hotbar) {
                if (i.blockName == blockName) {
                    return i;
                }
            }
            for (auto i : hotbarSecondary) {
                if (i.blockName == blockName) {
                    return i;
                }
            }
            return hotbar.front();
        }

        void toggleInstructions() {
            screen.children[(size_t)instructionIndex].air = !screen.children[(size_t)instructionIndex].air;
            return;
        }

        void updateMoonPhase() {
            moonPhase++;
            moonPhase %= moonPhases.size();
            centreOfWorldNode.children[(size_t)skyIndex].children[(size_t)moonIndex].children[0].textureID = moonPhases[moonPhase];
        }

        void toggleCutscene() {
            if (playerCamera.pos.y != groundLevel + eyeLevel && !cutsceneEnabled) {
                return;
            } else if (!check3x3Area(playerCamera.pos) && !cutsceneEnabled) {
                return;
            }
            cutsceneEnabled = !cutsceneEnabled;
            if (cutsceneEnabled) {
                cutsceneTick = (float) glfwGetTime();
                cutsceneCamera.pos.x = playerCamera.pos.x;
                cutsceneCamera.pos.y = playerCamera.pos.y;
                cutsceneCamera.pos.z = playerCamera.pos.z;
                cutsceneCamera.pitch = playerCamera.pitch;
                cutsceneCamera.yaw = playerCamera.yaw;
                bed.translation = playerCamera.pos;
                bed.translation.y -= eyeLevel;
            }
        }

        bool check3x3Area(glm::vec3 pos) {
            glm::vec3 tempPos;
            tempPos.x = round(pos.x);
            tempPos.z = round(pos.z);
            tempPos.y = round(pos.y - eyeLevel);
            for (size_t i = tempPos.x - (size_t)1; i <= tempPos.x + 1; i++) {
                for (size_t j = tempPos.z - (size_t)1; j <= tempPos.z + 1; j++) {
                    if (isCoordOutBoundaries(i, tempPos.y, j)) continue;
                    if (!terrain[i][tempPos.y][j].air) {
                        std::cout << "You are surrounded by blocks!\n";
                        return false;
                    }
                }
            }
            return true;
        }

        bool getCutsceneStatus() {
            return cutsceneEnabled;
        }

        void animateCutscene() {
            // Calculating delta time
            auto now = (float) glfwGetTime() - cutsceneTick;
            updateBlocksToRender();

            auto desiredYaw = playerCamera.yaw - 180.0f;
            if (desiredYaw < 0.0f) {
                desiredYaw = 360.0f + desiredYaw;
            } else if (desiredYaw >= 360.0f) {
                desiredYaw -= 360.0f;
            }

            // Controls the path of camera
            std::vector<glm::vec3> controlPointA = {
                {0.0f,   0.0f, 0.0f},
                {0.0f,   4.0f, 0.0f},
                {4.0f * (float)glm::sin(glm::radians(playerCamera.yaw)), 0.0f, 4.0f * -glm::cos(glm::radians(playerCamera.yaw))},
                {4.0f * (float)glm::sin(glm::radians(playerCamera.yaw)), 4.0f, 4.0f * (float)-glm::cos(glm::radians(playerCamera.yaw))},
            };

            // Ease in, Ease out speed
            std::vector<glm::vec3> controlPointB = {
                {0.0f, 0.0f, 0.0f},
                {0.5f, 0.0f, 0.0f},
                {0.5f, 1.0f, 0.0f},
                {1.0f, 1.0f, 0.0f},
            };

            // Moves the controlPointA path to be relative to the current position
            for (size_t i = 0; i < controlPointA.size(); i++) {
                controlPointA[i] += playerCamera.pos;
            }

            float t = 0.25f * (float)now;
            if (t > 1.0f) {
                t = 1.0f;
            }
            getCurrCamera()->yaw = playerCamera.yaw - (playerCamera.yaw - desiredYaw) * utility::cubicBezier(controlPointB, t).y;
            getCurrCamera()->pitch = playerCamera.pitch - (playerCamera.pitch + 30.0f) * utility::cubicBezier(controlPointB, t).y;
            getCurrCamera()->pos = playerCamera.pos + ((utility::cubicBezier(controlPointA, t) - playerCamera.pos) * utility::cubicBezier(controlPointB, t).y);

            return;
        }

        void tickStars() {
            for (size_t i = 0; i < (rand() % 50); i++) {
                bool status = (rand() % 2 == 0);
                size_t starIndex = rand() % MAX_STARS;
                if (starIndex == 0) continue;
                // Index 0 is the moon. Skip it

                node_t *starPointer = &centreOfWorldNode.children[(size_t)skyIndex].children[(size_t)moonIndex].children[starIndex];
                starPointer->air = status;
                if (starPointer->children.size() > 0) {
                    starPointer->children.front().air = status;
                }
            }
        }

        void switchHotbars() {
            std::vector<blockData> tempHotbar = hotbar;
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
            tickStars();
            centreOfWorldNode.children[skyIndex].translation = playerCamera.pos;
            centreOfWorldNode.children[skyIndex].translation.y -= eyeLevel + 6.5f;
            centreOfWorldNode.children[skyIndex].rotation = glm::vec3(0, 0, degree);
            centreOfWorldNode.children[skyIndex].children[skySphereIndex].diffuse = skyColor;
        }

        void bobHand(float dt) {
            // Only bob hand if the player is on the ground
            if (playerCamera.pos.y == (float)groundLevel + eyeLevel && swingCycle == -1.0f) {
                walkCycle += dt * 2.0f;
                walkCycle = fmod(walkCycle, 1.0f);
                std::vector<glm::vec3> controlPoint = {
                    {0.0f,  0.0f,  0.0f},
                    {1.0f, -1.0f,  0.0f},
                    {1.0f,  1.0f,  0.0f},
                    {2.0f,  0.0f,  0.0f},
                };

                // Moves the controlPoint path to be relative to the current position
                for (size_t i = 0; i < controlPoint.size(); i++) {
                    // Scaling it down
                    controlPoint[i] *= 0.05f;
                    controlPoint[i] += oldHandPos;
                }
                
                screen.children[handIndex].translation.y = utility::cubicBezier(controlPoint, walkCycle).y;
            }
        }

        void swingHand(float dt) {
            if (swingCycle >= 0.0f) {

                std::vector<glm::vec3> controlPoint = {
                    {0.0f,  0.00f,  0.0f},
                    {0.0f, -0.15f, -0.1f},
                    {0.0f,  0.05f, -0.1f},
                    {0.0f,  0.00f,  0.0f},
                };

                std::vector<glm::vec3> controlPointRotation = {
                    {0.0f,  0.00f,  0.0f},
                    {0.0f, -0.1f, -0.1f},
                    {0.0f,  0.1f, -0.1f},
                    {0.0f,  0.00f,  0.0f},
                };

                // Moves the controlPoint path to be relative to the current position
                for (size_t i = 0; i < controlPoint.size(); i++) {
                    controlPoint[i] += oldHandPos;
                }

                for (size_t i = 0; i < controlPointRotation.size(); i++) {
                    // Scaling the parametric curves
                    controlPointRotation[i] *= 500.0f;
                    controlPointRotation[i] += oldHandRotation;
                }

                swingCycle += dt * 2.0f;
                float t = std::min(1.0f, swingCycle);

                screen.children[handIndex].rotation.x = utility::cubicBezier(controlPointRotation, t).z;
                screen.children[handIndex].translation = utility::cubicBezier(controlPoint, t);

                if (t == 1.0f) {
                    swingCycle = -1.0f;
                }
            }
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
            screen.children[handIndex].textureID = hotbar[hotbarIndex].texture;

            int tempIndex = hotbarIndex - 4;
            if (tempIndex < 0) {
                tempIndex = hotbar.size() + tempIndex;
            }
            for (int i : hotbarTextureIndex) {
                screen.children[hotbarHUDIndex].children[i].textureID = hotbar[tempIndex].texture;
                tempIndex++;
                tempIndex %= hotbar.size();
            }

        }
    
        glm::vec3 findCursorBlock(bool giveBlockBefore) {
            if (cutsceneEnabled) return glm::vec3(-1, -1, -1); 
            glm::vec3 lookingDirection = player::getLookingDirection(&playerCamera, increments);
            float rayX = playerCamera.pos.x, rayY = playerCamera.pos.y, rayZ = playerCamera.pos.z;

            int limit = 0;
            while (limit < playerReachRange) {
    
                rayX += lookingDirection.x;
                rayY += lookingDirection.y;
                rayZ += lookingDirection.z;
                limit++;
                
                if (isCoordOutBoundaries((int)round(rayX), (int)round(rayY), (int)round(rayZ))) {
                    continue;
                } else if (!terrain[(int)round(rayX)][(int)round(rayY)][(int)round(rayZ)].air) {
                    break;
                }
            }
            if (limit >= playerReachRange || isCoordOutBoundaries((int)round(rayX), (int)round(rayY), (int)round(rayZ))) {
                return glm::vec3(-1, -1, -1);
            } else if (giveBlockBefore) {
                return glm::vec3((int)round(rayX - lookingDirection.x), (int)round(rayY - lookingDirection.y), (int)round(rayZ - lookingDirection.z));
            } else {
                return glm::vec3((int)round(rayX), (int)round(rayY), (int)round(rayZ));
            }
        }

        void rightClickPlace(renderer::renderer_t *renderInfo) {
            swingCycle = 0;
            screen.children[handIndex].translation = oldHandPos;
            screen.children[handIndex].rotation = oldHandRotation;

            auto placeBlockVector = findCursorBlock(true);
            size_t placeX = (size_t)placeBlockVector.x, placeY = (size_t)placeBlockVector.y, placeZ = (size_t)placeBlockVector.z;
            
            if (isCoordOutBoundaries((int)placeX, (int)placeY, (int)placeZ)) {
                return;
            }
            
            if (terrain[placeX][placeY][placeZ].air) {
                placeBlock(scene::createBlock((int)placeX, (int)placeY, (int)placeZ, hotbar[hotbarIndex], false, !hotbar[hotbarIndex].illuminating));

                // Adding a light source to the block
                if (terrain[placeX][placeY][placeZ].illuminating) {
                    terrain[placeX][placeY][placeZ].lightID = renderInfo->addLightSource(glm::vec3(placeX, placeY, placeZ), hotbar[hotbarIndex].rgb, hotbar[hotbarIndex].intensity);
                    
                    if (terrain[placeX][placeY][placeZ].lightID < 0) {
                        std::cout << "Maximum lights reached, can only have up to " << renderInfo->getMaxLights() << " point lights\n";
                    }
                }
                // If player is inside a block, then destroy the block
                if (checkInsideBlock()) {
                    renderInfo->removeLightSource(terrain[placeX][placeY][placeZ].lightID);
                    terrain[placeX][placeY][placeZ].air = true;
                    terrain[placeX][placeY][placeZ].transparent = true;
                    terrain[placeX][placeY][placeZ].lightID = -1;
                    return;
                } else if (hotbar[hotbarIndex].rotatable && abs(playerCamera.pitch) <= 35.0f && !shiftMode) {
                    // Rotate the block if it is a block that can be rotated
                    // Blocks will not rotate if shift is being pressed
                    switch (utility::getDirection(playerCamera.yaw)) {
                        case 0:
                            terrain[placeX][placeY][placeZ].rotation = glm::vec3(90.0f, 0.0f, 0.0f);
                            break;
                        case 1:
                            terrain[placeX][placeY][placeZ].rotation = glm::vec3(0.0f, 0.0f, 90.0f);
                            break;
                        case 2:
                            terrain[placeX][placeY][placeZ].rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
                            break;
                        case 3:
                            terrain[placeX][placeY][placeZ].rotation = glm::vec3(0.0f, 0.0f, -90.0f);
                            break;
                    }
                }

                // If program reaches here, the blocks to be rendered must be updated
                updateBlocksToRender(true);
            }
        }

        void leftClickDestroy(renderer::renderer_t *renderInfo) {
            swingCycle = 0;
            screen.children[handIndex].translation = oldHandPos;
            screen.children[handIndex].rotation = oldHandRotation;
            
            auto placeBlockVector = findCursorBlock(false);
            auto placeX = placeBlockVector.x, placeY = placeBlockVector.y, placeZ = placeBlockVector.z;
            
            if (isCoordOutBoundaries(placeX, placeY, placeZ)) {
                return;
            }

            if (!terrain[placeX][placeY][placeZ].air) {
                terrain[placeX][placeY][placeZ].air = true;
                terrain[placeX][placeY][placeZ].transparent = true;
                terrain[placeX][placeY][placeZ].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                if (terrain[placeX][placeY][placeZ].lightID != -1) {
                    renderInfo->removeLightSource(terrain[placeX][placeY][placeZ].lightID);
                    terrain[placeX][placeY][placeZ].lightID = -1;
                }
                // If program reaches here, the blocks to be rendered must be updated
                updateBlocksToRender(true);
            }
        }

        void middleClickPick() {
            auto placeBlockVector = findCursorBlock(false);
            auto placeX = placeBlockVector.x, placeY = placeBlockVector.y, placeZ = placeBlockVector.z;
            
            if (isCoordOutBoundaries(placeX, placeY, placeZ)) {
                return;
            }
            int index = 0;
            bool found = false;
            for (auto i : hotbar) {
                if (i.texture == terrain[placeX][placeY][placeZ].textureID) {
                    hotbarIndex = index;
                    found = true;
                    break;
                }
                index += 1;
            }
            
            if (!found) {
                index = 0;
                for (auto i : hotbarSecondary) {
                    if (i.texture == terrain[placeX][placeY][placeZ].textureID) {
                        switchHotbars();
                        hotbarIndex = index;
                        break;
                    }
                    index += 1;
                }
            }
            scrollHotbar(1);
            scrollHotbar(-1);
        }

        void placeBlock(node_t block) {
            size_t blockX = block.x, blockY = block.y, blockZ = block.z;
            scene::destroy(&terrain[blockX][blockY][blockZ], false);
            block.transparent = hotbar[hotbarIndex].transparent;
            block.illuminating = hotbar[hotbarIndex].illuminating;
            terrain[blockX][blockY][blockZ] = block;
            return;
        }

        int getSunDistance() {
            if (RENDER_DISTANCE < 15) {
                return 15;
            } else {
                return RENDER_DISTANCE;
            }
            
        }


        /**
         * Function that controls how the player moves and obstructions by
         * blocks
         */
        void updatePlayerPositions(GLFWwindow *window, float dt) {
            swingHand(dt);

            player::update_cam_angles(playerCamera, window, dt);

            auto originalPosition = playerCamera.pos;

            if (flyingMode) {
                playerCamera.yVelocity = 0;
            }

            float step = dt * CAMERA_SPEED * walkingMultiplier;
            // Controls running
            if (playerCamera.yVelocity == 0 && playerCamera.pos.y == (float)groundLevel + eyeLevel && !shiftMode) {
                if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                    walkingMultiplier = 1.0f;
                } else {
                    walkingMultiplier = 0.5f;
                }
            }

            if (shiftMode) {
                walkingMultiplier = 0.2f;
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
                if (!flyingMode) {
                    playerCamera.yVelocity = JUMP_POWER;
                } else {
                    playerCamera.pos.y += step;
                    if (checkInsideBlock()) {
                        playerCamera.pos.y -= step;
                    }
                }
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && playerCamera.pos.y != (float)groundLevel + eyeLevel) {
                if (flyingMode) {
                    playerCamera.pos.y -= step;
                    if (playerCamera.pos.y < (float)groundLevel + eyeLevel) {
                        playerCamera.pos.y = groundLevel + eyeLevel;
                    }
                }
            }

            if (originalPosition.x != playerCamera.pos.x || originalPosition.z != playerCamera.pos.z) {
                bobHand(dt);
                updateBlocksToRender();
            }


            // Controls sneaking
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !flyingMode) {
                eyeLevel = 0.75f;
                shiftMode = true;
            } else {
                eyeLevel = 1.0f;
                shiftMode = false;
            }
            
            // Enacting gravity onto the camera
            if (!flyingMode) {

                playerCamera.pos.y += playerCamera.yVelocity * dt;
                playerCamera.yVelocity += GRAVITY * dt;

                if (playerCamera.pos.y < groundLevel + eyeLevel) {
                    playerCamera.pos.y = groundLevel + eyeLevel;
                    playerCamera.yVelocity = 0;
                }
                if (playerCamera.yVelocity > 0 && checkInsideBlock()) {
                    playerCamera.pos.y -= 5 * playerCamera.yVelocity;
                    playerCamera.yVelocity = -JUMP_POWER;
                }
            }

            // World boundaries. Respawns when the y co-ordinate goes too low or too high
            if (playerCamera.pos.y - eyeLevel < -10 || playerCamera.pos.y > WORLD_HEIGHT - 1) {
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
                if (playerCamera.pos.y - eyeLevel < groundLevel) {
                    playerCamera.pos.y = groundLevel + eyeLevel;
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
            float playerPosY = playerCamera.pos.y - eyeLevel;
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
            
            float playerPosY = playerCamera.pos.y - eyeLevel;
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
            return (int)direction * (int)WORLD_HEIGHT * (int)WORLD_HEIGHT;
        }

        void drawWorld(renderer::renderer_t renderInfo) {

            glClearColor(0.f, 0.f, 0.2f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderInfo.setBasePters(playerCamera.pos);

            auto view_proj = renderInfo.projection * getCurrCamera()->get_view();
		    glUniformMatrix4fv(renderInfo.view_proj_loc, 1, GL_FALSE, glm::value_ptr(view_proj));

            drawElement(&centreOfWorldNode, glm::mat4(1.0f), renderInfo, defaultSpecular);
            drawTerrain(glm::mat4(1.0f), renderInfo);
            
            if (!shiftMode) {
                // Drawing the highlighted block
                highlightedBlock.translation = findCursorBlock(false);
                if (!isCoordOutBoundaries(highlightedBlock.translation.x, highlightedBlock.translation.y, highlightedBlock.translation.z)) {
                    drawElement(&highlightedBlock, glm::mat4(1.0f), renderInfo, defaultSpecular);
                }
            }

            // Draw bed if cutscene is occuring, otherwise draw HUD
            if (!cutsceneEnabled) {
                drawScreen(glm::mat4(1.0f), renderInfo);
            } else {
                drawElement(&bed, glm::mat4(1.0f), renderInfo, defaultSpecular);
            }
        }

        void drawTerrain(const glm::mat4 &parent_mvp, renderer::renderer_t renderInfo) {

            for (size_t i = 0; i < listOfBlocksToRender.size(); i++) {
                float x = listOfBlocksToRender[i]->translation.x;
                float y = listOfBlocksToRender[i]->translation.y;
                float z = listOfBlocksToRender[i]->translation.z;
                
                if (utility::calculateDistance(glm::vec3(x, y, z), getCurrCamera()->pos) <= RENDER_DISTANCE) {
                    if (frustum::isBlockInView(player::getLookingDirection(getCurrCamera(), 1), glm::vec3(x, y, z), getCurrCamera()->pos)) {
                        drawBlock(listOfBlocksToRender[i], parent_mvp, renderInfo, defaultSpecular);
                    } else if (utility::calculateDistance(glm::vec3(x, y, z), getCurrCamera()->pos) <= 2.0f) {
                        drawBlock(listOfBlocksToRender[i], parent_mvp, renderInfo, defaultSpecular);
                    }
                }
            }
        }

        void updateBlocksToRender(bool forceRender = false) {

            if (!forceRender && utility::calculateDistance(getCurrCamera()->pos, lastRenderedPos) <= 2.0f) return;

            listOfBlocksToRender.clear();

            std::vector<glm::vec3> transparentBlocks;
            int width = WORLD_WIDTH, height = WORLD_HEIGHT;
            auto minY = (int)std::max(0, (int)(getCurrCamera()->pos.y - RENDER_DISTANCE));
            auto maxY = (int)std::min((int)(getCurrCamera()->pos.y + RENDER_DISTANCE), height);
            auto minX = (int)std::max(0, (int)(getCurrCamera()->pos.x - RENDER_DISTANCE));
            auto maxX = (int)std::min((int)(getCurrCamera()->pos.x + RENDER_DISTANCE), width);
            auto minZ = (int)std::max(0, (int)(getCurrCamera()->pos.z - RENDER_DISTANCE));
            auto maxZ = (int)std::min((int)(getCurrCamera()->pos.z + RENDER_DISTANCE), width);
            glm::vec2 yRange = glm::vec2(minY, maxY);
            glm::vec2 xRange = glm::vec2(minX, maxX);
            glm::vec2 zRange = glm::vec2(minZ, maxZ);

            for (int y = yRange.x; y < yRange.y; y++) {
                for (int z = zRange.x; z < zRange.y; z++) {
                    for (int x = xRange.x; x < xRange.y; x++) {

                        if (isCoordOutBoundaries(x, y, z)) {
                            continue;
                        } else if (terrain[x][y][z].air) {
                            continue;
                        }
                            
                        // Figuring out which sides should be rendered or not.
                        // Only render side if it has air next to it
                        terrain[x][y][z].culledFaces = {true, true, true, true, true, true};

                        if (terrain[x][y][z].transparent) {
                            transparentBlocks.push_back(glm::vec3(x, y, z));
                            continue;
                        }

                        if (!terrain[x][y][z].ignoreCulling) getHiddenFaces(x, y, z, terrain[x][y][z].culledFaces, false);

                        if (utility::countFalses(terrain[x][y][z].culledFaces) < 6) {
                            listOfBlocksToRender.push_back(&terrain[x][y][z]);
                        }

                    }
                }
            }

            // Draws transparent blocks last
            for (auto i : transparentBlocks) {
                terrain[i.x][i.y][i.z].culledFaces = {true, true, true, true, true, true};
                if (!terrain[i.x][i.y][i.z].ignoreCulling) getHiddenFaces(i.x, i.y, i.z, terrain[i.x][i.y][i.z].culledFaces, true);
                if (utility::countFalses(terrain[i.x][i.y][i.z].culledFaces) < 6) {
                    listOfBlocksToRender.push_back(&terrain[i.x][i.y][i.z]);
                }
            }

            lastRenderedPos = playerCamera.pos;

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

        void drawScreen(const glm::mat4 &parent_mvp, renderer::renderer_t renderInfo) {

            screen.translation = playerCamera.pos;
            screen.rotation.x = playerCamera.pitch;
            screen.rotation.y = -playerCamera.yaw;

            drawElement(&screen, parent_mvp, renderInfo, defaultSpecular);
        }
    };

}
#endif //COMP3421_SCENE_OBJECT_HPP
