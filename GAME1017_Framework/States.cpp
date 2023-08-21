#include <iostream>
#include "States.h"
#include "StateManager.h"
#include "Engine.h"
// GameObject includes.
#include "Primitives.h"
#include "Button3.h"
#include "TiledLevel.h"
#include "Player.h"
#include "Enemy.h"
// Remaining managers.
#include "CollisionManager.h"
#include "EventManager.h"
#include "RenderManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "PathManager.h"
#include <iostream>
using namespace std;

void State::Update()
{
	for (auto const& i : m_objects)
	{
		//if (i.first == "player")
			//cout << i.second->GetDst()->x << endl;
		if (i.first == "enemy1") {
			if (GetChild("enemy1") != nullptr)	//CombatEnemy
			{
				if (MAMA::Distance(GetChild("enemy1")->GetCenter(), GetChild("player")->GetCenter()) <= 30.0 &&
					MAMA::Rad2Deg(MAMA::AngleBetweenPoints(GetChild("player")->GetCenter().y - GetChild("enemy1")->GetCenter().y, GetChild("player")->GetCenter().x - GetChild("enemy1")->GetCenter().x)) < 180)
				{
					cout << "Close range\n";
					i.second->Update(true);
					const std::string& key = m_objects[1].first;
					GameObject* object = m_objects[1].second;

					if (key == "player") {
						Player* player = dynamic_cast<Player*>(object);
						if (player) {
							player->Take_Damage();
						}
					}
				}
				else
					i.second->Update(false);
			}
		}
		else if (i.first == "enemy2") {
			if (GetChild("enemy2") != nullptr)	//RangedCombatEnemy
			{
				if (MAMA::Distance(GetChild("enemy2")->GetCenter(), GetChild("player")->GetCenter()) <= 30.0 &&
					MAMA::Rad2Deg(MAMA::AngleBetweenPoints(GetChild("player")->GetCenter().y - GetChild("enemy2")->GetCenter().y, GetChild("player")->GetCenter().x - GetChild("enemy2")->GetCenter().x)) < 180)
				{
					//cout << "Close range\n";
					i.second->Update();
					//i.second->Update(true);
					const std::string& key = m_objects[1].first;
					GameObject* object = m_objects[1].second;

					if (key == "player") {
						Player* player = dynamic_cast<Player*>(object);
						if (player) {
							//player->Take_Damage(); //dont damage this way. attack using projectile test
						}
					}
				}
				else
					i.second->Update();
			}
		}
		else
			i.second->Update();
		if (STMA::StateChanging()) return;
	}
}
void State::Render()
{
	for (auto const& i : m_objects)
		i.second->Render();
	if (dynamic_cast<GameState*>(this) && dynamic_cast<PauseState*>(STMA::GetStates().back()))
		return; // If GameState is rendering but PauseState is the current state, return.
	SDL_RenderPresent(REMA::GetRenderer());
}
void State::Exit()
{
	for (auto& i : m_objects)
	{
		delete i.second;
		i.second = nullptr;
	}
	m_objects.clear();
	m_objects.shrink_to_fit();
}
void State::AddChild(std::string key, GameObject* object)
{
	m_objects.push_back(pair<string, GameObject*>(key, object));
}
GameObject* State::GetChild(const std::string& key)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[&](const std::pair<std::string, GameObject*>& element)
		{
			return element.first == key;
		});
	if (it != m_objects.end())
		return it->second;
	else return nullptr;
}

template<class T>
T State::GetChild(const std::string& key)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[&](const std::pair<std::string, GameObject*>& element)
		{
			return element.first == key;
		});
	if (it != m_objects.end())
	{
		return static_cast<T>(it->second); // Downcasting now done here!
	}
	else return nullptr;
}

void State::RemoveChild(const std::string& key)
{
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[&](const std::pair<std::string, GameObject*>& element)
		{
			return element.first == key;
		});
	if (it != m_objects.end())
	{
		delete it->second; // Deallocate GameObject.
		it->second = nullptr; // Wrangle dangle.
		m_objects.erase(it);
		m_objects.shrink_to_fit();
	}
}

