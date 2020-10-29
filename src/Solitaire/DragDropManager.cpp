#include "DragDropManager.h"

#include <iostream>

#include "CardSprite.h"
#include "Game.h"
#include "Logger.h"

bool DragDropManager::bDrag{ false };
bool DragDropManager::bCaptured{ false };

DragDropManager& DragDropManager::get()
{
	static DragDropManager INSTANCE;
	return INSTANCE;
}

void DragDropManager::enterDrag(CardSprite& object, ICardPlace& source)
{
	this->dragObject = &object;
	this->dragSource = &source;
	bCaptured = true;

	Logger::get().log("Captured card with rank % and suit %", dragObject->getCard().getRank(), suitAsStr(dragObject->getCard()));
}

void DragDropManager::beginDrag(const sf::Event::MouseMoveEvent& moveEvent)
{
	if (bCaptured)
	{
		startPosition = { (float)moveEvent.x, (float)moveEvent.y };
		auto objPos = dragObject->getPosition();
		cursorOffset = { moveEvent.x - objPos.x, moveEvent.y - objPos.y };
		
		bDrag = true;

		Game::getGameInstance().setPriorityObject(dragObject);
		Game::playSound(Game::pickSound);
		dragSource->setBlocked(true);
	}
}

void DragDropManager::exitDrag()
{
	bCaptured = false;
}

void DragDropManager::drag(const sf::Event::MouseMoveEvent& buttonEvent)
{
	if (bCaptured)
	{
		dragObject->setPosition(buttonEvent.x - cursorOffset.x, buttonEvent.y - cursorOffset.y);
	}
}

void DragDropManager::drop(const sf::Event::MouseButtonEvent& buttonEvent)
{
	ICardPlace* place = Game::getGameInstance().getCardSourceFromArea(dragObject->getGlobalBounds());
	
	if (place && place != dragSource && place->canCardBeDropped(*dragObject))
	{
		if (place->getSize())
		{
			Logger::get().log("Dropped card %, % onto card %, %",
				dragObject->getCard().getRank(),
				suitAsStr(dragObject->getCard()),
				place->getTopCard()->getRank(),
				suitAsStr(*place->getTopCard())
			);
		}
		else
		{
			Logger::get().log("Dropped card %, % onto empty pile", 
				dragObject->getCard().getRank(), 
				suitAsStr(dragObject->getCard())
			);
		}
		
		dragSource->moveTo(*place, dragObject->getChildCount() + 1);

		Game::playSound(Game::dropSound);
	}
	else
	{
		dragObject->setPosition(startPosition - cursorOffset);

		Logger::get().log("Failed to drop card with rank % and suit %", 
			dragObject->getCard().getRank(), 
			suitAsStr(dragObject->getCard())
		);

		Game::playSound(Game::badDropSound);
	}

	Game::getGameInstance().setPriorityObject(nullptr);
	dragSource->setBlocked(false);
	dragSource = nullptr;
	dragObject = nullptr;
	bDrag = false;
}
