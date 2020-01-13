#include <memory>
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

   draw_t_size
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
      if ( world[i][j] > map[i][j] ) return Bang;
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

class Element : public sf::Drawable, public sf::Transformable {
};

class Number : public Element {
   sf::Font font;
   sf::Color color;
   sf::Sprite background;
   int number;
   public:

public:
   Number( sf::Font _font, sf::Color _color, int _number, sf::Sprite _background ) :
      font( _font ), color( _color), background( _background), number( _number ) {
   }

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states ) const {
      states.transform *= getTransform();
      target.draw(background, states);

      sf::Text text;
      text.setFont(font);
      text.setString(std::to_string( number ));
      text.setCharacterSize(TILE_SIZE);

      // center text
      sf::FloatRect textRect = text.getLocalBounds();
      text.setOrigin(textRect.left + textRect.width/2.0f,
                     textRect.top  + textRect.height/2.0f);
      text.move((0.5*TILE_SIZE), (0.5*TILE_SIZE));

      text.setColor( color );

      target.draw(text,states);
   }

};

class SpriteElement : public Element {
   sf::Sprite background;
   public:

public:
   SpriteElement( sf::Sprite _background ) :
      background( _background) {
   }

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states ) const {
      states.transform *= getTransform();
      target.draw(background, states);
   }

};

class RectangleShapeElement : public Element {
   sf::Color color;
public:
   RectangleShapeElement( sf::Color _color ) : color( _color ) {
   }
   
   virtual void draw( sf::RenderTarget &target, sf::RenderStates states ) const {
      states.transform *= getTransform();
      sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
      shape.setFillColor( color );
      target.draw(shape, states);
   }
};

class Animation : public Element {
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
      states.transform *= getTransform();
      draw( target, states, getCurrentFrame() );
   }

};

class VolatileNumber : public Animation {
   sf::Font font;
   sf::Color color;
   sf::Sprite background;
   int number;
   public:

   VolatileNumber( sf::Font _font, sf::Color _color, int _number, sf::Sprite _background ) :
      Animation( 50, 50 ), font( _font ), color( _color), background( _background), number( _number ) {
   }

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const {
      target.draw(background, states);

      sf::Text text;
      text.setFont(font);
      text.setString(std::to_string( number ));
      text.setCharacterSize(TILE_SIZE);

      // center text
      sf::FloatRect textRect = text.getLocalBounds();
      text.setOrigin(textRect.left + textRect.width/2.0f,
                     textRect.top  + textRect.height/2.0f);
      text.move((0.5*TILE_SIZE), (0.5*TILE_SIZE));

      if ( frame >= 25 )
         frame = 50 - frame;
      auto brightness = sf::Color(9 * frame, 9* frame, 9 * frame );
      auto dimness = sf::Color(9 * (25 - frame), 9* (25-frame), 9 * (25-frame) );

      text.setColor( sf::Color::Yellow * brightness + color * dimness );

      target.draw(text,states);
   }
};

class Explosion : public Animation {
   sf::Texture explosionTexture;
   sf::Sprite explosionSprite[12];
   sf::Sprite background;

public:

   Explosion( sf::Sprite _background) : Animation( 48,12 ), background( _background ) {
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
      target.draw(background, states);
      target.draw( explosionSprite[ frame], states );
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

   std::array<std::shared_ptr<Element>,draw_t_size > drawables;

   drawables[ Corner ] = std::shared_ptr<Element>( new RectangleShapeElement( sf::Color::Red ));
   drawables[ Edge ] =   std::shared_ptr<Element>( new RectangleShapeElement( sf::Color::Yellow ));
   drawables[ Wall ] =   std::shared_ptr<Element>( new SpriteElement( stoneSprite ));
   drawables[ Empty ] =  std::shared_ptr<Element>( new SpriteElement( woodSprite ));
   drawables[ Bang ] =   std::shared_ptr<Element>( new Explosion( woodSprite ));
   drawables[ P1_One ] = std::shared_ptr<Element>( new Number( font, sf::Color::Red, 1, woodSprite));
   drawables[ P2_One ] = std::shared_ptr<Element>( new Number( font, sf::Color::Green, 1, woodSprite));
   drawables[ P3_One ] = std::shared_ptr<Element>( new Number( font, sf::Color::Blue, 1, woodSprite));
   drawables[ P4_One ] = std::shared_ptr<Element>( new Number( font, sf::Color::White, 1, woodSprite));
   drawables[ P1_Two ] = std::shared_ptr<Element>( new Number( font, sf::Color::Red, 2, woodSprite));
   drawables[ P2_Two ] = std::shared_ptr<Element>( new Number( font, sf::Color::Green, 2, woodSprite));
   drawables[ P3_Two ] = std::shared_ptr<Element>( new Number( font, sf::Color::Blue, 2, woodSprite));
   drawables[ P4_Two ] = std::shared_ptr<Element>( new Number( font, sf::Color::White, 2, woodSprite));
   drawables[ P1_Three ] = std::shared_ptr<Element>( new Number( font, sf::Color::Red, 3, woodSprite));
   drawables[ P2_Three ] = std::shared_ptr<Element>( new Number( font, sf::Color::Green, 3, woodSprite));
   drawables[ P3_Three ] = std::shared_ptr<Element>( new Number( font, sf::Color::Blue, 3, woodSprite));
   drawables[ P4_Three ] = std::shared_ptr<Element>( new Number( font, sf::Color::White, 3, woodSprite));
   drawables[ P1_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Red, 1, woodSprite));
   drawables[ P2_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Green, 1, woodSprite));
   drawables[ P3_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Blue, 1, woodSprite));
   drawables[ P4_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::White, 1, woodSprite));
   drawables[ P1_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Red, 2, woodSprite));
   drawables[ P2_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Green, 2, woodSprite));
   drawables[ P3_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Blue, 2, woodSprite));
   drawables[ P4_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::White, 2, woodSprite));
   drawables[ P1_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Red, 3, woodSprite));
   drawables[ P2_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Green, 3, woodSprite));
   drawables[ P3_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::Blue, 3, woodSprite));
   drawables[ P4_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, sf::Color::White, 3, woodSprite));

   bool running = false;
   while (window.isOpen()) {
      if ( !atoms.finished ) {
         sf::Time elapsed = clock.getElapsedTime();
         if (elapsed.asSeconds() > 0.25f) {
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
            auto &cell = *drawables[ atoms.getContent(x, y) ];
            cell.setPosition( y*TILE_SIZE, x*TILE_SIZE );
            window.draw( cell );
         }
      }
      window.display();
   }

   return 0;
}
