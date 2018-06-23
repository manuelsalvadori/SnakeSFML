#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdexcept>
#include <random>
#include <limits.h>

#define SCALEX 0.1f
#define SCALEY 0.1f
#define SPEED 4
#define SIZE 20
#define xHEIGHT 600
#define yWIDTH 800


sf::Texture text;
std::vector<std::unique_ptr<sf::RectangleShape>> snake;
int counter = 0;
bool game_over = false;

sf::Color generateColor()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 255);
	return sf::Color(dis(gen),dis(gen),dis(gen),255);
}

sf::RectangleShape* createSegment()
{
	sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(SIZE,SIZE));
	rect->setFillColor(sf::Color(42,62,116,255));
	return rect;
}

void grow()
{
	sf::RectangleShape *rect = createSegment();
	rect->setPosition(snake[snake.size()-1]->getPosition()); //set proper position
	snake.push_back(std::unique_ptr<sf::RectangleShape>(rect));
}

sf::Vector2f randomPosition()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> disx(0, yWIDTH/20-1);
	std::uniform_int_distribution<> disy(0, xHEIGHT/20-1);
	int x = disx(gen)*20;
	int y = disy(gen)*20;
	std::cout << x << "," << y << std::endl;
	return sf::Vector2f(x,y);
}

sf::RectangleShape* spawnTarget()
{
	sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(SIZE,SIZE));
	rect->setFillColor(sf::Color(255,62,116,255));
	rect->setPosition(randomPosition());
	return rect;
}

const sf::Vector2f dirVectors[4] = {sf::Vector2f(0,-1), sf::Vector2f(0,1), sf::Vector2f(1,0), sf::Vector2f(-1,0)};

void move(int dir)
{
	for(int i = snake.size()-1; i>0; i--)
		snake[i]->setPosition(snake[i-1]->getPosition());
	snake[0]->setPosition(snake[0]->getPosition()+dirVectors[dir]*(float)SIZE);
}

void gameover()
{
	game_over = true;
	counter = INT_MAX;
}

void initSnake(sf::RenderWindow* window)
{
	if(snake.size() > 1)
		snake.clear();

	sf::RectangleShape *head = createSegment();
	head->setPosition(sf::Vector2f(window->getSize().x-SIZE*4, window->getSize().y/2.0f));
	snake.push_back(std::unique_ptr<sf::RectangleShape>(head));

	sf::RectangleShape *h2 = createSegment();
	h2->setPosition(sf::Vector2f(window->getSize().x-SIZE*3, window->getSize().y/2.0f));
	snake.push_back(std::unique_ptr<sf::RectangleShape>(h2));

	sf::RectangleShape *h3 = createSegment();
	h3->setPosition(sf::Vector2f(window->getSize().x-SIZE*2, window->getSize().y/2.0f));
	snake.push_back(std::unique_ptr<sf::RectangleShape>(h3));
}

enum class direction
{
	UP,
	DOWN,
	RIGHT,
	LEFT
};

int main()
{
	// window creation and setting
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(yWIDTH, xHEIGHT), "C++ Snake", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);
    sf::Color background(219, 247, 253, 255);

    // creation of snake head
    initSnake(&window);

    // initialization of global variables
    direction dir = direction::LEFT;
    int c1, c2 = 0;
    sf::Font font;

    // font loading and text initializing
    if (!font.loadFromFile("src/Galaxie.otf"))
    		throw std::runtime_error("ERROR: font not found!");

    sf::Text score;
    score.setFont(font);
    score.setFillColor(sf::Color(93,119,212,255));
    score.setCharacterSize(24);
    int scoreValue = 0;

    sf::Text gameovertext;
    gameovertext.setFont(font);
    gameovertext.setFillColor(sf::Color(93,119,212,255));
    gameovertext.setCharacterSize(72);
    gameovertext.setString("GAME OVER");

    sf::RectangleShape *target = spawnTarget();

    // game loop
    while(window.isOpen())
    {
        window.clear(background);
        gameovertext.setPosition(window.getSize().x/2-gameovertext.getGlobalBounds().width/2, window.getSize().y/2-gameovertext.getGlobalBounds().height);

        // movement
        counter++;
    		if(counter == SPEED)
    		{
    			counter = 0;
    			move((int)dir);
    		}

        // draw every snake segment
        for(std::unique_ptr<sf::RectangleShape>& s: snake)
        {
        		score.setString("Score: " + std::to_string(scoreValue));
        		window.draw(*(s));
        }
        window.draw(score);
        window.draw(*target);

        if(game_over)
        		window.draw(gameovertext);

        if(!snake[0]->getGlobalBounds().intersects(sf::FloatRect(0,0,(float)window.getSize().x,(float)window.getSize().y)))
        {
        		scoreValue = 0;
    			gameover();
        }

        for(int i = 1; i < snake.size(); i++)
        {
        		if(snake[0]->getGlobalBounds().intersects(snake[i]->getGlobalBounds()))
        		{
        			scoreValue = 0;
        			gameover();
        			break;
        		}
        }

        if(snake[0]->getGlobalBounds().intersects(target->getGlobalBounds()))
        {
        		scoreValue += 100;
        		grow();
        		target = spawnTarget();
        }

        c1++;
        if(c1 > 2000000000)
        {
        		c2 -= c1;
        		c1 = 0;
        }

        // event handling
    		sf::Event event;
    		while(window.pollEvent(event))
    		{
    			if(event.type == sf::Event::Closed)
    			{
    				delete target;
    				window.close();
    			}
    			// direction changes
    			if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down && dir != direction::UP)
    				if (c1 - c2 > SPEED)
    				{
    					c2 = c1;
    					dir = direction::DOWN;
    				}

    			if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up && dir != direction::DOWN)
    				if (c1 - c2 > SPEED)
    				{
    					c2 = c1;
    					dir = direction::UP;
    				}

    			if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right && dir != direction::LEFT)
    				if (c1 - c2 > SPEED)
    				{
    					c2 = c1;
    					dir = direction::RIGHT;
    				}

    			if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left && dir != direction::RIGHT)
    				if (c1 - c2 > SPEED)
    				{
    					c2 = c1;
    					dir = direction::LEFT;
    				}

    			if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && game_over)
    			{
    				game_over = false;
    				dir = direction::LEFT;
    				initSnake(&window);
    				counter = 0;
    			}
    		}

    		window.display();
    }
    return 0;
}
