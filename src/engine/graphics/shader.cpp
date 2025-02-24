#include "shader.h"

#include <filesystem>
#include <vector>

#define LOG_PROGRAM_ERROR() const size_t errorLength = 500;\
char error[errorLength];\
glGetProgramInfoLog(this->id, errorLength, nullptr, error);\
Logger::error("Could not compile ShaderProgram. Error: %s\n", error)

#define USE_PROGRAM() if (ShaderProgram::boundId != this->id) {\
	glUseProgram(this->id);\
	ShaderProgram::boundId = this->id;\
}

namespace Brainstorm {
	inline static void logProgramError(GLuint programId) {
		const size_t ErrorLength = 500;
		char error[ErrorLength];
		
		glGetProgramInfoLog(programId, ErrorLength, nullptr, error);
		Logger::error("Could not compile ShaderProgram. Error: %s\n", error);
	}

	inline static bool preprocessShader(const char* location, std::string& code, std::vector<std::filesystem::path>& includePathes, uint8_t includeDepth) {
		const uint8_t MaxIncludeDepth = 32;
		if (includeDepth >= MaxIncludeDepth) {
			Logger::error("Could not load shader file: %s. Reason: Exceeded maximum include depth - 32.", location);
			return false;
		}
		
		std::ifstream file(location);

		if (!file.is_open()) {
			Logger::error("Could not open shader file: %s", location);
			return false;
		}

		std::string line;
		std::filesystem::path baseDirectory = std::filesystem::path(location).parent_path();

		while (std::getline(file, line)) {
			std::string trimmedLine = line;
			void(std::remove_if(trimmedLine.begin(), trimmedLine.end(), isspace));

			const size_t NumSearchChars = 9; // Num chars in "#include" string
			if (trimmedLine.rfind("#include", 0) != 0) {
				code += line + '\n';
				continue;
			}
			if (NumSearchChars >= trimmedLine.size()) {
				Logger::error("Could not load shader file: %s.\n%s\n^^^^^^^^^ - No path provided for #include preprocessor.", location, line.c_str());
				file.close();

				return false;
			}

			size_t start = line.find('"');
			size_t end = line.find('"', start + 1);

			if (start == std::string::npos || end == std::string::npos || end <= start) {
				Logger::error("Could not load shader file: %s.\n%s\n        ^ - No path provided for #include preprocessor.", location, line.c_str());
				return false;
			}

			std::string include;
			std::filesystem::path includePath = (baseDirectory / line.substr(start + 1, end - start - 1)).lexically_normal();

			bool existingIncludeFound = false;
			for (const std::filesystem::path& path : includePathes) {
				if (includePath == path) {
					existingIncludeFound = true;
					break;
				}
			}

			if (existingIncludeFound) {
				continue;
			}
			if (!preprocessShader(includePath.c_str(), include, includePathes, includeDepth + 1)) {
				Logger::error("Could not include: #include \"%s\". From shader: %s.", includePath.c_str(), location);
				file.close();

				return false;
			}

			code += include + '\n';
			includePathes.push_back(includePath);
		}

		file.close();
		return true;
	}
	inline static GLuint setShader(GLuint programId, const char* location, unsigned int type) {
		std::string source;
		std::vector<std::filesystem::path> includePathes;
		
		if (!preprocessShader(location, source, includePathes, 0)) {
			return 0;
		}

		GLuint shaderId = 0;
		const char* code = source.c_str();

		shaderId = glCreateShader(type);
		glShaderSource(shaderId, 1, &code, nullptr);
		glCompileShader(shaderId);

		GLint success;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

		if (!success) {
			std::string name = "";
			switch (type) {
			case GL_VERTEX_SHADER: {
				name = "[VERTEX SHADER]";
				break;
			}
			case GL_FRAGMENT_SHADER: {
				name = "[FRAGMENT SHADER]";
				break;
			}
			case GL_GEOMETRY_SHADER: {
				name = "[GEOMETRY SHADER]";
				break;
			}
			}

			const size_t ErrorLength = 500;
			
			char error[ErrorLength];
			glGetShaderInfoLog(shaderId, ErrorLength, nullptr, error);

			Logger::error("%s Could not compile! Error:\n%s\n", name.c_str(), error);
		}

		glAttachShader(programId, shaderId);
		return shaderId;
	}

	GLuint ShaderProgram::boundId = 0;

	void ShaderProgram::create() {
		this->id = glCreateProgram();
		this->shaders = std::array<unsigned int, 3>();

		if (this->vertexLocation != nullptr) {
			this->shaders[0] = setShader(this->id, vertexLocation, GL_VERTEX_SHADER);
		}
		if (this->fragmentLocation != nullptr) {
			this->shaders[1] = setShader(this->id, fragmentLocation, GL_FRAGMENT_SHADER);
		}
		if (this->geometryLocation != nullptr) {
			this->shaders[2] = setShader(this->id, geometryLocation, GL_GEOMETRY_SHADER);
		}

		glLinkProgram(this->id);

		GLint success;
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);

