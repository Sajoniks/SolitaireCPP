#include "CardPile.h"
#include "Card.h"
#include "DragDropManager.h"
#include "Game.h"
#include "Logger.h"

#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "TextureManager.h"

void ICardPlace::popCards(size_t count)
{
	deque.resize(deque.size() - count);
}

void ICardPlace::moveTo(ICardPlace& newSource, size_t count)
{
	if (newSource.canAcceptCards())
	{
		std::move(deque.rbegin(), deque.rbegin() + count, std::back_inserter(newSource.deque));
		std::reverse(newSource.deque.rbegin(), newSource.deque.rbegin() +count);
		popCards(count);

		onMove();
		newSource.onMove();
	}
}

Card* ICardPlace::getTopCard()
{
	if (deque.empty())
		return nullptr;

	return &deque.back();
}

size_t ICardPlace::getSize() const
{
	return deque.size();
}

void ICardPlace::setBlocked(bool bBlocked)
{
	this->bBlocked = bBlocked;
}

bool ICardPlace::isBlocked() const
{
	return bBlocked;
}

void IDraggable::eventReceive(sf::Event& event)
{
	auto&& mb = event.mouseButton;
	auto&& mm = event.mouseMove;
	
	//Detect drag
	if (!DragDropManager::isDragging())
	{
		if (event.type == sf::Event::MouseButtonPressed && mb.button == sf::Mouse::Left)
		{
			if (!deque.empty())
			{
				auto sprite = getSpriteFromPosition(mb.x, mb.y);
				if (sprite && !sprite->getCard().isFlipped())
					DragDropManager::get().enterDrag(*sprite, *this);
			}
		}
		else if (DragDropManager::isCaptured())
		{
			if (event.type == sf::Event::MouseButtonReleased && mb.button == sf::Mouse::Left)
				DragDropManager::get().exitDrag();
			
			else if (event.type == sf::Event::MouseMoved)
				DragDropManager::get().beginDrag(mm);
		}
	}
	else
	{
		if (event.type == sf::Event::MouseButtonReleased && mb.button == sf::Mouse::Left)
		{
			DragDropManager::get().drop(mb);
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			DragDropManager::get().drag(mm);
		}
	}
}

bool IDraggable::checkCollision(int x, int y) const
{
	return dropArea.contains(x, y);
}

bool IDraggable::checkCollision(const sf::FloatRect& rect)
{
	return dropArea.intersects(rect);
}

void CardPile::realign() const
{
	if (bNeedsRealign && !sprites.empty())
	{
		sprites.front().setPosition(dropArea.left, dropArea.top);
		bNeedsRealign = false;
	}
}

void CardPile::onMove()
{
	updateSprites();
	updateLinks();
	bNeedsRealign = true;
}

void CardPile::updateLinks()
{
	//If there are any cards in pile relink them
	if (!sprites.empty())
	{
		for (auto it = sprites.begin(); it != sprites.end() - 1; ++it)
			it->setNextSprite(&*(it + 1));
	}
	//Else there are no need in relinking
}

void CardPile::updateSprites()
{
	sprites.clear();
	sprites.reserve(deque.size());
	for (auto&& c : deque)
		sprites.emplace_back(c);

}

CardSprite* CardPile::getSpriteFromPosition(int x, int y)
{
	auto it = std::find_if(sprites.rbegin(), sprites.rend(), [&](CardSprite& sprite)
	{
		return !sprite.getCard().isFlipped() && sprite.getGlobalBounds().contains(x, y);
	});

	if (it != sprites.rend())
		return &*it;

	return nullptr;
}

CardPile::CardPile(size_t startCount)
{
	auto&& pack = Game::getGameInstance().getCardPack();
	std::move(pack.rbegin(), pack.rbegin() + startCount, std::back_inserter(deque));
	pack.resize(pack.size() - startCount);

	sprites.reserve(deque.size());
	for (auto&& c : deque)
	{
		c.flip();
		sprites.emplace_back(c);
	}

	deque.back().flip();
	updateLinks();
}

void CardPile::setPosition(float x, float y)
{
	dropArea.top = y;
	dropArea.left = x;

	bNeedsRealign = true;
}

bool CardPile::canCardBeDropped(CardSprite& card)
{
	if (!canAcceptCards())
		return false;
	
	if (deque.empty())
		return card.getCard().getRankAsEnum() == Card::ECardRank::King;

	auto comp = getTopCard()->comparePosition(card.getCard());
	return comp == ECardPosition::After && getSize() + card.getChildCount() <= 13;
}

