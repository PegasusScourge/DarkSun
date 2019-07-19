#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} gs_in[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

void main() {    
    gl_Position = gl_in[0].gl_Position; 
	FragPos = gs_in[0].FragPos;
	Normal = gs_in[0].Normal;
	TexCoords = gs_in[0].TexCoords;
	FragPosLightSpace = gs_in[0].FragPosLightSpace;
    EmitVertex();
	
	gl_Position = gl_in[1].gl_Position; 
	FragPos = gs_in[1].FragPos;
	Normal = gs_in[1].Normal;
	TexCoords = gs_in[1].TexCoords;
	FragPosLightSpace = gs_in[1].FragPosLightSpace;
    EmitVertex();
	
	gl_Position = gl_in[2].gl_Position; 
	FragPos = gs_in[2].FragPos;
	Normal = gs_in[2].Normal;
	TexCoords = gs_in[2].TexCoords;
	FragPosLightSpace = gs_in[2].FragPosLightSpace;
    EmitVertex();
	
    EndPrimitive();
}  