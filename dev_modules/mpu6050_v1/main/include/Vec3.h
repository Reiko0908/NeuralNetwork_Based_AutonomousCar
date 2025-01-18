#ifndef VEC3_H
#define VEC3_H

#include <cstring>

template<typename T>
class Vec3 {
  public:
    T x, y, z;
    
    Vec3() {}
    Vec3(T x, T y, T z) {
      this->x = x;
      this->y = y;
      this->z = z;
    }
    
    Vec3<T> operator+(Vec3<T> a) {
      return {
        this->x + a.x,
        this->y + a.y,
        this->z + a.z
      };
    }
    Vec3<T> operator-(Vec3<T> a) {
      return {
        this->x - a.x,
        this->y - a.y,
        this->z - a.z
      };
    }
    Vec3<T> operator*(T a) {
      return {
        this->x * a,
        this->y * a,
        this->z * a
      };
    }

    Vec3<T> operator/(T a) {
      return {
        this->x / a,
        this->y / a,
        this->z / a
      };
    }

    void operator+=(Vec3<T> a) {
      this->x += a.x;
      this->y += a.y;
      this->z += a.z;
    }
    void operator-=(Vec3<T> a) {
      this->x -= a.x;
      this->y -= a.y;
      this->z -= a.z;
    }
    // void operator*=(Vec3<T> a) {
    //   this->x *= a.x;
    //   this->y *= a.y;
    //   this->z *= a.z;
    // }
    // void operator/=(Vec3<T> a) {
    //   this->x /= a.x;
    //   this->y /= a.y;
    //   this->z /= a.z;
    // }
    void operator*=(T a) {
      this->x *= a;
      this->y *= a;
      this->z *= a;
    }
    void operator/=(T a) {
      this->x /= a;
      this->y /= a;
      this->z /= a;
    }
};

template<typename T>
void vec2bytes(const Vec3<T>& vec, uint8_t* bytes) {
  size_t type_size = sizeof(T);
  std::memcpy(bytes + type_size * 0, &vec.x, type_size);
  std::memcpy(bytes + type_size * 1, &vec.y, type_size);
  std::memcpy(bytes + type_size * 2, &vec.z, type_size);
}

template<typename T>
void bytes2vec(const uint8_t* bytes, Vec3<T>& vec){
  size_t type_size = sizeof(T);
  std::memcpy(&vec.x, bytes + type_size * 0, type_size);
  std::memcpy(&vec.y, bytes + type_size * 1, type_size);
  std::memcpy(&vec.z, bytes + type_size * 2, type_size);
}

#endif
