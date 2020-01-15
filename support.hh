#ifndef SUPPORT_FILE_H
#define SUPPORT_FILE_H

template<typename type>
class Array2D {
   class Array2DHelper {
      Array2D *array;
      int i;
   public:
      Array2DHelper( Array2D *_array, int _i );
      type &operator[]( int j );
   };

   friend Array2DHelper;

   int y;
   type *data;
public:
   // Implement these properly if needed
   Array2D(const Array2D&) = delete;
   Array2D& operator=(const Array2D&) = delete;
   Array2D(const Array2D&&) = delete;
   Array2D& operator=(const Array2D&&) = delete;

   Array2D( int _x, int _y ) : y(_y) {
      data = new int[_x*y];
   }
   Array2DHelper operator[](int i) {
      return Array2DHelper( this, i );
   }
   ~Array2D() {
      delete[] data;
   }
};

template<typename type>
inline Array2D<type>::Array2DHelper::Array2DHelper( Array2D *_array, int _i ) : array(_array), i(_i) {
}

template<typename type>
inline type &Array2D<type>::Array2DHelper::operator[]( int j ) {
   return *(array->data + j * array->y + i);
}

#endif // SUPPORT_FILE_H
