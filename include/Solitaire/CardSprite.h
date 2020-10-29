#pragma once

#include "EventListener.h"
#include "SFML/Graphics/Sprite.hpp"

class Card;

constexpr size_t CARD_WIDTH = 80;
constexpr size_t CARD_HEIGHT = 120;

class CardSprite : public sf::Drawable
{

	mutable sf::Sprite cardSprite;
	mutable sf::Sprite cardShadow;
	
	Card* card{ nullptr };
	CardSprite* nextSprite{ nullptr };

	mutable bool bNeedsUpdate{ true };
	mutable bool bNeedsRecalculate{ false };
	mutable size_t cachedCount{ 0 };

	const size_t cardOffset = 10;
	
	void updateSprite() const;
	
public:

	explicit CardSprite(Card& card);
	CardSprite();

	void setCard(Card& card);
	void setCard(Card* card);
	
	Card& getCard() const;

	sf::Sprite* getSprite() const;

	void setNextSprite(CardSprite* sprite);
	CardSprite* getNextSprite() const;

	size_t getChildCount() const;
	
	bool isValid() const;

	void setPosition(float x, float y);
	void setPosition(const sf::Vector2f& pos);

	sf::FloatRect getGlobalBounds() const;
	sf::Vector2f getPosition() const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};