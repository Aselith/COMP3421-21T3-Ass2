#ifndef COMP3421_SCENE_OBJECT_HPP
#define COMP3421_SCENE_OBJECT_HPP

#include "static_mesh.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <ass2/utility.hpp>
#include <ass2/frustum.hpp>
#include <ass2/shapes.hpp>
#include <ass2/player.hpp>
#include <ass2/texture_2d.hpp>
#include <ass2/renderer.hpp>

#include <math.h>
#include <vector>
#include <iostream>

namespace scene {

    const float GRAVITY         = -19.6f;
    const float JUMP_POWER      = 6.0f;
    const float CAMERA_SPEED    = 5.0f;
    const float PLAYER_RADIUS   = 0.25f; // 0.25
    const float SCREEN_DISTANCE = 0.25f;

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
        std::string name;

        int x = 0, y = 0, z = 0;
        int lightID = -1;
        bool air = true, transparent = false, illuminating = false, ignoreCulling = false;
    };

    struct blockData {
        std::string blockName;
        GLuint texture = 0;
        GLuint specularMap = 0;
        glm::vec3 rgb = {0, 0, 0};
        bool transparent = false;
        bool illuminating = false;
        bool rotatable = false;
        float intensity = 1.0f;
    };

    struct miniBlockData {
        std::string blockName;
        glm::vec3 position;
        bool entireLayer, startAtMiddle;
        miniBlockData(std::string name, glm::vec3 pos, bool startFromCentre = false, bool fillEntireLayer = false) {
            blockName = name;
            position = pos;
            entireLayer = fillEntireLayer;
            startAtMiddle = startFromCentre;
        }
    };
    
    /**
     * @brief Render the given node and its children recursively. Renders selected sides based on
     * the node's culled faces
     * 
     * @param node 
     * @param model 
     * @param renderInfo 
     * @param defaultSpecular 
     */
    void drawBlock(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo, GLuint defaultSpecular);

    /**
     * @brief Render the given node and its children recursively. Renders all sides regardless
     * of culled faces
     * 
     * @param node 
     * @param model 
     * @param renderInfo 
     * @param defaultSpecular 
     */
    void drawElement(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo, GLuint defaultSpecular);

    /**
     * @brief Takes in the parameters and returns blockData with all the information given stored inside
     * 
     * @param stringName 
     * @param transparent 
     * @param illuminating 
     * @param rotatable 
     * @param color 
     * @param intensity 
     * @return blockData 
     */
    blockData combineBlockData(std::string stringName, bool transparent, bool illuminating, bool rotatable = false, glm::vec3 color = {0, 0, 0}, float intensity = 1.0f);

    /**
     * @brief Create a Block object with the given texID and specIDs
     * 
     * @param x 
     * @param y 
     * @param z 
     * @param texID 
     * @param specID 
     * @param transparent 
     * @param invertNormals 
     * @param affectedByLight 
     * @return node_t 
     */
    node_t createBlock(int x, int y, int z, GLuint texID, GLuint specID, bool transparent, bool invertNormals, bool affectedByLight);
    
    /**
     * @brief Create a Block object with the given blockData
     * 
     * @param x 
     * @param y 
     * @param z 
     * @param data 
     * @param invertNormals 
     * @param affectedByLight 
     * @return node_t 
     */
    node_t createBlock(int x, int y, int z, blockData data, bool invertNormals, bool affectedByLight);

    /**
     * @brief Create a Sky Sphere object that has the radius and tesselation of the given values
     * 
     * @param texID 
     * @param radius 
     * @param tesselation 
     * @return node_t 
     */
    node_t createSkySphere(GLuint texID, float radius, int tesselation);

    /**
     * @brief Create a Flat Square object that is one dimensional
     * 
     * @param texID 
     * @param invert 
     * @return node_t 
     */
    node_t createFlatSquare(GLuint texID, bool invert);

    /**
     * @brief Create a Bed Player object. This node_t will have the right children to construct a Minecraft Player Model
     * 
     * @param bedTexID 
     * @param playerTexID 
     * @return node_t 
     */
    node_t createBedPlayer(GLuint bedTexID, GLuint playerTexID);

    /**
     * @brief Destroys the node. destroyTexture = true to delete the texture as well.
     * 
     * @param node 
     * @param destroyTexture 
     */
    void destroy(const node_t *node, bool destroyTexture);

    // WORLD = Everything that shows up on the screen is controlled from here

    struct world {

        size_t worldWidth = 110;
        const size_t WORLD_HEIGHT = 50;
        const size_t MAX_STARS = 200;
        
        float eyeLevel = 1.0f;
        bool shiftMode = false;
        float walkingMultiplier = 0.5f;
        bool cutsceneEnabled = false;
        player::playerPOV playerCamera, cutsceneCamera;
        glm::vec3 oldPos, oldHandPos, oldHandRotation, lastRenderedPos;
        int increments = 200;
        int playerReachRange = 4 * increments;
        int groundLevel = -99999, aboveLevel = 99999;
        float cutsceneTick = 0;
        float swingCycle = -1.0f, walkCycle = 0.0f;
        GLuint defaultSpecular = texture_2d::init("./res/textures/blocks/default_specular.png");
        std::vector<GLuint> moonPhases;
        size_t moonPhase = 0;

        std::vector<std::vector<std::vector<node_t>>> terrain;
        std::vector<node_t *> listOfBlocksToRender;

        node_t screen;
        node_t centreOfWorld;
        node_t highlightedBlock;
        node_t bed;
        
        std::vector<blockData> hotbar;
        std::vector<blockData> hotbarSecondary;

        std::vector<int> hotbarTextureIndex;

        int renderDistance = 0;
        int hotbarIndex = 0;
        size_t handIndex = 0, hotbarHUDIndex = 0, flyingIconIndex = 0, skySphereIndex = 0, moonIndex = 0, instructionIndex = 0;
        bool flyingMode = false, startSwingHandAnim = false, runningMode = false;

        /**
         * @brief Creates a world with the given render distance and the world width
         * 
         * @param listOfBlocks 
         * @param inputRenderDistance 
         * @param inputWidth 
         */
        world(std::vector<miniBlockData> listOfBlocks, int inputRenderDistance, int inputWidth, renderer::renderer_t *renderInfo) {

            renderDistance = inputRenderDistance;
            worldWidth = (size_t)inputWidth;
            terrain = {worldWidth , std::vector< std::vector<node_t> > (WORLD_HEIGHT, std::vector<node_t> (worldWidth) ) };
            
            std::cout << "Generating world of size " << worldWidth << "x" << worldWidth << " with render distance " << renderDistance << ". Please standby...\n";
            GLuint flyingIcon = texture_2d::init("./res/textures/flying_mode.png");
            
            // SETTING UP BED SCENE GRAPH
            bed = createBedPlayer(texture_2d::init("./res/textures/blocks/bed.png"), texture_2d::init("./res/textures/player.png"));


            // SETTING UP CENTRE OF WORLD SCENE GRAPH
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

            highlightedBlock = scene::createBlock(0, 0, 0, texture_2d::init("./res/textures/blocks/highlight.png"), defaultSpecular, false, false, true);
            highlightedBlock.scale = glm::vec3(1.001, 1.001, 1.001);
            // Setting up Sun
            node_t sun = scene::createBlock(0, 0, 0, texture_2d::init("./res/textures/blocks/sun.png"), defaultSpecular, false, true, false);
            sun.scale = glm::vec3(0.001, 4.0, 4.0);
            sun.translation.x += (float)renderDistance;
            GLuint auraTextureID = texture_2d::init("./res/textures/blocks/sun_aura.png");
            node_t sunAura = scene::createBlock(0, 0, 0, auraTextureID, defaultSpecular, false, true, false);
            sunAura.scale = glm::vec3(3.0, 1.2, 1.2);
            sun.children.push_back(sunAura);
            // Setting up Moon
            node_t moonOrbit;
            node_t moon = scene::createBlock(0, 0, 0, moonPhases[(size_t)moonPhase], defaultSpecular, false, false, false);
            moon.scale = glm::vec3(0.001, 2.5, 2.5);
            moonOrbit.translation.x -= (float)renderDistance;

            moonOrbit.children.push_back(moon);

            // Generating random stars
            srand((glm::uint)time(0));
            GLuint starTexBlueID = texture_2d::init("./res/textures/blocks/star_blue.png");
            GLuint starTexYellowID = texture_2d::init("./res/textures/blocks/star_yellow.png");
            
            for (int i = 0; i < (int)MAX_STARS; i++) {
                
                float starX, starY, starZ;
                do {
                    // Randomly generates a Y and Z point, and then finds the X point
                    // via sphere general form
                    starY = (float)(rand() % renderDistance);
                    starY += (float)fmod(rand(), 9.0f) * 0.1f;
                    starY *= (rand() % 2 == 0) ? 1.0f : -1.0f;
                    starZ = (float)(rand() % renderDistance);
                    starZ += (float)fmod(rand(), 9.0f) * 0.1f;
                    starZ *= (rand() % 2 == 0) ? 1.0f : -1.0f;
                    starX = (float)sqrt(pow(renderDistance, 2) - pow(starY, 2) - pow(starZ, 2));
                } while (utility::calculateDistance(glm::vec3(0, 0, 0), glm::vec3(-starX + (float)renderDistance, starY, starZ)) <= 2.0f);

                node_t star = scene::createBlock(0, 0, 0, (rand() % 2) ? starTexBlueID : starTexYellowID, defaultSpecular, false, false, false);
                star.translation = {-starX + (float)renderDistance, starY, starZ};

                star.scale *= (float)((rand() % 6) + 2) / 40.0f;
                star.rotation.x += (float)(rand() % 10) / 10.0f;
                star.rotation.y += (float)(rand() % 10) / 10.0f;
                star.rotation.z += (float)(rand() % 10) / 10.0f;

                if (rand() % 10 == 0) {
                    // 1 in 10 chance that the star has an aura around it
                    star.children.push_back(scene::createBlock(0, 0, 0, auraTextureID, defaultSpecular, false, false, false));
                    star.children.back().scale = glm::vec3(2.5f, 2.5f, 2.5f);
                }

                moonOrbit.children.push_back(star);
            }

            node_t skySphere = createSkySphere(0, (float)renderDistance + 1.0f, 256);
            // skySphere.texture = texture_2d::init("./res/textures/sky.png");
            skySphere.rotation = glm::vec3(0, 0, 90);
            skySphere.diffuse = glm::vec4((float)173/255, (float)216/255, (float)230/255, 1.0f);

            centreOfWorld.children.push_back(skySphere);
            skySphereIndex = centreOfWorld.children.size() - (size_t)1;
            centreOfWorld.children.push_back(sun);
            centreOfWorld.children.push_back(moonOrbit);
            moonIndex = centreOfWorld.children.size() - (size_t)1;
            // skyIndex = centreOfWorldNode.children.size() - (size_t)1;

            // SETTING UP SCREEN SCENE GRAPH

            // Crosshair
            node_t crosshair = scene::createFlatSquare(texture_2d::init("./res/textures/crosshair.png"), false);
            crosshair.translation.z = -1 * SCREEN_DISTANCE;
            crosshair.scale = glm::vec3(0.02, 0.02, 0.02);
            screen.children.push_back(crosshair);

            // ADD BLOCKS HERE //
            // First hotbar
            hotbar.push_back(combineBlockData("dirt", false, false));
            hotbar.push_back(combineBlockData("coarse_dirt", false, false));
            hotbar.push_back(combineBlockData("grass_block", false, false));
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
            hotbar.push_back(combineBlockData("raw_iron", false, false));
            hotbar.push_back(combineBlockData("iron_block", false, false));
            hotbar.push_back(combineBlockData("raw_copper", false, false));
            hotbar.push_back(combineBlockData("copper_block", false, false));
            hotbar.push_back(combineBlockData("raw_gold", false, false));
            hotbar.push_back(combineBlockData("gold_block", false, false));
            hotbar.push_back(combineBlockData("coal_ore", false, false));
            hotbar.push_back(combineBlockData("iron_ore", false, false));
            hotbar.push_back(combineBlockData("copper_ore", false, false));
            hotbar.push_back(combineBlockData("gold_ore", false, false));
            hotbar.push_back(combineBlockData("lapis_ore", false, false));
            hotbar.push_back(combineBlockData("redstone_ore", false, true, false, glm::vec3(253.0f, 94.0f, 94.0f) * (1.0f / 255.0f), 0.5f));
            hotbar.push_back(combineBlockData("emerald_ore", false, false));
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
            node_t blockHand = scene::createBlock(0, 0, 0, hotbar[0].texture, hotbar[0].specularMap, false, false, true);
            blockHand.translation.z = -1.0f * SCREEN_DISTANCE;
            blockHand.translation.y -= 0.15f;
            blockHand.translation.x += 0.25f;
            blockHand.scale = glm::vec3(0.15, 0.15, 0.15);

            screen.children.push_back(blockHand);
            handIndex = screen.children.size() - 1;

            node_t flyingIconNode = scene::createFlatSquare(flyingIcon, false);
            flyingIconNode.translation.z = -2.8f * SCREEN_DISTANCE;
            flyingIconNode.translation.y += 0.096f;
            flyingIconNode.translation.x -= 0.17f;
            flyingIconNode.scale = glm::vec3(0.03, 0.03, 1);
            flyingIconNode.air = true;
            screen.children.push_back(flyingIconNode);
            flyingIconIndex = screen.children.size() - 1;

            node_t hotbarTexture = scene::createFlatSquare(texture_2d::init("./res/textures/hotbar.png"), false);
            hotbarTexture.translation.z = -2.7f * SCREEN_DISTANCE;
            hotbarTexture.translation.y -= 0.015f;
            hotbarTexture.scale = glm::vec3(0.25f, 0.25f, 1.0f);
            screen.children.push_back(hotbarTexture);
            hotbarHUDIndex = screen.children.size() - 1;

            // Filling up the hotbar and positioning it correctly onto the screen
            float xPos = -0.285f;
            for (int i = 0; i < 9; i++) {
                node_t hudHotbarBlock = scene::createBlock(0, 0, 0, hotbar[1].texture, defaultSpecular, false, false, false);
                hudHotbarBlock.translation.z += 0.55f;
                hudHotbarBlock.translation.y += 0.3f;
                hudHotbarBlock.translation.x = xPos;
                xPos += 0.0715f;
                hudHotbarBlock.rotation = glm::vec3(45, 35, 30);
                hudHotbarBlock.scale = glm::vec3(0.035, 0.035, 0.00001);
                screen.children[(size_t)hotbarHUDIndex].children.push_back(hudHotbarBlock);
                hotbarTextureIndex.push_back((int)screen.children[(size_t)hotbarHUDIndex].children.size() - 1);
            }

            node_t instructions = scene::createFlatSquare(texture_2d::init("./res/textures/instructions.png"), false);
            instructions.translation.z = -2.699f * SCREEN_DISTANCE;
            instructions.translation.x -= 0.11f;
            instructions.translation.y -= 0.03f;
            instructions.scale = glm::vec3(0.13, 0.13, 1);
            screen.children.push_back(instructions);
            instructionIndex = screen.children.size() - 1;
            
            // ALL SCENE GRAPHS
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


            // Generating the new world
            for (size_t i = 0; i < listOfBlocks.size(); i++) {
                
                auto generatingBlock = findDataBlockName(listOfBlocks[i].blockName);
                if (listOfBlocks[i].entireLayer) {
                    // Fills up the entire y level if specified
                    for (int x = 0; x < (int)terrain.size(); x++) {
                        for (int z = 0; z < (int)terrain.at(0).at(0).size(); z++) {
                            placeBlock(scene::createBlock(x, (int)listOfBlocks[i].position.y, z, generatingBlock, false, !generatingBlock.illuminating));
                            if (generatingBlock.illuminating) {
                                terrain.at(x).at(listOfBlocks[i].position.y).at(z).lightID = renderInfo->addLightSource(listOfBlocks[i].position, generatingBlock.rgb, generatingBlock.intensity);
                            }
                        }
                    }
                } else {
                    if (listOfBlocks[i].startAtMiddle) {
                        // Repositioning all the co-oridinates into the centre of the world if specified
                        listOfBlocks[i].position += glm::vec3((float)terrain.size() / 2.0f, 0.0f, (float)terrain.at(0).at(0).size() / 2.0f);
                    }
                    placeBlock(scene::createBlock((int)listOfBlocks[i].position.x, (int)listOfBlocks[i].position.y, (int)listOfBlocks[i].position.z, generatingBlock, false, !generatingBlock.illuminating));
                    if (generatingBlock.illuminating) {
                        terrain.at(listOfBlocks[i].position.x).at(listOfBlocks[i].position.y).at(listOfBlocks[i].position.z).lightID = renderInfo->addLightSource(listOfBlocks[i].position, generatingBlock.rgb, generatingBlock.intensity);
                    }
                }
                terrain.at((size_t)listOfBlocks[i].position.x).at((size_t)listOfBlocks[i].position.y).at((size_t)listOfBlocks[i].position.z).transparent = generatingBlock.transparent;
            }
            // Keeping track of where the hand and rotation is
            oldHandPos = screen.children[handIndex].translation;
            oldHandRotation = screen.children[handIndex].rotation;
            std::cout << "World Created\n\n";
        }

        /**
         * @brief Get the current camera being used to render the scene
         * 
         * @return player::playerPOV* 
         */
        player::playerPOV *getCurrCamera() {
            if (cutsceneEnabled) {
                return &cutsceneCamera;
            }
            return &playerCamera;
        }

        /**
         * @brief Based on the blockName, find the correct blockData that represents the desired block
         * Will return the first blockData if the name is not valid
         * 
         * @param blockName 
         * @return blockData 
         */
        blockData findDataBlockName(std::string blockName) {
            for (size_t i = 0; i < hotbar.size(); i++) {
                if (hotbar[i].blockName == blockName) {
                    return hotbar[i];
                }
            }
            for (size_t i = 0; i < hotbarSecondary.size(); i++) {
                if (hotbarSecondary[i].blockName == blockName) {
                    return hotbarSecondary[i];
                }
            }
            std::cout << "ERROR: BLOCK NOT FOUND!\n";
            return hotbar.front();
        }

        /**
         * @brief Turns the display of the instructions on or off
         * 
         * @param status 
         */
        void toggleInstructions(bool status) {
            screen.children[(size_t)instructionIndex].air = status;
            return;
        }

        /**
         * @brief Get the bool which governs if the instructions are to appear or not
         * 
         * @return true if the instructions are showing
         * @return false if the instructions are not showing
         */
        bool getInstructionStatus() {
            return screen.children[(size_t)instructionIndex].air;
        }

        /**
         * @brief Use this to change the moon phase to the next
         * 
         */
        void updateMoonPhase() {
            moonPhase++;
            moonPhase %= moonPhases.size();
            centreOfWorld.children[(size_t)moonIndex].children[0].textureID = moonPhases[moonPhase];
        }

        /**
         * @brief Use this to enable the sleeping cutscene. Can only occur if the player is on the ground and aren't surrounded by blocks
         * 
         */
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

        /**
         * @brief Checks if there are any blocks around the player
         * 
         * @param pos 
         * @return true if there aren't any blocks in the given 3 by 3
         * @return false if there are blocks in the given 3 by 3
         */
        bool check3x3Area(glm::vec3 pos) {
            glm::vec3 tempPos;
            tempPos.x = round(pos.x);
            tempPos.z = round(pos.z);
            tempPos.y = round(pos.y - eyeLevel);
            for (size_t i = tempPos.x - (size_t)1; i <= tempPos.x + 1; i++) {
                for (size_t j = tempPos.z - (size_t)1; j <= tempPos.z + 1; j++) {
                    if (isCoordOutBoundaries(i, tempPos.y, j)) continue;
                    if (!terrain.at(i).at(tempPos.y).at(j).air) {
                        std::cout << "You may not sleep, you are surrounded by blocks!\n";
                        return false;
                    }
                }
            }
            return true;
        }

        /**
         * @brief Get information on whether the sleeping cutscene is playing or not
         * 
         * @return true if cutscene is playing
         * @return false if the cutscene is not playing
         */
        bool getCutsceneStatus() {
            return cutsceneEnabled;
        }

        /**
         * @brief Advances the cutscene forward in the bezier curve
         * 
         */
        void animateCutscene() {
            // Calculating delta time
            auto now = (float) glfwGetTime() - cutsceneTick;

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
            } else {
                updateBlocksToRender();
            }
            getCurrCamera()->yaw = playerCamera.yaw - (playerCamera.yaw - desiredYaw) * utility::cubicBezier(controlPointB, t).y;
            getCurrCamera()->pitch = playerCamera.pitch - (playerCamera.pitch + 30.0f) * utility::cubicBezier(controlPointB, t).y;
            getCurrCamera()->pos = playerCamera.pos + ((utility::cubicBezier(controlPointA, t) - playerCamera.pos) * utility::cubicBezier(controlPointB, t).y);

            return;
        }

        /**
         * @brief Rotates the stars based on the given delta time. Also flicker a random star with each call
         * 
         * @param dt 
         */
        void tickStars(float dt) {
            // Index 0 is the moon. Skip it
            for (size_t i = 1; i < MAX_STARS; i++) {
                node_t *starPointer = &centreOfWorld.children[(size_t)moonIndex].children[i];
                bool status = (rand() % 20 == 0);
                starPointer->air = status;
                if (starPointer->children.size() > 0) {
                    starPointer->children.front().air = status;
                }
                starPointer->rotation.x += 10.0f * dt;
            }
        }

        /**
         * @brief Switches between the main hotbar and the secondary hotbar with the wool blocks
         * 
         */
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

        /**
         * @brief Updates the sun and moon position based on the given degree.
         * Also changes the diffuse of the sky sphere.
         * 
         * @param degree 
         * @param skyColor 
         * @param dt 
         */
        void updateSunPosition(float degree, glm::vec3 skyColor, float dt) {
            tickStars(dt);
            centreOfWorld.translation = playerCamera.pos;
            centreOfWorld.translation.y -= eyeLevel + 6.5f;
            centreOfWorld.rotation = glm::vec3(0, 0, degree);
            centreOfWorld.children[skySphereIndex].diffuse = skyColor;
        }

        /**
         * @brief Advances the bobbing hand animation based on the given delta time
         * 
         * @param dt 
         */
        void bobHand(float dt) {

            // Only bob hand if the player is on the ground
            if (playerCamera.pos.y == (float)groundLevel + eyeLevel && swingCycle == -1.0f) {
                // Controls how fast to bobhand
                if (shiftMode) {
                    dt /= 4.0f;
                } else if (runningMode) {
                    dt *= 1.5f;
                }
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

        /**
         * @brief Advances the swinging hand animation whenever the player presses left or right click
         * based on the given delta time
         * 
         * @param dt 
         */
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

                swingCycle += dt * 3.0f;
                float t = std::min(1.0f, swingCycle);

                screen.children[handIndex].rotation.x = utility::cubicBezier(controlPointRotation, t).z;
                screen.children[handIndex].translation = utility::cubicBezier(controlPoint, t);

                if (t == 1.0f) {
                    swingCycle = -1.0f;
                }
            }
        }

        /**
         * @brief Toggles between flying or walking mode
         * 
         */
        void toggleMode() {
            flyingMode = !flyingMode;
            screen.children[flyingIconIndex].air = !flyingMode;
        }

        /**
         * @brief Scrolls hotbar in the direction given. Negative to scroll down, positive to scroll up
         * 
         * @param direction 
         */
        void scrollHotbar(int direction) {
            hotbarIndex += direction;

            if (hotbarIndex >= (int)hotbar.size()) {
                hotbarIndex = 0;
            } else if (hotbarIndex < 0) {
                hotbarIndex = (int)hotbar.size() + hotbarIndex;
            }
            screen.children[handIndex].textureID = hotbar[hotbarIndex].texture;
            screen.children[handIndex].specularID = hotbar[hotbarIndex].specularMap;
            // Updating the textures in the hotbar
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
    
        /**
         * @brief Finds the block that the player is looking at within a certain limit.
         * giveBlockBefore to return the location of the nieghbouring block which is closest
         * to where the player was looking
         * 
         * @param giveBlockBefore 
         * @return glm::vec3 
         */
        glm::vec3 findCursorBlock(bool giveBlockBefore) {
            // Don't bother if the cutscene is playing
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
                } else if (!terrain.at((int)round(rayX)).at((int)round(rayY)).at((int)round(rayZ)).air) {
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

        /**
         * @brief Call this to place the hotbar block in the players looking direction.
         * This function call also deals with rotating blocks and creating light sources
         * 
         * @param renderInfo 
         * @param forcedPlace 
         */
        void rightClickPlace(renderer::renderer_t *renderInfo) {
            swingCycle = 0;
            screen.children[handIndex].translation = oldHandPos;
            screen.children[handIndex].rotation = oldHandRotation;

            size_t placeX, placeY, placeZ;

            auto placeBlockVector = findCursorBlock(true);
            placeX = (size_t)placeBlockVector.x;
            placeY = (size_t)placeBlockVector.y;
            placeZ = (size_t)placeBlockVector.z;
            
            
            if (isCoordOutBoundaries((int)placeX, (int)placeY, (int)placeZ)) {
                return;
            }
            
            if (terrain.at(placeX).at(placeY).at(placeZ).air) {
                placeBlock(scene::createBlock((int)placeX, (int)placeY, (int)placeZ, hotbar[hotbarIndex], false, !hotbar[hotbarIndex].illuminating));

                // Adding a light source to the block
                if (terrain.at(placeX).at(placeY).at(placeZ).illuminating) {
                    terrain.at(placeX).at(placeY).at(placeZ).lightID = renderInfo->addLightSource(glm::vec3(placeX, placeY, placeZ), hotbar[hotbarIndex].rgb, hotbar[hotbarIndex].intensity);
                    
                    if (terrain.at(placeX).at(placeY).at(placeZ).lightID < 0) {
                        std::cout << "Maximum lights reached, can only have up to " << renderInfo->getMaxLights() << " point lights\n";
                    }
                }
                // If player is inside a block, then destroy the block
                if (checkInsideBlock()) {
                    renderInfo->removeLightSource(terrain.at(placeX).at(placeY).at(placeZ).lightID);
                    terrain.at(placeX).at(placeY).at(placeZ).air = true;
                    terrain.at(placeX).at(placeY).at(placeZ).transparent = true;
                    terrain.at(placeX).at(placeY).at(placeZ).lightID = -1;
                    return;
                } else if (hotbar[hotbarIndex].rotatable && abs(playerCamera.pitch) <= 35.0f && !shiftMode) {
                    // Rotate the block if it is a block that can be rotated
                    // Blocks will not rotate if shift is being pressed
                    switch (utility::getDirection(playerCamera.yaw)) {
                        case 0:
                            terrain.at(placeX).at(placeY).at(placeZ).rotation = glm::vec3(90.0f, 0.0f, 0.0f);
                            break;
                        case 1:
                            terrain.at(placeX).at(placeY).at(placeZ).rotation = glm::vec3(0.0f, 0.0f, 90.0f);
                            break;
                        case 2:
                            terrain.at(placeX).at(placeY).at(placeZ).rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
                            break;
                        case 3:
                            terrain.at(placeX).at(placeY).at(placeZ).rotation = glm::vec3(0.0f, 0.0f, -90.0f);
                            break;
                    }
                }

                // If program reaches here, the blocks to be rendered must be updated
                updateBlocksToRender(true);
            }
        }

        /**
         * @brief Call this to destroy the block in the players looking direction.
         * If forcedPlace is a valid position, it will destroy the block in that position instead.
         * This function call also deals deleting light sources
         * 
         * @param renderInfo 
         * @param forcedPlace 
         */
        void leftClickDestroy(renderer::renderer_t *renderInfo, glm::vec3 forcedPlace = {-10, -10, -10}) {
            swingCycle = 0;
            screen.children[handIndex].translation = oldHandPos;
            screen.children[handIndex].rotation = oldHandRotation;
            
            glm::vec3 placeBlockVector;
            if (forcedPlace.x < 0) {
                placeBlockVector = findCursorBlock(false);
            } else {
                placeBlockVector = forcedPlace;
            }
            auto placeX = placeBlockVector.x, placeY = placeBlockVector.y, placeZ = placeBlockVector.z;
            
            if (isCoordOutBoundaries(placeX, placeY, placeZ)) {
                return;
            }

            if (!terrain.at(placeX).at(placeY).at(placeZ).air) {
                terrain.at(placeX).at(placeY).at(placeZ).air = true;
                terrain.at(placeX).at(placeY).at(placeZ).transparent = true;
                terrain.at(placeX).at(placeY).at(placeZ).rotation = glm::vec3(0.0f, 0.0f, 0.0f);
                if (terrain.at(placeX).at(placeY).at(placeZ).lightID != -1) {
                    renderInfo->removeLightSource(terrain.at(placeX).at(placeY).at(placeZ).lightID);
                    terrain.at(placeX).at(placeY).at(placeZ).lightID = -1;
                }
                // If program reaches here, the blocks to be rendered must be updated
                updateBlocksToRender(true);
            }
        }

        /**
         * @brief Call this to change the hotbar block into the block that the player is looking at
         * 
         * @param renderInfo 
         * @param forcedPlace 
         */
        void middleClickPick() {
            auto placeBlockVector = findCursorBlock(false);
            auto placeX = placeBlockVector.x, placeY = placeBlockVector.y, placeZ = placeBlockVector.z;
            
            if (isCoordOutBoundaries(placeX, placeY, placeZ)) {
                return;
            }
            int index = 0;
            bool found = false;
            for (auto i : hotbar) {
                if (i.texture == terrain.at(placeX).at(placeY).at(placeZ).textureID) {
                    hotbarIndex = index;
                    found = true;
                    break;
                }
                index += 1;
            }
            
            if (!found) {
                index = 0;
                for (auto i : hotbarSecondary) {
                    if (i.texture == terrain.at(placeX).at(placeY).at(placeZ).textureID) {
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

        /**
         * @brief Places the block into the world using it's x, y, z values
         * 
         * @param block 
         */
        void placeBlock(node_t block) {
            size_t blockX = (size_t)block.x, blockY = (size_t)block.y, blockZ = (size_t)block.z;
            scene::destroy(&terrain.at(blockX).at(blockY).at(blockZ), false);
            block.transparent = hotbar[(size_t)hotbarIndex].transparent;
            block.illuminating = hotbar[(size_t)hotbarIndex].illuminating;
            terrain.at(blockX).at(blockY).at(blockZ) = block;
            return;
        }

        /**
         * @brief Finds a suitable respawn position of player and teleports them to there
         * If a block is in the way, the block will be deleted. If there aren't any blocks
         * beneath the respawn location, flying mode will be toggled on
         * 
         * @param block 
         */
        void findRespawnPosition(renderer::renderer_t *renderInfo) {
            playerCamera.pos = {round(terrain.size() / 2), 6.0f, round(terrain.at(0).at(0).size() / 2)};
            while (checkInsideBlock()) {
                std::cout << "Some blocks were destroyed to make room for you to respawn in\n";
                leftClickDestroy(renderInfo, playerCamera.pos);
                leftClickDestroy(renderInfo, {round(terrain.size() / 2), 5.0f, round(terrain.at(0).at(0).size() / 2)});
            }
            if (findClosestBlockAboveBelow(-1) < 0) {
                std::cout << "No blocks below detected. Force flying mode enabled\n";
                flyingMode = false;
                toggleMode();
            }
        }

        /**
         * @brief Function that controls the hitboxes and how the player camera moves with WASD, Shift etc.
         * 
         * @param window 
         * @param dt 
         * @param renderInfo 
         */
        void updatePlayerPositions(GLFWwindow *window, float dt, renderer::renderer_t *renderInfo) {
            swingHand(dt);
            // camera angle update is reserved for player.cpp as that does not depend on the terrain
            player::updateCameraAngle(playerCamera, window, dt);

            auto originalPosition = playerCamera.pos;

            //
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && flyingMode) {
                playerCamera.yVelocity = -1.0f;
            } else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && flyingMode) {
                playerCamera.yVelocity = 1.0f;
            } else if (flyingMode) {
                playerCamera.yVelocity = 0.0f;
            }

            float step = dt * CAMERA_SPEED * walkingMultiplier;
            // Controls running
            if (runningMode) {
                walkingMultiplier = 1.2f;
            } else {
                walkingMultiplier = 0.5f;
            }

            if (shiftMode) {
                walkingMultiplier = 0.2f;
            }

            if (findClosestBlockAboveBelow(-1) == playerCamera.pos.y - eyeLevel) {
                // Prevent accidentally shifting off a block
                playerCamera.yVelocity = 0.0f;
            }
            
            glm::mat4 trans = glm::translate(glm::mat4(1.0), -playerCamera.pos);
            trans *= glm::rotate(glm::mat4(1.0), -glm::radians(playerCamera.yaw), glm::vec3(0, 1, 0));
            trans *= glm::rotate(glm::mat4(1.0), glm::radians(playerCamera.pitch), glm::vec3(1, 0, 0));

            auto right = glm::vec3(trans[0]);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                playerCamera.pos.z += step * -glm::cos(glm::radians(playerCamera.yaw));
                // Step backwards if player is sneaking and teetering off block, or they are inside a block
                if (checkInsideBlock() || (playerCamera.yVelocity == 0.0f && shiftMode && findClosestBlockAboveBelow(-1) < playerCamera.pos.y - eyeLevel)) {
                    playerCamera.pos.z -= step * -glm::cos(glm::radians(playerCamera.yaw));
                }
                playerCamera.pos.x += step * glm::sin(glm::radians(playerCamera.yaw));
                if (checkInsideBlock() || (playerCamera.yVelocity == 0.0f && shiftMode && findClosestBlockAboveBelow(-1) < playerCamera.pos.y - eyeLevel)) {
                    playerCamera.pos.x -= step * glm::sin(glm::radians(playerCamera.yaw));
                }
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                playerCamera.pos.z -= step * -glm::cos(glm::radians(playerCamera.yaw));
                if (checkInsideBlock() || (playerCamera.yVelocity == 0.0f && shiftMode && findClosestBlockAboveBelow(-1) < playerCamera.pos.y - eyeLevel)) {
                    playerCamera.pos.z += step * -glm::cos(glm::radians(playerCamera.yaw));
                }
                playerCamera.pos.x -= step * glm::sin(glm::radians(playerCamera.yaw));
                if (checkInsideBlock() || (playerCamera.yVelocity == 0.0f && shiftMode && findClosestBlockAboveBelow(-1) < playerCamera.pos.y - eyeLevel)) {
                    playerCamera.pos.x += step * glm::sin(glm::radians(playerCamera.yaw));
                }
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                playerCamera.pos.z += right.z * -step;
                if (checkInsideBlock() || (playerCamera.yVelocity == 0.0f && shiftMode && findClosestBlockAboveBelow(-1) < playerCamera.pos.y - eyeLevel)) {
                    playerCamera.pos.z -= right.z * -step;
                }
                playerCamera.pos.x += right.x * -step;
                if (checkInsideBlock() || (playerCamera.yVelocity == 0.0f && shiftMode && findClosestBlockAboveBelow(-1) < playerCamera.pos.y - eyeLevel)) {
                    playerCamera.pos.x -= right.x * -step;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                playerCamera.pos.z += right.z * step;
                if (checkInsideBlock() || (playerCamera.yVelocity == 0.0f && shiftMode && findClosestBlockAboveBelow(-1) < playerCamera.pos.y - eyeLevel)) {
                    playerCamera.pos.z -= right.z * step;
                }
                playerCamera.pos.x += right.x * step;
                if (checkInsideBlock() || (playerCamera.yVelocity == 0.0f && shiftMode && findClosestBlockAboveBelow(-1) < playerCamera.pos.y - eyeLevel)) {
                    playerCamera.pos.x -= right.x * step;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (playerCamera.yVelocity == 0 || flyingMode)) {
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

            // If the positions differ, then bob the hand
            if (originalPosition.x != playerCamera.pos.x || originalPosition.z != playerCamera.pos.z || originalPosition.y != playerCamera.pos.y) {
                bobHand(dt);
                updateBlocksToRender();
            }

            if (glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS) {
                runningMode = false;
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
                // Enacting player terminal velocity
                if (abs(playerCamera.yVelocity) >= 50.0f) {
                    playerCamera.yVelocity = 50.0f * (playerCamera.yVelocity / abs(playerCamera.yVelocity));
                }

                if (playerCamera.pos.y < groundLevel + eyeLevel) {
                    // Prevents player from falling through the ground
                    playerCamera.pos.y = groundLevel + eyeLevel;
                    playerCamera.yVelocity = 0.0f;
                }
            }

            // World boundaries. Respawns when the y co-ordinate goes too low
            // || playerCamera.pos.y > WORLD_HEIGHT - 1
            if (playerCamera.pos.y - eyeLevel < -1 * (int)WORLD_HEIGHT) {
                std::cout << "Respawned, you went too low!\n";

                findRespawnPosition(renderInfo);
                groundLevel = findClosestBlockAboveBelow(-1);

                if (groundLevel > 0) {
                    playerCamera.pos.y = groundLevel;
                } else {
                    playerCamera.pos.y = 4;
                }

            }

            groundLevel = findClosestBlockAboveBelow(-1);
            aboveLevel = findClosestBlockAboveBelow(1);

            // Below keeps player steady on ground
            if (playerCamera.pos.y - eyeLevel < groundLevel) {
                playerCamera.pos.y = groundLevel + eyeLevel;
                playerCamera.yVelocity = 0;
            }
            if (playerCamera.yVelocity > 0) {
                // Below keeps player under the ceiling
                if (playerCamera.pos.y + 1.0f >= aboveLevel) {
                    if (flyingMode) {
                        playerCamera.pos.y = aboveLevel - 1;
                    }
                    playerCamera.yVelocity *= -0.5;
                }
            }
        
        }

        /**
         * @brief Checks if the given x, y, z co-ordinates exists in the terrain/world
         * 
         * @param x 
         * @param y 
         * @param z 
         * @return true if out of bounds
         * @return false if inside bounds
         */
        bool isCoordOutBoundaries(int x, int y, int z) {
            try {
                terrain.at(x).at(y).at(z).air = terrain.at(x).at(y).at(z).air;
            } catch (...) {
                return true;
            }
            return false;
        }

        /**
         * @brief Checks if the player is inside a block
         * 
         * @return true if the player is clipping into a block
         * @return false if the player is not clipping into a block
         */
        bool checkInsideBlock() {
            
            float playerPosY = playerCamera.pos.y - eyeLevel;
            float playerPosX = playerCamera.pos.x;
            float playerPosZ = playerCamera.pos.z;

            size_t xPosRd = (size_t)round(playerPosX);
            size_t yPosRd = (size_t)round(playerPosY);
            size_t zPosRd = (size_t)round(playerPosZ);

            // Checks around the player in a circle if they are touching a block or not
            for (float degree = 0; degree < 360.0f; degree += 5.0f) {
                xPosRd = (size_t)round(playerPosX + PLAYER_RADIUS * (float)glm::sin(glm::radians(degree)));
                zPosRd = (size_t)round(playerPosZ + PLAYER_RADIUS * (float)-glm::cos(glm::radians(degree)));
                if (!isCoordOutBoundaries(xPosRd, yPosRd, zPosRd) && !terrain.at(xPosRd).at(yPosRd).at(zPosRd).air) {
                    return true;
                }
                if (!isCoordOutBoundaries(xPosRd, yPosRd + 1, zPosRd) && !terrain.at(xPosRd).at(yPosRd + 1).at(zPosRd).air) {
                    return true;
                }
            }
            return false;
            
        }
        
        /**
         * @brief Find the highest or lowest level the player can go with their x and z co-ordinate
         * 
         * @param direction (Positive to find the closest block above. Negative to find the closest block below)
         * @return int 
         */
        int findClosestBlockAboveBelow(int direction) {
            
            float playerPosY = playerCamera.pos.y - eyeLevel;
            float playerPosX = playerCamera.pos.x;
            float playerPosZ = playerCamera.pos.z;

            int maxInt = (direction < 0) ? 1 : WORLD_HEIGHT;

            for (int i = (int)playerPosY; i * direction < maxInt; i += direction) {
                int xPosRd = (int)round(playerPosX);
                int yPosRd = (int)round(i);
                int zPosRd = (int)round(playerPosZ);

                // Checks around the player in a circle if they are touching a block or not
                for (float degree = 0; degree < 360.0f; degree += 5.0f) {
                    xPosRd = (int)round(playerPosX + PLAYER_RADIUS * (float)glm::sin(glm::radians(degree)));
                    zPosRd = (int)round(playerPosZ + PLAYER_RADIUS * (float)-glm::cos(glm::radians(degree)));
                    if (isCoordOutBoundaries(xPosRd, yPosRd, zPosRd)) {
                        continue;
                    }
                    if (!terrain.at(xPosRd).at(yPosRd).at(zPosRd).air) {
                        // Correcting
                        if (direction < 0) {
                            return yPosRd - direction;
                        } else {
                            return yPosRd;
                        }
                        
                    }
                }
                
            }
            
            return (int)direction * (int)WORLD_HEIGHT * (int)WORLD_HEIGHT;
        }

        /**
         * @brief Draws the terrain, HUD, lighting effects etc.
         * 
         * @param renderInfo 
         */
        void drawWorld(renderer::renderer_t renderInfo) {

            glClearColor(0.f, 0.f, 0.2f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderInfo.setBasePters(playerCamera.pos);

            auto view_proj = renderInfo.projection * getCurrCamera()->get_view();
		    glUniformMatrix4fv(renderInfo.view_proj_loc, 1, GL_FALSE, glm::value_ptr(view_proj));

            drawElement(&centreOfWorld, glm::mat4(1.0f), renderInfo, defaultSpecular);
            drawTerrain(glm::mat4(1.0f), renderInfo);
            
            if (!shiftMode) {
                // Drawing the highlighted block if shift mode is not enabled
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

        /**
         * @brief Draws whatever is currently inside listOfBlocksToRender
         * Also calculates the degree between the block and the players looking vector to determine
         * if the block is within the players view point
         * 
         * @param parent_mvp 
         * @param renderInfo 
         */
        void drawTerrain(const glm::mat4 &parent_mvp, renderer::renderer_t renderInfo) {

            for (size_t i = 0; i < listOfBlocksToRender.size(); i++) {
                float x = listOfBlocksToRender[i]->translation.x;
                float y = listOfBlocksToRender[i]->translation.y;
                float z = listOfBlocksToRender[i]->translation.z;
                
                if (utility::calculateDistance(glm::vec3(x, y, z), getCurrCamera()->pos) <= renderDistance) {
                    if (frustum::isBlockInView(player::getLookingDirection(getCurrCamera(), 1), glm::vec3(x, y, z), getCurrCamera()->pos)) {
                        drawBlock(listOfBlocksToRender[i], parent_mvp, renderInfo, defaultSpecular);
                    } else if (utility::calculateDistance(glm::vec3(x, y, z), getCurrCamera()->pos) <= 2.0f) {
                        drawBlock(listOfBlocksToRender[i], parent_mvp, renderInfo, defaultSpecular);
                    }
                }
            }
        }

        /**
         * @brief Call this to update the listOfBlocks to render. Only works if the last rendered position is far away enough from
         * the player's current position. forceRender = true to ignore this
         * 
         * @param forceRender 
         */
        void updateBlocksToRender(bool forceRender = false) {

            // Don't bother to render if the last rendered position is less than (renderDistance / 10) blocks away
            // unless force render is true
            if (!forceRender && utility::calculateDistance(getCurrCamera()->pos, lastRenderedPos) <= (float)(renderDistance / 10.0f)) return;

            listOfBlocksToRender.clear();

            std::vector<glm::vec3> transparentBlocks;
            int width = worldWidth, height = WORLD_HEIGHT;
            auto minY = (int)std::max(0, (int)(getCurrCamera()->pos.y - renderDistance));
            auto maxY = (int)std::min((int)(getCurrCamera()->pos.y + renderDistance), height);
            auto minX = (int)std::max(0, (int)(getCurrCamera()->pos.x - renderDistance));
            auto maxX = (int)std::min((int)(getCurrCamera()->pos.x + renderDistance), width);
            auto minZ = (int)std::max(0, (int)(getCurrCamera()->pos.z - renderDistance));
            auto maxZ = (int)std::min((int)(getCurrCamera()->pos.z + renderDistance), width);
            glm::vec2 yRange = glm::vec2(minY, maxY);
            glm::vec2 xRange = glm::vec2(minX, maxX);
            glm::vec2 zRange = glm::vec2(minZ, maxZ);

            for (int y = yRange.x; y < yRange.y; y++) {
                for (int z = zRange.x; z < zRange.y; z++) {
                    for (int x = xRange.x; x < xRange.y; x++) {

                        if (isCoordOutBoundaries(x, y, z)) {
                            continue;
                        } else if (terrain.at(x).at(y).at(z).air) {
                            continue;
                        }
                        
                        if (terrain.at(x).at(y).at(z).transparent) {
                            transparentBlocks.push_back(glm::vec3(x, y, z));
                            continue;
                        }

                        // Figuring out which sides should be rendered or not.
                        // Only render side if it has air next to it
                        terrain.at(x).at(y).at(z).culledFaces = {true, true, true, true, true, true};

                        getHiddenFaces(x, y, z, terrain.at(x).at(y).at(z).culledFaces, true);

                        if (utility::countFalses(terrain.at(x).at(y).at(z).culledFaces) < 6) {
                            listOfBlocksToRender.push_back(&terrain.at(x).at(y).at(z));
                        }

                    }
                }
            }

            // Draws transparent blocks last
            for (auto i : transparentBlocks) {
                
                terrain.at(i.x).at(i.y).at(i.z).culledFaces = {true, true, true, true, true, true};

                getHiddenFaces(i.x, i.y, i.z, terrain.at(i.x).at(i.y).at(i.z).culledFaces, false);

                if (utility::countFalses(terrain.at(i.x).at(i.y).at(i.z).culledFaces) < 6) {
                    listOfBlocksToRender.push_back(&terrain.at(i.x).at(i.y).at(i.z));
                }
            }

            lastRenderedPos = getCurrCamera()->pos;
        }

        /**
         * @brief Stores the hidden faces inside the given std::vector
         * 
         * @param x 
         * @param y 
         * @param z 
         * @param faces 
         * @param glassIncluded 
         */
        void getHiddenFaces(int x, int y, int z, std::vector<bool> &faces, bool glassIncluded) {
            if (!isCoordOutBoundaries(x, y - 1, z) && !(terrain.at(x).at(y - 1).at(z).air || glassIncluded * terrain.at(x).at(y - 1).at(z).transparent)) {
                faces[0] = false; // 0 bottom
            }
            if (!isCoordOutBoundaries(x, y + 1, z) && !(terrain.at(x).at(y + 1).at(z).air || glassIncluded * terrain.at(x).at(y + 1).at(z).transparent)) {
                faces[1] = false; // 1 Is top
            }
            if (!isCoordOutBoundaries(x, y, z + 1) && !(terrain.at(x).at(y).at(z + 1).air || glassIncluded * terrain.at(x).at(y).at(z + 1).transparent)) {
                faces[2] = false;
            }
            if (!isCoordOutBoundaries(x, y, z - 1) && !(terrain.at(x).at(y).at(z - 1).air || glassIncluded * terrain.at(x).at(y).at(z - 1).transparent)) {
                faces[3] = false;
            }
            if (!isCoordOutBoundaries(x + 1, y, z) && !(terrain.at(x + 1).at(y).at(z).air || glassIncluded * terrain.at(x + 1).at(y).at(z).transparent)) {
                faces[4] = false;
            }
            if (!isCoordOutBoundaries(x - 1, y, z) && !(terrain.at(x - 1).at(y).at(z).air || glassIncluded * terrain.at(x - 1).at(y).at(z).transparent)) {
                faces[5] = false;
            }
        }

        /**
         * @brief Call this to draw the HUD and other elements which rotate in respect to the player's view point
         * 
         * @param parent_mvp 
         * @param renderInfo 
         */
        void drawScreen(const glm::mat4 &parent_mvp, renderer::renderer_t renderInfo) {

            screen.translation = playerCamera.pos;
            screen.rotation.x = playerCamera.pitch;
            screen.rotation.y = -playerCamera.yaw;

            drawElement(&screen, parent_mvp, renderInfo, defaultSpecular);
        }

        /**
         * @brief Prints out a list of code lines to be used for copying and pasting into preset worlds
         * 
         */
        void convertCurrWorldIntoData() {
            for (size_t x = 0; x < terrain.size(); x++) {
                for (size_t y = 0; y < terrain.at(0).size(); y++) {
                    for (size_t z = 0; z < terrain.at(0).at(0).size(); z++) {
                        if (!terrain.at(x).at(y).at(z).air) {
                            std::string code = "listOfBlocks.emplace_back(scene::miniBlockData(\"" + terrain.at(x).at(y).at(z).name + "\", glm::vec3(";
                            std::cout << code.c_str() << x - (float)terrain.size() / 2.0f << ", " << y << ", " << z - (float)terrain.at(0).at(0).size() / 2.0f << "), true));\n";
                        }
                    }  
                }
            }
        }

        /**
         * @brief Destroys all meshes and maps used for this world
         * 
         */
        void destroyEverthing() {
            for (size_t x = 0; x < terrain.size(); x++) {
                for (size_t y = 0; y < terrain.at(0).size(); y++) {
                    for (size_t z = 0; z < terrain.at(0).at(0).size(); z++) {
                        destroy(&terrain.at(x).at(y).at(z), true);
                    }  
                }
            }
            destroy(&bed, true);
            destroy(&centreOfWorld, true);
            destroy(&screen, true);
            destroy(&highlightedBlock, true);

            for (auto i : moonPhases) {
                texture_2d::destroy(i);
            }
            texture_2d::destroy(defaultSpecular);
        }
    };

}
#endif //COMP3421_SCENE_OBJECT_HPP
