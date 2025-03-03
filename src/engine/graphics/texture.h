#pragma once
#include <glad/glad.h>
#include <stb_image.h>
#include <array>
#include "../io/logger.h"

namespace Brainstorm {
	class Texture {
	private:
		static bool initialized;
		static std::array<GLuint, 32> boundIds;
	public:
		static GLint FORMAT_RGBA;
		static GLint FORMAT_RGB;
		static GLint FORMAT_RG;
		static GLint FORMAT_R;

		static GLint FILTER_LINEAR;
		static GLint FILTER_NEAREST;

		static GLint CLAMP_TO_EDGE;
		static GLint CLAMP_TO_BORDER;
		static GLint CLAMP_REPEAT;
		static GLint CLAMP_MIRRORED_REPEAT;

		static GLuint loadFromFile(const char* location, GLint filter = Texture::FILTER_LINEAR, GLint clamp = Texture::CLAMP_REPEAT);
		static GLuint create(const unsigned char* data, GLsizei width, GLsizei height, GLint format, GLint filter = Texture::FILTER_LINEAR, GLint clamp = Texture::CLAMP_REPEAT);
		
		static void use(GLuint texture, GLint index = 0);
		static void destroy(GLuint texture);
		static void drop();
	};
}