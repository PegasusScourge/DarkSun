#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 lightPositions[4]; 
uniform vec3 viewPos; 
uniform vec3 lightColors[4];
uniform bool lightAttenuates[4];
uniform vec3 objectColor;
uniform bool gamma;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	// projCoords *= 2.0;
    
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
	// check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : -0.01;

    return shadow;
}

vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor, bool attenuate)
{
	// diffuse
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;
	// specular
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
	vec3 specular = spec * lightColor;    
	// simple attenuation
	float max_distance = 1.5;
	float distance = length(lightPos - fragPos);
	float attenuation = 1.0;

	if(attenuate) 
	{
		attenuation = 1.0 / (gamma ? distance * distance : distance);
	}
	else
	{
		attenuation = 1.0 / (distance * 0.01);
	}
	diffuse *= attenuation;
	specular *= attenuation;
	
	return diffuse + specular;
}

void main()
{
	vec3 color = texture(texture_diffuse1, TexCoords).rgb;
	vec3 lighting = vec3(0.0);
	for(int i = 0; i < 4; ++i)
	{
		vec3 bph = BlinnPhong(normalize(Normal), FragPos, lightPositions[i], lightColors[i], lightAttenuates[i]);
		if( i == 1)
		{
			float shadow = ShadowCalculation(FragPosLightSpace);
			lighting += bph * (1.0 - shadow);
		}
		else
		{
			lighting += bph;
		}
	}
	color *= lighting;
	if(gamma)
		color = pow(color, vec3(1.0/2.2));
	FragColor = vec4(color * objectColor, 1.0);
} 