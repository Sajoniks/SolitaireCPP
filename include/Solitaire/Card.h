#pragma once

enum class ECardPosition : unsigned char
{
	Before,
	After,
	NoPosition
};

class Card
{
	//Encoded data
	//0|0|00|0000
	//First bit is
	//Second bit is
	//Next two bits are suit
	//Last four bits are rank
	unsigned char data;

	//Flag if card was updated
	//Used in card sprite to update sprite texture
	bool bDirty{ false };

	friend class CardSprite;
	friend ECardPosition comparePosition(const Card& first, const Card& second);

#if _DEBUG

	unsigned _Rank;
	unsigned _Suit;
	
#endif
	
public:

	enum class ECardRank : unsigned char
	{
		Ace = 1,
		Jack = 11,
		Queen,
		King
	};

	enum class ECardSuit : unsigned char
	{
		Hearts = 0,
		Diamonds,
		Spades,
		Clubs
	};

	Card();
	Card(ECardRank rank, ECardSuit suit);
	Card(unsigned rank, ECardSuit suit);

	ECardSuit getSuit() const;

	unsigned getRank() const;
	ECardRank getRankAsEnum() const;
	
	inline void flip();
	inline void block();

	inline bool isBlocked() const;
	inline bool isFlipped() const;

	/** Compare position of this card to another card
	 * @param to card to compare this card
	 * @return One of the positions
	 * @note ECardPosition::After - if this card's rank is higher than comparable - it orders before comparable card - comparable card lays after
	 * @note ECardPosition::Before - if this card's rank is lower than comparable - it orders after comparable card - comparable card lays before
	 * @note ECardPosition::NoPosition - cards cannot be compared
	 */
	ECardPosition comparePosition(const Card& to) const;

	ECardPosition compareRanks(const Card& to) const;


#ifdef _DEBUG

	void printInfo() const;
	static void runTests();
	
#endif
};

ECardPosition comparePosition(const Card& first, const Card& second);
const char* suitAsStr(const Card& c);