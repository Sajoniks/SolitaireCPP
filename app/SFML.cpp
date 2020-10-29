
#include <iostream>
#include <queue>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <utility>

#include "Game.h"
#include "TextureManager.h"

/*class AnimationBase
{
	
};

class AnimationProcedural : public AnimationBase
{
	
};

class Animation : public AnimationBase
{
	struct AnimationTrack
	{
		template <class T>
		struct Keyframe
		{
			T data;
			float time;

			bool operator<(const Keyframe& k) const
			{
				return time < k.time;
			}
		};

		using VectorKeyframe = Keyframe<sf::Vector2f>;
		using FloatKeyframe = Keyframe<float>;
		
		std::vector<VectorKeyframe> posTrack;
		std::vector<VectorKeyframe> scaleTrack;
		std::vector<FloatKeyframe> rotationTrack;

		void sortKeys()
		{
			std::stable_sort(posTrack.begin(), posTrack.end());
			std::stable_sort(scaleTrack.begin(), scaleTrack.end());
			std::stable_sort(rotationTrack.begin(), rotationTrack.end());
		}

	} track;

	using VKey = AnimationTrack::VectorKeyframe;
	using FKey = AnimationTrack::FloatKeyframe;

	float currentTime{ 0.f };

	sf::Vector2f prevPosValue;
	sf::Vector2f prevScaleValue;
	float prevRotationValue{ 0.f };
	
public:
	
	void pushPositionKey(const VKey& key)
	{
		track.posTrack.push_back(key);
		track.sortKeys();
	}
	
	void pushRotationKey(const FKey& key)
	{
		track.rotationTrack.push_back(key);
		track.sortKeys();
	}
	
	void pushScaleKey(const VKey& key)
	{
		track.scaleTrack.push_back(key);
		track.sortKeys();
	}

	void update(sf::Transform& t, float deltaTick)
	{
		
	}

	bool isFinished() const { return false; }
};

class Animatable
{

	std::queue<Animation> anims;
	sf::Transform transform;

public:

	const sf::Transform& getTransform() const {
		return transform;
	}

	void playAnimation(const Animation& animation)
	{
		if (anims.empty() || (!anims.empty() && &anims.back() != &animation))
		{
			anims.push(animation);
		}
	}

	void tickUpdate(float deltaTick)
	{
		if (!anims.empty())
		{
			if (anims.front().isFinished())
				anims.pop();
			else
				anims.front().update(transform, deltaTick);
		}
	}
};

class AnimationDecorator : public Animatable, public sf::Drawable
{
	Drawable* wrappee;
	
public:

	AnimationDecorator(Drawable& drawable) :
		wrappee(&drawable) {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= getTransform();
		target.draw(*wrappee, states);
	}
};*/

class RotationMatrix
{
	sf::Transform tf;

public:
	
	RotationMatrix(float a11, float a12, float a13, float a21, float a22, float a23, float a31, float a32, float a33):
		tf(
			a11, a12, a13,
			a21, a22, a23,
			a31, a32, a33
		)
	{
		
	}

	RotationMatrix():
		tf() {}

	const sf::Transform& getTransform() const { return tf; }

	static RotationMatrix xRotationMatrix(float angle)
	{
		const float sin = std::sin(angle);
		const float cos = std::cos(angle);
		
		return RotationMatrix(
			1, 0, 0,
			0, cos, -sin,
			0, sin, cos
		);
	}

	static RotationMatrix yRotationMatrix(float angle)
	{
		const float sin = std::sin(angle);
		const float cos = std::cos(angle);
		
		return RotationMatrix(
			cos, 0, sin,
			0, 1, 0,
			-sin, 0, cos
		);
	}

	static RotationMatrix zRotationMatrix(float angle)
	{
		const float sin = std::sin(angle);
		const float cos = std::cos(angle);
		
		return RotationMatrix(
			cos, -sin, 0,
			sin, cos, 0,
			0, 0, 1
		);
	}
};

class Wrapper: public sf::Drawable
{
	sf::Sprite spr;

	RotationMatrix m;
	
public:

	Wrapper(sf::Sprite sprite):
		spr(std::move(sprite))
	{
		m = RotationMatrix::yRotationMatrix(30);
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.transform *= m.getTransform();
		target.draw(spr, states);
	}
};

int main()
{
	//Game::getGameInstance().start();

	sf::RenderWindow rw({ 800,600 }, "Features");
	sf::Sprite s(TextureManager::get().getCardSuitAtlas());

	Wrapper w(s);
	
	sf::Clock c;

	while (rw.isOpen())
	{
		
		c.restart();
		
		sf::Event e;
		while (rw.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				rw.close();
		}

		rw.clear();

		rw.draw(s);
		
		rw.display();
	}
	
}