// Begin TitleState.
TitleState::TitleState() = default;
void TitleState::Enter()
{
	cout << "Entering TitleState..." << endl;
	TEMA::Load("../Assets/img/play.png", "play");
	FOMA::Load("../Assets/img/ltype.TTF", "ltype34", 34);
	AddChild("play", new PlayButton({ 0, 0, 400, 100 }, { 412.0f, 384.0f, 200.0f, 50.0f }, "play"));
	AddChild("info", new Label("ltype34", 225, 175, "David Uwem Asher - 101448950"));
	AddChild("info", new Label("ltype34", 225, 210, "Melih Burak Asci - 101458113"));
	AddChild("info", new Label("ltype34", 225, 245, "Vali Alasgaarov - 101399073"));
	SOMA::AllocateChannels(16);
	SOMA::SetMusicVolume(32);
}
void TitleState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_N))
	{
		STMA::ChangeState(new GameState());
		return;
	}
	State::Update();
}
void TitleState::Render()
{
	SDL_SetRenderDrawColor(REMA::GetRenderer(), 255, 0, 0, 255);
	SDL_RenderClear(REMA::GetRenderer());
	State::Render();
}
void TitleState::Exit()
{
	cout << "Exiting TitleState..." << endl;
	TEMA::Unload("play");
	FOMA::Unload("ltype34");
	State::Exit();
}
// End TitleState.

// Begin PauseState.
PauseState::PauseState() = default;
void PauseState::Enter()
{
	cout << "Entering PauseState..." << endl;
	AddChild("pauseText", new Label("ltype24", 325, 175, "Paused - press R to resume"));
}
void PauseState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_R))
	{
		STMA::PopState();
		return;
	}
	State::Update();
}
void PauseState::Render()
{
	// First render the GameState
	STMA::GetStates().front()->Render();
	// Now render rest of PauseState
	SDL_SetRenderDrawColor(REMA::GetRenderer(), 0, 0, 255, 128);
	SDL_Rect rect = { 256, 128, 512, 512 };
	SDL_RenderFillRect(REMA::GetRenderer(), &rect);
	State::Render();
}
void PauseState::Exit()
{
	cout << "Exiting PauseState..." << endl;
	State::Exit();
}
// End PauseState.

