#pragma once
#include <SFML/Graphics/Texture.hpp>

class TextureManager
{

	TextureManager();

	sf::Texture cardsAtlas;
	sf::Texture cardSuitAtlas;
	sf::Texture cardEmpty;
	sf::Texture targetEmpty;
	
public:

	//Get singleton
	static TextureManager& get();

	const sf::Texture& getCardsAtlas() const;
	const sf::Texture& getCardSuitAtlas() const;
	const sf::Texture& getEmptyPileTexture() const;
	const sf::Texture& getTargetPileTexture() const;
};
