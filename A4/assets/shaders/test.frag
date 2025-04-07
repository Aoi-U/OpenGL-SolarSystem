#version 330 core

uniform sampler2D baseColorTexture;
uniform sampler2D overlayColorTexture;

uniform bool noShade = false;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec3 outColor;
in vec2 uvOut;
out vec4 fragColor;

void main()
{	
	vec4 sampledColor = texture(baseColorTexture, uvOut);
	
	// discard transparent fragments
	if (sampledColor.a < 0.1)
	{
		discard;
	}

	// do not shade the fragment if noShade is true (used for clouds and sun)
	if (noShade)
	{
		fragColor = sampledColor;
		return;
	}
	
	// caclulate the ambient light on the fragment
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos); // calculate the direction of the light to the fragment

	// calculate the diffusion of light on the fragment
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	// calculate the specular reflection
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos); // calculate the direction of the camera to the fragment
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
	vec3 specular = specularStrength * spec * lightColor;
	
	// calculate the final color of the fragment
	fragColor = vec4((ambient + diffuse + specular) * sampledColor.rgb, 1.0);
}
