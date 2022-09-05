#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/// <summary>
/// 通用着色器对象。 从文件编译，生成编译/链接时错误消息并托管多个实用程序功能，以便于管理。
/// </summary>
class Shader {
public:
	// 状态
	unsigned int ID;

	Shader() {}

	// 将当前着色器设置为活动状态
	Shader& Use();

	// 根据给定的源代码编译着色器
	void Compile(const char* vertexSource, const char* fragmentSource,
		const char* geometrySource = nullptr);

	// 工具函数
	void SetFloat(const char* name, float value, bool useShader = false);

	void SetInteger(const char* name, int value, bool useShader = false);

	void SetVector2f(const char* name, float x, float y, bool useShader = false);

	void SetVector2f(const char* name, const glm::vec2& value,
		bool useShader = false);

	void SetVector3f(const char* name, float x, float y, float z,
		bool useShader = false);

	void SetVector3f(const char* name, const glm::vec3& value,
		bool useShader = false);

	void SetVector4f(const char* name, float x, float y, float z, float w,
		bool useShader = false);

	void SetVector4f(const char* name, const glm::vec4& value,
		bool useShader = false);

	void SetMatrix4(const char* name, const glm::mat4& matrix,
		bool useShader = false);

private:
	// 检查编译或链接是否失败，如果是，则打印错误日志
	void checkCompileErrors(unsigned int object, std::string type);
};

#endif