/*
 * polygon.h
 *
 *  Created on: Oct 26, 2012
 *      Author: taylormckinney
 */

#ifndef POLYGON_H_
#define POLYGON_H_

#include <iostream>
#include <vector>
#include "./vec.h"
#include "./material.h"
#include "./color.h"
#include "./vertex.h"

struct Face {
//    vector<Vertex3f> vertices;
    vector<int> vertices;
    vector<int> textureVertices;
    Vec3f normal;
    Material mtl;

    Face(const std::vector<int>& p, const std::vector<int>& pt) {
      vertices = p;
      textureVertices = pt;
    }
};
#endif
