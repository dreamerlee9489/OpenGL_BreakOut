#ifndef GAME_H
#define GAME_H

#include <tuple>
#include <vector>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "game_level.h"
#include "power_up.h"

// 代表了游戏的当前状态
enum GameState {
	GAME_ACTIVE, GAME_MENU, GAME_WIN
};

// Represents the four possible (collision) directions
enum Direction {
	UP, RIGHT, DOWN, LEFT
};

// 代表碰撞数据的元组 <是否碰撞, 碰撞反向, 球心和最近点的矢量差>
typedef std::tuple<bool, Direction, glm::vec2> Collision;

// 挡板大小
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// 挡板速率
const float PLAYER_VELOCITY(500.0f);
// 球的速率
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// 球的半径
const float BALL_RADIUS = 12.5f;

/// <summary>
/// 游戏拥有所有与游戏相关的状态和功能。
/// 将所有与游戏相关的数据组合到一个类中，以便轻松访问每个组件和可管理性。
/// </summary>
class Game {
public:
	// game state
	GameState State;
	bool Keys[1024];
	// 记录处理过的按键，下一帧不再处理（直到松开）
	bool KeysProcessed[1024];
	unsigned int Width, Height;
	std::vector <GameLevel> Levels;
	std::vector <PowerUp> PowerUps;
	unsigned int Level;
	unsigned int Lives;

	//
	Game(unsigned int width, unsigned int height);

	~Game();

	// 初始化游戏状态（加载所有着色器/纹理/关卡）
	void Init();

	// 游戏循环
	void ProcessInput(float dt);

	void Update(float dt);

	void Render();

	void DoCollisions();

	// reset
	void ResetLevel();

	void ResetPlayer();

	/// <summary>
	/// 以一定概率在给定砖块位置生成一个道具
	/// </summary>
	/// <param name="block"></param>
	void SpawnPowerUps(GameObject& block);

	/// <summary>
	/// 管理所有当前被激活的道具
	/// </summary>
	/// <param name="dt"></param>
	void UpdatePowerUps(float dt);
};

#endif