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
void RenderMesh();
void DrawFloor(float, float, float, float);
void DrawBounds();

Mesh mesh;
GLuint* texture_ids;

//---------------------------------------------------------------------------//
//  Booleans for options
bool draw_normals = false;
bool draw_axis = false;
bool draw_box = false;
//---------------------------------------------------------------------------//
// Used for the camera functions
// eye is a 3d Vector that represents the Vector Eye - Point of Rotation
// Our ArcBall is centered around the origin only for now
Vec3d eye = Vec3d::makeVec(200.0, 200.0, 500.0);
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
  glEnable(GL_RESCALE_NORMAL);

  setRotation();
  setZoom();
  if (draw_axis)
    DrawAxis();
  if (draw_box)
    DrawBounds();
  RenderMesh();

  // TODO set up lighting, material properties and render mesh.
  // Be sure to call glEnable(GL_RESCALE_NORMAL) so your normals
  // remain normalized throughout transformations.

  glFlush();
  glutSwapBuffers();
}

void DrawRect(const Vec3f & u, const Vec3f & v, const Vec3f & o) {
  glBegin(GL_LINE_STRIP);
  glVertex3fv(o.x);
  glVertex3fv((o+u).x);
  glVertex3fv((o+u+v).x);
  glVertex3fv((o+v).x);
  glVertex3fv(o.x);
  glEnd();
}

void DrawBounds() {
  BoundingBox bb = mesh.bb();
  Vec3f u, v, m1[] = {bb.min, bb.max}, m2[] = {bb.max, bb.min};
  glColor3f(0, 0, 1);
  glLineWidth(1.0);
  glDisable(GL_LIGHTING);
  for (int k = 0; k < 2; k++) {
    for (int i = 0; i < 3; i++) {
      for (int j = i+1; j < 3; j++) {
        u = m1[k];
        v = m1[k];
        u.x[i] = m2[k].x[i];
        v.x[j] = m2[k].x[j];
        u = u-m1[k];
        v = v-m1[k];
        DrawRect(u, v, m1[k]);
      }
    }
  }
  glEnable(GL_LIGHTING);
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
  const float L = 100.0;
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

  return Vec3f::makeVec(xf, -1.0 * yf, zf).unit();
}

void RenderMesh() {
  vector<Face*> faces = mesh.getFaces();
  vector<Vertex3f*> vt = mesh.getVertices();
  int limitv = vt.size();

  glEnable(GL_LIGHTING);
  for (int i = faces.size() - 1; i >= 0; i--) {
    int limitf = faces[i]->vertices.size();
    glBegin(GL_POLYGON);
    for (int j = 0; j < limitf; j++) {
      Vertex3f v(*(vt[faces[i]->vertices[j]]));
      glColor3fv(v.color.c);
      glNormal3fv(v.normal.x);
      glVertex3fv(v.point.x);
    }
    glEnd();

    //  Draws normals of the vertices
    if (draw_normals) {
      glLineWidth(1.0f);
      glColor3f(0.0f, 0.0f, 1.0f);
      glDisable(GL_LIGHTING);
      for (int j = 0; j < limitf; j++) {
        Vertex3f v(*(vt[faces[i]->vertices[j]]));
        glPushMatrix();
        glTranslatef(v.point.x[0], v.point.x[1], v.point.x[2]);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3fv(v.normal.x);
        glEnd();
        glPopMatrix();
      }

      Vertex3f v(*(vt[faces[i]->vertices[0]]));
      glPushMatrix();
      glTranslatef(v.point.x[0], v.point.x[1], v.point.x[2]);
      glBegin(GL_LINES);
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3fv(faces[i]->normal.x);
      glEnd();
      glPopMatrix();
      glEnable(GL_LIGHTING);
    }
  }
}

void Keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'a':
      draw_axis = !(draw_axis);
      break;
    case 'b':
      draw_box = !(draw_box);
      break;
    case 'n':
      draw_normals = !(draw_normals);
      break;
    case 'q':
    case 27:  // esc
      exit(0);
      break;
  }
  glutPostRedisplay();
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

    // Parse arguments
    for (int i = 2; i < argc; i++) {
      if (string(argv[i]) == "-l") {
        scene_lighting = true;
      } else if (string(argv[i]) == "-n") {
        draw_normals = true;
      } else if (string(argv[i]) == "-a") {
        draw_axis = true;
      } else if (string(argv[i]) == "-b") {
        draw_box = true;
      }
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
