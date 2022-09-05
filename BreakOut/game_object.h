#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "sprite_renderer.h"
#include "texture.h"

/// <summary>
/// 用于保存与单个游戏对象实体相关的所有状态的容器对象。 
/// 游戏中的每个对象都可能需要 GameObject 中描述的最小状态。
/// </summary>
class GameObject {
public:
	// 物体状态
	glm::vec2 Position, Size, Velocity;
	glm::vec3 Color;
	float Rotation;
	bool IsSolid;
	bool Destroyed;
	// 渲染状态
	Texture2D Sprite;

	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite,
		glm::vec3 color = glm::vec3(1.0f),
		glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	// 绘制精灵
	virtual void Draw(SpriteRenderer& renderer);
};

#endif