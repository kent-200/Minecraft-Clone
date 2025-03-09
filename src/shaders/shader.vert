#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCordInput;
layout(location = 2) in float shadowInput;

out vec2 texCord;
out float shadow;

mat4 model = mat4(1.0); // define in vertex
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    texCord = texCordInput;
    shadow = shadowInput;
}
