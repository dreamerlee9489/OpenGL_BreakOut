#include <algorithm>
#include <iostream>
#include <sstream>

#include <learnopengl/filesystem.h>

#include <irrklang/irrKlang.h>

using namespace irrklang;

#include "ball_object.h"
#include "game.h"
#include "game_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "text_renderer.h"

// 游戏相关状态数据
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
PostProcessor* Effects;
ISoundEngine* SoundEngine = createIrrKlangDevice();
TextRenderer* Text;

float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height),
	Level(0), Lives(3) {}

Game::~Game() {
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	delete Text;
	SoundEngine->drop();
}

void Game::Init() {
	// 加载着色器
	ResourceManager::LoadShader("sprite.vert", "sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("particle.vert", "particle.frag", nullptr,
		"particle");
	ResourceManager::LoadShader("post_processing.vert", "post_processing.frag",
		nullptr, "postprocessing");
	// 配置着色器
	glm::mat4 projection =
		glm::ortho(0.0f, static_cast<float>(this->Width),
			static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
	// 加载纹理
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/background.jpg").c_str(), false,
		"background");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/awesomeface.png").c_str(), true,
		"face");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/block.png").c_str(), false,
		"block");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/block_solid.png").c_str(), false,
		"block_solid");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/paddle.png").c_str(), true,
		"paddle");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/particle.png").c_str(), true,
		"particle");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/powerup_speed.png").c_str(), true,
		"powerup_speed");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/powerup_sticky.png").c_str(),
		true, "powerup_sticky");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/powerup_increase.png").c_str(),
		true, "powerup_increase");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/powerup_confuse.png").c_str(),
		true, "powerup_confuse");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/powerup_chaos.png").c_str(), true,
		"powerup_chaos");
	ResourceManager::LoadTexture(
		FileSystem::getPath("resources/textures/powerup_passthrough.png").c_str(),
		true, "powerup_passthrough");
	// 设置渲染专用控制
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	Particles =
		new ParticleGenerator(ResourceManager::GetShader("particle"),
			ResourceManager::GetTexture("particle"), 500);
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"),
		this->Width, this->Height);
	Text = new TextRenderer(this->Width, this->Height);
	Text->Load(FileSystem::getPath("resources/fonts/OCRAEXT.TTF").c_str(), 24);
	// 加载关卡
	GameLevel one;
	one.Load(FileSystem::getPath("resources/levels/one.lvl").c_str(), this->Width,
		this->Height / 2);
	GameLevel two;
	two.Load(FileSystem::getPath("resources/levels/two.lvl").c_str(), this->Width,
		this->Height / 2);
	GameLevel three;
	three.Load(FileSystem::getPath("resources/levels/three.lvl").c_str(),
		this->Width, this->Height / 2);
	GameLevel four;
	four.Load(FileSystem::getPath("resources/levels/four.lvl").c_str(),
		this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;
	// configure game objects
	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE,
		ResourceManager::GetTexture("paddle"));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
		-BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
		ResourceManager::GetTexture("face"));
	// audio
	SoundEngine->play2D(
		FileSystem::getPath("resources/audio/breakout.mp3").c_str(), true);
}

void Game::Update(float dt) {
	// 更新球的位置
	Ball->Move(dt, this->Width);
	// 检测碰撞
	this->DoCollisions();
	// 更新粒子
	Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
	// update PowerUps
	this->UpdatePowerUps(dt);
	// 减少晃动时间
	if (ShakeTime > 0.0f) {
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = false;
	}
	// 检测失败条件
	if (Ball->Position.y >= this->Height) {
		--this->Lives;
		// 玩家失去所有生命时，重置游戏
		if (this->Lives == 0) {
			this->ResetLevel();
			this->State = GAME_MENU;
		}
		this->ResetPlayer();
	}
	// 检测胜利条件
	if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted()) {
		this->ResetLevel();
		this->ResetPlayer();
		Effects->Chaos = true;
		this->State = GAME_WIN;
	}
}

