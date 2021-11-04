#ifndef COMP3421_SHAPES_HPP
#define COMP3421_SHAPES_HPP

#include <glm/glm.hpp>
#include <ass2/static_mesh.hpp>

namespace shapes {

    /**
    * Creates a cube static mesh
    */
    static_mesh::mesh_t createCube(int x, int y, int z, bool invertNormals, bool affectedByLight);

    static_mesh::mesh_t createFlatSquare(bool invert);

    static_mesh::mesh_t createSphere(float radius, unsigned int tessellation);

    static_mesh::mesh_t createBed();
    static_mesh::mesh_t createPlayerHead();
    static_mesh::mesh_t createPlayerTorso();
    static_mesh::mesh_t createPlayerLegRight();
    static_mesh::mesh_t createPlayerLegLeft();
    static_mesh::mesh_t createPlayerArmRight();
    static_mesh::mesh_t createPlayerArmLeft();

}

#endif //COMP3421_SHAPES_HPP
