#ifndef VEC3_H
#define VEC3_H

template<typename T>
class Vec3{
  public:
    T x, y, z;
    
    Vec3(){}
    Vec3(T x, T y, T z){
      this->x = x;
      this->y = y;
      this->z = z;
    }
    
    Vec3<T> operator+(Vec3<T> a){
      return {
        this->x + a.x,
        this->y + a.y,
        this->z + a.z
      };
    }
    Vec3<T> operator-(Vec3<T> a){
      return {
        this->x - a.x,
        this->y - a.y,
        this->z - a.z
      };
    }
    Vec3<T> operator*(T a){
      return {
        this->x * a,
        this->y * a,
        this->z * a
      };
    }

    Vec3<T> operator/(T a){
      return {
        this->x / a,
        this->y / a,
        this->z / a
      };
    }

    void operator+=(Vec3<T> a){
      this->x += a.x;
      this->y += a.y;
      this->z += a.z;
    }
    void operator-=(Vec3<T> a){
      this->x -= a.x;
      this->y -= a.y;
      this->z -= a.z;
    }
    // void operator*=(Vec3<T> a){
    //   this->x *= a.x;
    //   this->y *= a.y;
    //   this->z *= a.z;
    // }
    // void operator/=(Vec3<T> a){
    //   this->x /= a.x;
    //   this->y /= a.y;
    //   this->z /= a.z;
    // }
    void operator*=(T a){
      this->x *= a;
      this->y *= a;
      this->z *= a;
    }
    void operator/=(T a){
      this->x /= a;
      this->y /= a;
      this->z /= a;
    }
};

#endif
