#include "Card.h"

#ifdef _DEBUG

#include <iostream>

#endif

#include <utility>

unsigned Card::getRank() const
{
	return static_cast<unsigned>(data & 15);
}

Card::Card():
data(0) {}

Card::Card(ECardRank rank, ECardSuit suit):
data(static_cast<unsigned char>(rank) | static_cast<unsigned char>(suit) << 4)
{
#if _DEBUG
	_Rank = (unsigned)rank;
	_Suit = (unsigned)suit;
#endif
}

Card::Card(unsigned rank, ECardSuit suit):
data(static_cast<unsigned char>(rank) | static_cast<unsigned char>(suit) << 4)
{
#if _DEBUG
	_Rank = rank;
	_Suit = (unsigned)suit;
#endif
}

Card::ECardSuit Card::getSuit() const
{
	return static_cast<ECardSuit>((data & 48) >> 4);
}

Card::ECardRank Card::getRankAsEnum() const
{
	return static_cast<ECardRank>(getRank());
}

void Card::flip()
{
	data = isFlipped() ? data & 127 : data | ~127;
	bDirty = true;
}

void Card::block()
{
	data = isBlocked() ? data & 191 : data | ~191;
}

bool Card::isBlocked() const
{
	return static_cast<bool>(data >> 6 & 1);
}

bool Card::isFlipped() const
{
	return static_cast<bool>(data >> 7);
}

ECardPosition Card::comparePosition(const Card& to) const
{
	auto thisSuitBit = (bool)(static_cast<unsigned char>(getSuit()) >> 1);
	auto thatSuitBit = (bool)(static_cast<unsigned char>(to.getSuit()) >> 1);

	if (thisSuitBit == thatSuitBit)
		return ECardPosition::NoPosition;

	return compareRanks(to);
}

ECardPosition Card::compareRanks(const Card& to) const
{
	auto thisRank = getRank();
	auto thatRank = to.getRank();

	const auto delta = thisRank < thatRank ? thatRank - thisRank : thisRank - thatRank;
	if (delta != 1)
		return ECardPosition::NoPosition;

	if (thisRank < thatRank)
		return ECardPosition::Before;
	if (thisRank > thatRank)
		return ECardPosition::After;

	return ECardPosition::NoPosition;
}

#ifdef _DEBUG
void Card::printInfo() const
{
	auto rank = getRank();
	auto suit = getSuit();

	std::cout << "Card info: rank ";
	
	if (rank == 1)
		std::cout << "ace";
	else
	if (rank == 11)
		std::cout << "jack";
	else
	if (rank == 12)
		std::cout << "queen";
	else
	if (rank == 13)
		std::cout << "king";
	else
		std::cout << rank;

	std::cout << ", suit ";

	if (suit == ECardSuit::Clubs)
		std::cout << "clubs";

	if (suit == ECardSuit::Diamonds)
		std::cout << "diamonds";

	if (suit == ECardSuit::Hearts)
		std::cout << "hearts";

	if (suit == ECardSuit::Spades)
		std::cout << "spades";

	std::cout << std::boolalpha << ", flipped: " << isFlipped() << ", blocked: " << isBlocked() << std::noboolalpha << '\n';
}

void Card::runTests()
{
	std::cout << "Testing card\n";

	Card c{ ECardRank::Ace, ECardSuit::Hearts };
	c.printInfo();

	std::cout << "Blocking card\n";
	c.block();
	c.printInfo();

	std::cout << "Flipping card\n";
	c.flip();
	c.printInfo();

	std::cout << "Unblocking card\n";
	c.block();
	c.printInfo();

	std::cout << "Blocking and unflipping card\n";
	c.block();
	c.flip();
	c.printInfo();

	std::cout << "Unblocking card, must return to first state\n";
	c.block();
	c.printInfo();
}
#endif

ECardPosition comparePosition(const Card& first, const Card& second)
{
	return first.comparePosition(second);
}

const char* suitAsStr(const Card& c)
{
	switch (c.getSuit())
	{
	case Card::ECardSuit::Hearts: return "Hearts";
	case Card::ECardSuit::Diamonds: return "Diamonds";
	case Card::ECardSuit::Spades: return "Spades";
	case Card::ECardSuit::Clubs: return "Clubs";
	default: return "";
	}
}
