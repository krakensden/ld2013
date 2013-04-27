#include <cstdio>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

int main() {
	printf("Hello world\n");
	sf::RenderWindow window(sf::VideoMode(800, 600), "mini samurai");

	window.clear();

	while (window.isOpen())
	{
		window.display();
	}
}
