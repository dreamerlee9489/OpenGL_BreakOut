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
// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2>
Collision; // <collision?, what direction?, difference vector center -
// closest point>

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const float PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game {
public:
	// game state
	GameState State;
	bool Keys[1024];
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

	// powerups
	void SpawnPowerUps(GameObject& block);

	void UpdatePowerUps(float dt);
};

#endif