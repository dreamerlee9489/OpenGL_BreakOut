#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"

// 保存与使用 FreeType 加载的字符相关的所有状态信息
struct Character {
	unsigned int TextureID; // 字形纹理的 ID 句柄
	glm::ivec2 Size;        // 字形大小
	glm::ivec2 Bearing;     // 从基线到字形左侧/顶部的偏移量
	unsigned int Advance;   // 水平偏移前进到下一个字形
};

/// <summary>
/// 一个渲染器类，用于渲染由使用 FreeType 库加载的字体显示的文本。 
/// 加载单个字体，处理成字符项列表以供以后渲染。
/// </summary>
class TextRenderer {
public:
	// 保存预编译字符列表
	std::map<char, Character> Characters;
	// 用于文本渲染的着色器
	Shader TextShader;

	TextRenderer(unsigned int width, unsigned int height);

	// 预编译给定字体的字符列表
	void Load(std::string font, unsigned int fontSize);

	// 使用预编译的字符列表呈现文本字符串
	void RenderText(std::string text, float x, float y, float scale,
		glm::vec3 color = glm::vec3(1.0f));

private:
	// 渲染状态
	unsigned int VAO, VBO;
};

#endif