void CardPile::eventReceive(sf::Event& event)
{
	IDraggable::eventReceive(event);

	if (!sprites.empty())
	{
		if (event.type == sf::Event::MouseButtonPressed)
		{
			auto&& topCard = sprites.back();

			if (topCard.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
			{
				if (topCard.getCard().isFlipped() && event.mouseButton.button == sf::Mouse::Left)
				{
					topCard.getCard().flip();

					Logger::get().log("Flipping card, opened %, %",
						topCard.getCard().getRank(),
						suitAsStr(topCard.getCard())
					);

					Game::playSound(Game::pickSound);
				}
				else if (!topCard.getCard().isFlipped() && event.mouseButton.button == sf::Mouse::Right)
				{
					TargetPile* pile = Game::getGameInstance().getMatchingTargetPile(topCard.getCard());
					if (pile)
					{
						Logger::get().log("Performing fast move with %, % to %, %",
							topCard.getCard().getRank(), suitAsStr(topCard.getCard()),
							pile->getTopCard()->getRank(), suitAsStr(*pile->getTopCard())
						);
						
						moveTo(*pile, 1);
					}
				}
			}
		}
	}
}


bool CardPile::checkCollision(const sf::FloatRect& rect)
{
	if (deque.empty())
		return dropArea.intersects(rect);

	return sprites.back().getGlobalBounds().intersects(rect);
}

void CardPile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	realign();

	if (!sprites.empty())
	{
		target.draw(sprites.front(), states);
	}
}

/*void CardPile::eventReceive(sf::Event& event)
{
	auto&& mb = event.mouseButton;
	auto&& mm = event.mouseMove;
	
	//Detect drag
	if (!DragDropManager::isDragging())
	{
		if (event.type == sf::Event::MouseButtonPressed && mb.button == sf::Mouse::Left)
		{
			if (!deque.empty())
			{
				if (getTopCard()->isFlipped())
				{
					getTopCard()->flip();
				}
				else
				{
					auto it = std::find_if(sprites.rbegin(), sprites.rend(), [&](CardSprite& sprite)
					{
						return !sprite.getCard().isFlipped() && sprite.getGlobalBounds().contains(mb.x, mb.y);
					});

					if (it != sprites.rend())
						DragDropManager::get().enterDrag(*it, *this);
				}
			}
		}
		else if (DragDropManager::isCaptured())
		{
			if (event.type == sf::Event::MouseButtonReleased && mb.button == sf::Mouse::Left)
				DragDropManager::get().exitDrag();
			
			else if (event.type == sf::Event::MouseMoved)
				DragDropManager::get().beginDrag(mm);
		}
	}
	else
	{
		if (event.type == sf::Event::MouseButtonReleased && mb.button == sf::Mouse::Left)
		{
			DragDropManager::get().drop(mb);
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			DragDropManager::get().drag(mm);
		}
	}
}*/

CardSprite* TargetPile::getSpriteFromPosition(int x, int y)
{
	if (topCard.getGlobalBounds().contains(x, y))
		return &topCard;

	return nullptr;
}

TargetPile::TargetPile()
{
	emptySprite.setTexture(TextureManager::get().getTargetPileTexture());
	emptySprite.setPosition(dropArea.left, dropArea.top);
	emptySprite.setColor({ 20, 40, 60 });
}

sf::Sprite* TargetPile::getTopSprite() const
{
	return topCard.getSprite();
}

bool TargetPile::canCardBeDropped(CardSprite& card)
{
	if (card.getChildCount() > 1 || !canAcceptCards())
		return false;

	if (deque.empty())
		return card.getCard().getRank() == 1;

	if (card.getCard().getSuit() == getTopCard()->getSuit())
	{
		auto comp = getTopCard()->compareRanks(card.getCard());
		return comp == ECardPosition::Before && getSize() + 1 <= 13;
	}

	return false;
}

void TargetPile::setPosition(float x, float y)
{
	topCard.setPosition(x, y);
	secondCard.setPosition(x, y);
	emptySprite.setPosition(x, y);
	
	dropArea.left = x;
	dropArea.top = y;
}

void TargetPile::onMove()
{
	if (!deque.empty())
	{
		topCard.setCard(deque.back());

		if (deque.size() > 1)
			secondCard.setCard(*(deque.rbegin() + 1));

		topCard.setPosition({ dropArea.left, dropArea.top });
		secondCard.setPosition({ dropArea.left, dropArea.top });
	}
	else
	{
		topCard.setCard(nullptr);
		secondCard.setCard(nullptr);
	}
}

