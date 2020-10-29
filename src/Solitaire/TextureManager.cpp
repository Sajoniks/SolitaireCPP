#include "TextureManager.h"

TextureManager::TextureManager()
{
	cardsAtlas.loadFromFile("card_atlas.png");
	cardSuitAtlas.loadFromFile("card_suits.png");
	cardEmpty.loadFromFile("card_empty.png");
	targetEmpty.loadFromFile("card_target.png");
}

TextureManager& TextureManager::get()
{
	static TextureManager INSTANCE;
	return INSTANCE;
}

const sf::Texture& TextureManager::getCardsAtlas() const
{
	return cardsAtlas;
}

const sf::Texture& TextureManager::getCardSuitAtlas() const
{
	return cardSuitAtlas;
}

const sf::Texture& TextureManager::getEmptyPileTexture() const
{
	return cardEmpty;
}

const sf::Texture& TextureManager::getTargetPileTexture() const
{
	return targetEmpty;
}