void Game::ProcessInput(float dt) {
	if (this->State == GAME_MENU) {
		if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER]) {
			this->State = GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
		}
		if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]) {
			this->Level = (this->Level + 1) % 4;
			this->KeysProcessed[GLFW_KEY_W] = true;
		}
		if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S]) {
			if (this->Level > 0)
				--this->Level;
			else
				this->Level = 3;
			// this->Level = (this->Level - 1) % 4;
			this->KeysProcessed[GLFW_KEY_S] = true;
		}
	}
	if (this->State == GAME_WIN) {
		if (this->Keys[GLFW_KEY_ENTER]) {
			this->KeysProcessed[GLFW_KEY_ENTER] = true;
			Effects->Chaos = false;
			this->State = GAME_MENU;
		}
	}
	if (this->State == GAME_ACTIVE) {
		float velocity = PLAYER_VELOCITY * dt;
		// 移动挡板、球
		if (this->Keys[GLFW_KEY_A]) {
			if (Player->Position.x >= 0.0f) {
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (this->Keys[GLFW_KEY_D]) {
			if (Player->Position.x <= this->Width - Player->Size.x) {
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		}
		if (this->Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;
	}
}

void Game::Render() {
	if (this->State == GAME_ACTIVE || this->State == GAME_MENU ||
		this->State == GAME_WIN) {
		// 开始向后处理帧缓冲区渲染
		Effects->BeginRender();
		// 绘制背景
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(this->Width, this->Height), 0.0f);
		// 绘制关卡
		this->Levels[this->Level].Draw(*Renderer);
		// 绘制挡板
		Player->Draw(*Renderer);
		// 渲染道具
		for (PowerUp& powerUp : this->PowerUps)
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		// 渲染粒子
		Particles->Draw();
		// 绘制球
		Ball->Draw(*Renderer);
		// 结束对后处理帧缓冲区的渲染
		Effects->EndRender();
		// 渲染后处理纹理
		Effects->Render((float)glfwGetTime());
		// 渲染文本（不要包括到后处理中）
		std::stringstream ss;
		ss << this->Lives;
		Text->RenderText("Lives:" + ss.str(), 5.0f, 5.0f, 1.0f);
	}
	if (this->State == GAME_MENU) {
		Text->RenderText("Press ENTER to start", this->Width / 2.0f - 150, this->Height / 2.0f, 1.0f);
		Text->RenderText("Press W or S to select level", this->Width / 2.0f - 155,
			this->Height / 2.0f + 20.0f, 0.75f);
	}
	if (this->State == GAME_WIN) {
		Text->RenderText("You WON!!!", this->Width / 2.0f - 150, this->Height / 2.0f - 20.0f, 1.0f,
			glm::vec3(0.0f, 1.0f, 0.0f));
		Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f,
			this->Height / 2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
	}
}

void Game::ResetLevel() {
	if (this->Level == 0)
		this->Levels[0].Load("levels/one.lvl", this->Width, this->Height / 2);
	else if (this->Level == 1)
		this->Levels[1].Load("levels/two.lvl", this->Width, this->Height / 2);
	else if (this->Level == 2)
		this->Levels[2].Load("levels/three.lvl", this->Width, this->Height / 2);
	else if (this->Level == 3)
		this->Levels[3].Load("levels/four.lvl", this->Width, this->Height / 2);

	this->Lives = 3;
}

void Game::ResetPlayer() {
	// 重置玩家/球的统计数据
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS,
		-(BALL_RADIUS * 2.0f)),
		INITIAL_BALL_VELOCITY);
	// 还禁用所有活动的道具
	Effects->Chaos = Effects->Confuse = false;
	Ball->PassThrough = Ball->Sticky = false;
	Player->Color = glm::vec3(1.0f);
	Ball->Color = glm::vec3(1.0f);
}

bool IsOtherPowerUpActive(std::vector <PowerUp>& powerUps, std::string type);

