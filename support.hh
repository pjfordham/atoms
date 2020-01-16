#ifndef SUPPORT_FILE_H
#define SUPPORT_FILE_H

template<typename type>
class Array2D {
   int y;
   type *data;
public:
   // Implement these properly if needed
   Array2D(const Array2D&) = delete;
   Array2D& operator=(const Array2D&) = delete;
   Array2D(const Array2D&&) = delete;
   Array2D& operator=(const Array2D&&) = delete;

   // implelemt an iterator that walks whole array
   // have some get x,y pos maybe too to optimize other stuff

   Array2D( int _x, int _y ) : y(_y) {
      data = new int[_x*y];
   }
   type *operator[](int i) {
      return data + (i * y);
   }
   ~Array2D() {
      delete[] data;
   }
};

#endif // SUPPORT_FILE_H
