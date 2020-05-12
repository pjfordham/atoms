#include "atoms.hh"
#include <random>

Atoms::Atoms(int _width, int _height ) :
   width(_width),
   height(_height),
   player(width,height),
   map(width,height),
   world(width,height),
   other_world(width,height)
{
   clear();
   editing = false;
   clear();
}

bool Atoms::game_over() const {
   int max_score = 0;
   int total_atoms = 0;
   if (first_go[0] || first_go[1] || first_go[2] || first_go[3])
      return false;
   for(const int & score : scores) {
      max_score = std::max( max_score, score );
      total_atoms += score;
   }
   return (max_score == total_atoms);
}

int Atoms::get_current_player() const {
   return current_player;
}

bool Atoms::is_player_dead( int i ) const {
   return scores[ i ] == 0 && !first_go[ i ];
}

int Atoms::get_player_score( int i ) const {
   return scores[i];
}

Atoms::~Atoms() = default;

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
      calculate_map();
   } else {
      std::minstd_rand rd;
      rd.seed(5);
      std::mt19937 randomNumbers(rd());
      current_player = 0;
      scores[0] = scores[1] = scores[2] = scores[3] = 0;
      first_go[0] = first_go[1] = first_go[2] = first_go[3] = true;
      for ( int i = 0; i < height; i++ ) {
         for ( int j = 0; j < width; j++ ) {
            if ( map[i][j] < 2 ) {
               world[i][j] = 0;
            } else {
               world[i][j] = std::uniform_int_distribution<int>(0,map[i][j]-1)(randomNumbers);
            }
            other_world[i][j] = 0;
            player[i][j] = 20;
         }
      }
   }
}

void Atoms::calculate_map() {
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

void Atoms::recalculate_board() {
   finished = true;
   for ( int i = 0; i < height; i++ ) {
      for ( int j = 0; j < width; j++ ) {
         other_world[i][j] = world[i][j];
      }
   }
   for ( int i = 0; i < height; i++ ) {
      for ( int j = 0; j < width; j++ ) {
         if ( map[i][j] != 0 ) {
            if (other_world[i][j] > (int)map[i][j]) {
               world[i][j]-= ((int)map[i][j] + 1 );
               world[i-1][j]++;
               world[i][j-1]++;
               world[i+1][j]++;
               world[i][j+1]++;
               player[i-1][j]=current_player;
               player[i][j-1]=current_player;
               player[i+1][j]=current_player;
               player[i][j+1]=current_player;
               finished = false;
            }
         } else {
            world[i][j] = 0;
         }
      }
   }

   scores[0] = 0;
   scores[1] = 0;
   scores[2] = 0;
   scores[3] = 0;
   for ( int i = 0; i < height; i++ ) {
      for ( int j = 0; j < width; j++ ) {
         if ( map[i][j] != 0 )
            if ( player[i][j] != 20 )
               scores[player[i][j]] += world[i][j];
      }
   }

   if (finished)
   {
      first_go[ current_player ] = false;
      do {
         current_player = (current_player + 1) % 4;
      } while ( scores[ current_player ] == 0 && !first_go[ current_player ] );
   }
}


void Atoms::click( int j, int i )
{
   if (editing ) {
      map[i][j] = (map[i][j] == 0) ? 3 : 0;
      calculate_map();
   } else {
      if ( map[i][j] != 0 && ( player[i][j] == current_player || world[i][j] == 0 ) ) {
         world[i][j]++;
         player[i][j] = current_player;
         finished = false;
      }
   }
}

Atoms::draw_t Atoms::get_content(int i, int j) const {
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
         case 20: return S_One;
         case 0: return isVolatile ? P1_V_One : P1_One;
         case 1: return isVolatile ? P2_V_One : P2_One;
         case 2: return isVolatile ? P3_V_One : P3_One;
         case 3: return isVolatile ? P4_V_One : P4_One;
         }
      case 2:
         switch (player[i][j]) {
         case 20: return S_Two;
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

