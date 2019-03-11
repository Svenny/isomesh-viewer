#version 330 core

in vec3 normal;

uniform vec3 lightDir;
uniform bool useLighting;
uniform bool useNormalColor;

const vec3 ambient = vec3(0.1);

out vec4 fragColor;

void main () {
	vec3 objectColor;
	if (useNormalColor) {
		objectColor = abs(normal);
	} else {
		objectColor = vec3(0.64, 0.64, 0.64);
	}

    if (useLighting) {
		vec3 diffuse = vec3(max(dot(normal, lightDir), 0.0));
		vec3 result = (ambient + diffuse) * objectColor;
		fragColor = vec4(result, 1.0);
    } else {
		fragColor = vec4(objectColor, 1.0);
	}
}
