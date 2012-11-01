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
void RenderMesh(Mesh* me);
void DrawBounds();
void SetEye();
void setEyeLight();
void setWorldLight();
void moveWorldLight();

//---------------------------------------------------------------------------//
// window parameters
int window_width = 800, window_height = 600;
float window_aspect = window_width / static_cast<float>(window_height);
//---------------------------------------------------------------------------//
//  Booleans for options
bool draw_normals = false;
bool draw_axis = false;
bool draw_box = false;
bool debug = false;
int model = GL_SMOOTH;
int light = GL_LIGHT0;
//---------------------------------------------------------------------------//
// Used for the camera functions
// eye is a 3d Vector that represents the Vector Eye - Point of Rotation
// Our ArcBall is centered around the origin only for now
Vec3f eye = Vec3f::makeVec(100.0, 100.0, 250.0);
GLfloat cur_trans[] = {1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1};
//---------------------------------------------------------------------------//
// Variables used for ArcBall Rotation and zoom
GLfloat m[16];
int startx = 0, starty = 0, curx = 0, cury = 0, zoom_y0 = 0, zoom_y1 = 0;
bool lDown = false, rDown = false;
Vec3f start = Vec3f::makeVec(0, 0, 1), end = Vec3f::makeVec(0, 0, 1), rotateV;
float rotateAngle, zoom = 1.0;
const double zoomScale = 0.9f, zoomMin = 0.01, zoomMax = 100.0;
//---------------------------------------------------------------------------//
//  Mesh, material, and texture details
Mesh mesh;
GLuint* texture_ids;
Material* mtl;

void Display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TODO call gluLookAt such that mesh fits nicely in viewport.
  // mesh.bb() may be useful.
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_RESCALE_NORMAL);

  SetEye();
  glEnable(GL_LIGHTING);
  glShadeModel(model);
  glMultMatrixf(cur_trans);
  glEnable(light);
  if (light == GL_LIGHT1) {
    moveWorldLight();
  }

  if (draw_axis)
    DrawAxis();
  if (draw_box)
    DrawBounds();

  RenderMesh(&mesh);
  glDisable(light);
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


void Init() {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // resize the window
  window_aspect = window_width/static_cast<float>(window_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, window_aspect, 1, 1500);

  glMatrixMode(GL_MODELVIEW);
  setEyeLight();
  setWorldLight();
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
    startx = x;
    curx = x;
    starty = y;
    cury = y;
    if (state == GLUT_DOWN)
      lDown = true;
    else
      lDown = false;
  } else if (button == GLUT_RIGHT_BUTTON) {
    zoom_y0 = y;
    zoom_y1 = y;
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
    curx = x;
    cury = y;
  }
  if (rDown) {
    zoom_y1 = y;
  }
  glutPostRedisplay();
}

//---------------------------------------------------------------------------//
// Returns a guaranteed unit vector
// between where the mouse click and drag
// has occurred
Vec3f getArcBallVector(int x, int y) {
  float xf, yf, zf;
  xf = (2.0f * x) / window_width - 1.0f;
  yf = (2.0f * y) / window_height - 1.0f;
  float temp = xf * xf + yf * yf;

  if (temp <= 1.0f)
    zf = sqrt(1.0 - temp);

  //  Handles the case where the mouse is outside the circle
  else
    zf = 0.0f;

  return Vec3f::makeVec(xf, -1.0 * yf, zf).unit();
}

//---------------------------------------------------------------------------//
//  Sets up the rotation vector and angle, then rotates
//  Should rotate the entire scene, including the lights
void setRotation() {
  if (startx != curx || starty != cury) {
    start = getArcBallVector(startx, starty);
    end = getArcBallVector(curx, cury);
    rotateV = start ^ end;
    rotateAngle = acos(start * end) * 180 / PI;

    startx = curx;
    starty = cury;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRotatef(rotateAngle, rotateV.x[0], rotateV.x[1], rotateV.x[2]);
    glMultMatrixf(cur_trans);
    glGetFloatv(GL_MODELVIEW_MATRIX, cur_trans);
    glPopMatrix();
  }
}

