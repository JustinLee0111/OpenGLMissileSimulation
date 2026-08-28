#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
	unsigned int shaderID = 0;

	std::string readFile(const std::string& filepath);
	Shader() = default;
	Shader(const std::string& vertex_filepath, const std::string& fragment_filepath);
	~Shader();

	unsigned int make_module(const std::string& filepath, unsigned int module_type);
	void use() const;

	void setMat4(const std::string& name, const glm::mat4& matrix) const;
};
