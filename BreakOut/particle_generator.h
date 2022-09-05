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

/// <summary>
/// ParticleGenerator 充当容器，通过重复生成和更新粒子并在给定时间后杀死它们来渲染大量粒子。
/// </summary>
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
	void Update(float dt, GameObject& object, unsigned int newParticles,
		glm::vec2 offset = glm::vec2(0.0f, 0.0f));

	// 渲染所有粒子
	void Draw();

private:
	// 状态
	std::vector <Particle> particles;
	unsigned int amount;
	// 渲染状态
	Shader shader;
	Texture2D texture;
	unsigned int VAO;

	// 初始化缓冲区和顶点属性
	void init();

	// 返回当前未使用的第一个粒子索引，例如 如果当前没有粒子处于非活动状态，则 Life <= 0.0f 或 0
	unsigned int firstUnusedParticle();

	// 重生粒子
	void respawnParticle(Particle& particle, GameObject& object,
		glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif