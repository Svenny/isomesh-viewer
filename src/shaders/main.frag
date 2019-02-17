#version 330 core

in vec4 myColor;

out vec4 fragColor;

void main () {
    fragColor = abs (myColor);
}
