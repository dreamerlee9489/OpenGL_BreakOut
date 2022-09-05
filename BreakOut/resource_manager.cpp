#include "resource_manager.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "stb_image.h"

// 实例化静态变量
std::map <std::string, Texture2D> ResourceManager::Textures;
std::map <std::string, Shader> ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const char* vShaderFile,
	const char* fShaderFile,
	const char* gShaderFile, std::string name) {
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name) { return Shaders[name]; }

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha,
	std::string name) {
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D& ResourceManager::GetTexture(std::string name) {
	return Textures[name];
}

void ResourceManager::Clear() {
	// （正确）删除所有着色器
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	// （正确）删除所有纹理
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile,
	const char* fShaderFile,
	const char* gShaderFile) {
	// 1.从filePath中检索顶点/片段源代码
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try {
		// 打开文件
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// 将文件的缓冲区内容读入流
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// 关闭文件处理程序
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// 将流转换为字符串
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// 如果存在几何着色器路径，还加载几何着色器
		if (gShaderFile != nullptr) {
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e) {
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	// 2. 现在从源代码创建着色器对象
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode,
		gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha) {
	// 创建纹理对象
	Texture2D texture;
	if (alpha) {
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}
	// 加载图像
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
	// 现在生成纹理
	texture.Generate(width, height, data);
	// 最后是免费的图像数据
	stbi_image_free(data);
	return texture;
}