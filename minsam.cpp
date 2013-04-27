#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <cmath>
#include <cstdio>

const int width = 800;
const int height = 800;
const float stepping = 500.0;
const float swingTime = 250; // ms

double swing(double x)
{
	return 5*pow(x,4);
}

class Player {
	sf::CircleShape self;
	sf::Image swordImage;
	sf::Texture swordTexture;
	sf::Sprite sword;

	sf::Keyboard::Key action, wiggle;

	bool leaping, swinging;
	bool firstPlayer;

	int64_t elapsedSwingTime;

	public:
	Player(bool firstPlayer)
		: self(10)
		, sword(swordTexture)
		, leaping(false)
		, swinging(false)
		, firstPlayer(firstPlayer)
		, elapsedSwingTime(0)
	{
		sf::IntRect stRect(0,0,11,14);
		swordImage.loadFromFile("sword.png");
		if ( firstPlayer )
			swordImage.flipHorizontally();
		swordTexture.loadFromImage(swordImage);
		sword.setTextureRect(stRect);
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
		int selfYCoord = height/2-pHeight/2;

		if ( firstPlayer )
		{
			self.setPosition(width/4,selfYCoord);

			self.setFillColor(sf::Color(250, 218, 221));
			self.setOutlineColor(sf::Color(0,0,0));
			self.setOutlineThickness(1);
		}
		else 
		{
			self.setPosition(3*width/4,selfYCoord);

			self.setFillColor(sf::Color(239, 48, 36));
			self.setOutlineColor(sf::Color(0,0,0));
			self.setOutlineThickness(1);
		}
		resetSword();

		leaping = false;
		swinging = false;
		elapsedSwingTime = 0;
	}

	void resetSword()
	{
		float y = selfBound().top  + selfBound().height/2 - swordBound().height/2;
		float x = selfBound().left;
		// Move to edge
		if ( firstPlayer )
		{
			x += selfBound().width/2;
		}
		sword.setPosition(x,y);
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
				if ( (self.getPosition().x + selfBound().width) < width )
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
		if ( swinging )
		{
			elapsedSwingTime += dt;
			float swingPercent = elapsedSwingTime / swingTime;
			resetSword();

			if ( firstPlayer )
				sword.move(swingPercent*30, swing(swingPercent));
			else
				sword.move(-1*swingPercent*30, -1*swing(swingPercent));

			if ( elapsedSwingTime > swingTime )
			{
				swinging = false;
				elapsedSwingTime = 0;
				resetSword();
			}
		}
		else
		{
			sword.move(step,0);
		}
	}

	void draw(sf::RenderWindow& w)
	{
		w.draw(self);
		w.draw(sword);

	}

	bool swordCollides(const Player& other) const {
		if ( sword.getGlobalBounds().intersects(other.self.getGlobalBounds()) )
			return true;
		return false;
	}
};

int main() {
	sf::RenderWindow window(sf::VideoMode(width, height), "mini samurai");
	sf::Clock clock;

	sf::Texture back;
	sf::Sprite background(back);
	background.setTextureRect(sf::IntRect(0,0,width,height));
	back.loadFromFile("drawing.png");

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

		if ( s.swordCollides(f) )
			printf("second player's sword is hitting the first player\n");
		if ( f.swordCollides(s) )
			printf("first player's sword is hitting the second player\n");


		// The universe's least-efficient drawing method
		window.clear();
		window.draw(background);

		f.draw(window);
		s.draw(window);

		clock.restart();
		window.display();
	}
}
