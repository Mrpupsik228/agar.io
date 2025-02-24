#version 410
layout (location = 0) in vec2 texcoord; 
uniform sampler2D BallTexture;

uniform vec4 hue;

out vec4 FragColor;

void main() {
    FragColor = texture2D(BallTexture, texcoord) * hue;
} 