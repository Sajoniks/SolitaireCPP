#pragma once

#include <vector>


#include "CardPile.h"
#include "SFML/Window/Event.hpp"

class DragDropManager
{

	class CardSprite* dragObject;
	class ICardPlace* dragSource;

	sf::Vector2f startPosition;
	sf::Vector2f cursorOffset;
	
	static bool bDrag;
	static bool bCaptured;
	
public:

	static DragDropManager& get();
	static bool isDragging() { return bDrag; }
	static bool isCaptured() { return bCaptured; }

	void enterDrag(CardSprite& object, ICardPlace& source);
	void beginDrag(const sf::Event::MouseMoveEvent& moveEvent);
	void exitDrag();

	void drag(const sf::Event::MouseMoveEvent& buttonEvent);
	void drop(const sf::Event::MouseButtonEvent& buttonEvent);
};
