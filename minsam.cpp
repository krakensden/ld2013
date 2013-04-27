#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <cmath>
#include <cstdio>

const int width = 800;
const int height = 800;
const float stepping = 250.0;

double distance(sf::Vector2f a, sf::Vector2f b)
{
	return sqrt(((b.x-a.x)*(b.x-a.x))+((b.y-a.y)*(b.y-a.y)));
}

double cross(sf::Vector2f a, sf::Vector2f b)
{
	return a.x * b.y - b.x * a.y;
}

double swing(double x)
{
	return pow(1.5*x,8);
}

sf::Vector2f swingFrom(sf::Vector2f src, double percent)
{
	sf::Vector2f dst = src, rval;

	// move dst
	dst.x += 10;
	dst.y += 50;

	double new_distance =  distance(src, dst)*percent;
    double direction = atan2(src.y - dst.y, src.x - dst.x);

	rval.x = src.x - cos(direction)*new_distance;
	rval.y = src.y - sin(direction)*new_distance;

	return rval;
}


class Player {
	sf::CircleShape self;
	sf::CircleShape sword;

	sf::Keyboard::Key action, wiggle;

	bool leaping, swinging;
	bool firstPlayer;

	int64_t elapsedSwingTime;

	public:
	Player(bool firstPlayer)
		: self(10)
		, sword(3)
		, leaping(false)
		, swinging(false)
		, firstPlayer(firstPlayer)
		, elapsedSwingTime(0)
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
			//sword.setPosition(width/4,swordYCoord);

			self.setFillColor(sf::Color(250, 218, 221));
			self.setOutlineColor(sf::Color(0,0,0));
			self.setOutlineThickness(1);
			sword.setFillColor(sf::Color(111,111,111));
		}
		else 
		{
			self.setPosition(3*width/4,selfYCoord);
			//sword.setPosition(3*width/4+pHeight-sHeight,swordYCoord);

			self.setFillColor(sf::Color(239, 48, 36));
			self.setOutlineColor(sf::Color(0,0,0));
			self.setOutlineThickness(1);

			sword.setFillColor(sf::Color(111,111,111));
		}
		resetSword();
		leaping = false;
		swinging = false;
		elapsedSwingTime = 0;
	}

	void resetSword()
	{
		float pHeight = selfBound().height;
		float sHeight = swordBound().height;
		int swordYCoord = selfBound().top-sHeight/2;
		if ( firstPlayer )
		{
			sword.setPosition(selfBound().left,swordYCoord);
		}
		else 
		{
			sword.setPosition(selfBound().left+pHeight-sHeight,swordYCoord);
		}
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
			double swingPercent = elapsedSwingTime / 250.0;
			if ( firstPlayer )
				sword.setPosition(selfBound().left+swingPercent*50, selfBound().top+swing(swingPercent));
			else
				sword.setPosition(selfBound().left-swingPercent*50, selfBound().top+swing(-1*swingPercent));
			printf("%d %d %F\n", dt, swingPercent, swing(swingPercent));
			if ( elapsedSwingTime > 250 )
			{
				swinging = false;
				elapsedSwingTime = 0;
				sword.setPosition(selfBound().left, selfBound().top);
				printf("---\n");
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

		// The universe's least-efficient drawing method
		window.clear();
		window.draw(background);

		f.draw(window);
		s.draw(window);

		clock.restart();
		window.display();
	}
}
