#ifndef POWER_UP_H
#define POWER_UP_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"

// The size of a PowerUp block
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
// Velocity a PowerUp block has when spawned
const glm::vec2 VELOCITY(0.0f, 150.0f);

/// <summary>
/// PowerUp从GameObject继承了它的状态和渲染功能
/// 但也持有额外的信息来说明它的持续时间和它是否被激活
/// </summary>
class PowerUp : public GameObject {
public:
    // powerup state
    std::string Type;
    float Duration;
    bool Activated;

    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="type"></param>
    /// <param name="color"></param>
    /// <param name="duration">持续时间，0表示无限长</param>
    /// <param name="position"></param>
    /// <param name="texture"></param>
    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position,
            Texture2D texture)
            : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY),
              Type(type), Duration(duration), Activated() {}
};

#endif