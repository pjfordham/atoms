#include <tuple>
#include <string.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>

#include <iostream>

const int BOARD_SIZE = 100;
const float TILE_SIZE = 10.0f;

#define HEIGHT BOARD_SIZE
#define WIDTH BOARD_SIZE

enum content_t {
    Snake, Food, Empty, Head };

struct Shape {
public:
   int height;
   int width;
   const char * const* figure;
   const char *name;
};

#define SubShape( NAME, ... )                             \
   struct NAME : public Shape {                           \
      NAME() {                                            \
         static const char *shape[] = { __VA_ARGS__ };    \
         static const char *Name = #NAME;                 \
         name = Name;                                     \
         figure = shape;                                  \
         height = sizeof( shape ) / sizeof( *shape );     \
         width = strlen( shape[0] );                      \
      }                                                   \
   };

SubShape( Crab,                                 \
          "....X.....X....",                    \
          "....X.....X....",                    \
          "....XX...XX....",                    \
          "...............",                    \
          "XXX..XX.XX..XXX",                    \
          "..X.X.X.X.X.X..",                    \
          "....XX...XX....",                    \
          "...............",                    \
          "....XX...XX....",                    \
          "..X.X.X.X.X.X..",                    \
          "XXX..XX.XX..XXX",                    \
          "...............",                    \
          "....XX...XX....",                    \
          "....X.....X....",                    \
          "....X.....X...." )

SubShape(RPentomino,                            \
         ".XX",                                 \
         "XX.",                                 \
         ".X." )

SubShape(Glider,                                \
         ".X.",                                 \
         "..X",                                 \
         "XXX" )

SubShape(Blinker, \
         "XXX" )

SubShape(Almond,                                \
         ".X.",                                 \
         "X.X",                                 \
         "X.X",                                 \
         ".X." )

SubShape(SpaceShip,                             \
         "X...X.",                              \
         ".....X",                              \
         "X....X",                              \
         ".XXXXX" )

class GameOfLife {
public:
   GameOfLife();
   void addShape( Shape shape, int xCoord = -1 , int yCoord = -1);
   void click( int i, int j );

   void print();
   void update();
   void clear();
   content_t getContent( int i, int j);
   char getState( char state , int xCoord , int yCoord , bool toggle);
   void iterate(unsigned int iterations);
private:
   char world[HEIGHT][WIDTH];
   char otherWorld[HEIGHT][WIDTH];
   bool toggle;
   std::random_device rd;
   std::mt19937 randomNumbers;
};

GameOfLife::GameOfLife() :
   toggle(true), randomNumbers(rd())
{
   for ( int i = 0; i < HEIGHT; i++ ) {
      for ( int j = 0; j < WIDTH; j++ ) {
         world[i][j] = '.';
      }
   }
}

void GameOfLife::clear() {
   toggle = true;
   for ( int i = 0; i < HEIGHT; i++ ) {
      for ( int j = 0; j < WIDTH; j++ ) {
         world[i][j] = '.';
      }
   }
}

void GameOfLife::click( int j, int i )
{
   if ( toggle ) {
      if ( world[i][j] == 'X' ){
         world[i][j] = '.';
      } else {
         world[i][j] = 'X';
      }
   } else {
      if ( otherWorld[i][j] == 'X' ){
         otherWorld[i][j] = '.';
      } else {
         otherWorld[i][j] = 'X';
      }
   }
}

void GameOfLife::addShape( Shape shape, int xCoord, int yCoord )
{
   std::uniform_int_distribution<int> randomXLocationRange(0, BOARD_SIZE-shape.width);
   std::uniform_int_distribution<int> randomYLocationRange(0, BOARD_SIZE-shape.height);

   if ( xCoord == -1 )
      xCoord = randomXLocationRange( randomNumbers );
   if ( yCoord == -1 )
      yCoord = randomYLocationRange( randomNumbers );

   for ( int i = yCoord; i - yCoord < shape.height; i++ ) {
      for ( int j = xCoord; j - xCoord < shape.width; j++ ) {
         if ( i < HEIGHT && j < WIDTH ) {
            if ( toggle ) {
               world[i][j] =
                  shape.figure[ i - yCoord ][j - xCoord ];
            } else {
               otherWorld[i][j] =
                  shape.figure[ i - yCoord ][j - xCoord ];
            }
         }
      }
   }
}

void GameOfLife::print() {
    if ( toggle ) {
        for ( int i = 0; i < HEIGHT; i++ ) {
            for ( int j = 0; j < WIDTH; j++ ) {
                std::cout << world[i][j];
            }
            std::cout << std::endl;
        }
    } else {
        for ( int i = 0; i < HEIGHT; i++ ) {
            for ( int j = 0; j < WIDTH; j++ ) {
                std::cout << otherWorld[i][j];
            }
            std::cout << std::endl;
        }
    }
    for ( int i = 0; i < WIDTH; i++ ) {
        std::cout << '=';
    }
    std::cout << std::endl;
}

content_t GameOfLife::getContent(int i, int j) {
   int content;
   if ( toggle ) {
      content = world[i][j];
   } else {
      content = otherWorld[i][j];
   }
   switch (content) {
   case 'X': return Head;
   default:
      return Empty;
   }
}

void GameOfLife::update() {
    if (toggle) {
        for ( int i = 0; i < HEIGHT; i++ ) {
            for ( int j = 0; j < WIDTH; j++ ) {
                otherWorld[i][j] =
                    GameOfLife::getState(world[i][j] , i , j , toggle);
            }
        }
        toggle = !toggle;
    } else {
        for ( int i = 0; i < HEIGHT; i++ ) {
            for ( int j = 0; j < WIDTH; j++ ) {
                world[i][j] =
                    GameOfLife::getState(otherWorld[i][j] , i , j , toggle);
            }
        }
        toggle = !toggle;
    }
}

