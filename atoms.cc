#include <tuple>
#include <string.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>


#include <iostream>

const int BOARD_SIZE = 20;
const float TILE_SIZE = 50.0f;

#define HEIGHT BOARD_SIZE
#define WIDTH BOARD_SIZE

enum draw_t {
   // Map Elements
   Wall, Corner, Edge, Empty, Explosion,

   // Player One
   P1_One, P1_Two, P1_Three,
   P1_V_One, P1_V_Two, P1_V_Three,

   // Player Two
   P2_One, P2_Two, P2_Three,
   P2_V_One, P2_V_Two, P2_V_Three,

   // Player Three
   P3_One, P3_Two, P3_Three,
   P3_V_One, P3_V_Two, P3_V_Three,

   // Player Four
   P4_One, P4_Two, P4_Three,
   P4_V_One, P4_V_Two, P4_V_Three,
};

class Atoms {
public:
   Atoms();
   void click( int i, int j );
   void clear();
   void calculateMap();
   void recalculateBoard();
   draw_t getContent( int i, int j);
   bool editing = true;
   bool finished = true;
private:
   int currentPlayer;
   bool firstGo[4];
   int scores[4];
   int player[HEIGHT][WIDTH];
   int map[HEIGHT][WIDTH];
   int world[HEIGHT][WIDTH];
   int otherWorld[HEIGHT][WIDTH];
};

Atoms::Atoms()
{
   clear();
   editing = false;
   clear();
}

void Atoms::clear() {
   if (editing) {
      for ( int i = 0; i < HEIGHT; i++ ) {
         for ( int j = 0; j < WIDTH; j++ ) {
            if ( i == 0 || j == 0 || i == HEIGHT -1 || j == WIDTH - 1) {
               map[i][j] = 0;
            } else {
               map[i][j] = 1;
            }
         }
      }
      calculateMap();
   } else {
      currentPlayer = 0;
      scores[0] = scores[1] = scores[2] = scores[3] = 0;
      firstGo[0] = firstGo[1] = firstGo[2] = firstGo[3] = true;
      for ( int i = 0; i < HEIGHT; i++ ) {
         for ( int j = 0; j < WIDTH; j++ ) {
            world[i][j] = 0;
            otherWorld[i][j] = 0;
            player[i][j] = 0;
         }
      }
   }
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
               player[i-1][j]=currentPlayer;
               player[i][j-1]=currentPlayer;
               player[i+1][j]=currentPlayer;
               player[i][j+1]=currentPlayer;
               finished = false;
            }
         } else {
            world[i][j] = 0;
         }
      }
   }
   if (finished)
   {
      scores[0] = 0;
      scores[1] = 0;
      scores[2] = 0;
      scores[3] = 0;
      for ( int i = 0; i < HEIGHT; i++ ) {
         for ( int j = 0; j < WIDTH; j++ ) {
            if ( map[i][j] != Wall )
               scores[player[i][j]] += world[i][j];
         }
      }
      firstGo[ currentPlayer ] = false;
      do {
         currentPlayer = (currentPlayer + 1) % 4;
      } while ( scores[ currentPlayer ] == 0 && !firstGo[ currentPlayer ] );
   }
}


