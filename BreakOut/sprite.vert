#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

// 注意我们省略了视图矩阵； 视图永远不会改变，所以我们基本上有一个单位视图矩阵，因此可以省略它。
uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); // 观察空间坐标 = 世界空间坐标
}