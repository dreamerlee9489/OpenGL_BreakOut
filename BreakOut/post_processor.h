#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "sprite_renderer.h"
#include "texture.h"

/// <summary>
/// PostProcessor 托管 Breakout Game 的所有 PostProcessing 效果。 
/// 它在带纹理的四边形上渲染游戏，之后可以通过启用 Confuse、Chaos 或 Shake 布尔值来启用特定效果。
/// 需要在渲染游戏之前调用 BeginRender() 并在渲染游戏之后调用 EndRender() 才能使类工作。
/// </summary>
class PostProcessor {
public:
	// 状态
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;
	// 选项
	bool Confuse, Chaos, Shake;

	PostProcessor(Shader shader, unsigned int width, unsigned int height);

	// 在渲染游戏之前准备后处理器的帧缓冲操作
	void BeginRender();

	// 应该在渲染游戏后调用，因此它将所有渲染数据存储到纹理对象中
	void EndRender();

	// 渲染 PostProcessor 纹理四边形（作为一个包含屏幕的大精灵）
	void Render(float time);

private:
	// 渲染状态
	unsigned int MSFBO, FBO; // MSFBO = 多重采样 FBO。 FBO 是常规的，用于将 MS 颜色缓冲区传输到纹理
	unsigned int RBO; // RBO 用于多采样颜色缓冲区
	unsigned int VAO;

	// 初始化四边形以渲染后处理纹理
	void initRenderData();
};

#endif