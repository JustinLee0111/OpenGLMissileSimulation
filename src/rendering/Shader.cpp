#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "rendering/Shader.h"

using namespace std;

std::string Shader::readFile(const std::string& filepath) {
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		file.open(filepath);
		std::stringstream stream;
		stream << file.rdbuf();
		file.close();
		return stream.str();
	}
	catch (const std::ifstream::failure& e) {
		std::cerr << "Shader File Read Fail: " << filepath
			<< " (Message: " << e.what() << ")" << std::endl;
		return "";
	}
}

Shader::Shader(const string& vertex_filepath, const string& fragment_filepath) {
	vector<unsigned int> modules;
	modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));
	modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));

	if (modules[0] == 0 || modules[1] == 0) {
		std::cerr << "Shader Compilation Failed\n";
		for (unsigned int shaderModule : modules) {
			if (shaderModule != 0) glDeleteShader(shaderModule);
		}
		return;
	}

	shaderID = glCreateProgram();
	for (unsigned int shaderModule : modules) {
		glAttachShader(shaderID, shaderModule);
	}
	glLinkProgram(shaderID);

	int success;
	glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[1024];
		glGetProgramInfoLog(shaderID, sizeof(infoLog), nullptr, infoLog);
		cerr << "Shader Linking Failed: " << infoLog << endl;
		glDeleteProgram(shaderID);
		shaderID = 0;
		return;
	}

	for (unsigned int shaderModule : modules) {
		glDetachShader(shaderID, shaderModule);
		glDeleteShader(shaderModule);
	}
}
	
unsigned int Shader::make_module(const string& filepath, unsigned int module_type) {
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

void Shader::use() const {
	glUseProgram(shaderID);
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const {
	int loc = glGetUniformLocation(shaderID, name.c_str());
	if (loc == -1) {
		std::cout << "ERROR SETTING MATRIX: " << name << std::endl;
		return;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const std::string& name, const glm::vec3& vector) const {
	int loc = glGetUniformLocation(shaderID, name.c_str());
	if (loc == -1) {
		std::cout << "ERROR SETTING VECTOR: " << name << std::endl;
		return;
	}
	glUniform3fv(loc, 1, glm::value_ptr(vector));
}

void Shader::setVec4(const std::string& name, const glm::vec4& vector) const {
	int loc = glGetUniformLocation(shaderID, name.c_str());
	if (loc == -1) {
		std::cout << "ERROR SETTING VECTOR: " << name << std::endl;
		return;
	}
	glUniform4fv(loc, 1, glm::value_ptr(vector));
}

void Shader::setFloat(const std::string& name, const float& value) const {
	int loc = glGetUniformLocation(shaderID, name.c_str());
	if (loc == -1) {
		std::cout << "ERROR SETTING FLOAT: " << name << std::endl;
		return;
	}
	glUniform1fv(loc, 1, &value);
}

void Shader::setBool(const std::string& name, const bool& value) const {
	int loc = glGetUniformLocation(shaderID, name.c_str());
	if (loc == -1) {
		std::cout << "ERROR SETTING FLOAT: " << name << std::endl;
		return;
	}
	glUniform1i(loc, static_cast<int>(value));
}

Shader::~Shader() {
	if (shaderID != 0) {
		glDeleteProgram(shaderID);
	}
}