void Game::UpdatePowerUps(float dt) {
	for (PowerUp& powerUp : this->PowerUps) {
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated) {
			powerUp.Duration -= dt;
			if (powerUp.Duration <= 0.0f) {
				// 之后将这个道具从vector中移除
				powerUp.Activated = false;
				// 停用效果
				if (powerUp.Type == "sticky") {
					if (!IsOtherPowerUpActive(
						this->PowerUps,
						"sticky")) { // 仅在没有其它sticky效果处于激活状态时重置
						Ball->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through") {
					if (!IsOtherPowerUpActive(
						this->PowerUps,
						"pass-through")) {
						Ball->PassThrough = false;
						Ball->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse") {
					if (!IsOtherPowerUpActive(
						this->PowerUps,
						"confuse")) {
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos") {
					if (!IsOtherPowerUpActive(
						this->PowerUps,
						"chaos")) {
						Effects->Chaos = false;
					}
				}
			}
		}
	}
	// 从vector中移除所有已摧毁且失活的道具（要么离开地图，要么结束）
	// 使用一个lambda表达式来移除每个已摧毁且失活的道具
	// remove_if函数将lamda表达式为true的元素移动至容器的末尾并返回一个迭代器指向应被移除的元素范围的开始部分
	// 容器的erase函数接着擦除这个迭代器指向的元素与容器末尾元素之间的所有元素
	this->PowerUps.erase(
		std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
			[](const PowerUp& powerUp) {
				return powerUp.Destroyed && !powerUp.Activated;
			}),
		this->PowerUps.end());
}

bool ShouldSpawn(unsigned int chance) {
	unsigned int random = rand() % chance;
	return random == 0;
}

void Game::SpawnPowerUps(GameObject& block) {
	if (ShouldSpawn(75)) // 1/75的几率
		this->PowerUps.push_back(
			PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position,
				ResourceManager::GetTexture("powerup_speed")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(
			PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position,
				ResourceManager::GetTexture("powerup_sticky")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp(
			"pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position,
			ResourceManager::GetTexture("powerup_passthrough")));
	if (ShouldSpawn(75))
		this->PowerUps.push_back(PowerUp(
			"pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position,
			ResourceManager::GetTexture("powerup_increase")));
	if (ShouldSpawn(15)) // 负面道具被更频繁的生成
		this->PowerUps.push_back(
			PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position,
				ResourceManager::GetTexture("powerup_confuse")));
	if (ShouldSpawn(15))
		this->PowerUps.push_back(
			PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position,
				ResourceManager::GetTexture("powerup_chaos")));
}