void Atoms::click( int j, int i )
{
   if (editing ) {
      map[i][j] = (map[i][j] == Wall) ? Empty : Wall;
      calculateMap();
   } else {
      if ( player[i][j] == currentPlayer || world[i][j] == 0 ) {
         world[i][j]++;
         player[i][j] = currentPlayer;
         finished = false;
      }
   }
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
      bool isVolatile = (world[i][j] == map[i][j]);
      switch( world[i][j] ) {
      case 0: return Empty;
      case 1:
         switch (player[i][j]) {
         case 0: return isVolatile ? P1_V_One : P1_One;
         case 1: return isVolatile ? P2_V_One : P2_One;
         case 2: return isVolatile ? P3_V_One : P3_One;
         case 3: return isVolatile ? P4_V_One : P4_One;
         }
      case 2:
         switch (player[i][j]) {
         case 0: return isVolatile ? P1_V_Two : P1_Two;
         case 1: return isVolatile ? P2_V_Two : P2_Two;
         case 2: return isVolatile ? P3_V_Two : P3_Two;
         case 3: return isVolatile ? P4_V_Two : P4_Two;
         }
      case 3:
         switch (player[i][j]) {
         case 0: return isVolatile ? P1_V_Three : P1_Three;
         case 1: return isVolatile ? P2_V_Three : P2_Three;
         case 2: return isVolatile ? P3_V_Three : P3_Three;
         case 3: return isVolatile ? P4_V_Three : P4_Three;
         }
      default: return Explosion;
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
               if (event.key.code == sf::Keyboard::C){
                  gol.clear();
               }
               if (event.key.code == sf::Keyboard::Space){
                  if (!gol.editing) {
                     gol.clear();
                  }
                  gol.editing = !gol.editing;
               }
            }
         }
      }
      // Clear window to Blue to do blue border.
      window.clear( sf::Color::Black );

      for( int x=0;x<BOARD_SIZE;x++ ){
         for ( int y = 0;y<BOARD_SIZE;y++) {
            switch (gol.getContent(x, y)) {
            case Wall:
            case Edge:
            case Corner:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition(y*TILE_SIZE, x*TILE_SIZE);
               switch (gol.getContent(x,y)) {
               case Wall: shape.setFillColor(sf::Color::White); break;
               case Edge: shape.setFillColor(sf::Color::Yellow); break;
               case Corner: shape.setFillColor(sf::Color::Red); break;
               default: break;
               }
               window.draw(shape);
               break;
            }
            case Empty:
               break;
            case P1_One:
            case P2_One:
            case P3_One:
            case P4_One:
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
               switch(gol.getContent(x, y)) {
               case P1_One: text.setColor(sf::Color::Red); break;
               case P2_One: text.setColor(sf::Color::Green); break;
               case P3_One: text.setColor(sf::Color::Blue); break;
               default:     text.setColor(sf::Color::White); break;
               }
               window.draw(text);
               break;
            }
            case P1_Two:
            case P2_Two:
            case P3_Two:
            case P4_Two:
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
               switch(gol.getContent(x, y)) {
               case P1_Two: text.setColor(sf::Color::Red); break;
               case P2_Two: text.setColor(sf::Color::Green); break;
               case P3_Two: text.setColor(sf::Color::Blue); break;
               default:     text.setColor(sf::Color::White); break;
               }
               window.draw(text);
               break;
            }
            case P1_Three:
            case P2_Three:
            case P3_Three:
            case P4_Three:
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
               switch(gol.getContent(x, y)) {
               case P1_Three: text.setColor(sf::Color::Red); break;
               case P2_Three: text.setColor(sf::Color::Green); break;
               case P3_Three: text.setColor(sf::Color::Blue); break;
               default:     text.setColor(sf::Color::White); break;
               }
               window.draw(text);
               break;
            }
            case P1_V_One:
            case P2_V_One:
            case P3_V_One:
            case P4_V_One:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition(y*TILE_SIZE, x*TILE_SIZE);
               shape.setFillColor(sf::Color::Yellow);
               window.draw(shape);

               sf::Text text;
               text.setFont(font);
               text.setString("1");
               text.setCharacterSize(TILE_SIZE);

               //center text
               sf::FloatRect textRect = text.getLocalBounds();
               text.setOrigin(textRect.left + textRect.width/2.0f,
                              textRect.top  + textRect.height/2.0f);
               text.setPosition((y+0.5)*TILE_SIZE, (x+0.5)*TILE_SIZE);

               switch(gol.getContent(x, y)) {
               case P1_V_One: text.setColor(sf::Color::Red); break;
               case P2_V_One: text.setColor(sf::Color::Green); break;
               case P3_V_One: text.setColor(sf::Color::Blue); break;
               default:     text.setColor(sf::Color::White); break;
               }
               window.draw(text);
               break;
            }
            case P1_V_Two:
            case P2_V_Two:
            case P3_V_Two:
            case P4_V_Two:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition(y*TILE_SIZE, x*TILE_SIZE);
               shape.setFillColor(sf::Color::Yellow);
               window.draw(shape);

               sf::Text text;
               text.setFont(font);
               text.setString("2");
               text.setCharacterSize(TILE_SIZE);

               //center text
               sf::FloatRect textRect = text.getLocalBounds();
               text.setOrigin(textRect.left + textRect.width/2.0f,
                              textRect.top  + textRect.height/2.0f);
               text.setPosition((y+0.5)*TILE_SIZE, (x+0.5)*TILE_SIZE);
               switch(gol.getContent(x, y)) {
               case P1_V_Two: text.setColor(sf::Color::Red); break;
               case P2_V_Two: text.setColor(sf::Color::Green); break;
               case P3_V_Two: text.setColor(sf::Color::Blue); break;
               default:     text.setColor(sf::Color::White); break;
               }
               window.draw(text);
               break;
            }
            case P1_V_Three:
            case P2_V_Three:
            case P3_V_Three:
            case P4_V_Three:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition(y*TILE_SIZE, x*TILE_SIZE);
               shape.setFillColor(sf::Color::Yellow);
               window.draw(shape);

               sf::Text text;
               text.setFont(font);
               text.setString("3");
               text.setCharacterSize(TILE_SIZE);

               //center text
               sf::FloatRect textRect = text.getLocalBounds();
               text.setOrigin(textRect.left + textRect.width/2.0f,
                              textRect.top  + textRect.height/2.0f);
               text.setPosition((y+0.5)*TILE_SIZE, (x+0.5)*TILE_SIZE);
               switch(gol.getContent(x, y)) {
               case P1_V_Three: text.setColor(sf::Color::Red); break;
               case P2_V_Three: text.setColor(sf::Color::Green); break;
               case P3_V_Three: text.setColor(sf::Color::Blue); break;
               default:     text.setColor(sf::Color::White); break;
               }
               window.draw(text);
               break;
            }
            case Explosion:
            {
               sf::Text text;
               text.setFont(font);
               text.setString("*");
               text.setCharacterSize(TILE_SIZE);

               //center text
               sf::FloatRect textRect = text.getLocalBounds();
               text.setOrigin(textRect.left + textRect.width/2.0f,
                              textRect.top  + textRect.height/2.0f);
               text.setPosition((y+0.5)*TILE_SIZE, (x+0.5)*TILE_SIZE);

               text.setColor(sf::Color::Yellow);
               window.draw(text);
               break;
            }
            }
         }
      }
      window.display();
   }

   return 0;
}
