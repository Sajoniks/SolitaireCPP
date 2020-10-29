#pragma once
#include <deque>
#include <stack>


#include "Card.h"
#include "CardSprite.h"
#include "EventListener.h"

struct TraceResult
{
	TraceResult() = default;
	
	bool bHit;
	CardSprite* sprite;
};

class ICardPlace
{
protected:
	
	using CardDeque = std::deque<Card>;
	CardDeque deque;

	bool bBlocked{};
	
	virtual void popCards(size_t count = 1);

public:

	virtual void onMove() {}
	CardDeque& getDeque() { return deque; }
	
	virtual void moveTo(ICardPlace& newSource, size_t count = 1);
	Card* getTopCard();
	virtual bool canAcceptCards() const { return false; }
	virtual bool canCardBeDropped(CardSprite& card) { return false; }
	virtual void setPosition(float x, float y) = 0;
	size_t getSize() const;

	void setBlocked(bool bBlocked);
	bool isBlocked() const;
	
	virtual ~ICardPlace() = default;
};

//Card place from which you can drag and drop the cards
class IDraggable : public ICardPlace, public EventListener
{

protected:

	sf::FloatRect dropArea{ 0,0, CARD_WIDTH, CARD_HEIGHT };
	virtual CardSprite* getSpriteFromPosition(int x, int y) = 0;
	
public:
	void eventReceive(sf::Event& event) override;
	virtual bool checkCollision(int x, int y) const;
	virtual bool checkCollision(const sf::FloatRect& rect);
};

class CardPile : public sf::Drawable, public IDraggable
{

	mutable std::vector<CardSprite> sprites;
	
	mutable bool bNeedsRealign{ true };
	void realign() const;
	void onMove() override;
	void updateLinks();
	void updateSprites();

	CardSprite* getSpriteFromPosition(int x, int y) override;
	
public:

	explicit CardPile(size_t startCount);
	bool canAcceptCards() const override { return deque.size() < 13; }
	void setPosition(float x, float y) override;
	bool checkCollision(const sf::FloatRect& rect) override;
	bool canCardBeDropped(CardSprite& card) override;
	void eventReceive(sf::Event& event) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class TargetPile : public IDraggable, public sf::Drawable
{

	CardSprite topCard{};
	CardSprite secondCard{};

	sf::Sprite emptySprite;
	
	CardSprite* getSpriteFromPosition(int x, int y) override;
	
public:

	TargetPile();

	sf::Sprite* getTopSprite() const;
	
	bool canAcceptCards() const override { return deque.size() < 13; }
	bool canCardBeDropped(CardSprite& card) override;
	void setPosition(float x, float y) override;
	void onMove() override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class SourcePile : public sf::Drawable, public IDraggable
{
	size_t nextIdx{ 0 };
	size_t startIdx{ 0 };
	
	CardSprite* getSpriteFromPosition(int x, int y) override;
	
	void onMove() override;
	void updateSprites();
	void updatePositions();

	sf::Sprite topSprite;

	mutable struct CardPortion
	{
	private:
		int pickupIdx{ -1 };
		CardSprite sprites[3];

	public:
		void next()
		{
			sprites[pickupIdx - 1].setCard(nullptr);
			--pickupIdx;

			if (pickupIdx < 0)
				pickupIdx = -1;
		}

		void reset(int idx = -1) { pickupIdx = idx; }

		int getIdx() const { return pickupIdx; }

		CardSprite* top()
		{
			if (pickupIdx != -1)
				return &sprites[pickupIdx - 1];

			return nullptr;
		}
		
		CardSprite& operator[](size_t i) { return sprites[i]; }

	} portion;
	
public:

	SourcePile();

	void moveTo(ICardPlace& newSource, size_t count) override;
	void pushCards(CardDeque& deque);
	void eventReceive(sf::Event& event) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setPosition(float x, float y) override;
};