/// <summary>
/// 根据道具类型激活道具效果
/// </summary>
/// <param name="powerUp"></param>
void ActivatePowerUp(PowerUp& powerUp) {
	if (powerUp.Type == "speed") {
		Ball->Velocity *= 1.2;
	}
	else if (powerUp.Type == "sticky") {
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through") {
		Ball->PassThrough = true;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase") {
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse") {
		if (!Effects->Chaos)
			Effects->Confuse = true; // 只有在“混沌”特效未激活时生效
	}
	else if (powerUp.Type == "chaos") {
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
}

/// <summary>
/// 检查是否有同类道具处于激活状态
/// </summary>
/// <param name="powerUps"></param>
/// <param name="type"></param>
/// <returns></returns>
bool IsOtherPowerUpActive(std::vector <PowerUp>& powerUps, std::string type) {
	// 检查另一个相同类型的 PowerUp 是否仍然处于活动状态，在这种情况下我们不会禁用它的效果（还）
	for (const PowerUp& powerUp : powerUps) {
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}
	return false;
}

// 碰撞检测
bool CheckCollision(GameObject& one, GameObject& two);

Collision CheckCollision(BallObject& one, GameObject& two);

Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions() {
	for (GameObject& box : this->Levels[this->Level].Bricks) {
		if (!box.Destroyed) {
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) // 发生碰撞
			{
				if (!box.IsSolid) {    // 如果空心砖块就销毁
					box.Destroyed = true;
					this->SpawnPowerUps(box);    // 生成道具
					SoundEngine->play2D(
						FileSystem::getPath("resources/audio/bleep.mp3").c_str(), false);
				}
				else { // 如果是实心的，激活晃动特效
					ShakeTime = 0.05f;
					Effects->Shake = true;
					SoundEngine->play2D(
						FileSystem::getPath("resources/audio/bleep.mp3").c_str(), false);
				}
				// 碰撞处理
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				// 如果激活了穿透属性，不对空心砖块进行碰撞处理
				if (!(Ball->PassThrough && !box.IsSolid)) {
					if (dir == LEFT || dir == RIGHT) // 水平方向碰撞
					{
						Ball->Velocity.x = -Ball->Velocity.x; // 反转水平速度
						float penetration =
							Ball->Radius - std::abs(diff_vector.x); // 渗透的水平距离
						if (dir == LEFT)
							Ball->Position.x += penetration; // 将球右移
						else
							Ball->Position.x -= penetration; // 将球左移
					}
					else                               // 垂直方向碰撞
					{
						Ball->Velocity.y = -Ball->Velocity.y; // 反转垂直速度
						float penetration =
							Ball->Radius - std::abs(diff_vector.y); // 渗透的垂直距离
						if (dir == UP)
							Ball->Position.y -= penetration; // 将球上移
						else
							Ball->Position.y += penetration; // 将球下移
					}
				}
			}
		}
	}

	// 检查道具的碰撞情况，如果有，就激活它们
	for (PowerUp& powerUp : this->PowerUps) {
		if (!powerUp.Destroyed) {
			// 首先检查上电是否通过底部边缘，如果是：保持非活动状态并销毁
			if (powerUp.Position.y >= this->Height)
				powerUp.Destroyed = true;

			if (CheckCollision(*Player, powerUp)) {
				ActivatePowerUp(powerUp);    // 激活道具
				powerUp.Destroyed = true;
				powerUp.Activated = true;
				SoundEngine->play2D(
					FileSystem::getPath("resources/audio/powerup.wav").c_str(), false);
			}
		}
	}

	// 最后检查挡板的碰撞情况
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result)) {
		// 检查碰到了挡板的哪个位置，并根据碰到哪个位置来改变速度
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage =
			distance / (Player->Size.x /
				2.0f); // 小球中心和挡板中心的距离占挡板半边长的比例(-1, 1)
		// 根据比例移动
		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity; // 垂直速度保持不变
		Ball->Velocity.x =
			INITIAL_BALL_VELOCITY.x * percentage * strength; // 更新水平速度
		Ball->Velocity = glm::normalize(Ball->Velocity) *
			glm::length(oldVelocity); // 保持速度不变
		// 修复粘板问题，使球向上运动
		// Ball->Velocity.y = -Ball->Velocity.y;
		Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

		// 如果激活了Sticky道具，将球粘到挡板上
		Ball->Stuck = Ball->Sticky;

		SoundEngine->play2D(
			FileSystem::getPath("resources/audio/bleep.wav").c_str(), false);
	}
}

// AABB - AABB 碰撞
bool CheckCollision(GameObject& one, GameObject& two) {
	// x轴方向碰撞？
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// y轴方向碰撞？
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// 只有两个轴向都有碰撞时才碰撞
	return collisionX && collisionY;
}

// AABB - 圆 碰撞
Collision CheckCollision(BallObject& one, GameObject& two) {
	// 获取圆的中心
	glm::vec2 center(one.Position + one.Radius);
	// 计算AABB信息 (中心, 半边长)
	glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x,
		two.Position.y + aabb_half_extents.y);
	// 获取两个中心的矢量差
	glm::vec2 difference = center - aabb_center;
	// clamped一个分量总是位于AABB的边上， 另一个分量被半边长限制
	glm::vec2 clamped =
		glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
	glm::vec2 closest = aabb_center + clamped;
	// 获得圆心center和最近点closest的矢量差并判断是否 length <= radius
	difference = closest - center;
	if (glm::length(difference) < one.Radius) // 不是 <=
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

/// <summary>
/// 计算小球撞击AABB的方向（上下左右）
/// </summary>
/// <param name="target">碰撞方向</param>
/// <returns></returns>
Direction VectorDirection(glm::vec2 target) {
	glm::vec2 compass[] = {
			glm::vec2(0.0f, 1.0f),  // up
			glm::vec2(1.0f, 0.0f),  // right
			glm::vec2(0.0f, -1.0f), // down
			glm::vec2(-1.0f, 0.0f)  // left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	// 点积越大，夹角绝对值越小（同向为1）
	for (unsigned int i = 0; i < 4; i++) {
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}
