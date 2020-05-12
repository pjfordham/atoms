#include <iostream>
#include <iomanip>
#include "atoms.hh"
#include <cassert>
#include <random>
#include <ios>


void dump_state(const Atoms &atoms) {
   std::ios_base::fmtflags f( std::cout.flags() );
   for ( int i = 0; i < 10; i++ ) {
      for ( int j = 0; j < 10; j++ ) {
         std::cout  << std::setfill('0') << std::setw(2) << atoms.player[i][j]<< ":";
      }
      std::cout << "\n";
   }
      std::cout << "\n";
   for ( int i = 0; i < 10; i++ ) {
      for ( int j = 0; j < 10; j++ ) {
         std::cout  << std::setfill('0') << std::setw(2) << atoms.get_content(i , j ) << ":";
      }
      std::cout << "\n";
   }
   std::cout.flags( f );
   std::cout << "\n";
   for ( int i = 0; i < 10; i++ ) {
      for ( int j = 0; j < 10; j++ ) {
         std::cout  << std::setfill('0') << std::setw(2) << atoms.world[i][j] << ":";
      }
      std::cout << "\n";
   }
   std::cout.flags( f );
   std::cout << "\n";
   std::cout << (atoms.game_over() ? "Game over" : "Game not over") << "\n";
   for (int i = 0; i< 4; i++ ){
      std::cout << (atoms.is_player_dead( i ) ? "true " : "false") << ":" << atoms.get_player_score(i) << "\n";
   }
   std::cout << "\n";
}

int main() {
   Atoms atoms(10,10);

   std::minstd_rand rd;
   rd.seed(5);
   std::mt19937 randomNumbers(rd());
   auto rnd = [&randomNumbers]() {return std::uniform_int_distribution<int>(0,9)(randomNumbers);};

   dump_state(atoms);

   while (!atoms.game_over()) {
      auto x = rnd();
      auto y = rnd();
      atoms.click(x,y);
      dump_state(atoms);
      while (!atoms.finished && !atoms.game_over()) {
         atoms.recalculate_board();
         dump_state(atoms);
      }
   }

   dump_state(atoms);

   std::cout << atoms.get_current_player() << "\n";
   return 0;
}
