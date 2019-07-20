#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform vec3 lightPositions[4]; 
uniform vec3 lightColors[4];
uniform bool lightAttenuates[4];
uniform vec3 objectColor;
uniform bool gamma;

uniform sampler2D texture_diffuse1;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPosU;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
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
	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * lightColor;    
	// simple attenuation
	float max_distance = 1.5;
	float distance = length(lightPos - fragPos);
	float attenuation = 1.0 / (gamma ? distance * distance : distance);
	// if(distance > max_distance)
	// {
	//	attenuation = 0.0;
	// }

	if(attenuate) 
	{
		diffuse *= attenuation;
		specular *= attenuation;
	}
	
	return diffuse + specular;
}

void main()
{
	vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
	vec3 lighting = vec3(0.0);
	for(int i = 0; i < 4; ++i)
	{
		vec3 bph = BlinnPhong(normalize(fs_in.Normal), fs_in.FragPos, lightPositions[i], lightColors[i], lightAttenuates[i]);
		if( i == 1 )
		{
			float shadow = ShadowCalculation(fs_in.FragPos);
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