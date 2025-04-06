#version 330 core

uniform sampler2D baseColorTexture;
uniform sampler2D overlayColorTexture;

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
	
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * lightColor;


	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * sampledColor.rgb;
	fragColor = vec4(result, 1.0f);
}
