#version 330 core

uniform mat4 MVP;

layout (location = 0) in vec3 v;
layout (location = 1) in vec3 c;

out vec3 normal;

void main () {
    gl_Position = MVP * vec4 (v, 1.0f);
    normal = c;
}
