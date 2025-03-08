#version 330 core
in vec2 texCord;
in float shadow;

out vec4 finalColor;

uniform sampler2D tex0;

void main() {
    vec4 textureColour = texture(tex0, texCord);
    finalColor = textureColour * shadow;
}