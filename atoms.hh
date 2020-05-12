#ifndef ATOMS_FILE_H
#define ATOMS_FILE_H

#include "support.hh"

class Atoms {
public:

   enum draw_t {
      Nothing = 0,

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

      // Starting atoms
      S_One, S_Two,

      draw_t_size
   };

   Atoms(int _width, int _height);
   ~Atoms();
   void click( int i, int j );
   void clear( bool randomize );
   void calculate_map();
   void recalculate_board();
   draw_t get_content( int i, int j) const;
   int get_current_player() const;
   bool is_player_dead( int i ) const;
   int get_player_score( int i ) const;
   bool game_over() const;
   bool editing = true;
   bool finished = true;

   friend void dump_state(const Atoms &atoms);

private:
   int scores[4];
   int current_player;
   int width;
   int height;
   bool first_go[4];
   Array2D<int> player, map, world, other_world;
};

#endif // ATOMS_FILE_H
