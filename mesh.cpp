#include <iostream>
#include <vector>

#include "./mesh.h"

using namespace std;

Mesh::Mesh() {
  _cur_mtl = -1;
  cur_face = 0;
}

// This will be called by the obj parser
void Mesh::AddVertex(const Vec3f& v) {
  Vertex3f* vt = new Vertex3f(v);
  vertices.push_back(vt);
  _bb(v);
}

// This will be called by the obj parser
void Mesh::AddTextureVertex(const Vec3f& v) {
  Vec3f* vt = new Vec3f(v);
  textureVertices.push_back(vt);
}

// p is the list of indices of vertices for this polygon.  For example,
// if p = {0, 1, 2} then the polygon is a triangle with the zeroth, first and
// second vertices added using AddVertex.
//
// pt is the list of texture indices for this polygon, similar to the
// actual vertices described above.
void Mesh::AddPolygon(const std::vector<int>& p, const std::vector<int>& pt) {
  // updates the poly2mat map
  _polygon2material.push_back(_cur_mtl);

  Face* f = new Face(p, pt);

  //  Calculate the normal by crossing two vectors that define the plane
  //  Two vectors obtained using three points on the plane

  Vec3f u(vertices[p[1]]->point - vertices[p[0]]->point);
  Vec3f v(vertices[p[2]]->point - vertices[p[0]]->point);
  f->normal = (u^v).unit();

  faces.push_back(f);

  for (int i = p.size() - 1; i >= 0 ; i--) {
    vertices[p[i]]->faces.push_back(cur_face);
  }
  cur_face++;
}

// Computes a normal for each vertex.
void Mesh::compute_normals() {
  for (int i = vertices.size() - 1; i >= 0; i--) {
    Vertex3f* v = vertices[i];
    v->normal = Vec3f::makeVec(0, 0, 0);
    for (int j = v->faces.size() - 1; j >= 0; j--) {
      v->normal += (faces[v->faces[j]])->normal;
    }
    v->normal = v->normal.unit();
  }
}
