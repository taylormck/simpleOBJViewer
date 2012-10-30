#include <iostream>
#include <vector>

#include "./mesh.h"

using namespace std;

Mesh::Mesh() {
  _cur_mtl = -1;
}

// This will be called by the obj parser
void Mesh::AddVertex(const Vec3f& v) {
//  Vertex3f* vt = new Vertex3f(v);
//  vertices.push_back(vt);

  // updates the bounding box
  _bb(v);
}

// This will be called by the obj parser
void Mesh::AddTextureVertex(const Vec3f& v) {
//  Vertex3f* vt = new Vertex3f(v);
//  textureVertices.push_back(vt);
}

// p is the list of indices of vertices for this polygon.  For example,
// if p = {0, 1, 2} then the polygon is a triangle with the zeroth, first and
// second vertices added using AddVertex.
//
// pt is the list of texture indices for this polygon, similar to the
// actual vertices described above.

/**
 * Undefined if passed less than 3 vertices
 */
void Mesh::AddPolygon(const std::vector<int>& p, const std::vector<int>& pt) {
  // updates the poly2mat map
  _polygon2material.push_back(_cur_mtl);

  Face* f = new Face(p, pt);

//  Vec3f u(vertices[f->vertices[1]]->point - vertices[f->vertices[0]]->point);
//  Vec3f v(vertices[f->vertices[2]]->point - vertices[f->vertices[0]]->point);

//  f->normal = (u^v).unit();
//  faces.push_back(f);
//
//  for (int i = p.size(); i >= 0; i++) {
//    Vec3f* n = &(vertices[p[i]]->normal);
//    *n = (*n + f->normal).unit();
//  }
}

// Computes a normal for each vertex.
void Mesh::compute_normals() {
  // TODO don't forget to normalize your normals!
}
