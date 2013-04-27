#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

const int width = 800;
const int height = 800;
const float stepping = 250.0;

class Player {
	sf::CircleShape self;
	sf::CircleShape sword;

	sf::Keyboard::Key action, wiggle;

	bool leaping, swinging;
	bool firstPlayer;

	public:
	Player(bool firstPlayer)
		: self(10)
		, sword(3)
		, leaping(false)
		, swinging(false)
		, firstPlayer(firstPlayer)
	{
		reset();
	}

	sf::FloatRect swordBound() {
		return sword.getGlobalBounds();
	}

	sf::FloatRect selfBound() {
		return self.getGlobalBounds();
	}

	void reset()
	{
		float pHeight = selfBound().height;
		float sHeight = swordBound().height;

		int selfYCoord = height/2-pHeight/2;
		int swordYCoord = height/2-sHeight/2;

		if ( firstPlayer )
		{
			self.setPosition(width/4,selfYCoord);
			sword.setPosition(width/4,swordYCoord);

			self.setFillColor(sf::Color(250, 218, 221));
			sword.setFillColor(sf::Color(111,111,111));
		}
		else 
		{
			self.setPosition(3*width/4,selfYCoord);
			sword.setPosition(3*width/4+pHeight-sHeight,swordYCoord);

			self.setFillColor(sf::Color(239, 48, 36));
			sword.setFillColor(sf::Color(111,111,111));
		}
		leaping = false;
		swinging = false;
	}

	void act() {
		if ( !leaping )
			leaping = true;
		else
			swinging = true;
	}

	void tick(int64_t dt) {
		float step = 0.0;
		if ( leaping )
		{
			if ( firstPlayer )
			{
				if ( self.getPosition().x < width )
				{
					step = stepping*dt/1000;
				}
			}
			else
			{
				if ( self.getPosition().x > 0 )
				{
					step = -1*stepping*dt/1000;
				}
			}
		}
		self.move(step,0);
		sword.move(step,0);
	}

	void draw(sf::RenderWindow& w)
	{
		w.draw(self);
		w.draw(sword);

	}
};

int main() {
	sf::RenderWindow window(sf::VideoMode(width, height), "mini samurai");
	sf::Clock clock;

	window.clear();

	Player f(true), s(false);

	while (window.isOpen())
	{
		sf::Event e;
		int64_t dt = clock.getElapsedTime().asMilliseconds();
		while ( window.pollEvent(e) )
		{
				if ( e.type == sf::Event::Closed )
				{
					return 0;
				}
				if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape ) ) 
				{
					return 0;
				}
				if ( sf::Keyboard::isKeyPressed(sf::Keyboard::A) ) 
					f.act();
				if ( sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ) 
					s.act();
				if ( sf::Keyboard::isKeyPressed(sf::Keyboard::R) )
				{
					// RESET
					s.reset();
					f.reset();
					window.clear();
				}

		}
		s.tick(dt);
		f.tick(dt);
		window.clear();

		f.draw(window);
		s.draw(window);
		clock.restart();

		window.display();
	}
}
