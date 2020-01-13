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

class Animation : public sf::Drawable {
   sf::Clock masterClock;
   int frameRate;
   sf::Time startTime;
   int frames;

   int getCurrentFrame() const {
      sf::Time deltaTime = masterClock.getElapsedTime() - startTime;
      int millisPerFrame = 1000 / frameRate;
      int animationLength = frames * millisPerFrame;
      int offset = deltaTime.asMilliseconds() % animationLength;
      int frame = offset / millisPerFrame;
      return  frame % frames;
   }

public:
   Animation( int _frameRate, int _frames ) :
      frameRate( _frameRate ),
      frames( _frames ) {
      startTime = masterClock.getElapsedTime();

   }

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const = 0;

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states ) const {
      draw( target, states, getCurrentFrame() );
   }

};

class VolatileNumber : public Animation {
   sf::Font font;
   sf::Color color;
   int number;
   int x;
   int y;
   public:
   void setPosition(int _y, int _x) {
      x = _x;
      y = _y;
   }

   VolatileNumber( sf::Font _font, sf::Color _color, int _number ) :
      Animation( 50, 50 ), font( _font ), color( _color), number( _number ), y(0), x(0) {
   }

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const {
      sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
      shape.setPosition(y,x);
      if ( frame >= 25 )
         frame = 50 - frame;
      shape.setFillColor( sf::Color( frame*9, frame*9, 0 ) );
      target.draw(shape);

      sf::Text text;
      text.setFont(font);
      text.setString(std::to_string( number ));
      text.setCharacterSize(TILE_SIZE);

      // center text
      sf::FloatRect textRect = text.getLocalBounds();
      text.setOrigin(textRect.left + textRect.width/2.0f,
                     textRect.top  + textRect.height/2.0f);
      text.setPosition(y+(0.5*TILE_SIZE), x+(0.5*TILE_SIZE));

      text.setColor(color);
      target.draw(text);
   }
};

int main()
{
   Atoms atoms;

   sf::Font font;
   if (!font.loadFromFile("Instruction.ttf") ) {
      std::cerr << "Font error." << std::endl;
      exit( -1 );
   }

   sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * (int)TILE_SIZE, BOARD_SIZE * (int)TILE_SIZE), "Atoms");

   sf::Clock clock;

   VolatileNumber p1vone( font, sf::Color::Red, 1);
   VolatileNumber p2vone( font, sf::Color::Green, 1);
   VolatileNumber p3vone( font, sf::Color::Blue, 1);
   VolatileNumber p4vone( font, sf::Color::White, 1);
   VolatileNumber p1vtwo( font, sf::Color::Red, 2);
   VolatileNumber p2vtwo( font, sf::Color::Green, 2);
   VolatileNumber p3vtwo( font, sf::Color::Blue, 2);
   VolatileNumber p4vtwo( font, sf::Color::White, 2);
   VolatileNumber p1vthree( font, sf::Color::Red, 3);
   VolatileNumber p2vthree( font, sf::Color::Green, 3);
   VolatileNumber p3vthree( font, sf::Color::Blue, 3);
   VolatileNumber p4vthree( font, sf::Color::White, 3);

   bool running = false;
   while (window.isOpen()) {
      if ( !atoms.finished ) {
         sf::Time elapsed = clock.getElapsedTime();
         if (elapsed.asSeconds() > 0.05f) {
            atoms.recalculateBoard();
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
         if ( atoms.finished ) {
            if (event.type == sf::Event::MouseButtonPressed) {
               if (event.mouseButton.button == sf::Mouse::Left) {
                  atoms.click( ((int)event.mouseButton.x / (int)TILE_SIZE ),
                             ((int)event.mouseButton.y / (int)TILE_SIZE ) );
               }
            }
            else if (event.type == sf::Event::KeyPressed) {
               if (event.key.code == sf::Keyboard::C){
                  atoms.clear();
               }
               if (event.key.code == sf::Keyboard::Space){
                  if (!atoms.editing) {
                     atoms.clear();
                  }
                  atoms.editing = !atoms.editing;
               }
            }
         }
      }
      window.clear( sf::Color::Black );

      for( int x=0;x<BOARD_SIZE;x++ ){
         for ( int y = 0;y<BOARD_SIZE;y++) {
            switch (atoms.getContent(x, y)) {
            case Wall:
            case Edge:
            case Corner:
            {
               sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
               shape.setPosition(y*TILE_SIZE, x*TILE_SIZE);
               switch (atoms.getContent(x,y)) {
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
               switch(atoms.getContent(x, y)) {
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
               switch(atoms.getContent(x, y)) {
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
               switch(atoms.getContent(x, y)) {
               case P1_Three: text.setColor(sf::Color::Red); break;
               case P2_Three: text.setColor(sf::Color::Green); break;
               case P3_Three: text.setColor(sf::Color::Blue); break;
               default:     text.setColor(sf::Color::White); break;
               }
               window.draw(text);
               break;
            }
            case P1_V_One:
               p1vone.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p1vone );
               break;
            case P2_V_One:
               p2vone.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p2vone );
               break;
            case P3_V_One:
               p3vone.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p3vone );
               break;
            case P4_V_One:
               p4vone.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p4vone );
               break;
            case P1_V_Two:
               p1vtwo.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p1vtwo );
               break;
            case P2_V_Two:
               p2vtwo.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p2vtwo );
               break;
            case P3_V_Two:
               p3vtwo.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p3vtwo );
               break;
            case P4_V_Two:
               p4vtwo.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p4vtwo );
               break;
            case P1_V_Three:
               p1vthree.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p1vthree );
               break;
            case P2_V_Three:
               p2vthree.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p2vthree );
               break;
            case P3_V_Three:
               p3vthree.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p3vthree );
               break;
            case P4_V_Three:
               p4vthree.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( p4vthree );
               break;
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
