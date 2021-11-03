#include <ass2/static_mesh.hpp>
#include <ass2/shapes.hpp>
#include <ass2/utility.hpp>

namespace shapes {

    const float lengthOfTexture = 96.0;
    const float lengthOfSide = 16.0;

    static_mesh::mesh_t createCube(int x, int y, int z, bool invertNormals, bool affectedByLight) {
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

        utility::calcVertNormals(cube);
        if (!affectedByLight) {
            for (auto i = size_t{0}; i < cube.normals.size(); i++) {
                cube.normals[i] = glm::vec3(0, 0, 0);
            }
        }
        if (invertNormals) {
            for (auto i = size_t{0}; i < cube.normals.size(); i++) {
                cube.normals[i] *= -1;
            }
        }
        return static_mesh::init(cube);
    }


    static_mesh::mesh_t createFlatSquare(bool invert) {
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

        utility::calcVertNormals(square);
        for (auto i = size_t{0}; i < square.normals.size(); i++) {
            square.normals[i] = glm::vec3(0, 0, 0);
        }
        if (invert) {
            utility::invertShape(square);
        }
        return static_mesh::init(square);
    }


    static_mesh::mesh_t createSphere(float radius, unsigned int tessellation) {
		static_mesh::mesh_template_t sphere;

		float ang_inc = 2.0f * (float)M_PI / (float)tessellation;
		unsigned int stacks = tessellation / 2;
		unsigned int start_angle_i = 3 * tessellation / 4;
		for (unsigned int i = start_angle_i; i <= start_angle_i + stacks; ++i) {
			float alpha = ang_inc * (float)i;
			float y = radius * std::sin(alpha);
			float slice_radius = radius * std::cos(alpha);
			for (unsigned int j = 0; j <= tessellation; ++j) {
				float beta = ang_inc * (float)j;
				float z = slice_radius * std::cos(beta);
				float x = slice_radius * std::sin(beta);
				sphere.positions.emplace_back(x, y, z);
				sphere.tex_coords.emplace_back((float)j * 1.0f / (float)tessellation,
				                               (float)(i - start_angle_i) * 2.0f / (float)tessellation);
			}
		}
		// create the indices
		for (unsigned int i = 1; i <= tessellation / 2; ++i) {
			unsigned int prev = (1u + tessellation) * (i - 1);
			unsigned int curr = (1u + tessellation) * i;
			for (unsigned int j = 0; j < tessellation; ++j) {
				sphere.indices.push_back(curr + j);
				sphere.indices.push_back(prev + j);
				sphere.indices.push_back(prev + j + 1);
				sphere.indices.push_back(prev + j + 1);
				sphere.indices.push_back(curr + j + 1);
				sphere.indices.push_back(curr + j);
			}
		}

        utility::calcVertNormals(sphere);
        utility::invertShape(sphere);

		return static_mesh::init(sphere);
	}


}
