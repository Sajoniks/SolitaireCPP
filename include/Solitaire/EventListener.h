#pragma once
#include <vector>
#include "SFML/Window/Event.hpp"

namespace sf {
	class Event;
}

class EventListener
{
public:

	virtual void eventReceive(sf::Event& event) = 0;
	virtual ~EventListener() = default;
};

class EventBus
{

	std::vector<EventListener*> listeners;

	EventBus() = default;
	
public:

	static EventBus& get();

	void subscribe(EventListener& listener);
	void subscribe(const std::initializer_list<EventListener*>& listeners);

	void clearListeners();

	void send(sf::Event& event);
};
