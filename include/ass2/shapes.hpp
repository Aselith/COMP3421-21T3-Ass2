#ifndef COMP3421_SHAPES_HPP
#define COMP3421_SHAPES_HPP

#include <glm/glm.hpp>
#include <ass2/static_mesh.hpp>

namespace shapes {

    /**
    * Creates a cube static mesh
    */
    static_mesh::mesh_t createCube(int x, int y, int z);

    static_mesh::mesh_t createFlatSquare();

}

#endif //COMP3421_SHAPES_HPP
