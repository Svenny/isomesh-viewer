#version 330 core

in vec3 normal;
in vec3 pos;

uniform vec3 lightDir;
uniform bool useLighting;
uniform bool useNormalColor;
uniform bool useTexture;
uniform float textureScale;

uniform sampler2D ourTexture;

const vec3 ambient = vec3(0.1);

out vec4 fragColor;

void main () {
	vec3 objectColor;
	if (useTexture) {
		vec3 blending = abs( normal );
		blending = normalize(max(blending, 0.00001)); // Force weights to sum to 1.0
		float b = (blending.x + blending.y + blending.z);
		blending /= vec3(b, b, b);

		vec4 xaxis = texture2D( ourTexture, pos.yz * textureScale);
		vec4 yaxis = texture2D( ourTexture, pos.xz * textureScale);
		vec4 zaxis = texture2D( ourTexture, pos.xy * textureScale);
		vec4 tex = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
		objectColor = tex.xyz;
	} else if (useNormalColor) {
		objectColor = abs(normal);
	} else {
		objectColor = vec3(0.64, 0.64, 0.64);
	}

    if (useLighting) {
		vec3 diffuse = vec3(max(dot(normal, lightDir), 0.0));

		fragColor = vec4((ambient + diffuse) * objectColor, 1.0);
    } else {
		fragColor = vec4(objectColor, 1.0);
	}
}
