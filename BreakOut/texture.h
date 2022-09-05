#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

// Texture2D 能够在 OpenGL 中存储和配置纹理。 它还托管实用程序功能，以便于管理。
class Texture2D {
public:
	// 保存纹理对象的ID，用于所有纹理操作引用该特定纹理
	unsigned int ID;
	// 纹理图像尺寸
	unsigned int Width, Height; // 加载图像的宽度和高度（以像素为单位）
	// 纹理格式
	unsigned int Internal_Format; // 纹理对象的格式
	unsigned int Image_Format;    // 加载图像的格式
	// 纹理配置
	unsigned int Wrap_S;     // S轴环绕模式
	unsigned int Wrap_T;     // T轴环绕模式
	unsigned int Filter_Min; // 纹理像素 < 屏幕像素时的过滤模式
	unsigned int Filter_Max; // 纹理像素 > 屏幕像素时的过滤模式

	// 设置默认纹理模式
	Texture2D();

	// 接受一个字节数组以及宽度和高度，并生成一个2D纹理图像
	void Generate(unsigned int width, unsigned int height, unsigned char* data);

	// 将纹理绑定为当前活动的GL_TEXTURE_2D纹理对象
	void Bind() const;
};

#endif