#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

namespace Brainstorm {
	GLint Texture::FORMAT_R = GL_R;
	GLint Texture::FORMAT_RG = GL_RG;
	GLint Texture::FORMAT_RGB = GL_RGB;
	GLint Texture::FORMAT_RGBA = GL_RGBA;

	GLint Texture::FILTER_LINEAR = GL_LINEAR;
	GLint Texture::FILTER_NEAREST = GL_NEAREST;

	GLint Texture::CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE;
	GLint Texture::CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER;
	GLint Texture::CLAMP_REPEAT = GL_REPEAT;
	GLint Texture::CLAMP_MIRRORED_REPEAT = GL_MIRRORED_REPEAT;

	bool Texture::initialized = false;
	std::array<GLuint, 32> Texture::boundIds = {};

	GLuint Texture::loadFromFile(const char* location, GLint filter, GLint clamp) {
		int width, height, channels;
		unsigned char* pixels = stbi_load(location, &width, &height, &channels, STBI_rgb_alpha);

		if (pixels == nullptr) {
			Logger::error("Could not load texture file: \"%s\"", location);
			return 0;
		}

		int format;
		switch (channels)
		{
		case 4:
			format = Texture::FORMAT_RGBA;
			break;
		case 3:
			format = Texture::FORMAT_RGB;
			break;
		case 1:
			format = Texture::FORMAT_R;
			break;
		case 2:
			format = Texture::FORMAT_RG;
			break;
		default:
			Logger::error("Format of texture at: \"%s\" is not supported.", location);
			return 0;
		}

		return Texture::create(pixels, width, height, format, filter, clamp);
	}
	GLuint Texture::create(const unsigned char* data, GLsizei width, GLsizei height, GLint format, GLint filter, GLint clamp) {
		GLuint texture;
		
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter - GL_NEAREST + GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		Texture::drop();
		return texture;
	}

	void Texture::use(GLuint texture, GLint index) {
		if (index > 31) {
			Logger::error("Texture binding index out of bounds! 0 (inclusive) - 32 (exclusive).");
			return;
		}
		if (Texture::boundIds[index] == texture) {
			return;
		}

		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	void Texture::drop() {
		Texture::boundIds = {};
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::destroy(GLuint texture) {
		glDeleteTextures(1, &texture);
	}
}