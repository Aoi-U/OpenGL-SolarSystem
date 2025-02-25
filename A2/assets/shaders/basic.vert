#version 330 core
layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec2 uvIn;

uniform mat4 transformation;

out vec2 uvOut;

void main() {
	uvOut = uvIn;
	gl_Position = transformation * vec4(positionIn, 1.0);
}
