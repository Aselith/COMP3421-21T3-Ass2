#include <ass2/scene.hpp>
#include <ass2/texture_2d.hpp>

namespace scene {
    
    void drawBlock(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo, std::vector<bool> faces) {
        model *= glm::translate(glm::mat4(1.0), node->translation);
        model *= glm::scale(glm::mat4(1.0), node->scale);
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.z), glm::vec3(0, 0, 1));
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.y), glm::vec3(0, 1, 0));
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.x), glm::vec3(1, 0, 0));

        glUniformMatrix4fv(renderInfo.model_loc, 1, GL_FALSE, glm::value_ptr(model));

        if (node->mesh.vbo) {
            texture_2d::bind(node->texture);
            glUniform1f(renderInfo.mat_tex_factor_loc, node->texture ? 1.0f : 0.0f);
			glUniform4fv(renderInfo.mat_color_loc, 1, glm::value_ptr(node->color));
			glUniform3fv(renderInfo.mat_diffuse_loc, 1, glm::value_ptr(node->diffuse));
            glBindVertexArray(node->mesh.vao);
            // Ensures to only render the sides that has an air block with that side
            for (std::vector<int>::size_type index = 0; index < faces.size(); index++) {
                if (faces.at(index)) {
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(index * 6 * (int)sizeof(GLuint)));
                }
            }
            glBindVertexArray(0);
        }
    }

    void drawElement(const node_t *node, glm::mat4 model, renderer::renderer_t renderInfo) {

        
        model *= glm::translate(glm::mat4(1.0), node->translation);
        model *= glm::scale(glm::mat4(1.0), node->scale);
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.z), glm::vec3(0, 0, 1));
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.y), glm::vec3(0, 1, 0));
        model *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.x), glm::vec3(1, 0, 0));

        glUniformMatrix4fv(renderInfo.model_loc, 1, GL_FALSE, glm::value_ptr(model));

        if (node->mesh.vbo && !node->air) {
            texture_2d::bind(node->texture);
            glUniform1f(renderInfo.mat_tex_factor_loc, node->texture ? 1.0f : 0.0f);
            glUniform4fv(renderInfo.mat_color_loc, 1, glm::value_ptr(node->color));
            glUniform3fv(renderInfo.mat_diffuse_loc, 1, glm::value_ptr(node->diffuse));

            glBindVertexArray(node->mesh.vao);
            glDrawElements(GL_TRIANGLES, node->mesh.indices_count, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
        
        // Recursively draw the celestial bodies that are dependent on this celestial body
        for (auto child : node->children) {
            scene::drawElement(&child, model, renderInfo);
        }
        
        return;
    }


    void destroy(const node_t *node) {
        for (auto child : node->children) {
            scene::destroy(&child);
        }
        static_mesh::destroy(node->mesh);
        texture_2d::destroy(node->texture);
    }
    
    /*
    Creates a node with a block for it
    */
    node_t createBlock(int x, int y, int z, GLuint texID, bool invertNormals, bool affectedByLight) {

        node_t block;
        block.air = false;
        block.mesh = shapes::createCube(x, y, z, invertNormals, affectedByLight);
        block.texture = texID;
        block.x = x;
        block.y = y;
        block.z = z;

        return block;
    }

    node_t createSkySphere(GLuint texID, float radius, int tesselation) {

        node_t sphere;
        sphere.mesh = shapes::createSphere(radius, tesselation);
        sphere.texture = texID;
        sphere.air = false;

        return sphere;
    }

    node_t createFlatSquare(GLuint texID, bool invert) {

        node_t square;
        square.mesh = shapes::createFlatSquare(invert);
        square.texture = texID;
        square.air = false;
        return square;
    }

    node_t createBedPlayer(GLuint bedTexID, GLuint playerTexID) {

        node_t bed;
        bed.mesh = shapes::createBed();
        bed.texture = bedTexID;
        bed.translation = glm::vec3(WORLD_WIDTH / 2.0f, 4.0f, WORLD_WIDTH / 2.0f);
        bed.air = false;

        node_t player;

        node_t playerTorso;
        playerTorso.mesh = shapes::createPlayerTorso();
        playerTorso.texture = playerTexID;
        playerTorso.air = false;

        node_t playerHead;
        playerHead.mesh = shapes::createPlayerHead();
        playerHead.texture = playerTexID;
        playerHead.translation.y += 0.0625 * 10;
        playerHead.air = false;
        playerHead.rotation.x += 12.5f;
        playerTorso.children.push_back(playerHead);

        node_t playerLeftArm;
        playerLeftArm.mesh = shapes::createPlayerArmLeft();
        playerLeftArm.texture = playerTexID;
        playerLeftArm.translation.x += 0.0625 * 6.2;
        playerLeftArm.air = false;
        playerLeftArm.rotation.z += 2.5f;
        playerTorso.children.push_back(playerLeftArm);

        node_t playerLeftLeg;
        playerLeftLeg.mesh = shapes::createPlayerLegLeft();
        playerLeftLeg.texture = playerTexID;
        playerLeftLeg.translation.y -= 0.0625 * 12;
        playerLeftLeg.translation.x += 0.0625 * 2;
        playerLeftLeg.air = false;
        playerTorso.children.push_back(playerLeftLeg);

        node_t playerRightArm;
        playerRightArm.mesh = shapes::createPlayerArmRight();
        playerRightArm.texture = playerTexID;
        playerRightArm.translation.x -= 0.0625 * 6.2;
        playerRightArm.air = false;
        playerRightArm.rotation.z -= 2.5f;
        playerTorso.children.push_back(playerRightArm);

        node_t playerRightLeg;
        playerRightLeg.mesh = shapes::createPlayerLegRight();
        playerRightLeg.texture = playerTexID;
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
