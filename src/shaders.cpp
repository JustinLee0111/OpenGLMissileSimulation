#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

#include "shaders.h"

using namespace std;

unsigned int shaders::make_shader(const string& vertex_filepath, const string& fragment_filepath) {
	vector<unsigned int> modules;
	modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));
	modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));

	if (modules[0] == 0 || modules[1] == 0) {
		for (unsigned int shaderModule : modules) {
			if (shaderModule != 0) glDeleteShader(shaderModule);
		}
		return 0;
	}

	unsigned int shader = glCreateProgram();
	for (unsigned int shaderModule : modules) {
		glAttachShader(shader, shaderModule);
	}
	glLinkProgram(shader);

	int success;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
		cerr << "Shader Linking failed: " << infoLog << endl;
		glDeleteProgram(shader);
		return 0;
	}

	for (unsigned int shaderModule : modules) {
		glDetachShader(shader, shaderModule);
		glDeleteShader(shaderModule);
	}

	return shader;
}
	
unsigned int shaders::make_module(const string& filepath, unsigned int module_type) {
	ifstream file;
	stringstream bufferedLines;
	string line;
	file.open(filepath);

	if (!file.is_open()) {
		cerr << "ERROR: Shader file NOT found at path: " << filepath << endl;
		return 0;
	}

	bufferedLines << file.rdbuf();
	string shaderSource = bufferedLines.str();
	const char* shaderSrc = shaderSource.c_str();
	bufferedLines.str("");
	file.close();

	unsigned int shaderModule = glCreateShader(module_type);
	glShaderSource(shaderModule, 1, &shaderSrc, nullptr);
	glCompileShader(shaderModule);

	int success;
	glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetShaderInfoLog(shaderModule, sizeof(infoLog), nullptr, infoLog);
		cerr << "Shader compilation failed: " << infoLog << endl;
		glDeleteShader(shaderModule);
		return 0;
	}
	return shaderModule;
}