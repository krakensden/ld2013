#include <cstdio>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

class Player {
	sf::CircleShape self;
	sf::CircleShape sword;

	sf::Keyboard::Key action, wiggle;

	Player()
	{}
};

int main() {
	printf("Hello world\n");
	sf::RenderWindow window(sf::VideoMode(800, 600), "mini samurai");
	sf::Clock clock;

	window.clear();

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
		}
		window.display();
	}
}
