#pragma once
#include <glm/glm.hpp>

namespace Brainstorm {
	struct MatrixHelper {
		static glm::mat4 perspective(float fov, float aspect, float near, float far);

		static glm::mat4 orthographic(float left, float right, float top, float bottom);
		static glm::mat4 orthographic(float left, float right, float top, float bottom, float near, float far);

		static glm::mat4 view(glm::vec3 position, glm::vec3 rotation);

		static glm::mat4 transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
		static glm::mat4 transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 anchor);
	};
}