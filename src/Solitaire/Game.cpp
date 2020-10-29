#include "Game.h"

#include <ctime>
#include <iostream>

#include "SFML/Audio.hpp"
#include "CardSprite.h"
#include "EventListener.h"
#include "Logger.h"

sf::Sound Game::pickSound;
sf::Sound Game::flipSound;
sf::Sound Game::dropSound;
sf::Sound Game::badDropSound;

void RandomDifficulty::shuffleCards(CardDeque& pack)
{
	for (int i = 51; i > 0; --i)
	{
		int j = rand() % (i + 1);
		std::swap(pack[i], pack[j]);
	}
}

void Game::shuffleCards()
{
	difficulty->shuffleCards(cards);
}

void Game::drawObjects()
{	
	for (auto&& p : piles)
	{
		gameWindow.draw(p);
	}

	for (auto&& t : targets)
		gameWindow.draw(t);

	gameWindow.draw(sourcePile);

	for (auto&& s : spr)
		gameWindow.draw(s);
	
	//for (auto&& s : sp)
		//gameWindow.draw(s.sprite);

	static sf::Sprite s;
	s.setTexture(rt.getTexture());
	
	gameWindow.draw(s);
	
	if (priorityObject)
		gameWindow.draw(*priorityObject);
}

void Game::realign()
{
	//Game window width with horizontal paddings
	size_t wX = gameWindow.getSize().x - 2 * paddingX;

	//Gap between card piles
	gapX = (wX - 7 * CARD_WIDTH) / 8;
	size_t targetsOffset = 3 * gapX + 3 * CARD_WIDTH;
	
	for (size_t i = 0; i < 4; ++i)
	{
		targets[i].setPosition(paddingX + gapX * (i+1) + CARD_WIDTH * i + targetsOffset, paddingY);
	}

	for (size_t i = 0; i < 7; ++i)
	{
		piles[i].setPosition(paddingX + gapX * (i+1) + CARD_WIDTH * i, CARD_HEIGHT + paddingY + 40);
	}

	sourcePile.setPosition(paddingX + gapX, paddingY);
}

Game::Game() :
	difficulty(new RandomDifficulty()),
	gameWindow({ 1024,768 }, "Solitaire"),
	gameEvent()
{
	pickBuffer.loadFromFile("sounds/soundPick.ogg");
	pickSound.setBuffer(pickBuffer);

	flipBuffer.loadFromFile("sounds/soundFlip.ogg");
	flipSound.setBuffer(flipBuffer);

	dropBuffer.loadFromFile("sounds/soundDrop.ogg");
	dropSound.setBuffer(dropBuffer);

	badDropBuffer.loadFromFile("sounds/soundBadDrop.ogg");
	badDropSound.setBuffer(badDropBuffer);
	
	srand(time(NULL));
	
	for (int i = 0; i < 4; ++i)
	{
		for (size_t j = 1; j <= 13; ++j)
		{
			cards.emplace_back(j, static_cast<Card::ECardSuit>(i));
		}
	}

}

void Game::playSound(sf::Sound& sound)
{
	sound.play();
}

void Game::undo()
{
}

void Game::save()
{
}

Game& Game::getGameInstance()
{
	static Game game;
	return game;
}

Game::CardDeque& Game::getCardPack()
{
	return cards;
}

sf::Vector2u Game::getView() const
{
	return gameWindow.getSize();
}

ICardPlace* Game::getCardSourceFromPosition(int x, int y)
{
	return nullptr;
}

ICardPlace* Game::getCardSourceFromArea(const sf::FloatRect& rect)
{
	auto it = std::find_if(piles.begin(), piles.end(), [&](CardPile& pile)
	{
		return !pile.isBlocked() && pile.checkCollision(rect);
	});

	if (it != piles.end())
		return &*it;

	//Next try to get one of target piles
	auto it2 = std::find_if(targets.begin(), targets.end(), [&](TargetPile& pile)
	{
		return !pile.isBlocked() && pile.checkCollision(rect);
	});

	if (it2 != targets.end())
		return &*it2;

	return nullptr;
}

TargetPile* Game::getMatchingTargetPile(const Card& card)
{
	auto it = std::find_if(targets.begin(), targets.end(), [&](TargetPile& pile)
	{
		return pile.getSize() &&
			pile.getTopCard()->getSuit() == card.getSuit() && 
			pile.getTopCard()->compareRanks(card) == ECardPosition::Before;
	});

	if (it != targets.end())
		return &*it;

	return nullptr;
}

void Game::finishGame()
{
	EventBus::get().clearListeners();
	bIsFinished = true;

	if (m.openFromFile("sounds/win.ogg"))
		m.play();
}

const float n1 = 7.5625;
const float d1 = 2.75;

void Game::updatePhysics()
{
	for (auto&& s : sp)
	{
		if (s.sprite.getPosition().x + CARD_WIDTH >= 0 && s.sprite.getPosition().x <= gameWindow.getSize().x)
		{
			s.gravitySpeed += s.gravity;
			s.sprite.move(s.speedX, s.speedY + s.gravitySpeed);

			float dist = gameWindow.getSize().y - (float)CARD_HEIGHT;
			if (s.sprite.getPosition().y > dist)
			{
				s.sprite.setPosition(s.sprite.getPosition().x, dist);
				s.gravitySpeed = -(s.gravitySpeed * s.bounce);
			}

			rt.draw(s.sprite);
		}
	}
}

void Game::start()
{
	size_t idx = 0;
	size_t counter = 500;

	m.setVolume(50.f);
	
	shuffleCards();

	piles.reserve(7);
	for (size_t i = 1; i <= 7; ++i)
	{
		piles.emplace_back(i);
		EventBus::get().subscribe(piles.back());
	}

	for (auto&& t : targets)
		EventBus::get().subscribe(t);
	
	sourcePile.pushCards(cards);
	EventBus::get().subscribe(sourcePile);
	
	realign();

	rt.create(gameWindow.getSize().x, gameWindow.getSize().y);
	rt.display();
	
	while (gameWindow.isOpen())
	{
		while (gameWindow.pollEvent(gameEvent))
		{
			if (gameEvent.type == sf::Event::KeyPressed && gameEvent.key.code == sf::Keyboard::F)
				finishGame();

			if (gameEvent.type == sf::Event::KeyPressed && gameEvent.key.code == sf::Keyboard::R)
			{
				//Restart
			}
			
			if (gameEvent.type == sf::Event::Closed)
			{
				gameWindow.close();
				return;
			}

			if (gameEvent.type == sf::Event::Resized)
			{
				sf::FloatRect newArea(0, 0, gameEvent.size.width, gameEvent.size.height);
				gameWindow.setView(sf::View{newArea});

				realign();
			}

			if (!bIsFinished)
			{
				bool bFull = true;
				for (auto&& t : targets)
				{
					if (t.getSize() != 13)
					{
						bFull = false;
						break;
					}
				}

				if (bFull)
					finishGame();

				EventBus::get().send(gameEvent);
			}
		}

		++tick;

		gameWindow.clear({20, 40, 60});

		drawObjects();

		if (bIsFinished)
		{
			Logger::get().log("Game is finished. Ya!");

			if (counter != 500)
			{
				++counter;
			}
			else
			{
				counter = 0;

				if (targets[idx].getTopSprite())
				{
					sp.emplace_back(*targets[idx].getTopSprite());
					targets[idx].getDeque().pop_back();
					targets[idx].onMove();
				}
				idx = (idx + 1) % 4;
			}

			updatePhysics();
		}
		
		gameWindow.display();
	}
}

Game::~Game()
{
	delete difficulty;
}
