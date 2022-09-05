#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"

/// <summary>
/// BallObject 持有从 GameObject 继承相关状态数据的 Ball 对象的状态。 
/// 包含一些特定于 Breakout 的球对象的额外功能，这些功能对于单独的 GameObject 来说太具体了。
/// </summary>
class BallObject : public GameObject {
public:
	// 小球状态
	float Radius;
	bool Stuck;
	bool Sticky, PassThrough;

	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	// 移动球，使其保持在窗口边界内（底部边缘除外）； 返回新位置
	glm::vec2 Move(float dt, unsigned int window_width);

	// 以给定的位置和速度将球重置为原始状态
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif