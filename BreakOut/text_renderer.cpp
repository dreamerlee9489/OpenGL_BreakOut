#include <iostream>

#include <ft2build.h>
#include <glm/gtc/matrix_transform.hpp>

#include FT_FREETYPE_H

#include "resource_manager.h"
#include "text_renderer.h"

TextRenderer::TextRenderer(unsigned int width, unsigned int height) {
	// 加载和配置着色器
	this->TextShader = ResourceManager::LoadShader("text_2d.vert", "text_2d.frag",
		nullptr, "text");
	this->TextShader.SetMatrix4("projection",
		glm::ortho(0.0f, static_cast<float>(width),
			static_cast<float>(height), 0.0f),
		true);
	this->TextShader.SetInteger("text", 0);
	// 为纹理四边形配置 VAO/VBO
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, unsigned int fontSize) {
	// 先清除之前加载的Characters
	this->Characters.clear();
	// 然后初始化并加载 FreeType 库
	FT_Library ft;
	// 每当发生错误时，所有函数都会返回一个非 0 的值
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, font.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	// 将字体加载为面
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	// 禁用字节对齐限制
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// 然后对于前 128 个 ASCII 字符，预加载/编译它们的字符并存储它们
	for (GLubyte c = 0; c < 128; c++)
	{
		// 加载字符字形
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// 生成纹理
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
			face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);
		// 设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 现在存储字符以备后用
		Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				(unsigned int)face->glyph->advance.x };
		Characters.insert(std::pair<char, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// 完成后销毁 FreeType
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale,
	glm::vec3 color) {
	// 激活对应的渲染状态
	this->TextShader.Use();
	this->TextShader.SetVector3f("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(this->VAO);

	// 遍历所有字符
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// 为每个字符更新VBO
		float vertices[6][4] = {
				{xpos,     ypos + h, 0.0f, 1.0f},
				{xpos + w, ypos,     1.0f, 0.0f},
				{xpos,     ypos,     0.0f, 0.0f},

				{xpos,     ypos + h, 0.0f, 1.0f},
				{xpos + w, ypos + h, 1.0f, 1.0f},
				{xpos + w, ypos,     1.0f, 0.0f}
		};
		// 在四边形上渲染字形纹理
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// 更新 VBO 内存的内容
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferSubData(
			GL_ARRAY_BUFFER, 0, sizeof(vertices),
			vertices); // 一定要使用 glBufferSubData 而不是 glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// 渲染四边形
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// 现在为下一个字形推进光标
		x += (ch.Advance >> 6) * scale; // 位移 6 以获得像素值（1/64 乘以 2^6 = 64）
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}