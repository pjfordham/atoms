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

enum map_t {
   Wall = 0, Corner = 1, Edge = 2, Empty = 3 };

class Atoms {
public:
   Atoms();
   void click( int i, int j );

   void print();
   void update();
   void clear();
   void calculateMap();
   map_t getContent( int i, int j);
   char getState( char state , int xCoord , int yCoord , bool toggle);
   void iterate(unsigned int iterations);
   bool editing = false;
private:
   char player[HEIGHT][WIDTH];
   map_t map[HEIGHT][WIDTH];
   char world[HEIGHT][WIDTH];
   char otherWorld[HEIGHT][WIDTH];
   bool toggle;
   std::random_device rd;
   std::mt19937 randomNumbers;
};

Atoms::Atoms() :
   randomNumbers(rd())
{
   clear();
}

void Atoms::clear() {
   toggle = true;
   for ( int i = 0; i < HEIGHT; i++ ) {
      for ( int j = 0; j < WIDTH; j++ ) {
         world[i][j] = 0;
         otherWorld[i][j] = 0;
         player[i][j] = 0;
         if ( i == 0 || j == 0 || i == HEIGHT -1 || j == WIDTH - 1) {
            map[i][j] = Wall;
         } else {
            map[i][j] = Empty;
         }
      }
   }
   calculateMap();
}

void Atoms::calculateMap() {
   for ( int i = 0; i < HEIGHT; i++ ) {
      for ( int j = 0; j < WIDTH; j++ ) {
         if ( map[i][j] != Wall ) {
            map[i][j] = (map_t)(3 -
               ((map[i-1][j] == Wall) ? 1 : 0) -
               ((map[i][j-1] == Wall) ? 1 : 0) -
               ((map[i+1][j] == Wall) ? 1 : 0) -
                                ((map[i][j+1] == Wall) ? 1 : 0));
            std::cout << i << ":" << j << " " << (int)map[i][j] << std::endl;
         }
      }
   }
}

void Atoms::click( int j, int i )
{
   if (editing ) {
      map[i][j] = (map[i][j] == Wall) ? Empty : Wall;
      calculateMap();
   } else {
      if ( toggle ) {
         world[i][j]++;
         if (world[i][j] > map[i][j]){
            world[i][j] = 0;
            world[i-1][j]++;
            world[i][j-1]++;
            world[i+1][j]++;
            world[i][j+1]++;
         } else {
            otherWorld[i][j]++;
            if (otherWorld[i][j] > map[i][j]) otherWorld[i][j] = 0;
         }
      }
   }
}

void Atoms::print() {
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

map_t Atoms::getContent(int i, int j) {
   int content;
   if (editing )
      return map[i][j];
   else if (map [i][j] == Wall)
      return Wall;
   else {
      return (map_t)world[i][j];
   }
}

void Atoms::update() {
    if (toggle) {
        for ( int i = 0; i < HEIGHT; i++ ) {
            for ( int j = 0; j < WIDTH; j++ ) {
                otherWorld[i][j] =
                    Atoms::getState(world[i][j] , i , j , toggle);
            }
        }
        toggle = !toggle;
    } else {
        for ( int i = 0; i < HEIGHT; i++ ) {
            for ( int j = 0; j < WIDTH; j++ ) {
                world[i][j] =
                    Atoms::getState(otherWorld[i][j] , i , j , toggle);
            }
        }
        toggle = !toggle;
    }
}

char Atoms::getState( char state, int yCoord, int xCoord, bool toggle ) {
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

void Atoms::iterate( unsigned int iterations ) {
    for ( int i = 0; i < iterations; i++ ) {
        update();
    }
}


int main()
{

   Atoms gol;

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
               // shapeIndex+=event.mouseWheelScroll.delta;
               // if (shapeIndex < 0)
               //    shapeIndex = ( sizeof( shapes ) / sizeof (shapes[0] ) ) - 1;
               // if (shapeIndex >= ( sizeof( shapes ) / sizeof (shapes[0] ) ))
               //    shapeIndex = 0;
            }
         } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
               gol.click( ((int)event.mouseButton.x / (int)TILE_SIZE ) - 1,
                          ((int)event.mouseButton.y / (int)TILE_SIZE ) - 1 );
            } else if (event.mouseButton.button == sf::Mouse::Right) {
               // gol.addShape( shapes[ shapeIndex],
               //               ((int)event.mouseButton.x / (int)TILE_SIZE ) - 1,
               //               ((int)event.mouseButton.y / (int)TILE_SIZE ) - 1 );
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
               gol.editing = !gol.editing;
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
            case Wall:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::Red);
               window.draw(shape);
            }
            break;
            case Edge:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::White);
               window.draw(shape);
            }
            break;
            case Corner:
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
         text.setString("Hello!");
         text.setCharacterSize(4*TILE_SIZE); // in pixels, not points!
         text.setPosition(4*TILE_SIZE, 4*TILE_SIZE);
         text.setColor(sf::Color::Red);
         window.draw(text);
      }

      window.display();
   }

   return 0;
}
