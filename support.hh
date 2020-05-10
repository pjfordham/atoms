#ifndef SUPPORT_FILE_H
#define SUPPORT_FILE_H

#include <cstddef>
#include <utility>

template<typename Type>
class Array2D {

   size_t y;
   Type *data;

public:

   Array2D(size_t _x, size_t _y) :
      y{_y}, data{new Type[_x * y]} {}

   ~Array2D() {
      delete[] data;
   }

   // We can't do a copy since we don't know
   // what size to allocate the array.
   Array2D(const Array2D&) = delete;
   Array2D& operator=(const Array2D&) = delete;

   Array2D(Array2D&& move) noexcept :
      y{std::exchange(move.y, 0)},
      data{std::exchange(move.data, nullptr)} {}

   Array2D& operator=(Array2D&& move) noexcept {
      std::swap(y, move.y);
      std::swap(data, move.data);
      return *this;
   }

   Type *operator[](size_t i) noexcept {
      // Defined in-terms of const version to
      // avoid code duplication.
      return const_cast<Type*>(static_cast<const Array2D*>(this)->operator[](i));
   }

   const Type *operator[](size_t i) const noexcept {
      return data + (i * y);
   }
};

#endif // SUPPORT_FILE_H
