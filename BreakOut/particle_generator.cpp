#include "particle_generator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture,
                                     unsigned int amount)
    : shader(shader), texture(texture), amount(amount) {
  this->init();
}

void ParticleGenerator::Update(float dt, GameObject &object,
                               unsigned int newParticles, glm::vec2 offset) {
  // 更新消亡粒子
  for (unsigned int i = 0; i < newParticles; ++i) {
    int unusedParticle = this->firstUnusedParticle();
    this->respawnParticle(this->particles[unusedParticle], object, offset);
  }
  // 更新所有粒子
  for (unsigned int i = 0; i < this->amount; ++i) {
    Particle &p = this->particles[i];
    p.Life -= dt;        // 减少生命值
    if (p.Life > 0.0f) { // 更新位置、透明度
      p.Position -= p.Velocity * dt;
      p.Color.a -= dt * 2.5f;
    }
  }
}

// 渲染所有粒子
void ParticleGenerator::Draw() {
  // 使用叠加混合，使其具有"发热"效果
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  this->shader.Use();
  for (Particle particle : this->particles) {
    if (particle.Life > 0.0f) {
      this->shader.SetVector2f("offset", particle.Position);
      this->shader.SetVector4f("color", particle.Color);
      this->texture.Bind();             // 绑定纹理
      glBindVertexArray(this->VAO);     // 绑定网格
      glDrawArrays(GL_TRIANGLES, 0, 6); // 绘制网格
      glBindVertexArray(0);             // 解绑网格
    }
  }
  // 重置为默认混合模式
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init() {
  // 设置网格和属性
  unsigned int VBO;
  float particle_quad[] = {
      // 本地坐标 // 纹理坐标
      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

      0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(this->VAO);
  // 填充网格/顶点属性数组缓冲
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad,
               GL_STATIC_DRAW);
  // 设置网格/顶点属性数组属性
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glBindVertexArray(0);

  // 构造amount个默认粒子实例
  for (unsigned int i = 0; i < this->amount; ++i)
    this->particles.push_back(Particle());
}

// 存储上一个消亡粒子的索引
unsigned int lastUsedParticle = 0;

/// <summary>
/// 搜索下一个消亡的粒子
/// </summary>
/// <returns></returns>
unsigned int ParticleGenerator::firstUnusedParticle() {
  // 从上一个消亡粒子开始搜索，这通常会立即返回
  for (unsigned int i = lastUsedParticle; i < this->amount; ++i) {
    if (this->particles[i].Life <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }
  // 否则，线性检索
  for (unsigned int i = 0; i < lastUsedParticle; ++i) {
    if (this->particles[i].Life <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }
  // 没有找到下一个消亡粒子（生命值太长或粒子数太少）
  lastUsedParticle = 0;
  return 0;
}

/// <summary>
/// 将粒子生命值重置为1，颜色值置为>=0.5的随机值，根据object分配位置和速度
/// </summary>
/// <param name="particle"></param>
/// <param name="object"></param>
/// <param name="offset"></param>
void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object,
                                        glm::vec2 offset) {
  float random = ((rand() % 100) - 50) / 10.0f;
  float rColor = 0.5f + ((rand() % 100) / 100.0f);
  particle.Position = object.Position + random + offset;
  particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
  particle.Life = 1.0f;
  particle.Velocity = object.Velocity * 0.1f;
}