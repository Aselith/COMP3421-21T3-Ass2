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

}

#endif //COMP3421_SHAPES_HPP
