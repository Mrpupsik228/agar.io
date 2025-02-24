#pragma once
#include <glad/glad.h>
#include <vector>

#include "../graphics/texture.h"
#include "../io/logger.h"

#include <glm/glm.hpp>

namespace Brainstorm {
	enum class AttachmentType : GLenum {
		DEPTH, COLOR_RGBA, COLOR_RGB, NORMAL
	};

	struct Attachment {
		GLuint texture;

		AttachmentType type;
		GLint filter, clamp;

		Attachment(AttachmentType type, GLint filter, GLint clamp);
	};

	class FrameBuffer {
	private:
		std::vector<Attachment> attachments = {};

		GLuint id, depthId;
		GLsizei width, height;

		inline void createAttachments();
	public:
		FrameBuffer(const std::vector<Attachment>& attachments, GLsizei width, GLsizei height);
		~FrameBuffer();

		void use() const;

		void drop() const;
		void drop(GLsizei previousWidth, GLsizei previousHeight) const;

		void clear() const;
		void resize(GLsizei width, GLsizei height);

		GLint getTexture(size_t attachment) const;

		GLsizei getWidth() const;
		GLsizei getHeight() const;

		glm::vec2 getSize() const;
	};
}