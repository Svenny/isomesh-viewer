#version 330 core

uniform mat4 MVP;

layout (location = 0) in vec3 v;
layout (location = 1) in vec3 c;

out vec4 myColor;

void main () {
    gl_Position = MVP * vec4 (v, 1.0f);
    myColor = vec4 (c, 1.0f);
}
