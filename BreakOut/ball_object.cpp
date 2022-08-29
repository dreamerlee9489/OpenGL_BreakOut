#include "ball_object.h"

BallObject::BallObject()
        : GameObject(), Radius(12.5f), Stuck(true), Sticky(false),
          PassThrough(false) {}

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity,
                       Texture2D sprite)
        : GameObject(pos, glm::vec2(radius * 2.0f, radius * 2.0f), sprite,
                     glm::vec3(1.0f), velocity),
          Radius(radius), Stuck(true), Sticky(false), PassThrough(false) {}

glm::vec2 BallObject::Move(float dt, unsigned int window_width) {
    // 没有被固定在挡板上
    if (!this->Stuck) {
        // 移动球
        this->Position += this->Velocity * dt;
        // 检查是否在窗口边界以外，如果是的话反转速度并恢复到正确的位置
        if (this->Position.x <= 0.0f) {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = 0.0f;
        } else if (this->Position.x + this->Size.x >= window_width) {
            this->Velocity.x = -this->Velocity.x;
            this->Position.x = window_width - this->Size.x; // 直径
        }
        if (this->Position.y <= 0.0f) {
            this->Velocity.y = -this->Velocity.y;
            this->Position.y = 0.0f;
        }
    }
    return this->Position;
}

// resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::Reset(glm::vec2 position, glm::vec2 velocity) {
    this->Position = position;
    this->Velocity = velocity;
    this->Stuck = true;
    this->Sticky = false;
    this->PassThrough = false;
}