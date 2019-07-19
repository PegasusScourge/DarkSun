#pragma once
#ifndef SHADER_H
#define SHADER_H

/**

Borrowed from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h

*/

#include "Log.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace darksun {
	class Shader {
	private:
		// utility function for checking shader compilation/linking errors.
		// ------------------------------------------------------------------------
		void checkCompileErrors(unsigned int shader, std::string type) {
			int success;
			char infoLog[1024];
			if (type != "PROGRAM") {
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);
					//std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
					dout.error("ERROR::SHADER_COMPILATION_ERROR of type: " + type);
					dout.log(infoLog);
					dout.log("-- --------------------------------------------------- --");
				}
			}
			else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);
					//std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
					dout.error("ERROR::PROGRAM_LINKING_ERROR of type: " + type);
					dout.log(infoLog);
					dout.log("-- --------------------------------------------------- --");
				}
			}
		}

		// Shader sources
		const GLchar* basicVertex = R"glsl(
    #version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aNormal;
	layout (location = 2) in vec2 aTexCoords;

	out vec3 FragPos;
	out vec3 Normal;
	out vec2 TexCoords;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		FragPos = vec3(model * vec4(aPos, 1.0));
		Normal = mat3(transpose(inverse(model))) * aNormal;  
		TexCoords = aTexCoords;
		gl_Position = projection * view * vec4(FragPos, 1.0);
	}
)glsl";
		const GLchar* basicFragment = R"glsl(
	#version 330 core
	out vec4 FragColor;

	in vec3 Normal;  
	in vec3 FragPos;  
	in vec2 TexCoords;
  
	uniform vec3 lightPositions[4]; 
	uniform vec3 viewPos; 
	uniform vec3 lightColors[4];
	uniform bool lightAttenuates[4];
	uniform vec3 objectColor;
	uniform bool gamma;

	uniform sampler2D texture_diffuse1;

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
    
		if(attenuate) 
		{
			diffuse *= attenuation;
			specular *= attenuation;
		}
    
		return diffuse + specular;
	}

	void main()
	{
		vec3 color = texture(texture_diffuse1, TexCoords).rgb;
		vec3 lighting = vec3(0.0);
		for(int i = 0; i < 4; ++i)
			lighting += BlinnPhong(normalize(Normal), FragPos, lightPositions[i], lightColors[i], lightAttenuates[i]);
		color *= lighting;
		if(gamma)
			color = pow(color, vec3(1.0/2.2));
		FragColor = vec4(color * objectColor, 1.0);
	} 
)glsl";

	public:
		unsigned int ID;
		// constructor generates the shader on the fly
		// ------------------------------------------------------------------------
		Shader(const char* vertexPath, const char* fragmentPath) {
			// 1. retrieve the vertex/fragment source code from filePath
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			// ensure ifstream objects can throw exceptions:
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				// open files
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;
				// read file's buffer contents into streams
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();
				// close file handlers
				vShaderFile.close();
				fShaderFile.close();
				// convert stream into string
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch (std::ifstream::failure e) {
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}
			const char* vShaderCode = vertexCode.c_str();
			const char * fShaderCode = fragmentCode.c_str();
			// 2. compile shaders
			unsigned int vertex, fragment;
			// vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "VERTEX");
			// fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "FRAGMENT");
			// shader Program
			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			glLinkProgram(ID);
			checkCompileErrors(ID, "PROGRAM");
			// delete the shaders as they're linked into our program now and no longer necessary
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}
		// Default constructor uses in-built shaders.
		Shader() {
			// 1. Shaders are preloaded (basicVertex and basicFragment)
			// 2. compile shaders
			unsigned int vertex, fragment;
			// vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &basicVertex, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "VERTEX");
			// fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &basicFragment, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "FRAGMENT");
			// shader Program
			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			glLinkProgram(ID);
			checkCompileErrors(ID, "PROGRAM");
			// delete the shaders as they're linked into our program now and no longer necessary
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}
		~Shader() {
			glDeleteProgram(ID);
		}
		// activate the shader
		// ------------------------------------------------------------------------
		void use() {
			glUseProgram(ID);
		}
		// utility uniform functions
		// ------------------------------------------------------------------------
		void setBool(const std::string &name, bool value) const
		{
			glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
		}
		// ------------------------------------------------------------------------
		void setInt(const std::string &name, int value) const
		{
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		}
		// ------------------------------------------------------------------------
		void setFloat(const std::string &name, float value) const
		{
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		}
		// ------------------------------------------------------------------------
		void setVec2(const std::string &name, const glm::vec2 &value) const
		{
			glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec2(const std::string &name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
		}
		// ------------------------------------------------------------------------
		void setVec3(const std::string &name, const glm::vec3 &value) const
		{
			glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec3(const std::string &name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
		}
		// ------------------------------------------------------------------------
		void setVec4(const std::string &name, const glm::vec4 &value) const
		{
			glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec4(const std::string &name, float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
		}
		// ------------------------------------------------------------------------
		void setMat2(const std::string &name, const glm::mat2 &mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		// ------------------------------------------------------------------------
		void setMat3(const std::string &name, const glm::mat3 &mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		// ------------------------------------------------------------------------
		void setMat4(const std::string &name, const glm::mat4 &mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
	};
}
#endif