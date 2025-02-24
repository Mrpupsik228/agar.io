#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace Brainstorm {
	class AABB3D {
	private:
		float clipAxis(float velocity, const AABB3D& other, glm::vec3::length_type axis) const;
	public:
		class CollisionInfo {
			std::vector<AABB3D> colliders;
			bool onGround;
		};

		glm::vec3 position, scale;

		AABB3D(const glm::vec3& position, const glm::vec3& scale);
		AABB3D(const glm::vec3& scale);

		glm::vec3 move(glm::vec3 velocity, const std::vector<AABB3D>& others);
	};
	class AABB2D {
	private:
		float clipAxis(float velocity, const AABB2D& other, glm::vec2::length_type axis) const;
	public:
		glm::vec2 position, scale;

		AABB2D(const glm::vec2& position, const glm::vec2& scale);
		AABB2D(const glm::vec2& scale);

		glm::vec2 move(glm::vec2 velocity, const std::vector<AABB2D>& others);
	};

	struct VelocityHelper {
		template<glm::length_t L>
		static glm::vec<L, float> applyExplosion(const glm::vec<L, float>& velocity, const glm::vec<L, float>& position, const glm::vec<L, float>& explosionPosition, float explosionRadius, float explosionPower);
		
		template<glm::length_t L>
		static glm::vec<L, float> accelerate(const glm::vec<L, float>& velocity, const glm::vec<L, float>& wishVelocity, float acceleration);
	};

	template<glm::length_t L>
	glm::vec<L, float> VelocityHelper::applyExplosion(const glm::vec<L, float>& velocity, const glm::vec<L, float>& position, const glm::vec<L, float>& explosionPosition, float explosionRadius, float explosionPower) {
		glm::vec3 difference = position - explosionPosition;
		float length = glm::length(difference);

		difference = difference / length * (explosionRadius - glm::min(length, explosionRadius));
		return (velocity + difference * explosionPower);
	}

	template<glm::length_t L>
	glm::vec<L, float> VelocityHelper::accelerate(const glm::vec<L, float>& velocity, const glm::vec<L, float>& wishVelocity, float acceleration) {
		return glm::mix(velocity, wishVelocity, acceleration);
	}
}