#include <ass2/static_mesh.hpp>
#include <ass2/shapes.hpp>
#include <ass2/utility.hpp>

namespace shapes {

    const float lengthOfTexture = 96.0;
    const float lengthOfSide = 16.0;

    const std::vector<GLuint> basicCubicIndices = {
        0, 2, 1, // Face 0 1 2 3
        0, 3, 2,

        4, 5, 6, // Face 4 5 6 7
        4, 6, 7,

        8, 9, 10, // Face 8 9 10 11
        8, 10, 11,

        12, 13, 14, // Face 12 13 14 15
        12, 14, 15,

        16, 17, 18, // Face 16 17 18 19
        16, 18, 19,

        20, 21, 22, // Face 20 21 22 23
        20, 22, 23,
    };

    static_mesh::mesh_t createCube(bool invertNormals, bool affectedByLight) {
        static_mesh::mesh_template_t cube;

        cube.positions = {

            // Bottom Face
            {-0.5, -0.5,  0.5},
            {0.5, -0.5,  0.5},
            {0.5, -0.5, -0.5},
            {-0.5, -0.5, -0.5},

            // Top Face
            {-0.5,  0.5,  0.5},
            { 0.5,  0.5,  0.5},
            { 0.5,  0.5,  -0.5},
            {-0.5,  0.5,  -0.5},

            // Side A
            {-0.5, -0.5,  0.5},
            {0.5, -0.5,  0.5},
            {0.5,  0.5,  0.5},
            {-0.5,  0.5,  0.5},
            
            // Back Side A
            { 0.5, -0.5, -0.5},
            {-0.5, -0.5, -0.5},
            {-0.5,  0.5, -0.5},
            { 0.5,  0.5, -0.5},

            // Side A
            {0.5,  -0.5,  0.5},
            {0.5,  -0.5,  -0.5},
            {0.5,  0.5,  -0.5},
            {0.5,  0.5,  0.5},
            
            // Back Side A
            {-0.5,  -0.5, -0.5},
            {-0.5,  -0.5,  0.5},
            {-0.5,  0.5,  0.5},
            {-0.5,  0.5, -0.5},
        };

        double bottomFaceRange = lengthOfSide / lengthOfTexture;
        double topFaceRange = 2 * lengthOfSide / lengthOfTexture;
        double sideFaceARange = 3 * lengthOfSide / lengthOfTexture;
        double sideFaceBRange = 4 * lengthOfSide / lengthOfTexture;
        double sideFaceCRange = 5 * lengthOfSide / lengthOfTexture;

        cube.tex_coords = {

            // Bottom Face
            {  bottomFaceRange,  1}, // 8
            {  0,  1}, // 9
            {  0,  0}, // 10
            {  bottomFaceRange,  0}, // 11

            // Top Face
            {  bottomFaceRange,  1}, // 8
            {  topFaceRange,  1}, // 9
            {  topFaceRange,  0}, // 10
            {  bottomFaceRange,  0}, // 11

            // Side A
            {  topFaceRange,  1}, // 8
            {  sideFaceARange,  1}, // 9
            {  sideFaceARange,  0}, // 10
            {  topFaceRange,  0}, // 11
            
            // Back Side A
            {  sideFaceBRange,  1}, // 12
            {  sideFaceCRange,  1}, // 13
            {  sideFaceCRange,  0}, // 14
            {  sideFaceBRange,  0}, // 15

            // Side A
            {  sideFaceARange,  1}, // 12
            {  sideFaceBRange,  1}, // 13
            {  sideFaceBRange,  0}, // 14
            {  sideFaceARange,  0}, // 15
            
            // Back Side A
            {  sideFaceCRange,  1}, // 12
            {  1,  1}, // 13
            {  1,  0}, // 14
            {  sideFaceCRange,  0}, // 15
            
        };

        cube.indices = basicCubicIndices;

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

    static_mesh::mesh_t createBed() {
        static_mesh::mesh_template_t bed;

        double height = 9.0f / 16.0f;
        double legHeight = 3.0f / 16.0f;
        double texWidth = 72.0f;
        double texHeight = 32.0f;
        bed.positions = {

            // Bottom Face
            {-0.5, -0.5 + legHeight,  1.0},
            { 0.5, -0.5 + legHeight,  1.0},
            { 0.5, -0.5 + legHeight, -1.0},
            {-0.5, -0.5 + legHeight, -1.0},

            // Top Face
            {-0.5, -0.5 + height,  1.0},
            { 0.5, -0.5 + height,  1.0},
            { 0.5, -0.5 + height, -1.0},
            {-0.5, -0.5 + height, -1.0},

            // Side A
            {-0.5, -0.5, 1.0},
            { 0.5, -0.5, 1.0},
            { 0.5, -0.5 + height, 1.0},
            {-0.5, -0.5 + height, 1.0},
            
            // Back Side A
            {-0.5, -0.5, -1.0},
            { 0.5, -0.5, -1.0},
            { 0.5, -0.5 + height, -1.0},
            {-0.5, -0.5 + height, -1.0},

            // Side A
            { 0.5,  -0.5, -1.0},
            { 0.5,  -0.5, 1.0},
            { 0.5,  -0.5 + height,  1.0},
            { 0.5,  -0.5 + height, -1.0},
            
            // Back Side A
            { -0.5,  -0.5, -1.0},
            { -0.5,  -0.5,  1.0},
            { -0.5,  -0.5 + height,   1.0},
            { -0.5,  -0.5 + height,  -1.0},
        };

        bed.tex_coords = {

            // Bottom Face
            {  50.0f / texWidth,  1}, // 3
            {  34.0f / texWidth,  1}, // 0
            {  34.0f / texWidth,  0}, // 1
            {  50.0f / texWidth,  0}, // 2

            // Top Face
            {  25.0f / texWidth,  1}, // 7
            {  9.0f / texWidth,  1}, // 4
            {  9.0f / texWidth,  0}, // 5
            {  25.0f / texWidth,  0}, // 6

            // Side A
            {  66.0f / texWidth,  texHeight / texHeight}, // 14
            {  50.0f / texWidth,  texHeight / texHeight}, // 15
            {  50.0f / texWidth,  23.0f / texHeight}, // 12
            {  66.0f / texWidth,  23.0f / texHeight}, // 13
            
            // Back Side A
            {  66.0f / texWidth,  23.0f / texHeight}, // 14
            {  50.0f / texWidth,  23.0f / texHeight}, // 15
            {  50.0f / texWidth,  14.0f / texHeight}, // 12
            {  66.0f / texWidth,  14.0f / texHeight}, // 13

            // Side A
            {  0.0f / texWidth,  0.0f / texHeight}, // 14
            {  0.0f / texWidth,  texHeight / texHeight}, // 13
            {  9.0f / texWidth,  texHeight / texHeight}, // 12
            {  9.0f / texWidth,  0.0f / texHeight}, // 15
            
            
            // Back Side A
            {  34.0f / texWidth,  0.0f / texHeight}, // 15
            {  34.0f / texWidth,  texHeight / texHeight}, // 12
            {  25.0f / texWidth,  texHeight / texHeight}, // 13
            {  25.0f / texWidth,  0.0f / texHeight}, // 14
        };

        bed.indices = {
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

        utility::calcVertNormals(bed);
        return static_mesh::init(bed);
    }


    static_mesh::mesh_t createPlayerHead() {
        static_mesh::mesh_template_t playerHead;

        float texWidth = 64.0f;
        float texHeight = 64.0f;
        playerHead.positions = {

            // Bottom Face
            {-0.25, -0.25,  0.25},
            { 0.25, -0.25,  0.25},
            { 0.25, -0.25, -0.25},
            {-0.25, -0.25, -0.25},

            // Top Face
            {-0.25,  0.25,  0.25},
            { 0.25,  0.25,  0.25},
            { 0.25,  0.25, -0.25},
            {-0.25,  0.25, -0.25},

            // Side A
            {-0.25, -0.25, 0.25},
            { 0.25, -0.25, 0.25},
            { 0.25,  0.25, 0.25},
            {-0.25,  0.25, 0.25},
            
            // Back Side A
            { 0.25, -0.25, -0.25},
            {-0.25, -0.25, -0.25},
            {-0.25,  0.25, -0.25},
            { 0.25,  0.25, -0.25},

            // Side B
            { 0.25,  -0.25,  0.25},
            { 0.25,  -0.25, -0.25},
            { 0.25,   0.25, -0.25},
            { 0.25,   0.25,  0.25},
            
            // Back Side B
            { -0.25,  -0.25, -0.25}, // 2
            { -0.25,  -0.25,  0.25}, // 1
            { -0.25,   0.25,  0.25}, // 4
            { -0.25,   0.25, -0.25}, // 3
            
        };

        playerHead.tex_coords = {

            // Bottom Face
            {  16.0f / texWidth,  8.0f / texHeight}, // 3
            {  24.0f / texWidth,  8.0f / texHeight}, // 0
            {  24.0f / texWidth,  0.0f / texHeight}, // 1
            {  16.0f / texWidth,  0.0f / texHeight}, // 2

            // Top Face
            {  8.0f / texWidth,  8.0f / texHeight}, // 3
            {  16.0f / texWidth,  8.0f / texHeight}, // 0
            {  16.0f / texWidth,  0.0f / texHeight}, // 1
            {  8.0f / texWidth,  0.0f / texHeight}, // 2

            // Side A
            {  8.0f / texWidth,  16.0f / texHeight}, // 3
            {  16.0f / texWidth,  16.0f / texHeight}, // 0
            {  16.0f / texWidth,  8.0f / texHeight}, // 1
            {  8.0f / texWidth,  8.0f / texHeight}, // 2
            
            // Back Side A
            {  24.0f / texWidth,  16.0f / texHeight}, // 3
            {  32.0f / texWidth,  16.0f / texHeight}, // 0
            {  32.0f / texWidth,  8.0f / texHeight}, // 1
            {  24.0f / texWidth,  8.0f / texHeight}, // 2

            // Side B
            {  16.0f / texWidth,  16.0f / texHeight}, // 3
            {  24.0f / texWidth,  16.0f / texHeight}, // 0
            {  24.0f / texWidth,  8.0f / texHeight}, // 1
            {  16.0f / texWidth,  8.0f / texHeight}, // 2

            // Back Side B
            {  0.0f / texWidth,  16.0f / texHeight}, // 3
            {  8.0f / texWidth,  16.0f / texHeight}, // 0
            {  8.0f / texWidth,  8.0f / texHeight}, // 1
            {  0.0f / texWidth,  8.0f / texHeight}, // 2
            
        };

        playerHead.indices = basicCubicIndices;
        utility::calcVertNormals(playerHead);
        return static_mesh::init(playerHead);
    }

    static_mesh::mesh_t createPlayerTorso() {
        static_mesh::mesh_template_t playerTorso;

        float texWidth = 64.0f;
        float texHeight = 64.0f;
        float pixelLength = 0.0625f;
        playerTorso.positions = {
                
            // Bottom Face
            {-4 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 4 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 4 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-4 * pixelLength, -6 * pixelLength, -2 * pixelLength},

            // Top Face
            {-4 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 4 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 4 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            {-4 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side A aka front face
            {-4 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 4 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 4 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            {-4 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            
            // Back Side A
            { 4 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-4 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-4 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            { 4 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side B
            { 4 * pixelLength,  -6 * pixelLength,  2 * pixelLength},
            { 4 * pixelLength,  -6 * pixelLength, -2 * pixelLength},
            { 4 * pixelLength,   6 * pixelLength, -2 * pixelLength},
            { 4 * pixelLength,   6 * pixelLength,  2 * pixelLength},
            
            // Back Side B
            { -4 * pixelLength,  -6 * pixelLength, -2 * pixelLength}, // 2
            { -4 * pixelLength,  -6 * pixelLength,  2 * pixelLength}, // 1
            { -4 * pixelLength,   6 * pixelLength,  2 * pixelLength}, // 4
            { -4 * pixelLength,   6 * pixelLength, -2 * pixelLength}, // 3
        };

        playerTorso.tex_coords = {

            // Bottom Face
            {  28.0f / texWidth,  20.0f / texHeight}, // 3
            {  36.0f / texWidth,  20.0f / texHeight}, // 0
            {  36.0f / texWidth,  16.0f / texHeight}, // 1
            {  28.0f / texWidth,  16.0f / texHeight}, // 2

            // Top Face
            {  20.0f / texWidth,  20.0f / texHeight}, // 3
            {  28.0f / texWidth,  20.0f / texHeight}, // 0
            {  28.0f / texWidth,  16.0f / texHeight}, // 1
            {  20.0f / texWidth,  16.0f / texHeight}, // 2

            // Side A // Front face
            {  20.0f / texWidth,  32.0f / texHeight}, // 3
            {  28.0f / texWidth,  32.0f / texHeight}, // 0
            {  28.0f / texWidth,  20.0f / texHeight}, // 1
            {  20.0f / texWidth,  20.0f / texHeight}, // 2
            
            // Back Side A
            {  32.0f / texWidth,  32.0f / texHeight}, // 3
            {  40.0f / texWidth,  32.0f / texHeight}, // 0
            {  40.0f / texWidth,  20.0f / texHeight}, // 1
            {  32.0f / texWidth,  20.0f / texHeight}, // 2

            // Side B // Our Right
            {  28.0f / texWidth,  32.0f / texHeight}, // 3
            {  32.0f / texWidth,  32.0f / texHeight}, // 0
            {  32.0f / texWidth,  20.0f / texHeight}, // 1
            {  28.0f / texWidth,  20.0f / texHeight}, // 2

            // Back Side B // Left
            {  16.0f / texWidth,  32.0f / texHeight}, // 3
            {  20.0f / texWidth,  32.0f / texHeight}, // 0
            {  20.0f / texWidth,  20.0f / texHeight}, // 1
            {  16.0f / texWidth,  20.0f / texHeight}, // 2
        };

        playerTorso.indices = basicCubicIndices;
        utility::calcVertNormals(playerTorso);
        return static_mesh::init(playerTorso);
    }

    static_mesh::mesh_t createPlayerArmLeft() {
        static_mesh::mesh_template_t playerArmLeft;

        float texWidth = 64.0f;
        float texHeight = 64.0f;
        float pixelLength = 0.0625f;
        playerArmLeft.positions = {
                
            // Bottom Face
            {-2 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength, -6 * pixelLength, -2 * pixelLength},

            // Top Face
            {-2 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side A aka front face
            {-2 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            
            // Back Side A
            { 2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side B
            { 2 * pixelLength,  -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  -6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,   6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,   6 * pixelLength,  2 * pixelLength},
            
            // Back Side B
            { -2 * pixelLength,  -6 * pixelLength, -2 * pixelLength}, // 2
            { -2 * pixelLength,  -6 * pixelLength,  2 * pixelLength}, // 1
            { -2 * pixelLength,   6 * pixelLength,  2 * pixelLength}, // 4
            { -2 * pixelLength,   6 * pixelLength, -2 * pixelLength}, // 3
        };

        playerArmLeft.tex_coords = {

            // Bottom Face
            {  40.0f / texWidth,  52.0f / texHeight}, // 3
            {  44.0f / texWidth,  52.0f / texHeight}, // 0
            {  44.0f / texWidth,  48.0f / texHeight}, // 1
            {  40.0f / texWidth,  48.0f / texHeight}, // 2

            // Top Face
            {  36.0f / texWidth,  52.0f / texHeight}, // 3
            {  40.0f / texWidth,  52.0f / texHeight}, // 0
            {  40.0f / texWidth,  48.0f / texHeight}, // 1
            {  36.0f / texWidth,  48.0f / texHeight}, // 2

            // Side A // Front face
            {  36.0f / texWidth,  64.0f / texHeight}, // 3
            {  40.0f / texWidth,  64.0f / texHeight}, // 0
            {  40.0f / texWidth,  52.0f / texHeight}, // 1
            {  36.0f / texWidth,  52.0f / texHeight}, // 2
            
            // Back Side A
            {  44.0f / texWidth,  64.0f / texHeight}, // 3
            {  48.0f / texWidth,  64.0f / texHeight}, // 0
            {  48.0f / texWidth,  52.0f / texHeight}, // 1
            {  44.0f / texWidth,  52.0f / texHeight}, // 2

            // Side B // Our Right
            {  40.0f / texWidth,  64.0f / texHeight}, // 3
            {  44.0f / texWidth,  64.0f / texHeight}, // 0
            {  44.0f / texWidth,  52.0f / texHeight}, // 1
            {  40.0f / texWidth,  52.0f / texHeight}, // 2

            // Back Side B // Left
            {  32.0f / texWidth,  64.0f / texHeight}, // 3
            {  36.0f / texWidth,  64.0f / texHeight}, // 0
            {  36.0f / texWidth,  52.0f / texHeight}, // 1
            {  32.0f / texWidth,  52.0f / texHeight}, // 2
        };

        playerArmLeft.indices = basicCubicIndices;
        utility::calcVertNormals(playerArmLeft);
        return static_mesh::init(playerArmLeft);
    }


    static_mesh::mesh_t createPlayerLegLeft() {
        static_mesh::mesh_template_t playerLegLeft;

        float texWidth = 64.0f;
        float texHeight = 64.0f;
        float pixelLength = 0.0625f;
        playerLegLeft.positions = {
                
            // Bottom Face
            {-2 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength, -6 * pixelLength, -2 * pixelLength},

            // Top Face
            {-2 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side A aka front face
            {-2 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            
            // Back Side A
            { 2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side B
            { 2 * pixelLength,  -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  -6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,   6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,   6 * pixelLength,  2 * pixelLength},
            
            // Back Side B
            { -2 * pixelLength,  -6 * pixelLength, -2 * pixelLength}, // 2
            { -2 * pixelLength,  -6 * pixelLength,  2 * pixelLength}, // 1
            { -2 * pixelLength,   6 * pixelLength,  2 * pixelLength}, // 4
            { -2 * pixelLength,   6 * pixelLength, -2 * pixelLength}, // 3
        };

        playerLegLeft.tex_coords = {

            // Bottom Face
            {  24.0f / texWidth,  52.0f / texHeight}, // 3
            {  28.0f / texWidth,  52.0f / texHeight}, // 0
            {  28.0f / texWidth,  48.0f / texHeight}, // 1
            {  24.0f / texWidth,  48.0f / texHeight}, // 2

            // Top Face
            {  20.0f / texWidth,  52.0f / texHeight}, // 3
            {  24.0f / texWidth,  52.0f / texHeight}, // 0
            {  24.0f / texWidth,  48.0f / texHeight}, // 1
            {  20.0f / texWidth,  48.0f / texHeight}, // 2

            // Side A // Front face
            {  20.0f / texWidth,  64.0f / texHeight}, // 3
            {  24.0f / texWidth,  64.0f / texHeight}, // 0
            {  24.0f / texWidth,  52.0f / texHeight}, // 1
            {  20.0f / texWidth,  52.0f / texHeight}, // 2
            
            // Back Side A
            {  28.0f / texWidth,  64.0f / texHeight}, // 3
            {  32.0f / texWidth,  64.0f / texHeight}, // 0
            {  32.0f / texWidth,  52.0f / texHeight}, // 1
            {  28.0f / texWidth,  52.0f / texHeight}, // 2

            // Side B // Our Right
            {  24.0f / texWidth,  64.0f / texHeight}, // 3
            {  28.0f / texWidth,  64.0f / texHeight}, // 0
            {  28.0f / texWidth,  52.0f / texHeight}, // 1
            {  24.0f / texWidth,  52.0f / texHeight}, // 2

            // Back Side B // Left
            {  16.0f / texWidth,  64.0f / texHeight}, // 3
            {  20.0f / texWidth,  64.0f / texHeight}, // 0
            {  20.0f / texWidth,  52.0f / texHeight}, // 1
            {  16.0f / texWidth,  52.0f / texHeight}, // 2
        };

        playerLegLeft.indices = basicCubicIndices;
        utility::calcVertNormals(playerLegLeft);
        return static_mesh::init(playerLegLeft);
    }

    static_mesh::mesh_t createPlayerArmRight() {
        static_mesh::mesh_template_t playerArmRight;

        float texWidth = 64.0f;
        float texHeight = 64.0f;
        float pixelLength = 0.0625f;
        playerArmRight.positions = {
                
            // Bottom Face
            {-2 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength, -6 * pixelLength, -2 * pixelLength},

            // Top Face
            {-2 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side A aka front face
            {-2 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            
            // Back Side A
            { 2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side B
            { 2 * pixelLength,  -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  -6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,   6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,   6 * pixelLength,  2 * pixelLength},
            
            // Back Side B
            { -2 * pixelLength,  -6 * pixelLength, -2 * pixelLength}, // 2
            { -2 * pixelLength,  -6 * pixelLength,  2 * pixelLength}, // 1
            { -2 * pixelLength,   6 * pixelLength,  2 * pixelLength}, // 4
            { -2 * pixelLength,   6 * pixelLength, -2 * pixelLength}, // 3
        };

        playerArmRight.tex_coords = {

            // Bottom Face
            {  48.0f / texWidth,  20.0f / texHeight}, // 3
            {  52.0f / texWidth,  20.0f / texHeight}, // 0
            {  52.0f / texWidth,  16.0f / texHeight}, // 1
            {  48.0f / texWidth,  16.0f / texHeight}, // 2

            // Top Face
            {  44.0f / texWidth,  20.0f / texHeight}, // 3
            {  48.0f / texWidth,  20.0f / texHeight}, // 0
            {  48.0f / texWidth,  16.0f / texHeight}, // 1
            {  44.0f / texWidth,  16.0f / texHeight}, // 2

            // Side A // Front face
            {  44.0f / texWidth,  32.0f / texHeight}, // 3
            {  48.0f / texWidth,  32.0f / texHeight}, // 0
            {  48.0f / texWidth,  20.0f / texHeight}, // 1
            {  44.0f / texWidth,  20.0f / texHeight}, // 2
            
            // Back Side A
            {  52.0f / texWidth,  32.0f / texHeight}, // 3
            {  56.0f / texWidth,  32.0f / texHeight}, // 0
            {  56.0f / texWidth,  20.0f / texHeight}, // 1
            {  52.0f / texWidth,  20.0f / texHeight}, // 2

            // Side B // Our Right
            {  48.0f / texWidth,  32.0f / texHeight}, // 3
            {  52.0f / texWidth,  32.0f / texHeight}, // 0
            {  52.0f / texWidth,  20.0f / texHeight}, // 1
            {  48.0f / texWidth,  20.0f / texHeight}, // 2

            // Back Side B // Left
            {  40.0f / texWidth,  32.0f / texHeight}, // 3
            {  44.0f / texWidth,  32.0f / texHeight}, // 0
            {  44.0f / texWidth,  20.0f / texHeight}, // 1
            {  40.0f / texWidth,  20.0f / texHeight}, // 2
        };

        playerArmRight.indices = basicCubicIndices;
        utility::calcVertNormals(playerArmRight);
        return static_mesh::init(playerArmRight);
    }

    static_mesh::mesh_t createPlayerLegRight() {
        static_mesh::mesh_template_t playerLegRight;

        float texWidth = 64.0f;
        float texHeight = 64.0f;
        float pixelLength = 0.0625f;
        playerLegRight.positions = {
                
            // Bottom Face
            {-2 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength, -6 * pixelLength, -2 * pixelLength},

            // Top Face
            {-2 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side A aka front face
            {-2 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 2 * pixelLength, -6 * pixelLength, 2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, 2 * pixelLength},
            
            // Back Side A
            { 2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength, -6 * pixelLength, -2 * pixelLength},
            {-2 * pixelLength,  6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,  6 * pixelLength, -2 * pixelLength},

            // Side B
            { 2 * pixelLength,  -6 * pixelLength,  2 * pixelLength},
            { 2 * pixelLength,  -6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,   6 * pixelLength, -2 * pixelLength},
            { 2 * pixelLength,   6 * pixelLength,  2 * pixelLength},
            
            // Back Side B
            { -2 * pixelLength,  -6 * pixelLength, -2 * pixelLength}, // 2
            { -2 * pixelLength,  -6 * pixelLength,  2 * pixelLength}, // 1
            { -2 * pixelLength,   6 * pixelLength,  2 * pixelLength}, // 4
            { -2 * pixelLength,   6 * pixelLength, -2 * pixelLength}, // 3
        };

        playerLegRight.tex_coords = {

            // Bottom Face
            {  8.0f / texWidth,  20.0f / texHeight}, // 3
            {  12.0f / texWidth,  20.0f / texHeight}, // 0
            {  12.0f / texWidth,  16.0f / texHeight}, // 1
            {  8.0f / texWidth,  16.0f / texHeight}, // 2

            // Top Face
            {  4.0f / texWidth,  20.0f / texHeight}, // 3
            {  8.0f / texWidth,  20.0f / texHeight}, // 0
            {  8.0f / texWidth,  16.0f / texHeight}, // 1
            {  4.0f / texWidth,  16.0f / texHeight}, // 2

            // Side A // Front face
            {  4.0f / texWidth,  32.0f / texHeight}, // 3
            {  8.0f / texWidth,  32.0f / texHeight}, // 0
            {  8.0f / texWidth,  20.0f / texHeight}, // 1
            {  4.0f / texWidth,  20.0f / texHeight}, // 2
            
            // Back Side A
            {  12.0f / texWidth,  32.0f / texHeight}, // 3
            {  16.0f / texWidth,  32.0f / texHeight}, // 0
            {  16.0f / texWidth,  20.0f / texHeight}, // 1
            {  12.0f / texWidth,  20.0f / texHeight}, // 2

            // Side B // Our Right
            {  8.0f / texWidth,  32.0f / texHeight}, // 3
            {  12.0f / texWidth,  32.0f / texHeight}, // 0
            {  12.0f / texWidth,  20.0f / texHeight}, // 1
            {  8.0f / texWidth,  20.0f / texHeight}, // 2

            // Back Side B // Left
            {  0.0f / texWidth,  32.0f / texHeight}, // 3
            {  4.0f / texWidth,  32.0f / texHeight}, // 0
            {  4.0f / texWidth,  20.0f / texHeight}, // 1
            {  0.0f / texWidth,  20.0f / texHeight}, // 2
        };

        playerLegRight.indices = basicCubicIndices;
        utility::calcVertNormals(playerLegRight);
        return static_mesh::init(playerLegRight);
    }
}
