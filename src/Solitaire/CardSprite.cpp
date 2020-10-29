#include "CardSprite.h"
#include "Card.h"
#include "TextureManager.h"

#include "SFML/Graphics/RenderTarget.hpp"

#include <cassert>

#include "Game.h"

bool CardSprite::isValid() const
{
	return card;
}

void CardSprite::setPosition(float x, float y)
{
	cardSprite.setPosition(x, y);

	if (nextSprite)
	{
		if (card->isFlipped())
			nextSprite->setPosition(x, y + cardOffset);
		else
			nextSprite->setPosition(x, y + cardOffset + 10);
	}
}

void CardSprite::setPosition(const sf::Vector2f& pos)
{
	setPosition(pos.x, pos.y);
}

sf::FloatRect CardSprite::getGlobalBounds() const
{
	return cardSprite.getGlobalBounds();
}

sf::Vector2f CardSprite::getPosition() const
{
	return cardSprite.getPosition();
}

void CardSprite::updateSprite() const
{
	if (isValid() && (bNeedsUpdate || card->bDirty))
	{
		if (card->isFlipped())
		{
			cardSprite.setTexture(TextureManager::get().getCardSuitAtlas());
			cardSprite.setTextureRect({ 0,0,(int)CARD_WIDTH, (int)CARD_HEIGHT });

			cardShadow.setTextureRect(cardSprite.getTextureRect());
			cardShadow.setTexture(*cardSprite.getTexture());
		}
		else
		{
			cardSprite.setTexture(TextureManager::get().getCardsAtlas());

			auto xPos = static_cast<int>((card->getRank() - 1) * CARD_WIDTH);
			auto yPos = static_cast<int>(card->getSuit()) * static_cast<int>(CARD_HEIGHT);

			cardSprite.setTextureRect({ xPos, yPos, (int)CARD_WIDTH, (int)CARD_HEIGHT });

			cardShadow.setTextureRect(cardSprite.getTextureRect());
			cardShadow.setTexture(*cardSprite.getTexture());
		}


		bNeedsUpdate = false;
		card->bDirty = false;
	}
}

CardSprite::CardSprite(Card& card):
card(&card)
{	
	updateSprite();

	cardShadow.setColor({ 255,255,255,120 });
}

CardSprite::CardSprite():
card(nullptr)
{
}

void CardSprite::setCard(Card& card)
{
	if (&card != this->card)
	{
		this->card = &card;
		bNeedsUpdate = true;
	}
}

void CardSprite::setCard(Card* card)
{
	if (card)
		setCard(*card);
	else
		this->card = nullptr;
}

Card& CardSprite::getCard() const
{
	assert(isValid() && "Trying to get card in invalid sprite");
	return *card;
}

sf::Sprite* CardSprite::getSprite() const
{
	return isValid() ? &cardSprite : nullptr;
}

void CardSprite::setNextSprite(CardSprite* sprite)
{
	if (sprite != nextSprite)
	{
		if (sprite == nullptr)
		{
			auto next = nextSprite->getNextSprite();
			while (next)
			{
				auto tmp= next->getNextSprite();
				next->setNextSprite(nullptr);
				next = tmp;
			}
		}

		nextSprite = sprite;
		bNeedsRecalculate = true;
	}
}

CardSprite* CardSprite::getNextSprite() const
{
	return nextSprite;
}

size_t CardSprite::getChildCount() const
{
	if (bNeedsRecalculate)
	{
		size_t count = 0;
		auto next = getNextSprite();
		while (next)
		{
			++count;
			next = next->getNextSprite();
		}

		cachedCount = count;
		bNeedsRecalculate = false;
	}

	return cachedCount;
}

void CardSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isValid())
	{
		updateSprite();

		target.draw(cardSprite, states);

		if (nextSprite)
			nextSprite->draw(target, states);
	}
}
