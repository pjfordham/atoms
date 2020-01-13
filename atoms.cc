#include <tuple>
#include <string.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <map>

#include <iostream>

const int BOARD_SIZE = 20;
const float TILE_SIZE = 50.0f;

#define HEIGHT BOARD_SIZE
#define WIDTH BOARD_SIZE

enum draw_t {
   // Map Elements
   Wall, Corner, Edge, Empty, Bang,

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
      default: return Bang;
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

   virtual void setPosition(int _y, int _x) = 0;
 
   virtual void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const = 0;

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states ) const {
      draw( target, states, getCurrentFrame() );
   }

};

class VolatileNumber : public Animation {
   sf::Font font;
   sf::Color color;
   mutable sf::Sprite background;
   int number;
   int x;
   int y;
   public:
   void setPosition(int _y, int _x) {
      x = _x;
      y = _y;
   }

   VolatileNumber( sf::Font _font, sf::Color _color, int _number, sf::Sprite _background ) :
      Animation( 50, 50 ), font( _font ), color( _color), background( _background), number( _number ), y(0), x(0) {
   }

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const {
      background.setPosition( y, x );
      target.draw(background);

      sf::Text text;
      text.setFont(font);
      text.setString(std::to_string( number ));
      text.setCharacterSize(TILE_SIZE);

      // center text
      sf::FloatRect textRect = text.getLocalBounds();
      text.setOrigin(textRect.left + textRect.width/2.0f,
                     textRect.top  + textRect.height/2.0f);
      text.setPosition(y+(0.5*TILE_SIZE), x+(0.5*TILE_SIZE));

      if ( frame >= 25 )
         frame = 50 - frame;
      auto brightness = sf::Color(9 * frame, 9* frame, 9 * frame );
      auto dimness = sf::Color(9 * (25 - frame), 9* (25-frame), 9 * (25-frame) );

      text.setColor( sf::Color::Yellow * brightness + color * dimness );

      target.draw(text);
   }
};

class Explosion : public Animation {
   int x;
   int y;
   sf::Texture explosionTexture;
   mutable sf::Sprite explosionSprite[12];
   mutable sf::Sprite background;

public:
   void setPosition(int _y, int _x) {
      x = _x;
      y = _y;
   }

   Explosion( sf::Sprite _background) : Animation( 12,12 ), y(0), x(0), background( _background ) {
      if (!explosionTexture.loadFromFile("explosion.png"))
      {
         std::cerr << "Texture error." << std::endl;
         exit( -1 );
      }

      for( int i = 0; i<12 ; i++ ){
         explosionSprite[i] = sf::Sprite( explosionTexture, sf::IntRect(i * 96,0,96,96) );
         explosionSprite[i].scale( (float) TILE_SIZE / (float) 96, (float)TILE_SIZE / 96 );
      }
   };

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const {
      background.setPosition( y, x );
      target.draw(background);
      explosionSprite[ frame ].setPosition(y, x);
      target.draw( explosionSprite[ frame] );
   }

};


 int main()
{
   Atoms atoms;

   std::map<draw_t, Animation*> drawables;

   sf::Font font;
   if (!font.loadFromFile("Instruction.ttf") ) {
      std::cerr << "Font error." << std::endl;
      exit( -1 );
   }

   sf::Texture stoneTexture;
   if (!stoneTexture.loadFromFile("stone.png"))
   {
      std::cerr << "Texture error." << std::endl;
      exit( -1 );
   }

   sf::Vector2u stoneSize = stoneTexture.getSize();

   sf::Sprite stoneSprite;
   stoneSprite.setTexture(stoneTexture);
   stoneSprite.scale( (float) TILE_SIZE / (float) stoneSize.x, (float)TILE_SIZE / stoneSize.y );

   sf::Texture woodTexture;
   if (!woodTexture.loadFromFile("wood.png"))
   {
      std::cerr << "Texture error." << std::endl;
      exit( -1 );
   }

   sf::Vector2u woodSize = woodTexture.getSize();

   sf::Sprite woodSprite;
   woodSprite.setTexture(woodTexture);
   woodSprite.scale( (float) TILE_SIZE / (float) woodSize.x, (float)TILE_SIZE / woodSize.y );



   sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * (int)TILE_SIZE, BOARD_SIZE * (int)TILE_SIZE), "Atoms");

   sf::Clock clock;

   VolatileNumber p1vone( font, sf::Color::Red, 1, woodSprite);
   VolatileNumber p2vone( font, sf::Color::Green, 1, woodSprite);
   VolatileNumber p3vone( font, sf::Color::Blue, 1, woodSprite);
   VolatileNumber p4vone( font, sf::Color::White, 1, woodSprite);
   VolatileNumber p1vtwo( font, sf::Color::Red, 2, woodSprite);
   VolatileNumber p2vtwo( font, sf::Color::Green, 2, woodSprite);
   VolatileNumber p3vtwo( font, sf::Color::Blue, 2, woodSprite);
   VolatileNumber p4vtwo( font, sf::Color::White, 2, woodSprite);
   VolatileNumber p1vthree( font, sf::Color::Red, 3, woodSprite);
   VolatileNumber p2vthree( font, sf::Color::Green, 3, woodSprite);
   VolatileNumber p3vthree( font, sf::Color::Blue, 3, woodSprite);
   VolatileNumber p4vthree( font, sf::Color::White, 3, woodSprite);

   Explosion explosion( woodSprite );

   drawables[ Bang ] = &explosion;
   drawables[ P1_V_One ] = &p1vone;
   drawables[ P2_V_One ] = &p2vone;
   drawables[ P3_V_One ] = &p3vone;
   drawables[ P4_V_One ] = &p4vone;
   drawables[ P1_V_Two ] = &p1vtwo;
   drawables[ P2_V_Two ] = &p2vtwo;
   drawables[ P3_V_Two ] = &p3vtwo;
   drawables[ P4_V_Two ] = &p4vtwo;
   drawables[ P1_V_Three ] = &p1vthree;
   drawables[ P2_V_Three ] = &p2vthree;
   drawables[ P3_V_Three ] = &p3vthree;
   drawables[ P4_V_Three ] = &p4vthree;

   bool running = false;
   while (window.isOpen()) {
      if ( !atoms.finished ) {
         sf::Time elapsed = clock.getElapsedTime();
         if (elapsed.asSeconds() > 0.1f) {
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
               stoneSprite.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw(stoneSprite);
               break;
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
               woodSprite.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw(woodSprite);
               break;
            case P1_One:
            case P2_One:
            case P3_One:
            case P4_One:
            {
               woodSprite.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw(woodSprite);
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
               woodSprite.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw(woodSprite);

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
               woodSprite.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw(woodSprite);
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
            default:
               Animation &cell = *drawables[ atoms.getContent(x, y) ];
               cell.setPosition( y*TILE_SIZE, x*TILE_SIZE );
               window.draw( cell );
               break;
            }
         }
      }
      window.display();
   }

   return 0;
}
