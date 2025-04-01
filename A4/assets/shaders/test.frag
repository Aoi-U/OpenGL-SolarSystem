#version 330 core

uniform sampler2D baseColorTexture;
uniform sampler2D overlayColorTexture;

in vec3 outColor;
in vec2 uvOut;
out vec4 fragColor;

void main()
{
	vec4 sampledColor = texture(baseColorTexture, uvOut);
	if(sampledColor.a < 1e-4)
	{
		discard;// If the texture is transparent, don't draw the fragment
	}
	//fragColor = vec4(outColor, 1.0f);
	fragColor = sampledColor;
}
