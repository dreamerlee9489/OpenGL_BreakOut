#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "shader.h"
#include "texture.h"

// 代表单个粒子及其状态
struct Particle {
  glm::vec2 Position, Velocity;
  glm::vec4 Color;
  float Life;

  Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator {
public:
  /// <summary>
  /// 构造函数
  /// </summary>
  /// <param name="shader"></param>
  /// <param name="texture"></param>
  /// <param name="amount">粒子数量</param>
  ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);

  /// <summary>
  /// 更新所有粒子属性
  /// </summary>
  /// <param name="dt"></param>
  /// <param name="object">小球</param>
  /// <param name="newParticles">每帧更新的粒子数</param>
  /// <param name="offset">小球的1/2半径</param>
  void Update(float dt, GameObject &object, unsigned int newParticles,
              glm::vec2 offset = glm::vec2(0.0f, 0.0f));

  // render all particles
  void Draw();

private:
  // state
  std::vector<Particle> particles;
  unsigned int amount;
  // render state
  Shader shader;
  Texture2D texture;
  unsigned int VAO;

  // initializes buffer and vertex attributes
  void init();

  // returns the first Particle index that's currently unused e.g. Life <= 0.0f
  // or 0 if no particle is currently inactive
  unsigned int firstUnusedParticle();

  // respawns particle
  void respawnParticle(Particle &particle, GameObject &object,
                       glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif