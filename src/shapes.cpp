#include <ass2/static_mesh.hpp>
#include <ass2/shapes.hpp>

namespace shapes {

    const float lengthOfTexture = 96.0;
    const float lengthOfSide = 16.0;

    static_mesh::mesh_t createCube(int x, int y, int z) {
        static_mesh::mesh_template_t cube;

        double xDbl = (double)x;
        double yDbl = (double)y;
        double zDbl = (double)z;

        cube.positions = {

            // Bottom Face
            {xDbl + -0.5, yDbl + -0.5,  zDbl + 0.5},
            {xDbl + 0.5, yDbl + -0.5,  zDbl + 0.5},
            {xDbl + 0.5, yDbl + -0.5, zDbl + -0.5},
            {xDbl + -0.5, yDbl + -0.5, zDbl + -0.5},

            // Top Face
            {xDbl + -0.5,  yDbl + 0.5,  zDbl + 0.5},
            {xDbl + 0.5,  yDbl + 0.5,  zDbl + 0.5},
            {xDbl + 0.5,  yDbl + 0.5, zDbl + -0.5},
            {xDbl + -0.5,  yDbl + 0.5, zDbl + -0.5},

            // Side A
            {xDbl + -0.5, yDbl + -0.5,  zDbl + 0.5},
            {xDbl + 0.5, yDbl + -0.5,  zDbl + 0.5},
            {xDbl + 0.5,  yDbl + 0.5,  zDbl + 0.5},
            {xDbl + -0.5,  yDbl + 0.5,  zDbl + 0.5},
            
            // Back Side A
            {xDbl + -0.5, yDbl + -0.5, zDbl + -0.5},
            {xDbl + 0.5, yDbl + -0.5, zDbl + -0.5},
            {xDbl + 0.5,  yDbl + 0.5, zDbl + -0.5},
            {xDbl + -0.5,  yDbl + 0.5, zDbl + -0.5},

            // Side A
            {xDbl + 0.5,  yDbl + -0.5, zDbl + -0.5},
            {xDbl + 0.5,  yDbl + -0.5,  zDbl + 0.5},
            {xDbl + 0.5,  yDbl + 0.5,  zDbl + 0.5},
            {xDbl + 0.5,  yDbl + 0.5, zDbl + -0.5},
            
            // Back Side A
            {xDbl + -0.5,  yDbl + -0.5, zDbl + -0.5},
            {xDbl + -0.5,  yDbl + -0.5,  zDbl + 0.5},
            {xDbl + -0.5,  yDbl + 0.5,  zDbl + 0.5},
            {xDbl + -0.5,  yDbl + 0.5, zDbl + -0.5},
        };

        double bottomFaceRange = lengthOfSide / lengthOfTexture;
        double topFaceRange = 2 * lengthOfSide / lengthOfTexture;
        double sideFaceARange = 3 * lengthOfSide / lengthOfTexture;
        double sideFaceBRange = 4 * lengthOfSide / lengthOfTexture;
        double sideFaceCRange = 5 * lengthOfSide / lengthOfTexture;

        cube.tex_coords = {

            // Bottom Face
            {  0,  1}, // 0
            {  0,  0}, // 1
            {  bottomFaceRange,  0}, // 2
            {  bottomFaceRange,  1}, // 3

            // Top Face
            {  topFaceRange,  1}, // 4
            {  topFaceRange,  0}, // 5
            {  bottomFaceRange,  0}, // 6
            {  bottomFaceRange,  1}, // 7

            // Side A
            {  topFaceRange,  1}, // 8
            {  sideFaceARange,  1}, // 9
            {  sideFaceARange,  0}, // 10
            {  topFaceRange,  0}, // 11
            
            // Back Side A
            {  sideFaceCRange,  1}, // 12
            {  sideFaceBRange,  1}, // 13
            {  sideFaceBRange,  0}, // 14
            {  sideFaceCRange,  0}, // 15

            // Side A
            {  sideFaceBRange,  1}, // 8
            {  sideFaceARange,  1}, // 9
            {  sideFaceARange,  0}, // 10
            {  sideFaceBRange,  0}, // 11
            
            // Back Side A
            {  sideFaceCRange,  1}, // 12
            {  1,  1}, // 13
            {  1,  0}, // 14
            {  sideFaceCRange,  0}, // 15
            
        };

        cube.indices = {
            0, 2, 1, // Face 0 1 2 3
            0, 3, 2,

            4, 5, 6, // Face 4 5 6 7
            4, 6, 7,

            8, 9, 10, // Face 8 9 10 11
            8, 10, 11,

            12, 14, 13, // Face 12 13 14 15
            12, 15, 14,

            16, 18, 17, // Face 16 17 18 19
            16, 19, 18,

            20, 21, 22, // Face 20 21 22 23
            20, 22, 23,
        };


        return static_mesh::init(cube);
    }



    static_mesh::mesh_t createFlatSquare() {
        static_mesh::mesh_template_t square;

        square.positions = {

            // Side A
            {-0.5, -0.5,  0.5},
            {0.5, -0.5,  0.5},
            {0.5,  0.5,  0.5},
            {-0.5,  0.5, 0.5},
            
        };

        square.tex_coords = {

            // Side A
            {  0,  1}, // 8
            {  1,  1}, // 9
            {  1,  0}, // 10
            {  0,  0}, // 11
            
        };

        square.indices = {
            0, 1, 2,
            0, 2, 3,
        };


        return static_mesh::init(square);
    }

}