void setZoom() {
  if (zoom_y0 != zoom_y1) {
    zoom += zoomScale * (zoom_y0 - zoom_y1) / window_width;
    if (zoom < zoomMin) zoom = zoomMin;
    else if (zoom > zoomMax) zoom = zoomMax;

    zoom_y0 = zoom_y1;
  }
}

void SetEye() {
  Vec3f e = eye * zoom;
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(e.x[0], e.x[1], e.x[2],  0, 0, 0,  0, 1, 0);
}

void RenderMesh(Mesh* me) {
  vector<Face*> faces = me->getFaces();
  vector<Vertex3f*> vt = me->getVertices();
  int limitv = vt.size();

  for (int i = faces.size() - 1; i >= 0; i--) {
    //  Set the material
    if (me->num_materials() > 0) {
      mtl = &(me->material(me->polygon2material(i)));
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mtl->ambient().x);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mtl->diffuse().x);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mtl->specular().x);
    }

    int limitf = faces[i]->vertices.size();
    glBegin(GL_POLYGON);
    for (int j = 0; j < limitf; j++) {
      Vertex3f v(*(vt[faces[i]->vertices[j]]));
      glColor3fv(v.color.c);
      glNormal3fv(v.normal.x);
      glVertex3fv(v.point.x);
    }
    glEnd();

    //  Draws normals of the faces at their vertices
    if (draw_normals) {
      glDisable(GL_LIGHTING);

      for (int j = 0; j < limitf; j++) {
        Vertex3f v(*(vt[faces[i]->vertices[j]]));
        glPushMatrix();
        glColor3f(1, 0, 0);
        glTranslatef(v.point.x[0], v.point.x[1], v.point.x[2]);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3fv(v.normal.x);
        glEnd();
        glPopMatrix();
      }
    }
    glEnable(GL_LIGHTING);
  }
}

void setEyeLight() {
  Vec3f pos = eye * zoom;
  const GLfloat light0_ambient[] = { 0.75, 0.75, 0.75, 1 };
  const GLfloat light0_diffuse[] = { 0.5, 0.5, 0.5, 1 };
  const GLfloat light0_specular[] = { 1, 1, 1, 1 };
  glLightfv(GL_LIGHT0, GL_POSITION, pos.x);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.001);
}

void setWorldLight() {
  const float pos[] = {600.0, 600.0, 600.0};

  const GLfloat light1_ambient[] = { 0.75, 0.75, 0.75, 1 };
  const GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1 };
  const GLfloat light1_specular[] = { 1, 1, 1, 1 };
  glLightfv(GL_LIGHT1, GL_POSITION, pos);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.001);
}

void moveWorldLight() {
  const float pos[] = {600.0, 600.0, 600.0};
  glLightfv(GL_LIGHT1, GL_POSITION, pos);
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
    case 'f':
      if (model == GL_FLAT) {
        model = GL_SMOOTH;
      } else {
        model = GL_FLAT;
      }
      break;
    case 'l':
      if (light == GL_LIGHT0) {
        light = GL_LIGHT1;
      } else {
        light = GL_LIGHT0;
      }
      break;
    case 'q':
    case 27:  // esc
      exit(0);
      break;
  }
  glutPostRedisplay();
}

void Idle() {
  setRotation();
  setZoom();
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
  glutIdleFunc(Idle);

  Init();

  if (string(argv[1]) == "-s") {
    cout << "Create scene" << endl;
  } else {
    string filename(argv[1]);
    cout << filename << endl;

    // Parse arguments
    for (int i = 2; i < argc; i++) {
      if (string(argv[i]) == "-l") {
        light = GL_LIGHT1;
      } else if (string(argv[i]) == "-n") {
        draw_normals = true;
      } else if (string(argv[i]) == "-a") {
        draw_axis = true;
      } else if (string(argv[i]) == "-b") {
        draw_box = true;
      } else if (string(argv[i]) == "-debug") {
        debug = true;
      } else if (string(argv[i]) == "-f") {
        model = GL_FLAT;
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
