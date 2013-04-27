#include <cstdio>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

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
		if ( firstPlayer )
		{
			self.setPosition(10,10);
			sword.setPosition(10,11);

			self.setFillColor(sf::Color(250, 218, 221));
			sword.setFillColor(sf::Color(111,111,111));
		}
		else 
		{
			self.setPosition(200,10);
			sword.setPosition(200,11);

			self.setFillColor(sf::Color(239, 48, 36));
			sword.setFillColor(sf::Color(111,111,111));
		}
	}

	void leap() {
		leaping = true;
	}

	void swing() {
		swinging = true;
	}

	void tick(int64_t dt) {
		float stepping = 20.0; // 20 px per second
		if ( leaping )
		{
			if ( firstPlayer )
				self.move(stepping*dt/1000,0);
			else
				self.move(-1*stepping*dt/1000,0);
		}
	}

	void draw(sf::RenderWindow& w)
	{
		w.draw(self);
		w.draw(sword);

	}
};

int main() {
	printf("Hello world\n");
	sf::RenderWindow window(sf::VideoMode(800, 600), "mini samurai");
	sf::Clock clock;

	window.clear();

	Player f(true), s(false);

	while (window.isOpen())
	{
		sf::Event e;
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
					f.leap();
				if ( sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ) 
					s.leap();

				int64_t dt = clock.getElapsedTime().asMilliseconds();
				s.tick(dt);
				f.tick(dt);
				f.draw(window);
				s.draw(window);
		}
		window.display();
	}
}
