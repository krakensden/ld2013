#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <cmath>
#include <cstdio>

const int width = 800;
const int height = 600;
const float stepping = 500.0;
const float swingTime = 250; // ms

double swing(double x)
{
	return -15*sin(x*M_PI);
}

void center(sf::Transformable* ob)
{
	ob->setPosition(width/2, height/2);
}

class Player {
	sf::Image swordImage, selfImage;
	sf::Texture swordTexture, selfTexture;
	sf::Sprite self, sword;

	sf::Keyboard::Key action, wiggle;

	bool leaping, swinging, dead;
	bool firstPlayer;

	int64_t elapsedSwingTime;

	public:
	Player(bool firstPlayer)
		: leaping(false)
		, self(selfTexture)
		, sword(swordTexture)
		, swinging(false)
		, dead(false)
		, firstPlayer(firstPlayer)
		, elapsedSwingTime(0)
	{
		sf::IntRect stRect(0,0,11,14);
		swordImage.loadFromFile("sword.png");
		selfImage.loadFromFile("samurai.png");
		if ( firstPlayer )
		{
			swordImage.flipVertically();
			selfImage.flipHorizontally();
		}
		else
		{
			swordImage.flipVertically();
			swordImage.flipHorizontally();
		}
		swordTexture.loadFromImage(swordImage);
		selfTexture.loadFromImage(selfImage);

		sword.setTextureRect(stRect);
		self.setTextureRect(sf::IntRect(0,0,20,20));
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

			//self.setFillColor(sf::Color(250, 218, 221));
			//self.setOutlineColor(sf::Color(0,0,0));
			//self.setOutlineThickness(1);
		}
		else
		{
			self.setPosition(3*width/4,selfYCoord);

			//self.setFillColor(sf::Color(239, 48, 36));
			//self.setOutlineColor(sf::Color(0,0,0));
			//self.setOutlineThickness(1);
		}
		resetSword();

		leaping = false;
		swinging = false;
		dead = false;
		elapsedSwingTime = 0;
	}

	void resetSword()
	{
		float y = selfBound().top  + selfBound().height - swordBound().height;// - swordBound().height/2;
		float x = selfBound().left;
		// Move to edge
		if ( !firstPlayer )
		{
			x += selfBound().width/2;
		}
		sword.setPosition(x,y);
		sword.setRotation(0);
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
			if ( elapsedSwingTime < swingTime )
			{
				resetSword();

				if ( firstPlayer )
				{
					sword.setRotation(-90*swingPercent);
					sword.move(swingPercent*50, swing(swingPercent));
				}
				else
				{
					sword.setRotation(90*swingPercent);
					sword.move(-1*swingPercent*50, swing(swingPercent));
				}
			} else if ( elapsedSwingTime < 1.5*swingTime )
			{
				sword.move(0, -2 * (swingPercent - 1.5));
				if ( firstPlayer )
				{
					sword.setRotation(-90*swingPercent);
				}
				else
				{
					sword.setRotation(90*swingPercent);
				}
			}
			// retract for second swing:
				//swinging = false;
				//elapsedSwingTime = 0;
				//resetSword();
		}
		else
		{
			sword.move(step,0);
		}
	}

	void draw(sf::RenderWindow& w)
	{
		if ( !dead )
			w.draw(self);
		w.draw(sword);

	}

	bool swordCollides(const Player& other) const {
		if ( sword.getGlobalBounds().intersects(other.self.getGlobalBounds()) )
			return true;
		return false;
	}

	void kill() {
		dead = true;
	}
};

enum States
{
	Game,
	Victory
};

int main() {
	sf::RenderWindow window(sf::VideoMode(width, height), "mini samurai");
	sf::Clock clock;

	sf::Texture back;
	sf::Sprite background(back);
	background.setTextureRect(sf::IntRect(0,0,width,height));
	back.loadFromFile("drawing.png");
		
	sf::Font font;
	font.loadFromFile("DejaVuSansMono-Bold.ttf");
	sf::Text instructions("To Play:\n"
		"Get a friend, and share the keyboard. The player on the left has the 'a' key,\n"
		"the player on the right has the right shift key. Tap your key once to charge,\n"
		"and once more to strike. Whoever hits a sword first, loses.", font);

	sf::Text firstPlayerWins("First player wins!\n", font);
	sf::Text secondPlayerWins("Second player wins!\n", font);

	center(&firstPlayerWins);
	center(&secondPlayerWins);
	instructions.setCharacterSize(14);
	instructions.setPosition(width/2-instructions.getGlobalBounds().width/2
			,3*height/4-instructions.getGlobalBounds().height/2);

	window.clear();

	Player f(true), s(false);

	States state = Game;
	bool aWasPressed = false;
	bool shiftWasPressed = false;

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
			if ( state == Game )
			{
				if ( sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !aWasPressed )
					f.act();
				if ( sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) && !shiftWasPressed )
					s.act();
			}
			if ( sf::Keyboard::isKeyPressed(sf::Keyboard::R) )
			{
				// RESET
				s.reset();
				f.reset();
				window.clear();
				state = Game;
			}
			aWasPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
			shiftWasPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
		}
		if ( state == Game )
		{
			s.tick(dt);
			f.tick(dt);
		}


		bool swon = s.swordCollides(f), fwon = f.swordCollides(s);
		if ( swon || fwon )
		{
			state = Victory;
			if ( swon )
				f.kill();
			if ( fwon )
				s.kill();
		}

		// The universe's least-efficient drawing method
		window.clear();
		window.draw(background);

		f.draw(window);
		s.draw(window);

		if ( state == Victory )
		{
			if ( swon && !fwon )
			{
				window.draw(secondPlayerWins);
			}
			else if ( !swon && fwon )
			{
				window.draw(firstPlayerWins);
			}
			else if (swon && fwon )
			{ // errbody dead
			}
			else
			{ // nobody dead
			}
		}
		window.draw(instructions);

		clock.restart();
		window.display();
	}
}
