#include "maths.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Brainstorm {
    glm::mat4 MatrixHelper::perspective(float fov, float aspect, float near, float far) {
        if (glm::isnan(aspect)) return glm::mat4();
        return glm::perspective(glm::radians(fov), glm::max(aspect, glm::epsilon<float>() * 2.0f), near, far);
    }
    
    glm::mat4 MatrixHelper::orthographic(float left, float right, float top, float bottom) {
        return glm::ortho(left, right, bottom, top);
    }
    glm::mat4 MatrixHelper::orthographic(float left, float right, float top, float bottom, float near, float far) {
        return glm::ortho(left, right, bottom, top, near, far);
    }

    glm::mat4 MatrixHelper::view(glm::vec3 position, glm::vec3 rotation) {
        glm::mat4 matrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::translate(matrix, -position);

        return matrix;
    }

    glm::mat4 MatrixHelper::transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, scale);

        return matrix;
    }
    glm::mat4 MatrixHelper::transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 anchor) {
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position + anchor);
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, scale);
        matrix = glm::translate(matrix, -anchor);

        return matrix;
    }
}