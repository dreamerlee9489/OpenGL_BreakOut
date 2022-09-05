#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

// ע������ʡ������ͼ���� ��ͼ��Զ����ı䣬�������ǻ�������һ����λ��ͼ������˿���ʡ������
uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); // �۲�ռ����� = ����ռ�����
}