// Begin GameState.
GameState::GameState() : m_enemyCounter(0) {}
void GameState::Enter()
{
	cout << "Entering GameState..." << endl;
	FOMA::Load("../Assets/img/ltype.TTF", "ltype24", 24);
	FOMA::Load("../Assets/img/ltype.TTF", "ltype14", 10);
	FOMA::Load("../Assets/img/ltype.TTF", "tile", 10);
	TEMA::Load("../Assets/img/Tile.png", "tiles");
	TEMA::Load("../Assets/img/Player.png", "player");
	TEMA::Load("../Assets/img/Player.png", "enemy");
	TEMA::Load("../Assets/img/enemy-projectile.bmp", "enemyProjectile");
	SOMA::Load("../Assets/aud/play.ogg", "music", SOUND_MUSIC);
	SOMA::Load("../Assets/aud/move.wav", "move", SOUND_SFX);

	AddChild("level", new TiledLevel(24, 32, 32, 32, "../Assets/dat/Tiledata.txt", "../Assets/dat/Level1.txt", "tiles"));
	AddChild("player", new Player({ 0,256,128,128 }, { (float)(16) * 32, (float)(12) * 32.0f, 32.0f, 32.0f }, GetChild<TiledLevel*>("level")));
	AddChild("menu", new Label("ltype14", 35.0f, 7.0f, "MENU: (~) Toggle Debug | WADS: Move Player | V: To Attack Enemy | T: For Enemy Movement | Space: Toggle Heuristic", { 255,255,255,255 }));
	AddChild("enemy1", new CloseCombatEnemy({ 0,0,128,128 }, { (float)(4) * 32, (float)(1) * 32.0f, 32.0f, 32.0f }, GetChild<TiledLevel*>("level"), 1));	//far one
	AddChild("enemy2", new RangedCombatEnemy({ 0,0,128,128 }, { (float)(27) * 32, (float)(3) * 32.0f, 32.0f, 32.0f }, GetChild<TiledLevel*>("level"), 2));

	const std::string& playerkey = m_objects[1].first;
	GameObject* object = m_objects[1].second;

	const std::string& enemykey = m_objects[4].first;
	GameObject* enemyobject = m_objects[4].second;
	//GameObject* enemy = m_objects[4].second;

	if (enemykey == "enemy2") {
		Player* player = dynamic_cast<Player*>(object);
		Enemy* enemy = dynamic_cast<Enemy*>(enemyobject);
		if (enemy) {
			enemy->set_playerReference(player);
			//player->Take_Damage();
		}
	}

	//GetChild("enemy2"). 
	m_enemyCounter = 2;

	SOMA::PlayMusic("music");
	SOMA::SetSoundVolume(32);
	SOMA::SetMusicVolume(50);
}
void GameState::Update()
{

	if (EVMA::KeyPressed(SDL_SCANCODE_X))
	{
		STMA::ChangeState(new TitleState()); // Change to new TitleState
		return;
	}
	else if (EVMA::KeyPressed(SDL_SCANCODE_P))
	{
		SOMA::PauseMusic();
		STMA::PushState(new PauseState()); // Add new PauseState
		return;
	}
	else if (EVMA::KeyPressed(SDL_SCANCODE_GRAVE)) // ~ or ` key. Toggle debug mode.
	{
		GetChild<Enemy*>("enemy1")->SetDebugView();
		GetChild<Enemy*>("enemy2")->SetDebugView();
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_V))
	{
		if (GetChild("enemy1") != nullptr)	//Combat Enemy
		{
			if (MAMA::Distance(GetChild("enemy1")->GetCenter(), GetChild("player")->GetCenter()) <= 30.0 &&
				MAMA::Rad2Deg(MAMA::AngleBetweenPoints(GetChild("player")->GetCenter().y - GetChild("enemy1")->GetCenter().y, GetChild("player")->GetCenter().x - GetChild("enemy1")->GetCenter().x)) < 180)
			{
				GetChild<Enemy*>("enemy1")->TakeDamage();
				if (GetChild<Enemy*>("enemy1")->GetHealth() <= 0)
				{
					RemoveChild("enemy1");
					m_enemyCounter--;
				}
			}
		}

		if (GetChild("enemy2") != nullptr)	//RangedCombatEnemy
		{
			if (MAMA::Distance(GetChild("enemy2")->GetCenter(), GetChild("player")->GetCenter()) <= 30.0 &&
				MAMA::Rad2Deg(MAMA::AngleBetweenPoints(GetChild("player")->GetCenter().y - GetChild("enemy2")->GetCenter().y, GetChild("player")->GetCenter().x - GetChild("enemy2")->GetCenter().x)) < 180)
			{
				GetChild<Enemy*>("enemy2")->TakeDamage();
				if (GetChild<Enemy*>("enemy2")->GetHealth() <= 0)
				{
					RemoveChild("enemy2");
					m_enemyCounter--;
				}
			}
		}

	}

	// Update objects.
	State::Update();
}
void GameState::Render()
{
	SDL_SetRenderDrawColor(REMA::GetRenderer(), 0, 255, 0, 255);
	SDL_RenderClear(REMA::GetRenderer());
	State::Render();
}
void GameState::Exit()
{
	cout << "Exiting GameState..." << endl;
	PAMA::ClearPath();
	TEMA::Unload("tiles");
	TEMA::Unload("player");
	TEMA::Unload("enemy");
	SOMA::Unload("music", SOUND_MUSIC);
	SOMA::Unload("move", SOUND_SFX);
	FOMA::Quit();
	State::Exit();
}
void GameState::Resume()
{
	cout << "Resuming GameState..." << endl;
	SOMA::ResumeMusic();
}
// End GameState