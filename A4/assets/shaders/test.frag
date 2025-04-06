#version 330 core

uniform sampler2D baseColorTexture;
uniform sampler2D overlayColorTexture;

in vec3 outColor;
in vec2 uvOut;
out vec4 fragColor;

void main()
{
	vec4 sampledColor = texture(baseColorTexture, uvOut);

	//fragColor = vec4(outColor, 1.0f);
	fragColor = sampledColor;
}
