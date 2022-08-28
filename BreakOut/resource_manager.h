#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "shader.h"
#include "texture.h"

/**
 * 一个静态的Singleton ResourceManager类，承载了几个加载纹理和着色器的函数。
 * 每个加载的纹理和着色器也被存储起来，以便将来通过字符串句柄进行引用。
 * 所有函数和资源都是静态的，没有定义公共构造函数。
 */
class ResourceManager {
public:
	// resource storage
	static std::map <std::string, Shader> Shaders;
	static std::map <std::string, Texture2D> Textures;

	// 从文件中加载（并生成）一个着色器程序，加载顶点、片段、几何着色器的源代码
	static Shader LoadShader(const char* vShaderFile, const char* fShaderFile,
		const char* gShaderFile, std::string name);

	// 检索出一个已存储的Shader
	static Shader GetShader(std::string name);

	// loads (and generates) a texture from file
	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);

	// retrieves a stored texture
	static Texture2D& GetTexture(std::string name);

	// properly de-allocates all loaded resources
	static void Clear();

private:
	// private constructor, that is we do not want any actual resource manager
	// objects. Its members and functions should be publicly available (static).
	ResourceManager() {}

	// loads and generates a shader from file
	static Shader loadShaderFromFile(const char* vShaderFile,
		const char* fShaderFile,
		const char* gShaderFile = nullptr);

	// loads a single texture from file
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif