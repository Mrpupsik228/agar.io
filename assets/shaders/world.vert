#version 410
layout (location = 0) in vec2 aPos;
layout (location = 0) out vec2 texcoord; 

uniform float zoom;
uniform float size;
uniform float aspect;
uniform vec2 position;
uniform vec2 cameraPosition;

void main() {
    gl_Position = vec4(((aPos.x  * size + position.x - cameraPosition.x) / aspect) * zoom, (aPos.y  * size + position.y - cameraPosition.y) * zoom, 0, 1);
    texcoord = aPos.xy;
}