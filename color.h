/*
 * Color.h
 *
 *  Created on: Oct 26, 2012
 *      Author: taylormckinney
 */

#ifndef COLOR_H_
#define COLOR_H_

struct Color3f {
    float c[3];

    Color3f() {
      c[0] = 0.0f;
      c[1] = 0.0f;
      c[2] = 0.0f;
    }

    Color3f(float _r, float _g, float _b) {
      c[0] = _r;
      c[1] = _g;
      c[2] = _b;
    }

    Color3f operator=(const Color3f& rhs) {
      c[0] = rhs.c[0];
      c[1] = rhs.c[1];
      c[2] = rhs.c[2];
      return *this;
    }

    Color3f operator=(const Vec3f& rhs) {
      c[0] = rhs.x[0];
      c[1] = rhs.x[1];
      c[2] = rhs.x[2];
      return *this;
    }

    float getRed() { return c[0]; }
    float getGreen() { return c[1]; }
    float getBlue() { return c[2]; }

    float *getv() {
      float *result = new float[4];
      for (int i = 2; i >= 0; i--) result[i] = c[i];
      return result;
    }
};
#endif
