#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "./common.h"
#include "./bb.h"
#include "./mesh.h"
#include "./io.h"
#include "./texture.h"
#define PI 3.14159

using namespace std;
//---------------------------------------------------------------------------//
// Function declarations
Vec3f getArcBallVector(int, int);
void setRotation();
void setZoom();

Mesh mesh;
GLuint* texture_ids;

//---------------------------------------------------------------------------//
// Used for the camera functions
// eye is a 3d Vector that represents the Vector Eye - Point of Rotation
// Our ArcBall is centered around the origin only for now
Vec3d eye = Vec3d::makeVec(2.0, 2.0, 5.0);
GLfloat storedMatrix[] = {1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1};
//---------------------------------------------------------------------------//
// Variables used for ArcBall Rotation
int start_x = 0, start_y = 0, cur_x = 0, cur_y = 0;
bool lDown = false;
Vec3f start = Vec3f::makeVec(0, 0, 1), end = Vec3f::makeVec(0, 0, 1), rotateV;
float rotateAngle;
//---------------------------------------------------------------------------//
//  Used for zoom
//  zoomMin is set to 0.2 due to rendering problems when the eye is set
//  closer to that.
//  zoomMax is basically arbitrary, but details are near impossible to see
//  at 10x distance
bool rDown = false;
const double zoomScale = 0.9f, zoomMin = 0.2, zoomMax = 10.0;
double zoom = 1.0;
int start_zoom_y, cur_zoom_y;

//---------------------------------------------------------------------------//
// window parameters
int window_width = 800, window_height = 600;
float window_aspect = window_width / static_cast<float>(window_height);
bool scene_lighting;

void Display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TODO call gluLookAt such that mesh fits nicely in viewport.
  // mesh.bb() may be useful.
  glMatrixMode(GL_MODELVIEW);

  setRotation();
  setZoom();
  DrawAxis();

  // TODO set up lighting, material properties and render mesh.
  // Be sure to call glEnable(GL_RESCALE_NORMAL) so your normals
  // remain normalized throughout transformations.

  glFlush();
  glutSwapBuffers();
}

void PrintMatrix(GLfloat* m) {
  cout.precision(2);
  int w = 6;
  for (int i = 0; i < 4; ++i) {
    cout << setprecision(2) << setw(w) << m[i] << " "
        << setprecision(2) << setw(w) << m[i+4] << " "
        << setprecision(2) << setw(w) << m[i+8] << " "
        << setprecision(2) << setw(w) << m[i+12] << " "
        << endl;
  }
  cout << endl;
}

void PrintMatrix(GLint matrix) {
  GLfloat m[16];
  glGetFloatv(matrix, m);
  PrintMatrix(m);
}

void PrintMatrix() {
  PrintMatrix(GL_MODELVIEW_MATRIX);
}

void LoadMatrix(GLfloat* m) {
  // transpose to column-major
  for (int i = 0; i < 4; ++i) {
    for (int j = i; j < 4; ++j) {
      swap(m[i*4+j], m[j*4+i]);
    }
  }
  glLoadMatrixf(m);
}

void MultMatrix(GLfloat* m) {
  // transpose to column-major
  for (int i = 0; i < 4; ++i) {
    for (int j = i; j < 4; ++j) {
      swap(m[i*4+j], m[j*4+i]);
    }
  }
  glMultMatrixf(m);
}

void Init() {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  // resize the window
  window_aspect = window_width/static_cast<float>(window_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(40.0, window_aspect, 1, 1500);
  gluLookAt(eye.x[0], eye.x[1], eye.x[2],  0, 0, 0,  0, 1, 0);
}

void DrawAxis() {
  glDisable(GL_LIGHTING);
  glLineWidth(4);
  const Vec3f c = {0, 0, 0};
  const float L = 1;
  const Vec3f X = {L, 0, 0}, Y = {0, L, 0}, Z = {0, 0, L};

  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3fv(c.x);
  glVertex3fv((c+X).x);
  glColor3f(0, 1, 0);
  glVertex3fv(c.x);
  glVertex3fv((c+Y).x);
  glColor3f(0, 0, 1);
  glVertex3fv(c.x);
  glVertex3fv((c+Z).x);
  glEnd();
  glEnable(GL_LIGHTING);
}

//---------------------------------------------------------------------------//
// Sets up the integers that track the mouse position
// Based on which mouse click is being registered
void MouseButton(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    start_x = x;
    cur_x = x;
    start_y = y;
    cur_y = y;
    if (state == GLUT_DOWN)
      lDown = true;
    else
      lDown = false;
  } else if (button == GLUT_RIGHT_BUTTON) {
    start_zoom_y = y;
    cur_zoom_y = y;
    if (state == GLUT_DOWN)
      rDown = true;
    else
      rDown = false;
  }
}