void TargetPile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (!deque.empty())
	{
		if (deque.size() > 1)
			target.draw(secondCard, states);
		else
			target.draw(emptySprite);
		
		target.draw(topCard, states);
	}
	else
	{
		target.draw(emptySprite, states);
	}
}

CardSprite* SourcePile::getSpriteFromPosition(int x, int y)
{
	if (portion.top() && portion.top()->getGlobalBounds().contains(x, y))
	{
		Logger::get().log("Top card of portion is %, %",
			portion.top()->getCard().getRank(), suitAsStr(portion.top()->getCard())
		);

		return portion.top();
	}

	return nullptr;
}

void SourcePile::onMove()
{
	--nextIdx;
	portion.next();
}

void SourcePile::updateSprites()
{
	if (portion.top())
	{
		for (int i = 0; i < portion.getIdx(); ++i)
		{
			auto&& card = deque[startIdx + i];
			portion[i].setCard(card);

			Logger::get().log("Setting % card of portion to %, %",
				i, card.getRank(), suitAsStr(card)
			);
		}

		for (int i = portion.getIdx(); i < 3; ++i)
		{
			portion[i].setCard(nullptr);

			Logger::get().log("Nulling % card in portion", i);
		}
	}
}

void SourcePile::updatePositions()
{
	for (int i = 0; i < 3; ++i)
	{
		portion[i].setPosition(dropArea.left + 100 + 20*i, dropArea.top);
	}
}

SourcePile::SourcePile()
{
	updatePositions();
	updateSprites();

	topSprite.setTexture(TextureManager::get().getEmptyPileTexture());
	topSprite.setColor({ 20, 40, 60 });
}

void SourcePile::moveTo(ICardPlace& newSource, size_t count)
{
	if (newSource.canAcceptCards())
	{
		std::move(deque.begin() + startIdx + portion.getIdx() - 1, deque.begin() + startIdx + portion.getIdx(), std::back_inserter(newSource.getDeque()));
		deque.erase(deque.begin() + startIdx + portion.getIdx() - 1);
		
		onMove();
		newSource.onMove();


		updateSprites();
	}
}

void SourcePile::pushCards(CardDeque& deque)
{
	std::move(deque.rbegin(), deque.rend(), std::back_inserter(this->deque));
	deque.clear();
}

void SourcePile::eventReceive(sf::Event& event)
{
	IDraggable::eventReceive(event);

	if (!deque.empty())
	{
		if (event.type == sf::Event::MouseButtonPressed &&
			event.mouseButton.button == sf::Mouse::Left &&
			dropArea.contains(event.mouseButton.x, event.mouseButton.y)
			)
		{
			startIdx = nextIdx;
			nextIdx = (nextIdx + 3) > deque.size() ? deque.size() : nextIdx + 3;

			Logger::get().log("Range indexes now are [%:%)", startIdx, nextIdx);
			
			if (startIdx == deque.size())
			{
				startIdx = 0;
				nextIdx = 0;
				portion.reset();

				Logger::get().log("Found end of pile, resetting");
			}
			else
			{
				
			}

			portion.reset(nextIdx - startIdx);
			updateSprites();
			updatePositions();

			Game::playSound(Game::flipSound);
		}
		else if (event.type == sf::Event::MouseButtonPressed &&
			event.mouseButton.button == sf::Mouse::Right &&
			portion.top() && 
			portion.top()->getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)
			)
		{
			TargetPile* targetPile = Game::getGameInstance().getMatchingTargetPile(portion.top()->getCard());
			if (targetPile)
			{
				Logger::get().log("Performing fast move with %, % to %, %",
					portion.top()->getCard().getRank(), suitAsStr(portion.top()->getCard()),
					targetPile->getTopCard()->getRank(), suitAsStr(*targetPile->getTopCard())
				);
				
				moveTo(*targetPile, 1);
			}
		}
	}
}

void SourcePile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (nextIdx + 1 > deque.size())
	{
		target.draw(topSprite, states);
	}
	else
	{
		Card c{ 1, Card::ECardSuit::Hearts };
		c.block();
		c.flip();
		CardSprite spr(c);
		spr.setPosition(dropArea.left, dropArea.top);

		target.draw(spr, states);
	}

	for (int i = 0; i < portion.getIdx(); ++i)
		target.draw(portion[i]);
}

void SourcePile::setPosition(float x, float y)
{
	dropArea.left = x;
	dropArea.top = y;

	topSprite.setPosition(dropArea.left, dropArea.top);

	updatePositions();
}
