#include <tuple>
#include <string.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <string>


#include <iostream>

const int BOARD_SIZE = 20;
const float TILE_SIZE = 50.0f;

#define HEIGHT BOARD_SIZE
#define WIDTH BOARD_SIZE

enum draw_t {
   Wall = 0, Corner = 1, Edge = 2, Empty = 3, One = 4, Two = 5, Three = 6 };

class Atoms {
public:
   Atoms();
   void click( int i, int j );

   void print();
   void clear();
   void calculateMap();
   void recalculateBoard();
   int getCount( int i, int j);
   bool isVolatile( int i, int j);
   draw_t getContent( int i, int j);
   bool editing = false;
   bool finished = false;
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
   finished = true;
   memcpy(otherWorld, world, sizeof( world ));
   for ( int i = 0; i < HEIGHT; i++ ) {
      for ( int j = 0; j < WIDTH; j++ ) {
         if ( map[i][j] != Wall ) {
            if (otherWorld[i][j] > (int)map[i][j]) {
               world[i][j]-= ((int)map[i][j] + 1 );
               world[i-1][j]++;
               world[i][j-1]++;
               world[i+1][j]++;
               world[i][j+1]++;
               finished = false;
            }
         } else {
            world[i][j] = 0;
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
      world[i][j]++;
      finished = false;
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

int main()
{

   Atoms gol;

   sf::Font font;
   if (!font.loadFromFile("Instruction.ttf") ) {
      std::cerr << "Font error." << std::endl;
      exit( -1 );
   }

   sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * (int)TILE_SIZE, BOARD_SIZE * (int)TILE_SIZE), "Atoms");

   sf::Clock clock;
   sf::Clock messageClock;

   bool running = false;
   while (window.isOpen()) {
      if ( !gol.finished ) {
         sf::Time elapsed = clock.getElapsedTime();
         if (elapsed.asSeconds() > 0.05f) {
            gol.recalculateBoard();
            clock.restart();
         }
      }

      sf::Event event;
      while (window.pollEvent(event)) {
         if (event.type == sf::Event::Closed ||
             (event.type == sf::Event::KeyPressed &&
              event.key.code == sf::Keyboard::Escape) ) {
            window.close();
         }
         if ( gol.finished ) {
            if (event.type == sf::Event::MouseButtonPressed) {
               if (event.mouseButton.button == sf::Mouse::Left) {
                  gol.click( ((int)event.mouseButton.x / (int)TILE_SIZE ),
                             ((int)event.mouseButton.y / (int)TILE_SIZE ) );
               }
            }
            else if (event.type == sf::Event::KeyPressed) {
               if (event.key.code == sf::Keyboard::P){
                  gol.print();
               }
               if (event.key.code == sf::Keyboard::C){
                  gol.clear();
               }
               if (event.key.code == sf::Keyboard::Space){
                  gol.editing = !gol.editing;
               }
            }
         }
      }
      // Clear window to Blue to do blue boarder.
      window.clear( sf::Color::Black );

      for( int x=0;x<BOARD_SIZE;x++ ){
         for ( int y = 0;y<BOARD_SIZE;y++) {
            switch (gol.getContent(x, y)) {
            case Wall:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition(y*TILE_SIZE, x*TILE_SIZE);
               shape.setFillColor(sf::Color::White);
               window.draw(shape);
               break;
            }
            break;
            case Empty:
               break;
            case Edge:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition(y*TILE_SIZE, x*TILE_SIZE);
               shape.setFillColor(sf::Color::Yellow);
               window.draw(shape);
               break;
            }
            case Corner:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition(y*TILE_SIZE, x*TILE_SIZE);
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

               //center text
               sf::FloatRect textRect = text.getLocalBounds();
               text.setOrigin(textRect.left + textRect.width/2.0f,
                              textRect.top  + textRect.height/2.0f);
               text.setPosition((y+0.5)*TILE_SIZE, (x+0.5)*TILE_SIZE);

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

               //center text
               sf::FloatRect textRect = text.getLocalBounds();
               text.setOrigin(textRect.left + textRect.width/2.0f,
                              textRect.top  + textRect.height/2.0f);
               text.setPosition((y+0.5)*TILE_SIZE, (x+0.5)*TILE_SIZE);

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

               //center text
               sf::FloatRect textRect = text.getLocalBounds();
               text.setOrigin(textRect.left + textRect.width/2.0f,
                              textRect.top  + textRect.height/2.0f);
               text.setPosition((y+0.5)*TILE_SIZE, (x+0.5)*TILE_SIZE);

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