//---------------------------------------------------------------------------//
// Update the current position integers upon movement
void MouseMotion(int x, int y) {
  if (lDown) {
    cur_x = x;
    cur_y = y;
  }
  if (rDown) {
    cur_zoom_y = y;
  }
  glutPostRedisplay();
}

//---------------------------------------------------------------------------//
//  Sets up the rotation vector and angle, then rotates
//  Should rotate the entire scene, including the lights
void setRotation() {
  start = getArcBallVector(start_x, start_y);
  end = getArcBallVector(cur_x, cur_y);
  rotateV = start ^ end;
  rotateAngle = start * end;

  start_x = cur_x;
  start_y = cur_y;

  glRotatef(rotateAngle, rotateV.x[0], rotateV.x[1], rotateV.x[2]);
}

void setZoom() {
  //  Make sure mouse has moved first
  if (cur_zoom_y != start_zoom_y) {
    zoom += zoomScale * (start_zoom_y - cur_zoom_y) / window_width;
    if (zoom < zoomMin) zoom = zoomMin;
    else if (zoom > zoomMax) zoom = zoomMax;

    Vec3d newEye = zoom * eye;

    start_zoom_y = cur_zoom_y;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, window_aspect, 1, 1500);
    gluLookAt(newEye.x[0], newEye.x[1], newEye.x[2],  0, 0, 0,  0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
  }
}

//---------------------------------------//
// Returns a guaranteed unit vector
// between where the mouse click and drag
// has occurred
//---------------------------------------//
Vec3f getArcBallVector(int x, int y) {
  float xf, yf, zf;
  xf = (1.0 * x) / window_width * 2.0f - 1.0f;
  yf = (1.0 * y) / window_height * 2.0f - 1.0f;
  float temp = xf * xf + yf * yf;

  if (temp <= 1.0f)
    zf = sqrt(1.0 - temp);

  //  Handles the case where the mouse is outside the circle
  else
    zf = 0.0f;

  return Vec3f::makeVec(xf, -1 * yf, zf).unit();
}

void Keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q':
    case 27:  // esc
      exit(0);
      break;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << endl;
    cout << "Usage: ./viewer (filename.obj | -s) [-l]" << endl;
    cout << endl;
    cout << "To load data/test.obj: ./viewer data/test.obj" << endl;
    cout << "To load a custom scene: ./viewer -s" << endl;
    cout << endl;
    return 0;
  }

  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(window_width, window_height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Object viewer");
  glutMouseFunc(MouseButton);
  glutMotionFunc(MouseMotion);
  glutKeyboardFunc(Keyboard);
  glutDisplayFunc(Display);

  Init();

  if (string(argv[1]) == "-s") {
    cout << "Create scene" << endl;
  } else {
    string filename(argv[1]);
    cout << filename << endl;

    // Detect whether to fix the light source(s) to the camera or the world
    scene_lighting = false;
    if (argc > 2 && string(argv[2]) == "-l") {
      scene_lighting = true;
    }

    // Parse the obj file, compute the normals, read the textures

    ParseObj(filename, mesh);
    mesh.compute_normals();

    texture_ids = new GLuint[mesh.num_materials()];
    glGenTextures(mesh.num_materials(), texture_ids);

    for (int i = 0; i < mesh.num_materials(); ++i) {
      Material& material = mesh.material(i);
      material.LoadTexture(texture_ids[i]);
    }
  }

  glutMainLoop();

  return 0;
}
