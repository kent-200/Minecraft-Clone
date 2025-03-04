#version 330 core
in vec2 texCord;
out vec4 finalColor;

uniform sampler2D tex0;

void main() {
    finalColor = texture(tex0, texCord);
}