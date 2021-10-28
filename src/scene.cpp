#include <ass2/scene.hpp>
#include <ass2/texture_2d.hpp>

namespace scene {
    
    void drawBlock(const node_t *node, const glm::mat4 &parent_mvp, GLint mvp_loc, std::vector<bool> faces) {
        glm::mat4 mvp = parent_mvp;
        mvp *= glm::translate(glm::mat4(1.0), node->translation);
        mvp *= glm::scale(glm::mat4(1.0), node->scale);
        mvp *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.z), glm::vec3(0, 0, 1));
        mvp *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.y), glm::vec3(0, 1, 0));
        mvp *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.x), glm::vec3(1, 0, 0));

        texture_2d::bind(node->texture);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

        if (node->mesh.vbo) {
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

    void drawElement(const node_t *node, const glm::mat4 &parent_mvp, GLint mvp_loc) {

        glm::mat4 mvp = parent_mvp;
        mvp *= glm::translate(glm::mat4(1.0), node->translation);
        mvp *= glm::scale(glm::mat4(1.0), node->scale);
        mvp *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.z), glm::vec3(0, 0, 1));
        mvp *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.y), glm::vec3(0, 1, 0));
        mvp *= glm::rotate(glm::mat4(1.0), glm::radians(node->rotation.x), glm::vec3(1, 0, 0));

        texture_2d::bind(node->texture);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, glm::value_ptr(mvp));

        if (node->mesh.vbo) {
            static_mesh::draw(node->mesh, GL_TRIANGLES);
        }
        // Recursively draw the celestial bodies that are dependent on this celestial body
        for (auto child : node->children) {
            scene::drawElement(&child, mvp, mvp_loc);
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
    node_t createBlock(int x, int y, int z, GLuint texID) {

        node_t block;
        block.air = false;
        block.noCollisionBox = false;
        block.mesh = shapes::createCube(x, y, z);
        block.texture = texID;
        block.x = x;
        block.y = y;
        block.z = z;

        return block;
    }

    node_t createFlatSquare(GLuint texID) {

        node_t square;
        square.mesh = shapes::createFlatSquare();
        square.texture = texID;

        return square;
    }
}
