#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>

#include "../io/logger.h"

namespace Brainstorm {
	class ShaderProgram {
	private:
		GLuint id;
		static GLuint boundId;

		std::array<GLuint, 3> shaders;

		const char *vertexLocation, *fragmentLocation, *geometryLocation;
		inline void create();
	public:
		ShaderProgram(const char* vertexLocation, const char* fragmentLocation, const char* geometryLocation);
		~ShaderProgram();
		
		void use() const;
		void destroy() const;
		void reload();
		
		static void drop();

		void setBool(const char* location, bool value) const;
		void setInt(const char* location, int value) const;
		void setFloat(const char* location, float value) const;

		void setVector2(const char* location, const glm::vec2& value) const;
		void setVector2(const char* location, float x, float y) const;

		void setVector2i(const char* location, const glm::ivec2& value) const;
		void setVector2i(const char* location, int x, int y) const;

		void setVector3(const char* location, const glm::vec3& value) const;
		void setVector3(const char* location, float x, float y, float z) const;

		void setVector3i(const char* location, const glm::ivec3& value) const;
		void setVector3i(const char* location, int x, int y, int z) const;

		void setVector4(const char* location, const glm::vec4& value) const;
		void setVector4(const char* location, float x, float y, float z, float w) const;

		void setVector4i(const char* location, const glm::ivec4& value) const;
		void setVector4i(const char* location, int x, int y, int z, int w) const;

		void setMatrix2(const char* location, const glm::mat2& value) const;
		void setMatrix3(const char* location, const glm::mat3& value) const;
		void setMatrix4(const char* location, const glm::mat4& value) const;
	};
}