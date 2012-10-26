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

//    Color3f()
//    : c({1.0f, 1.0f, 1.0f}) {}

    Color3f(float _r, float _g, float _b)
    : c({_r, _g, _b}) {}

    float getRed() { return c[0]; }
    float getGreen() { return c[1]; }
    float getBlue() { return c[2]; }

    float *getv() {
      float *result = new float[4];
      for (int i = 2; i >= 0; i--) result[i] = c[i];
      return result;
    }
};

struct Color4f {
    float c[4];

    Color4f()
    : c({1.0f, 1.0f, 1.0f, 1.0f}) {}

    Color4f(float _r, float _g, float _b)
    : c({_r, _g, _b, 1.0f}) {}

    Color4f(float _r, float _g, float _b, float _a)
    : c({_r, _g, _b, _a}) {}

    float getRed() { return c[0]; }
    float getGreen() { return c[1]; }
    float getBlue() { return c[2]; }
    float getAlpha() { return c[3]; }

    float *getv() {
      float *result = new float[4];
      for (int i = 3; i >= 0; i--) result[i] = c[i];
      return result;
    }
};



#endif COLOR_H_
