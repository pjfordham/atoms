#include <memory>
#include <string>
#include <iostream>

#include <unistd.h>
#include <SFML/Graphics.hpp>

#include "atoms.hh"

class Element : public sf::Drawable, public sf::Transformable {
public:
   virtual void restart() {}
   virtual bool isAnimated() { return false; }
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
   int millisPerFrame;
   int frames;
   bool wrap;

   int getCurrentFrame() const {
      int frame = masterClock.getElapsedTime().asMilliseconds() / millisPerFrame;
      if ( wrap ) {
         return frame % frames;
      } else {
         return std::min( frame, frames - 1 );
      }
   }

public:
   Animation( int _frameRate, int _frames, bool _wrap = true ) :
      millisPerFrame( 1000 / _frameRate ),
      frames( _frames ),
      wrap( _wrap ) {
   }

   void restart() {
      masterClock.restart();
   }

   virtual bool isAnimated(){ return true; }

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
      auto dimness = sf::Color(255,255,255, frame*9 );

      text.setColor( color * dimness );

      target.draw(text,states);
   }
};

class Explosion : public Animation {
   sf::Texture explosionTexture;
   sf::Sprite explosionSprite[12];
   sf::Sprite background;

public:

   Explosion( sf::Sprite _background) : Animation( 48,12, false ), background( _background ) {
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

   sf::Color p1color = sf::Color::Red;
   sf::Color p2color = sf::Color::Green;
   sf::Color p3color = sf::Color::Blue;
   sf::Color p4color = sf::Color::Yellow;

   sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * (int)TILE_SIZE, BOARD_SIZE * (int)TILE_SIZE), "Atoms");
   window.setFramerateLimit( 50 );

   sf::Clock clock;

   std::array<std::shared_ptr<Element>,draw_t_size > drawables;

   drawables[ Corner ] = std::shared_ptr<Element>( new RectangleShapeElement( sf::Color::Red ));
   drawables[ Edge ] =   std::shared_ptr<Element>( new RectangleShapeElement( sf::Color::Yellow ));
   drawables[ Wall ] =   std::shared_ptr<Element>( new SpriteElement( stoneSprite ));
   drawables[ Empty ] =  std::shared_ptr<Element>( new SpriteElement( woodSprite ));
   drawables[ Bang ] =   std::shared_ptr<Element>( new Explosion( woodSprite ));
   drawables[ P1_One ] = std::shared_ptr<Element>( new Number( font, p1color, 1, woodSprite) );
   drawables[ P2_One ] = std::shared_ptr<Element>( new Number( font, p2color, 1, woodSprite) );
   drawables[ P3_One ] = std::shared_ptr<Element>( new Number( font, p3color, 1, woodSprite) );
   drawables[ P4_One ] = std::shared_ptr<Element>( new Number( font, p4color, 1, woodSprite) );
   drawables[ P1_Two ] = std::shared_ptr<Element>( new Number( font, p1color, 2, woodSprite) );
   drawables[ P2_Two ] = std::shared_ptr<Element>( new Number( font, p2color, 2, woodSprite) );
   drawables[ P3_Two ] = std::shared_ptr<Element>( new Number( font, p3color, 2, woodSprite) );
   drawables[ P4_Two ] = std::shared_ptr<Element>( new Number( font, p4color, 2, woodSprite) );
   drawables[ P1_Three ] = std::shared_ptr<Element>( new Number( font, p1color, 3, woodSprite) );
   drawables[ P2_Three ] = std::shared_ptr<Element>( new Number( font, p2color, 3, woodSprite) );
   drawables[ P3_Three ] = std::shared_ptr<Element>( new Number( font, p3color, 3, woodSprite) );
   drawables[ P4_Three ] = std::shared_ptr<Element>( new Number( font, p4color, 3, woodSprite) );
   drawables[ P1_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, p1color, 1, woodSprite) );
   drawables[ P2_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, p2color, 1, woodSprite) );
   drawables[ P3_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, p3color, 1, woodSprite) );
   drawables[ P4_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, p4color, 1, woodSprite) );
   drawables[ P1_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, p1color, 2, woodSprite) );
   drawables[ P2_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, p2color, 2, woodSprite) );
   drawables[ P3_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, p3color, 2, woodSprite) );
   drawables[ P4_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, p4color, 2, woodSprite) );
   drawables[ P1_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, p1color, 3, woodSprite) );
   drawables[ P2_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, p2color, 3, woodSprite) );
   drawables[ P3_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, p3color, 3, woodSprite) );
   drawables[ P4_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, p4color, 3, woodSprite) );

   draw_t screen[BOARD_SIZE][BOARD_SIZE];
   for( int x=0;x<BOARD_SIZE;x++ ){
      for ( int y = 0;y<BOARD_SIZE;y++) {
         screen[x][y] = Nothing;
      }
   }

   bool running = false;
   while (window.isOpen()) {
      bool changes = false;
      if ( !atoms.finished ) {
         sf::Time elapsed = clock.getElapsedTime();
         if (elapsed.asSeconds() > 0.25f) {
            atoms.recalculateBoard();
            clock.restart();
            drawables[ Bang ]->restart();
            changes = true;
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
                  // Reset clock to make sure we see full explosion animation before
                  // calling recalculate
                  clock.restart();
                  drawables[ Bang ]->restart();
                  changes = true;
               }
            }
            else if (event.type == sf::Event::KeyPressed) {
               if (event.key.code == sf::Keyboard::C){
                  atoms.clear();
                  changes = true;
               }
               if (event.key.code == sf::Keyboard::Space){
                  if (!atoms.editing) {
                     atoms.clear();
                  }
                  atoms.editing = !atoms.editing;
                  changes = true;
               }
            }
         }
      }

      bool nothing = true;
      if ( !changes ) {
         for( int x=0;x<BOARD_SIZE;x++ ){
            for ( int y = 0;y<BOARD_SIZE;y++) {
               if (screen[x][y] == Nothing ) {
                  draw_t content = atoms.getContent( x, y );
                  auto &cell = *drawables[ content ];
                  cell.setPosition( y*TILE_SIZE, x*TILE_SIZE );
                  window.draw( cell );
                  nothing = false;
                  if ( cell.isAnimated() ) {
                     screen[x][y] = Nothing;
                  } else {
                     screen[x][y] = content;
                  }
               }
            }
         }
      } else {
         for( int x=0;x<BOARD_SIZE;x++ ){
            for ( int y = 0;y<BOARD_SIZE;y++) {
               draw_t content = atoms.getContent( x, y );
               if ( screen[x][y] != content ) {
                  auto &cell = *drawables[ content ];
                  cell.setPosition( y*TILE_SIZE, x*TILE_SIZE );
                  window.draw( cell );
                  nothing = false;
                  if ( cell.isAnimated() ) {
                     screen[x][y] = Nothing;
                  } else {
                     screen[x][y] = content;
                  }
               }
            }
         }
      }
      if ( !nothing )
         window.display();
      else
         usleep(1000);
   }

   return 0;
}
