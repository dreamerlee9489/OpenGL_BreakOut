#include "sprite_renderer.h"

SpriteRenderer::SpriteRenderer(Shader shader) {
	this->shader = shader;
	this->initRenderData();
}

SpriteRenderer::SpriteRenderer(Shader& shader) {
	this->shader = shader;
	this->initRenderData();
}

SpriteRenderer::~SpriteRenderer() { glDeleteVertexArrays(1, &this->quadVAO); }

void SpriteRenderer::DrawSprite(Texture2D& texture, glm::vec2 position,
	glm::vec2 size, float rotate, glm::vec3 color) {
	// 准备转换
	this->shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(
		model,
		glm::vec3(
			position,
			0.0f)); // 首先是平移（变换是：先发生缩放，然后是旋转，最后发生平移；顺序相反）

	model = glm::translate(
		model, glm::vec3(0.5f * size.x, 0.5f * size.y,
			0.0f)); // 旋转前将原点移到四边形中心（已缩放）
	model = glm::rotate(model, glm::radians(rotate),
		glm::vec3(0.0f, 0.0f, 1.0f)); // 然后旋转（总是围绕原点）
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y,
		0.0f)); // 旋转后将原点移回左上角

	model = glm::scale(model, glm::vec3(size, 1.0f)); // 最后缩放

	this->shader.SetMatrix4("model", model);

	// 渲染纹理四边形
	this->shader.SetVector3f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0); // 激活纹理单元0
	texture.Bind();

	glBindVertexArray(this->quadVAO); // 绘制前绑定VAO
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0); // 绘制后解绑VAO
}

void SpriteRenderer::initRenderData() {
	// 配置 VAO/VBO
	unsigned int VBO;
	float vertices[] = {
		// 本地坐标  // 纹理坐标
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 向GPU发送顶点数据
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(vertices), vertices,
		GL_STATIC_DRAW); // 顶点数据用于渲染且固定不变，所以用GL_STATIC_DRAW
	// 配置顶点属性
	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0); // layout (location = 0) in vec4 vertex
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}