char GameOfLife::getState( char state, int yCoord, int xCoord, bool toggle ) {
    char neighbors = 0;
    if ( toggle ) {
        for ( int i = yCoord - 1; i <= yCoord + 1; i++ ) {
            for ( int j = xCoord - 1; j <= xCoord + 1; j++ ) {
                if ( i == yCoord && j == xCoord ) {
                    continue;
                }
                if ( i > -1 && i < HEIGHT && j > -1 && j < WIDTH ) {
                    if ( world[i][j] == 'X' ) {
                        neighbors++;
                    }
                }
            }
        }
    } else {
        for ( int i = yCoord - 1; i <= yCoord + 1; i++ ) {
            for ( int j = xCoord - 1; j <= xCoord + 1; j++ ) {
                if ( i == yCoord && j == xCoord ) {
                    continue;
                }
                if ( i > -1 && i < HEIGHT && j > -1 && j < WIDTH ) {
                    if ( otherWorld[i][j] == 'X' ) {
                        neighbors++;
                    }
                }
            }
        }
    }
    if (state == 'X') {
        return ( neighbors > 1 && neighbors < 4 ) ? 'X' : '.';
    }
    else {
        return ( neighbors == 3 ) ? 'X' : '.';
    }
}

void GameOfLife::iterate( unsigned int iterations ) {
    for ( int i = 0; i < iterations; i++ ) {
        update();
    }
}


int main()
{

   GameOfLife gol;

   Shape shapes[] = { Almond(), Glider(), Crab(), RPentomino(), SpaceShip(), Blinker() };
   int shapeIndex = 0;

   sf::Font font;
   if (!font.loadFromFile("Instruction.ttf") ) {
      std::cerr << "Font error." << std::endl;
      exit( -1 );
   }

   sf::RenderWindow window(sf::VideoMode((2+BOARD_SIZE) * (int)TILE_SIZE, (2+BOARD_SIZE) * (int)TILE_SIZE), "Game of Life");

   sf::Clock clock;
   sf::Clock messageClock;

   bool running = false;
   while (window.isOpen()) {
      if ( running ) {
         sf::Time elapsed = clock.getElapsedTime();
         if (elapsed.asSeconds() > 0.05f) {
            gol.iterate(1);
            clock.restart();
         }
      }

      sf::Event event;
      while (window.pollEvent(event)) {
         if (event.type == sf::Event::Closed) {
            window.close();
         } else if (event.type == sf::Event::MouseWheelScrolled) {
            if(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel ) {
               messageClock.restart();
               shapeIndex+=event.mouseWheelScroll.delta;
               if (shapeIndex < 0)
                  shapeIndex = ( sizeof( shapes ) / sizeof (shapes[0] ) ) - 1;
               if (shapeIndex >= ( sizeof( shapes ) / sizeof (shapes[0] ) ))
                  shapeIndex = 0;
            }
         } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
               gol.click( ((int)event.mouseButton.x / (int)TILE_SIZE ) - 1,
                          ((int)event.mouseButton.y / (int)TILE_SIZE ) - 1 );
            } else if (event.mouseButton.button == sf::Mouse::Right) {
               gol.addShape( shapes[ shapeIndex],
                             ((int)event.mouseButton.x / (int)TILE_SIZE ) - 1,
                             ((int)event.mouseButton.y / (int)TILE_SIZE ) - 1 );
            }
         }
         else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape){
               return 0;
            }
            if (event.key.code == sf::Keyboard::I){
               gol.iterate(1);
            }
            if (event.key.code == sf::Keyboard::P){
               gol.print();
            }
            if (event.key.code == sf::Keyboard::C){
               gol.clear();
            }
            if (event.key.code == sf::Keyboard::Space){
               running = !running;
            }
            if (event.key.code == sf::Keyboard::Left){
               // game.setDirection( Game::Left );
            }
            if (event.key.code == sf::Keyboard::Right){
               // game.setDirection( Game::Right );
            }
            if (event.key.code == sf::Keyboard::Up){
               // game.setDirection( Game::Up );
            }
            if (event.key.code == sf::Keyboard::Down){
               // game.setDirection( Game::Down );
            }

         }
      }

      // Clear window to Blue to do blue boarder.
      window.clear( sf::Color::Blue );

      // draw black background for theatre of life
      sf::RectangleShape shape(sf::Vector2f(TILE_SIZE*BOARD_SIZE, TILE_SIZE*BOARD_SIZE));
      shape.setPosition( TILE_SIZE, TILE_SIZE);
      shape.setFillColor(sf::Color::Black);
      window.draw(shape);

      for( int x=0;x<BOARD_SIZE;x++ ){
         for ( int y = 0;y<BOARD_SIZE;y++) {
            switch (gol.getContent(x, y)) {
            case Empty:
               // Do nothing
               break;
            case Food:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::Red);
               window.draw(shape);
            }
            break;
            case Snake:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::White);
               window.draw(shape);
            }
            break;
            case Head:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::Green);
               window.draw(shape);
            }
            break;
            }
         }
      }
      sf::Time elapsed = messageClock.getElapsedTime();
      if (elapsed.asSeconds() < 2.0f) {
         sf::Text text;
         text.setFont(font);
         text.setString(shapes[shapeIndex].name);
         text.setCharacterSize(4*TILE_SIZE); // in pixels, not points!
         text.setPosition(4*TILE_SIZE, 4*TILE_SIZE);
         text.setColor(sf::Color::Red);
         window.draw(text);
      }

      window.display();
   }

   return 0;
}
