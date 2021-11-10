#include <chicken3421/chicken3421.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <ass2/frustum.hpp>

namespace frustum {

	bool isBlockInView(glm::vec3 lookingDir, glm::vec3 blockPoint, glm::vec3 playerPos) {
		auto objectToPlayer = blockPoint - playerPos;
		objectToPlayer = glm::normalize(objectToPlayer);
		
		float CosTheta = glm::dot(objectToPlayer, lookingDir);
		if(glm::degrees(acosf(CosTheta)) > VIEW_DEGREE) {
			return false;
		} else {
			return true;
		}
	}

}