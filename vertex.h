#ifndef vertex_h
#define vertex_h

#include <iostream>
#include <vector>
#include "./vec.h"
#include "./material.h"
#include "./color.h"
#include "./polygon.h"

struct Vertex3f {
    Vec3f point;
    Vec3f normal;
    Color3f color;
    vector<int> faces;

    Vertex3f() {
      point = Vec3f::makeVec(0.0f, 0.0f, 0.0f);
      normal = Vec3f::makeVec(0.0f, 0.0f, 0.0f);
      color = Color3f(0.0f, 0.0f, 0.0f);
    }

    Vertex3f(float x, float y, float z) {
      point = Vec3f::makeVec(x, y, z);
      normal = Vec3f::makeVec(0.0f, 0.0f, 0.0f);
      color = Color3f(0.0f, 0.0f, 0.0f);
    }

    explicit Vertex3f(const Vec3f& v) {
      point = v;
      normal = Vec3f::makeVec(0.0f, 0.0f, 0.0f);
      color = Color3f(0.0f, 0.0f, 0.0f);
    }

    Vertex3f(const Vec3f& v, const Vec3f& c) {
      point = v;
      normal = Vec3f::makeVec(0.0f, 0.0f, 0.0f);
      color = c;
    }

    Vertex3f operator=(const Vertex3f& v) {
      point = v.point;
      normal = v.normal;
      color = v.color;
      return *this;
    }

    //  Returns the vector from v to this
    Vec3f operator-(const Vertex3f & v) {
      return point - v.point;
    }

    Vertex3f operator+(const Vec3f & v) {
      Vertex3f result;
      result.point = point + v;
      result.normal = normal;
      result.color = color;
      return result;
    }

    Vertex3f operator+=(const Vec3f & v) {
      point += v;
      return *this;
    }

    Vertex3f operator-(const Vec3f & v) {
      Vertex3f result;
      result.point = point - v;
      result.normal = normal;
      result.color = color;
      return result;
    }

    Vertex3f operator-=(const Vec3f & v) {
      point -= v;
      return *this;
    }

    friend ostream & operator<<(ostream & out, const Vertex3f & v);
};

inline ostream & operator<<(ostream & out, const Vertex3f & v) {
  out << "[Vertex3f: point - " << v.point
      << ", normal - " << v.normal << "]";
  return out;
}

#endif
