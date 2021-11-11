#include <ass2/scene.hpp>
#include <ass2/texture_2d.hpp>

#include <fstream>

namespace scene {

    void drawBlock(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo, GLuint defaultSpecular) {
        
        model *= glm::translate(glm::mat4(1.0), node->translation);
        model *= glm::scale(glm::mat4(1.0), node->scale);
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.z), glm::vec3(0, 0, 1));
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.y), glm::vec3(0, 1, 0));
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.x), glm::vec3(1, 0, 0));

        glUniformMatrix4fv(renderInfo.model_loc, 1, GL_FALSE, glm::value_ptr(model));

        if (node->mesh.vbo) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, node->textureID);
            glActiveTexture(GL_TEXTURE1);
            if (node->specularID == 4294967295) {
                glBindTexture(GL_TEXTURE_2D, defaultSpecular);
            } else {
                glBindTexture(GL_TEXTURE_2D, node->specularID);
            }
            glUniform1f(renderInfo.mat_tex_factor_loc, node->textureID ? 1.0f : 0.0f);
            glUniform1f(renderInfo.mat_specular_factor_loc, node->specularID ? 1.0f : 0.0f);
            glUniform4fv(renderInfo.mat_color_loc, 1, glm::value_ptr(node->color));
            glUniform3fv(renderInfo.mat_diffuse_loc, 1, glm::value_ptr(node->diffuse));
            glUniform4fv(renderInfo.mat_specular_loc, 1, glm::value_ptr(node->specular));
            glUniform1f(renderInfo.phong_exponent_loc, node->phong_exp);
            glBindVertexArray(node->mesh.vao);
            // Ensures to only render the sides that has an air block with that side
            if (node->ignoreCulling) {
                glDrawElements(GL_TRIANGLES, node->mesh.indices_count, GL_UNSIGNED_INT, nullptr);
            } else {
                for (std::vector<int>::size_type index = 0; index < node->culledFaces.size(); index++) {
                    if (node->culledFaces.at(index)) {
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(index * 6 * (int)sizeof(GLuint)));
                    }
                }
            }
            glBindVertexArray(0);
        }
    }

    void drawElement(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo, GLuint defaultSpecular) {

        model *= glm::translate(glm::mat4(1.0), node->translation);
        model *= glm::scale(glm::mat4(1.0), node->scale);
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.z), glm::vec3(0, 0, 1));
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.y), glm::vec3(0, 1, 0));
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.x), glm::vec3(1, 0, 0));

        glUniformMatrix4fv(renderInfo.model_loc, 1, GL_FALSE, glm::value_ptr(model));

        if (node->mesh.vbo && !node->air) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, node->textureID);
            glActiveTexture(GL_TEXTURE1);
            if (node->specularID == -1) {
                glBindTexture(GL_TEXTURE_2D, defaultSpecular);
            } else {
                glBindTexture(GL_TEXTURE_2D, node->specularID);
            }

            glUniform1f(renderInfo.mat_tex_factor_loc, node->textureID ? 1.0f : 0.0f);
            glUniform1f(renderInfo.mat_specular_factor_loc, node->specularID ? 1.0f : 0.0f);
            glUniform4fv(renderInfo.mat_color_loc, 1, glm::value_ptr(node->color));
            glUniform3fv(renderInfo.mat_diffuse_loc, 1, glm::value_ptr(node->diffuse));
            glUniform1f(renderInfo.phong_exponent_loc, node->phong_exp);

            glBindVertexArray(node->mesh.vao);
            glDrawElements(GL_TRIANGLES, node->mesh.indices_count, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
        
        // Recursively draw the celestial bodies that are dependent on this celestial body
        for (auto child : node->children) {
            scene::drawElement(&child, model, renderInfo, defaultSpecular);
        }
        
        return;
    }

    void destroy(const node_t *node, bool destroyTexture) {
        for (auto child : node->children) {
            scene::destroy(&child, destroyTexture);
        }
        static_mesh::destroy(node->mesh);
        if (destroyTexture) {
            texture_2d::destroy(node->textureID);
            texture_2d::destroy(node->specularID);
        }
    }

    blockData combineBlockData(std::string stringName, bool transparent, bool illuminating, bool rotatable, glm::vec3 color, float intensity) {
        blockData data;
        GLuint texID;
        GLuint specID;

        // Checks if the diffuse map exists before initialising it
        std::string diffuseFilePath = "./res/textures/blocks/wool/" + stringName;
        std::ifstream fileStreamA(diffuseFilePath + ".png");
        if (fileStreamA.good())
            texID = texture_2d::init(diffuseFilePath + ".png");
        else {
            diffuseFilePath = "./res/textures/blocks/" + stringName;
            texID = texture_2d::init(diffuseFilePath + ".png");
        }
        fileStreamA.close();
        
        // Checks if the specular map exists before initialising it
        std::string specularFilePath = diffuseFilePath + "_specular.png";
        std::ifstream fileStreamB(specularFilePath.c_str());
        if (fileStreamB.good()) {
            specID = texture_2d::init(specularFilePath.c_str());
        } else {
            specID = 4294967295;
        }
        fileStreamB.close();
        
        data.texture = texID;
        data.specularMap = specID;
        data.transparent = transparent;
        data.illuminating = illuminating;
        data.intensity = intensity;
        data.rgb = color;
        data.blockName = stringName;
        data.rotatable = rotatable;

        return data;
    }

    node_t createBlock(int x, int y, int z, GLuint texID, GLuint specID, bool transparent, bool invertNormals, bool affectedByLight) {

        node_t block;
        block.name = "N/A";
        block.air = false;
        block.mesh = shapes::createCube(invertNormals, affectedByLight);
        block.textureID = texID;
        block.specularID = specID;
        block.x = x;
        block.y = y;
        block.z = z;
        block.transparent = transparent;
        block.ignoreCulling = false;
        block.translation = glm::vec3(x, y, z);

        return block;
    }

    node_t createBlock(int x, int y, int z, blockData data, bool invertNormals, bool affectedByLight) {

        node_t block;
        block.name = data.blockName;
        block.air = false;
        block.mesh = shapes::createCube(invertNormals, affectedByLight);
        block.textureID = data.texture;
        block.specularID = data.specularMap;
        block.x = x;
        block.y = y;
        block.z = z;
        block.transparent = data.transparent;
        block.ignoreCulling = data.rotatable;
        block.translation = glm::vec3(x, y, z);

        return block;
    }

    node_t createSkySphere(GLuint texID, float radius, int tesselation) {

        node_t sphere;
        sphere.mesh = shapes::createSphere(radius, tesselation);
        sphere.textureID = texID;
        sphere.air = false;

        return sphere;
    }

    node_t createFlatSquare(GLuint texID, bool invert) {

        node_t square;
        square.mesh = shapes::createFlatSquare(invert);
        square.textureID = texID;
        square.air = false;
        return square;
    }

    node_t createBedPlayer(GLuint bedTexID, GLuint playerTexID) {

        node_t bed;
        bed.mesh = shapes::createBed();
        bed.textureID = bedTexID;
        bed.translation = glm::vec3(0.0f, 4.0f, 0.0f);
        bed.air = false;

        node_t player;

        node_t playerTorso;
        playerTorso.mesh = shapes::createPlayerTorso();
        playerTorso.textureID = playerTexID;
        playerTorso.air = false;

        node_t playerHead;
        playerHead.mesh = shapes::createPlayerHead();
        playerHead.textureID = playerTexID;
        playerHead.translation.y += 0.0625 * 10;
        playerHead.air = false;
        playerHead.rotation.x += 12.5f;
        playerTorso.children.push_back(playerHead);

        node_t playerLeftArm;
        playerLeftArm.mesh = shapes::createPlayerArmLeft();
        playerLeftArm.textureID = playerTexID;
        playerLeftArm.translation.x += 0.0625 * 6.2;
        playerLeftArm.air = false;
        playerLeftArm.rotation.z += 2.5f;
        playerTorso.children.push_back(playerLeftArm);

        node_t playerLeftLeg;
        playerLeftLeg.mesh = shapes::createPlayerLegLeft();
        playerLeftLeg.textureID = playerTexID;
        playerLeftLeg.translation.y -= 0.0625 * 12;
        playerLeftLeg.translation.x += 0.0625 * 2;
        playerLeftLeg.air = false;
        playerTorso.children.push_back(playerLeftLeg);

        node_t playerRightArm;
        playerRightArm.mesh = shapes::createPlayerArmRight();
        playerRightArm.textureID = playerTexID;
        playerRightArm.translation.x -= 0.0625 * 6.2;
        playerRightArm.air = false;
        playerRightArm.rotation.z -= 2.5f;
        playerTorso.children.push_back(playerRightArm);

        node_t playerRightLeg;
        playerRightLeg.mesh = shapes::createPlayerLegRight();
        playerRightLeg.textureID = playerTexID;
        playerRightLeg.translation.y -= 0.0625 * 12;
        playerRightLeg.translation.x -= 0.0625 * 2;
        playerRightLeg.air = false;
        playerTorso.children.push_back(playerRightLeg);

        player.translation.y += 0.0625 * 3;
        player.translation.z -= 0.0625 * 2;
        player.rotation.x -= 90.0f;
        player.children.push_back(playerTorso);
        bed.children.push_back(player);

        return bed;
    }
}
