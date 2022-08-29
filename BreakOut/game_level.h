#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "resource_manager.h"
#include "sprite_renderer.h"

/**
 * GameLevel保存着作为Breakout关卡一部分的所有砖块，并承载着从硬盘加载/渲染关卡的功能
 */
class GameLevel {
public:
    std::vector <GameObject> Bricks;

    // constructor
    GameLevel() {}

    // 从文件中加载关卡
    void Load(const char *file, unsigned int levelWidth,
              unsigned int levelHeight);

    // 渲染关卡
    void Draw(SpriteRenderer &renderer);

    // 检查一个关卡是否已完成 (所有非坚硬的砖块均被摧毁)
    bool IsCompleted();

private:
    // 由砖块数据初始化关卡
    void init(std::vector <std::vector<unsigned int>> tileData,
              unsigned int levelWidth, unsigned int levelHeight);
};

#endif