#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D   scene;
uniform vec2        offsets[9];     // 以片段为中心的3*3纹理坐标（中心为(0, 0)）
uniform int         edge_kernel[9]; // 边缘检测加权核
uniform float       blur_kernel[9]; // 模糊加权核

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
    // 初始化，默认情况下输出变量的值是未定义的
    color = vec4(0.0f);

    vec3 sample[9];
    // 如果使用卷积矩阵，采样周围9个纹理坐标
    if(chaos || shake)
        for(int i = 0; i < 9; i++)
            sample[i] = vec3(texture(scene, TexCoords.st + offsets[i]));

    // 处理特效
    if(chaos)
    {           
        for(int i = 0; i < 9; i++)
            color += vec4(sample[i] * edge_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    else if(confuse)
    {
        color = vec4(1.0 - texture(scene, TexCoords).rgb, 1.0);
    }
    else if(shake)
    {
        for(int i = 0; i < 9; i++)
            color += vec4(sample[i] * blur_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    else
    {
        color =  texture(scene, TexCoords);
    }
}
