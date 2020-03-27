#include "atoms.hh"
#include <random>

Atoms::Atoms(int _width, int _height ) :
   width(_width),
   height(_height),
   player(width,height),
   map(width,height),
   world(width,height),
   otherWorld(width,height)
{
   clear();
   editing = false;
   clear();
}

int Atoms::getCurrentPlayer() {
   return currentPlayer;
}

bool Atoms::isPlayerDead( int i ) {
   return scores[ i ] == 0 && !firstGo[ i ];
}

int Atoms::getPlayerScore( int i ) {
   return scores[i];
}

Atoms::~Atoms() {
}

void Atoms::clear() {
   if (editing) {
      for ( int i = 0; i < height; i++ ) {
         for ( int j = 0; j < width; j++ ) {
            if ( i == 0 || j == 0 || i == height - 1 || j == width - 1) {
               map[i][j] = 0;
            } else {
               map[i][j] = 3;
            }
         }
      }
      calculateMap();
   } else {
      std::random_device rd;
      std::mt19937 randomNumbers(rd());
      currentPlayer = 0;
      scores[0] = scores[1] = scores[2] = scores[3] = 0;
      firstGo[0] = firstGo[1] = firstGo[2] = firstGo[3] = true;
      for ( int i = 0; i < height; i++ ) {
         for ( int j = 0; j < width; j++ ) {
            if ( i == 0 || j == 0 || i == height - 1 || j == width - 1) {
               world[i][j] = 0;
            } else {
               world[i][j] = std::uniform_int_distribution<int>(0,map[i][j]-1)(randomNumbers);
            }
            otherWorld[i][j] = 0;
            player[i][j] = -1;
         }
      }
   }
}

void Atoms::calculateMap() {
   for ( int i = 0; i < height; i++ ) {
      for ( int j = 0; j < width; j++ ) {
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
   for ( int i = 0; i < height; i++ ) {
      for ( int j = 0; j < width; j++ ) {
         otherWorld[i][j] = world[i][j];
      }
   }
   for ( int i = 0; i < height; i++ ) {
      for ( int j = 0; j < width; j++ ) {
         if ( map[i][j] != 0 ) {
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
      for ( int i = 0; i < height; i++ ) {
         for ( int j = 0; j < width; j++ ) {
            if ( map[i][j] != 0 )
               if ( player[i][j] != -1 )
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
      map[i][j] = (map[i][j] == 0) ? 3 : 0;
      calculateMap();
   } else {
      if ( map[i][j] != 0 && ( player[i][j] == currentPlayer || world[i][j] == 0 ) ) {
         world[i][j]++;
         player[i][j] = currentPlayer;
         finished = false;
      }
   }
}

Atoms::draw_t Atoms::getContent(int i, int j) {
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
         case -1: return S_One;
         case 0: return isVolatile ? P1_V_One : P1_One;
         case 1: return isVolatile ? P2_V_One : P2_One;
         case 2: return isVolatile ? P3_V_One : P3_One;
         case 3: return isVolatile ? P4_V_One : P4_One;
         }
      case 2:
         switch (player[i][j]) {
         case -1: return S_Two;
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
   // We should never get here.
   return Nothing;
}

