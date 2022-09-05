#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"

class SpriteRenderer {
public:
	// 初始化着色器/形状
	SpriteRenderer(Shader shader);
	SpriteRenderer(Shader& shader);
	~SpriteRenderer();

	// 使用给定的精灵渲染定义的四边形纹理
	void DrawSprite(Texture2D& texture, glm::vec2 position,
		glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));

private:
	// 渲染状态
	Shader shader;
	unsigned int quadVAO;

	// 初始化并配置四边形的缓冲区和顶点属性
	void initRenderData();
};

#endif