#include "physics.h"

template<glm::length_t L>
static inline float clipAxisOnly(float velocity, const glm::vec<L, float>& a0, const glm::vec<L, float>& a1, const glm::vec<L, float>& b0, const glm::vec<L, float>& b1, glm::vec4::length_type axis) {
	if (velocity > 0.0f && a1[axis] <= b0[axis]) {
		float difference = b0[axis] - a1[axis] - 0.01f;
		if (difference < velocity) {
			return difference;
		}
	}
	if (velocity < 0.0f && a0[axis] >= b1[axis]) {
		float difference = b1[axis] - a0[axis] + 0.01f;
		if (difference > velocity) {
			return difference;
		}
	}

	return velocity;
}

namespace Brainstorm {
	AABB3D::AABB3D(const glm::vec3& scale) {
		this->position = glm::vec3();
		this->scale = scale;
	}
	AABB3D::AABB3D(const glm::vec3& position, const glm::vec3& scale) {
		this->position = position;
		this->scale = scale;
	}

	float AABB3D::clipAxis(float velocity, const AABB3D& other, glm::vec3::length_type axis) const {
		glm::vec3::length_type y = axis + 1 >= 3 ? 0 : axis + 1;
		glm::vec3::length_type z = y + 1 >= 3 ? 0 : y + 1;

		for (glm::vec3::length_type i : { y, z }) {
			if (this->position[i] >= other.position[i] + other.scale[i] || this->position[i] + this->scale[i] <= other.position[i]) {
				return velocity;
			}
		}

		return clipAxisOnly(velocity, this->position, this->position + this->scale, other.position, other.position + other.scale, axis);
	}
	glm::vec3 AABB3D::move(glm::vec3 velocity, const std::vector<AABB3D>& others) {
		for (glm::vec3::length_type axis = 0; axis < 3; axis++) {
			for (const AABB3D& other : others) {
				velocity[axis] = this->clipAxis(velocity[axis], other, axis);
			}

			this->position[axis] += velocity[axis];
		}

		return velocity;
	}

	AABB2D::AABB2D(const glm::vec2& scale) {
		this->position = glm::vec2();
		this->scale = scale;
	}
	AABB2D::AABB2D(const glm::vec2& position, const glm::vec2& scale) {
		this->position = position;
		this->scale = scale;
	}

	float AABB2D::clipAxis(float velocity, const AABB2D& other, glm::vec2::length_type axis) const {
		glm::vec2::length_type y = axis + 1 >= 2 ? 0 : axis + 1;

		if (this->position[y] >= other.position[y] + other.scale[y] || this->position[y] + this->scale[y] <= other.position[y]) {
			return velocity;
		}

		return clipAxisOnly(velocity, this->position, this->position + this->scale, other.position, other.position + other.scale, axis);
	}
	glm::vec2 AABB2D::move(glm::vec2 velocity, const std::vector<AABB2D>& others) {
		for (glm::vec2::length_type axis = 0; axis < 2; axis++) {
			for (const AABB2D& other : others) {
				velocity[axis] = this->clipAxis(velocity[axis], other, axis);
			}

			this->position[axis] += velocity[axis];
		}

		return velocity;
	}
}