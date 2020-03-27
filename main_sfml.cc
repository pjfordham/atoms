#include <memory>
#include <string>
#include <iostream>

#include <unistd.h>
#include <SFML/Graphics.hpp>

#include "atoms.hh"

const float TILE_SIZE = 50.0f;

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

      text.setFillColor( color );

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

      text.setFillColor( color * dimness );

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

const int BOARD_SIZE = 20;

int main()
{
   Atoms atoms(BOARD_SIZE, BOARD_SIZE);

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
   sf::Color scolor = sf::Color::White;

   sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * (int)TILE_SIZE, BOARD_SIZE * (int)TILE_SIZE), "Atoms");
   window.setFramerateLimit( 50 );

   sf::Clock clock;

   std::array<std::shared_ptr<Element>,Atoms::draw_t_size > drawables;

   drawables[ Atoms::Corner ] = std::shared_ptr<Element>( new RectangleShapeElement( sf::Color::Red ));
   drawables[ Atoms::Edge ] =   std::shared_ptr<Element>( new RectangleShapeElement( sf::Color::Yellow ));
   drawables[ Atoms::Wall ] =   std::shared_ptr<Element>( new SpriteElement( stoneSprite ));
   drawables[ Atoms::Empty ] =  std::shared_ptr<Element>( new SpriteElement( woodSprite ));
   drawables[ Atoms::Bang ] =   std::shared_ptr<Element>( new Explosion( woodSprite ));
   drawables[ Atoms::P1_One ] = std::shared_ptr<Element>( new Number( font, p1color, 1, woodSprite) );
   drawables[ Atoms::P2_One ] = std::shared_ptr<Element>( new Number( font, p2color, 1, woodSprite) );
   drawables[ Atoms::P3_One ] = std::shared_ptr<Element>( new Number( font, p3color, 1, woodSprite) );
   drawables[ Atoms::P4_One ] = std::shared_ptr<Element>( new Number( font, p4color, 1, woodSprite) );
   drawables[ Atoms::P1_Two ] = std::shared_ptr<Element>( new Number( font, p1color, 2, woodSprite) );
   drawables[ Atoms::P2_Two ] = std::shared_ptr<Element>( new Number( font, p2color, 2, woodSprite) );
   drawables[ Atoms::P3_Two ] = std::shared_ptr<Element>( new Number( font, p3color, 2, woodSprite) );
   drawables[ Atoms::P4_Two ] = std::shared_ptr<Element>( new Number( font, p4color, 2, woodSprite) );
   drawables[ Atoms::P1_Three ] = std::shared_ptr<Element>( new Number( font, p1color, 3, woodSprite) );
   drawables[ Atoms::P2_Three ] = std::shared_ptr<Element>( new Number( font, p2color, 3, woodSprite) );
   drawables[ Atoms::P3_Three ] = std::shared_ptr<Element>( new Number( font, p3color, 3, woodSprite) );
   drawables[ Atoms::P4_Three ] = std::shared_ptr<Element>( new Number( font, p4color, 3, woodSprite) );
   drawables[ Atoms::P1_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, p1color, 1, woodSprite) );
   drawables[ Atoms::P2_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, p2color, 1, woodSprite) );
   drawables[ Atoms::P3_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, p3color, 1, woodSprite) );
   drawables[ Atoms::P4_V_One ] = std::shared_ptr<Element>( new VolatileNumber( font, p4color, 1, woodSprite) );
   drawables[ Atoms::P1_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, p1color, 2, woodSprite) );
   drawables[ Atoms::P2_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, p2color, 2, woodSprite) );
   drawables[ Atoms::P3_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, p3color, 2, woodSprite) );
   drawables[ Atoms::P4_V_Two ] = std::shared_ptr<Element>( new VolatileNumber( font, p4color, 2, woodSprite) );
   drawables[ Atoms::P1_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, p1color, 3, woodSprite) );
   drawables[ Atoms::P2_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, p2color, 3, woodSprite) );
   drawables[ Atoms::P3_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, p3color, 3, woodSprite) );
   drawables[ Atoms::P4_V_Three ] = std::shared_ptr<Element>( new VolatileNumber( font, p4color, 3, woodSprite) );
   drawables[ Atoms::S_One ] = std::shared_ptr<Element>( new Number( font, scolor, 1, woodSprite) );
   drawables[ Atoms::S_Two ] = std::shared_ptr<Element>( new Number( font, scolor, 2, woodSprite) );

   window.setFramerateLimit(60);
   bool running = false;
   while (window.isOpen()) {
      if ( !atoms.finished ) {
         sf::Time elapsed = clock.getElapsedTime();
         if (elapsed.asSeconds() > 0.25f) {
            atoms.recalculateBoard();
            clock.restart();
            drawables[ Atoms::Bang ]->restart();
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
                  drawables[ Atoms::Bang ]->restart();
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

      for( int x=0;x<BOARD_SIZE;x++ ){
         for ( int y = 0;y<BOARD_SIZE;y++) {
            Atoms::draw_t content = atoms.getContent( x, y );
            auto &cell = *drawables[ content ];
            cell.setPosition( y*TILE_SIZE, x*TILE_SIZE );
            window.draw( cell );
         }
      }
      window.display();
   }

   return 0;
}