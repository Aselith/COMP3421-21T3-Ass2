#include <glad/glad.h>

#include <iostream>
#include <chicken3421/chicken3421.hpp>

#include <ass2/utility.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace utility {
    float calculateDistance(glm::vec3 posA, glm::vec3 posB) {
        return sqrt( pow(posB.x - posA.x, 2) + pow(posB.y - posA.y, 2) + pow(posB.z - posA.z, 2) );
    }
}
