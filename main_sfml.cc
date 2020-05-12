#include <memory>
#include <string>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

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

   void draw( sf::RenderTarget &target, sf::RenderStates states ) const override {
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
   explicit SpriteElement( sf::Sprite _background ) :
      background( _background) {
   }

   void draw( sf::RenderTarget &target, sf::RenderStates states ) const override {
      states.transform *= getTransform();
      target.draw(background, states);
   }

};

class RectangleShapeElement : public Element {
   sf::Color color;
public:
   explicit RectangleShapeElement( sf::Color _color ) : color( _color ) {
   }

   void draw( sf::RenderTarget &target, sf::RenderStates states ) const override {
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

   void restart() override {
      masterClock.restart();
   }

   bool isAnimated() override{ return true; }

   virtual void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const = 0;

   void draw( sf::RenderTarget &target, sf::RenderStates states ) const override {
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

   void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const override {
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

   explicit Explosion( sf::Sprite _background) : Animation( 48,12, false ), background( _background ) {
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

   void draw( sf::RenderTarget &target, sf::RenderStates states, int frame ) const override {
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

   sf::Color pColor[4] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow };
   sf::Color sColor = sf::Color::White;

   sf::RenderWindow window(sf::VideoMode((10+BOARD_SIZE) * (int)TILE_SIZE, BOARD_SIZE * (int)TILE_SIZE), "Atoms");
   window.setFramerateLimit( 50 );

   sf::Clock clock;

   std::array<std::unique_ptr<Element>,Atoms::draw_t_size > drawables;

   drawables[ Atoms::Corner ] = std::make_unique<RectangleShapeElement>( sf::Color::Red );
   drawables[ Atoms::Edge ] =   std::make_unique<RectangleShapeElement>( sf::Color::Yellow );
   drawables[ Atoms::Wall ] =   std::make_unique<SpriteElement>( stoneSprite );
   drawables[ Atoms::Empty ] =  std::make_unique<SpriteElement>( woodSprite );
   drawables[ Atoms::Bang ] =   std::make_unique<Explosion>( woodSprite );
   drawables[ Atoms::P1_One ] = std::make_unique<Number>(font, pColor[0], 1, woodSprite);
   drawables[ Atoms::P2_One ] = std::make_unique<Number>(font, pColor[1], 1, woodSprite);
   drawables[ Atoms::P3_One ] = std::make_unique<Number>(font, pColor[2], 1, woodSprite);
   drawables[ Atoms::P4_One ] = std::make_unique<Number>(font, pColor[3], 1, woodSprite);
   drawables[ Atoms::P1_Two ] = std::make_unique<Number>(font, pColor[0], 2, woodSprite);
   drawables[ Atoms::P2_Two ] = std::make_unique<Number>(font, pColor[1], 2, woodSprite);
   drawables[ Atoms::P3_Two ] = std::make_unique<Number>(font, pColor[2], 2, woodSprite);
   drawables[ Atoms::P4_Two ] = std::make_unique<Number>(font, pColor[3], 2, woodSprite);
   drawables[ Atoms::P1_Three ] = std::make_unique<Number>(font, pColor[0], 3, woodSprite);
   drawables[ Atoms::P2_Three ] = std::make_unique<Number>(font, pColor[1], 3, woodSprite);
   drawables[ Atoms::P3_Three ] = std::make_unique<Number>(font, pColor[2], 3, woodSprite);
   drawables[ Atoms::P4_Three ] = std::make_unique<Number>(font, pColor[3], 3, woodSprite);
   drawables[ Atoms::P1_V_One ] = std::make_unique<VolatileNumber>(font, pColor[0], 1, woodSprite);
   drawables[ Atoms::P2_V_One ] = std::make_unique<VolatileNumber>(font, pColor[1], 1, woodSprite);
   drawables[ Atoms::P3_V_One ] = std::make_unique<VolatileNumber>(font, pColor[2], 1, woodSprite);
   drawables[ Atoms::P4_V_One ] = std::make_unique<VolatileNumber>(font, pColor[3], 1, woodSprite);
   drawables[ Atoms::P1_V_Two ] = std::make_unique<VolatileNumber>(font, pColor[0], 2, woodSprite);
   drawables[ Atoms::P2_V_Two ] = std::make_unique<VolatileNumber>(font, pColor[1], 2, woodSprite);
   drawables[ Atoms::P3_V_Two ] = std::make_unique<VolatileNumber>(font, pColor[2], 2, woodSprite);
   drawables[ Atoms::P4_V_Two ] = std::make_unique<VolatileNumber>(font, pColor[3], 2, woodSprite);
   drawables[ Atoms::P1_V_Three ] = std::make_unique<VolatileNumber>(font, pColor[0], 3, woodSprite);
   drawables[ Atoms::P2_V_Three ] = std::make_unique<VolatileNumber>(font, pColor[1], 3, woodSprite);
   drawables[ Atoms::P3_V_Three ] = std::make_unique<VolatileNumber>(font, pColor[2], 3, woodSprite);
   drawables[ Atoms::P4_V_Three ] = std::make_unique<VolatileNumber>(font, pColor[3], 3, woodSprite);
   drawables[ Atoms::S_One ] = std::make_unique<Number>(font, sColor, 1, woodSprite);
   drawables[ Atoms::S_Two ] = std::make_unique<Number>(font, sColor, 2, woodSprite);

   window.setFramerateLimit(60);
   bool running = false;

   int32_t player = 0;
   bool client = false;
   sf::TcpSocket socket;
   sf::Socket::Status status = socket.connect("127.0.0.1", 53000);
   if (status == sf::Socket::Done) {
      client = true;
      sf::Packet packet;
      if (socket.receive( packet ) == sf::Socket::Done ) {
         packet >> player;
         std::cout << "Connected to server as player " << player + 1 << std::endl;
      }
      socket.setBlocking(false);
   }

   bool listening = true;
   sf::TcpListener listener;
   if (client ||  listener.listen(53000) != sf::Socket::Done) {
      std::cerr << "Unable to open listening socket, network play disabled." << std::endl;
      listening = false;
   } else {
      listener.setBlocking(false);
   }

   int32_t client_count = 0;
   sf::TcpSocket clients[4];

   while (window.isOpen()) {
      if ( !atoms.finished ) {
         sf::Time elapsed = clock.getElapsedTime();
         if (elapsed.asSeconds() > 0.25f) {
            atoms.recalculate_board();
            clock.restart();
            drawables[ Atoms::Bang ]->restart();
         }
      }

      if (listening && client_count < 3 && listener.accept(clients[client_count]) == sf::Socket::Done ) {
         clients[client_count].setBlocking( false );
         clients[client_count].send(sf::Packet() << client_count+1);
         client_count++;
         std::cerr << "Player " << client_count + 1 << " connected." << std::endl;
      }
      if (client_count == 3) {
         listener.close();
      }

      bool click = false;
      int32_t x = 0, y = 0;

      sf::Event event;
      if (window.pollEvent(event)) {
         if (event.type == sf::Event::Closed ||
             (event.type == sf::Event::KeyPressed &&
              event.key.code == sf::Keyboard::Escape) ) {
            window.close();
         }
         if ( atoms.finished ) {
            if (event.type == sf::Event::MouseButtonPressed) {
               if (event.mouseButton.button == sf::Mouse::Left /*&& player == atoms.getCurrentPlayer()*/) {
                  x = (int)event.mouseButton.x / (int)TILE_SIZE;
                  y = (int)event.mouseButton.y / (int)TILE_SIZE;
                  if ( x <= BOARD_SIZE && y <= BOARD_SIZE) {
                     if ( client ) {
                        if (socket.send(sf::Packet() << x << y) != sf::Socket::Done) {
                           // error...
                        }
                     } else {
                        click = true;
                     }
                     if (listening) {
                        for (int i = 0 ; i< client_count; ++i ) {
                           if (clients[i].send(sf::Packet() << x << y) != sf::Socket::Done) {
                              // error...
                           }
                        }
                     }
                  }
               }
            }
            else if (event.type == sf::Event::KeyPressed) {
               if (event.key.code == sf::Keyboard::R){
                  atoms.clear( true );
               }
               if (event.key.code == sf::Keyboard::C){
                  atoms.clear( false );
               }
               if (event.key.code == sf::Keyboard::Space){
                  if (!atoms.editing) {
                     atoms.clear( true );
                  }
                  atoms.editing = !atoms.editing;
               }
            }
         }
      }
      if (!click && client) {
         sf::Packet packet;
         if (socket.receive( packet ) == sf::Socket::Done ) {
            packet >> x >> y;
            click = true;
         }
      }
      if (!click && listening) {
         for (int i = 0 ; i< client_count; ++i ) {
            sf::Packet packet;
            if (clients[i].receive( packet ) == sf::Socket::Done ) {
               packet >> x >> y;
               click = true;
               for (int j = 0 ; j< client_count; ++j ) {
                     // Send click to the server
                     sf::Packet newPacket;
                     newPacket << x << y;
                     if (clients[j].send(newPacket) != sf::Socket::Done) {
                        // error...
                     }
               }
            }
         }
      }

      if (click) {
         atoms.click( x,y );
         // Reset clock to make sure we see full explosion animation before
         // calling recalculate
         clock.restart();
         drawables[ Atoms::Bang ]->restart();
      }

      for( int x=0;x<BOARD_SIZE;x++ ){
         for ( int y = 0;y<BOARD_SIZE;y++) {
            Atoms::draw_t content = atoms.get_content( x, y );
            auto &cell = *drawables[ content ];
            cell.setPosition( y*TILE_SIZE, x*TILE_SIZE );
            window.draw( cell );
         }
      }

      for( int x=0;x<BOARD_SIZE;x++ ){
         for ( int y = BOARD_SIZE;y<BOARD_SIZE+10;y++) {
            auto &cell = *drawables[ Atoms::Wall ];
            cell.setPosition( y*TILE_SIZE, x*TILE_SIZE );
            window.draw( cell );
         }
      }

      const char *buf = "Score Board";
      sf::Text text;
      text.setFont(font);
      text.setString(buf);
      text.setCharacterSize(TILE_SIZE-5);
      text.setPosition(BOARD_SIZE*(TILE_SIZE+9.5), TILE_SIZE);
      text.setFillColor(sf::Color::White);

      // center text
      sf::FloatRect textRect = text.getLocalBounds();
      text.setOrigin(textRect.left + textRect.width/2.0f,
                     textRect.top  + textRect.height/2.0f);
      text.move((0.5*TILE_SIZE), (0.5*TILE_SIZE));

      window.draw(text);

      for( int i=0;i<4;++i ) {
         char buf[21];
         sf::Text text;
         text.setFont(font);
         if(atoms.is_player_dead( i )) {
            snprintf( buf, 21, "Player %d:    DEAD", i+1);
         } else {
            if (atoms.game_over()) {
               snprintf( buf, 21, "Player %d: WINNER!", i+1);
            } else {
               snprintf( buf, 21, "Player %d:     %3d", i+1, atoms.get_player_score(i));
            }
         }
         text.setString(buf);
         text.setCharacterSize(TILE_SIZE-5);
         text.setPosition(BOARD_SIZE*TILE_SIZE+5, TILE_SIZE*(i+3)-5);
         if (i == atoms.get_current_player() ) {
            text.setOutlineThickness(5);
            text.setFillColor(pColor[i]);
            text.setOutlineColor(sf::Color::White);
         } else {
            text.setFillColor(pColor[i]);
         }
         window.draw(text);
      }

      {
         const char *buf = "New Game";
         sf::Text text;
         text.setFont(font);
         text.setString(buf);
         text.setCharacterSize(TILE_SIZE-5);
         text.setPosition(BOARD_SIZE*(TILE_SIZE+9.5), TILE_SIZE*9);
         text.setFillColor(sf::Color::White);
         // center text
         sf::FloatRect textRect = text.getLocalBounds();
         text.setOrigin(textRect.left + textRect.width/2.0f,
                        textRect.top  + textRect.height/2.0f);
         text.move((0.5*TILE_SIZE), (0.5*TILE_SIZE));
         window.draw(text);
      }
      {
         const char *buf = "Edit Board";
         sf::Text text;
         text.setFont(font);
         text.setString(buf);
         text.setCharacterSize(TILE_SIZE-5);
         text.setPosition(BOARD_SIZE*(TILE_SIZE+9.5), TILE_SIZE*10);
         text.setFillColor(sf::Color::White);

         // center text
         sf::FloatRect textRect = text.getLocalBounds();
         text.setOrigin(textRect.left + textRect.width/2.0f,
                        textRect.top  + textRect.height/2.0f);
         text.move((0.5*TILE_SIZE), (0.5*TILE_SIZE));

         text.setFillColor(sf::Color::White);
         window.draw(text);
      }
      {
         const char *buf = "Quit";
         sf::Text text;
         text.setFont(font);
         text.setString(buf);
         text.setCharacterSize(TILE_SIZE-5);
         text.setPosition(BOARD_SIZE*(TILE_SIZE+9.5), TILE_SIZE*11);
         text.setFillColor(sf::Color::White);

         // center text
         sf::FloatRect textRect = text.getLocalBounds();
         text.setOrigin(textRect.left + textRect.width/2.0f,
                        textRect.top  + textRect.height/2.0f);
         text.move((0.5*TILE_SIZE), (0.5*TILE_SIZE));

         text.setFillColor(sf::Color::White);
         window.draw(text);
      }
      
      window.display();
   }

   return 0;
}
