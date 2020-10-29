#include "EventListener.h"
#include "CardPile.h"
#include "CardSprite.h"

#include <iostream>

#include "Game.h"

EventBus& EventBus::get()
{
	static EventBus BUS;
	return BUS;
}

void EventBus::subscribe(EventListener& listener)
{
	if (std::find(listeners.begin(), listeners.end(), &listener) == listeners.end())
		listeners.push_back(&listener);
}

void EventBus::subscribe(const std::initializer_list<EventListener*>& listeners)
{
	for (auto&& l : listeners)
		subscribe(*l);
}

void EventBus::clearListeners()
{
	listeners.clear();
}

void EventBus::send(sf::Event& event)
{
	for (auto&& l : listeners)
		l->eventReceive(event);
}
