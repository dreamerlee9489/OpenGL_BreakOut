#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D {
public:
    // holds the ID of the texture object, used for all texture operations to
    // reference to this particlar texture
    unsigned int ID;
    // texture image dimensions
    unsigned int Width, Height; // width and height of loaded image in pixels
    // texture Format
    unsigned int Internal_Format; // format of texture object
    unsigned int Image_Format;    // format of loaded image
    // texture configuration
    unsigned int Wrap_S;     // wrapping mode on S axis
    unsigned int Wrap_T;     // wrapping mode on T axis
    unsigned int Filter_Min; // filtering mode if texture pixels < screen pixels
    unsigned int Filter_Max; // filtering mode if texture pixels > screen pixels
    // constructor (sets default texture modes)
    Texture2D();

    // 接受一个字节数组以及宽度和高度，并生成一个2D纹理图像
    void Generate(unsigned int width, unsigned int height, unsigned char *data);

    // 将纹理绑定为当前活动的GL_TEXTURE_2D纹理对象
    void Bind() const;
};

#endif