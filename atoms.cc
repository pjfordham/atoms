#include <tuple>
#include <string.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <string>


#include <iostream>

const int BOARD_SIZE = 10;
const float TILE_SIZE = 100.0f;

#define HEIGHT BOARD_SIZE
#define WIDTH BOARD_SIZE

enum draw_t {
   Wall = 0, Corner = 1, Edge = 2, Empty = 3, One = 4, Two = 5, Three = 6 };

class Atoms {
public:
   Atoms();
   void click( int i, int j );

   void print();
   void update();
   void clear();
   void calculateMap();
   void recalculateBoard();
   int getCount( int i, int j);
   bool isVolatile( int i, int j);
   draw_t getContent( int i, int j);
   char getState( char state , int xCoord , int yCoord);
   void iterate(unsigned int iterations);
   bool editing = false;
private:
   int player[HEIGHT][WIDTH];
   int map[HEIGHT][WIDTH];
   int world[HEIGHT][WIDTH];
   int otherWorld[HEIGHT][WIDTH];
   std::random_device rd;
   std::mt19937 randomNumbers;
};

Atoms::Atoms() :
   randomNumbers(rd())
{
   clear();
}

void Atoms::clear() {
   for ( int i = 0; i < HEIGHT; i++ ) {
      for ( int j = 0; j < WIDTH; j++ ) {
         world[i][j] = 0;
         otherWorld[i][j] = 0;
         player[i][j] = 0;
         if ( i == 0 || j == 0 || i == HEIGHT -1 || j == WIDTH - 1) {
            map[i][j] = 0;
         } else {
            map[i][j] = 1;
         }
      }
   }
   calculateMap();
}

bool Atoms::isVolatile( int i, int j ) {
   return (world[i][j] == map[i][j]);
}

void Atoms::calculateMap() {
   for ( int i = 0; i < HEIGHT; i++ ) {
      for ( int j = 0; j < WIDTH; j++ ) {
         if ( map[i][j] != 0 ) {
            map[i][j] =  3 -
               ((map[i-1][j] == 0) ? 1 : 0) -
               ((map[i][j-1] == 0) ? 1 : 0) -
               ((map[i+1][j] == 0) ? 1 : 0) -
               ((map[i][j+1] == 0) ? 1 : 0);
         }
      }
   }
}

void Atoms::recalculateBoard() {
   bool finished = false;
   while (!finished) {
      finished = true;
      memcpy(otherWorld, world, sizeof( world ));
      for ( int i = 0; i < HEIGHT; i++ ) {
         for ( int j = 0; j < WIDTH; j++ ) {
            if ( map[i][j] != Wall ) {
               if (world[i][j] > (int)map[i][j]) {
                  otherWorld[i][j]-= ((int)map[i][j] + 1 );
                  otherWorld[i-1][j]++;
                  otherWorld[i][j-1]++;
                  otherWorld[i+1][j]++;
                  otherWorld[i][j+1]++;
                  finished = false;
               }
            } else {
               otherWorld[i][j] = 0;
            }
         }
      }
      memcpy(world, otherWorld, sizeof( world ));
   }
}


void Atoms::click( int j, int i )
{
   if (editing ) {
      map[i][j] = (map[i][j] == Wall) ? Empty : Wall;
      calculateMap();
   } else {
      world[i][j]++;
      recalculateBoard();
   }
}

void Atoms::print() {
        for ( int i = 0; i < HEIGHT; i++ ) {
            for ( int j = 0; j < WIDTH; j++ ) {
                std::cout << world[i][j];
            }
            std::cout << std::endl;
        }
    for ( int i = 0; i < WIDTH; i++ ) {
        std::cout << '=';
    }
    std::cout << std::endl;
}

int Atoms::getCount( int i , int j )
{
   if (editing ) {
      return (int)map[i][j];
   }
   return world[i][j];
}

draw_t Atoms::getContent(int i, int j) {
   if (editing) {
      switch( map[i][j] ) {
      case 0: return Wall;
      case 1: return Corner;
      case 2: return Edge;
      case 3: return Empty;
      }
   } else {
      if ( map[i][j] == 0 ) return Wall;
      switch( world[i][j] ) {
      case 0: return Empty;
      case 1: return One;
      case 2: return Two;
      case 3: return Three;
      }
   }
}

void Atoms::update() {
}

char Atoms::getState( char state, int yCoord, int xCoord ) {
}

void Atoms::iterate( unsigned int iterations ) {
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
            case Wall:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::White);
               window.draw(shape);
               break;
            }
            break;
            case Empty:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::Black);
               window.draw(shape);
               break;
            }
            case Edge:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::Yellow);
               window.draw(shape);
               break;
            }
            case Corner:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               shape.setFillColor(sf::Color::Red);
               window.draw(shape);
               break;
            }
            case One:
            {
               sf::Text text;
               text.setFont(font);
               text.setString("1");
               text.setCharacterSize(TILE_SIZE);
               text.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               if (gol.isVolatile(x,y)) {
                  text.setColor(sf::Color::Yellow);
               } else {
                  text.setColor(sf::Color::Red);
               }
               window.draw(text);
               break;
               }
            case Two:
            {
               sf::Text text;
               text.setFont(font);
               text.setString("2");
               text.setCharacterSize(TILE_SIZE);
               text.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               if (gol.isVolatile(x,y)) {
                  text.setColor(sf::Color::Yellow);
               } else {
                  text.setColor(sf::Color::Red);
               }
               window.draw(text);
               break;
            }
            case Three:
            {
               sf::Text text;
               text.setFont(font);
               text.setString("3");
               text.setCharacterSize(TILE_SIZE);
               text.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               if (gol.isVolatile(x,y)) {
                  text.setColor(sf::Color::Yellow);
               } else {
                  text.setColor(sf::Color::Red);
               }
               window.draw(text);
               break;
            }
            }
         }
      }
      sf::Time elapsed = messageClock.getElapsedTime();
      if (elapsed.asSeconds() < 2.0f) {
               // sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               // shape.setPosition((y+1)*TILE_SIZE, (x+1)*TILE_SIZE);
               // shape.setFillColor(sf::Color::Red);
               // window.draw(shape);
      }

      window.display();
   }

   return 0;
}
