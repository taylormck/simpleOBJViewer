#ifndef vertex_h
#define vertex_h

#include <iostream>
#include "./vec.h"
#include "./material.h"
#include "./color.h"

struct Vertex3f {
    Vec3f point;
    Vec3f normal;
    Material mtl;
    Color3f c;


    friend ostream & operator<<(ostream & out, const Vertex3f & v);
};

inline ostream & operator<<(ostream & out, const Vertex3f & v) {
  out << "point: " << v.point
      << ", normal: " << v.normal;
  return out;
}

#endif
