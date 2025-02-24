#include "mesh.h"
#include <ctype.h>

namespace Brainstorm {
	const GLint Mesh::TRIANGLES = GL_TRIANGLES;
	const GLint Mesh::TRIANGLE_FAN = GL_TRIANGLE_FAN;
	const GLint Mesh::TRIANGLE_STRIP = GL_TRIANGLE_STRIP;

	const GLint Mesh::LINES = GL_LINES;
	const GLint Mesh::LINE_STRIP = GL_LINE_STRIP;
	const GLint Mesh::LINE_LOOP = GL_LINE_LOOP;

	const GLint Mesh::POINTS = GL_POINTS;

	GLuint Mesh::boundId = 0;

	VertexBuffer::VertexBuffer(const std::vector<float>& data, int dimensions) {
		this->data = data;
		this->dimensions = dimensions;
	}

	inline static GLuint createVertexBuffer(const VertexBuffer& vertices, GLuint index) {
		GLuint id;
		glGenBuffers(1, &id);
		glBindBuffer(GL_ARRAY_BUFFER, id);

		glBufferData(GL_ARRAY_BUFFER, vertices.data.size() * sizeof(float), vertices.data.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, vertices.dimensions, GL_FLOAT, false, 0, nullptr);

		return id;
	}

	Mesh::Mesh(const VertexBuffer& vertices, const std::vector<VertexBuffer>& additional, GLint renderMode) {
		this->id = 0;
		this->buffers = std::vector<GLuint>(additional.size() + 1);

		this->vertexCount = static_cast<GLsizei>(vertices.data.size() / vertices.dimensions);
		this->renderMode = renderMode;

		glGenVertexArrays(1, &this->id);
		glBindVertexArray(this->id);

		this->buffers[0] = createVertexBuffer(vertices, 0);
		for (size_t i = 1; i < this->buffers.size(); i++) {
			this->buffers[i] = createVertexBuffer(additional[i - 1], static_cast<GLuint>(i));
		}

		glBindVertexArray(0);
	}
	Mesh::~Mesh() {
		this->destroy();
	}
	
	void Mesh::render() const {
		if (Mesh::boundId != this->id) {
			glBindVertexArray(this->id);
			Mesh::boundId = this->id;
		}

		glDrawArrays(this->renderMode, 0, this->vertexCount);
	}
	void Mesh::destroy() {
		glDeleteVertexArrays(1, &this->id);
		for (GLuint buffer : this->buffers) {
			glDeleteBuffers(1, &buffer);
		}
	}
	
	void Mesh::drop() {
		Mesh::boundId = 0;
		glBindVertexArray(0);
	}
}