		if (!success) {
			logProgramError(this->id);
			return;
		}

		glValidateProgram(this->id);
		glGetProgramiv(this->id, GL_VALIDATE_STATUS, &success);

		if (!success) {
			logProgramError(this->id);
		}
	}

	ShaderProgram::ShaderProgram(const char* vertexLocation, const char* fragmentLocation, const char* geometryLocation)
			: vertexLocation(vertexLocation), fragmentLocation(fragmentLocation), geometryLocation(geometryLocation) {
		this->create();
	}
	ShaderProgram::~ShaderProgram() {
		this->destroy();
	}

	void ShaderProgram::use() const {
		USE_PROGRAM();
	}
	void ShaderProgram::drop() {
		ShaderProgram::boundId = 0;
		glUseProgram(0);
	}
	void ShaderProgram::reload() {
		this->destroy();
		this->create();
	}

	void ShaderProgram::destroy() const {
		glDetachShader(this->id, this->shaders[0]);
		glDetachShader(this->id, this->shaders[1]);
		glDetachShader(this->id, this->shaders[2]);

		glDeleteProgram(this->id);
	}

	void ShaderProgram::setBool(const char* location, bool value) const {
		USE_PROGRAM();
		glUniform1i(glGetUniformLocation(this->id, location), value);
	}
	void ShaderProgram::setInt(const char* location, int value) const {
		USE_PROGRAM();
		glUniform1i(glGetUniformLocation(this->id, location), value);
	}
	void ShaderProgram::setFloat(const char* location, float value) const {
		USE_PROGRAM();
		glUniform1f(glGetUniformLocation(this->id, location), value);
	}
	
	void ShaderProgram::setVector2(const char* location, const glm::vec2& value) const {
		USE_PROGRAM();
		glUniform2f(glGetUniformLocation(this->id, location), value.x, value.y);
	}
	void ShaderProgram::setVector2(const char* location, float x, float y) const {
		USE_PROGRAM();
		glUniform2f(glGetUniformLocation(this->id, location), x, y);
	}
	void ShaderProgram::setVector2i(const char* location, const glm::ivec2& value) const {
		USE_PROGRAM();
		glUniform2i(glGetUniformLocation(this->id, location), value.x, value.y);
	}
	void ShaderProgram::setVector2i(const char* location, int x, int y) const {
		USE_PROGRAM();
		glUniform2i(glGetUniformLocation(this->id, location), x, y);
	}

	void ShaderProgram::setVector3(const char* location, const glm::vec3& value) const {
		USE_PROGRAM();
		glUniform3f(glGetUniformLocation(this->id, location), value.x, value.y, value.z);
	}
	void ShaderProgram::setVector3(const char* location, float x, float y, float z) const {
		USE_PROGRAM();
		glUniform3f(glGetUniformLocation(this->id, location), x, y, z);
	}
	void ShaderProgram::setVector3i(const char* location, const glm::ivec3& value) const {
		USE_PROGRAM();
		glUniform3i(glGetUniformLocation(this->id, location), value.x, value.y, value.z);
	}
	void ShaderProgram::setVector3i(const char* location, int x, int y, int z) const {
		USE_PROGRAM();
		glUniform3i(glGetUniformLocation(this->id, location), x, y, z);
	}

	void ShaderProgram::setVector4(const char* location, const glm::vec4& value) const {
		USE_PROGRAM();
		glUniform4f(glGetUniformLocation(this->id, location), value.x, value.y, value.z, value.w);
	}
	void ShaderProgram::setVector4(const char* location, float x, float y, float z, float w) const {
		USE_PROGRAM();
		glUniform4f(glGetUniformLocation(this->id, location), x, y, z, w);
	}
	void ShaderProgram::setVector4i(const char* location, const glm::ivec4& value) const {
		USE_PROGRAM();
		glUniform4i(glGetUniformLocation(this->id, location), value.x, value.y, value.z, value.w);
	}
	void ShaderProgram::setVector4i(const char* location, int x, int y, int z, int w) const {
		USE_PROGRAM();
		glUniform4i(glGetUniformLocation(this->id, location), x, y, z, w);
	}

	void ShaderProgram::setMatrix2(const char* location, const glm::mat2& value) const {
		USE_PROGRAM();
		glUniformMatrix2fv(glGetUniformLocation(this->id, location), 1, false, &value[0][0]);
	}
	void ShaderProgram::setMatrix3(const char* location, const glm::mat3& value) const {
		USE_PROGRAM();
		glUniformMatrix3fv(glGetUniformLocation(this->id, location), 1, false, &value[0][0]);
	}
	void ShaderProgram::setMatrix4(const char* location, const glm::mat4& value) const {
		USE_PROGRAM();
		glUniformMatrix4fv(glGetUniformLocation(this->id, location), 1, false, &value[0][0]);
	}
}