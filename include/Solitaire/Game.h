#pragma once
#include <array>
#include <deque>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/RenderTexture.hpp>


#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

#include "Card.h"
#include "CardPile.h"

//TODO rework this to use pointers

class GameDifficulty
{
public:

	using CardDeque = std::deque<Card>;
	
	virtual void shuffleCards(CardDeque& pack) = 0;
	virtual ~GameDifficulty() = default;
};

class RandomDifficulty : public GameDifficulty
{
public:

	void shuffleCards(CardDeque& pack) override;
};

class IMemento
{
	
};

class Game
{
	using CardDeque = GameDifficulty::CardDeque;
	
	GameDifficulty* difficulty;
	CardDeque cards;

	bool bIsFinished{ false };

	std::vector<CardPile> piles;
	std::vector<TargetPile> targets{ 4 };
	
	SourcePile sourcePile;
	
	sf::RenderWindow gameWindow;
	sf::Event gameEvent;

	//TODO
	sf::Drawable* priorityObject{ nullptr };
	
	void shuffleCards();
	void drawObjects();
	void realign();
	
	Game();

	size_t gapX = 0;
	const size_t paddingX = 20;
	const size_t paddingY = 50;

	size_t tick = 0;

	struct TickSprite
	{
		TickSprite(const sf::Sprite& copy):
		sprite(copy)
		{
			srand(time(nullptr));

			speedX += std::pow(-1, rand()) * (rand() % 20) / 20;
			speedY += std::pow(-1, rand()) * (rand() % 10) / 10;
		}
		
		sf::Sprite sprite;
		size_t tickCount = 0;

		float speedX = -0.4f;
		float speedY = -2.f;

		float gravity = 0.02f;
		float gravitySpeed = 0.f;

		float bounce = 0.3f;
	};
	
	std::vector<TickSprite> sp;
	std::vector<sf::Sprite> spr;
	sf::RenderTexture rt;

	sf::Music m;

	sf::SoundBuffer pickBuffer;
	sf::SoundBuffer flipBuffer;
	sf::SoundBuffer dropBuffer;
	sf::SoundBuffer badDropBuffer;
	
public:

	static sf::Sound flipSound;
	static sf::Sound pickSound;
	static sf::Sound dropSound;
	static sf::Sound badDropSound;

	Game& operator=(const Game&) = delete;
	Game& operator=(Game&&) = delete;

	Game(const Game&) = delete;
	Game(Game&&) = delete;

	static void playSound(sf::Sound& sound);
	
	void undo();
	void save();
	
	static Game& getGameInstance();
	CardDeque& getCardPack();

	sf::Vector2u getView() const;
	
	ICardPlace* getCardSourceFromPosition(int x, int y);
	ICardPlace* getCardSourceFromArea(const sf::FloatRect& rect);

	TargetPile* getMatchingTargetPile(const Card& card);

	//TODO
	void setPriorityObject(sf::Drawable* drawable) { priorityObject = drawable; }

	void finishGame();

	void updatePhysics();
	void start();

	~Game();
};
