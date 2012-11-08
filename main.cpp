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
void setLights();
void Outline(Mesh* me);

//---------------------------------------------------------------------------//
// window parameters
int window_width = 800, window_height = 600;
float window_aspect = window_width / static_cast<float>(window_height);
//---------------------------------------------------------------------------//
//  Variables to run options and their default values
bool draw_normals = false;
bool draw_axis = false;
bool draw_box = false;
bool debug = false;
int model = GL_SMOOTH;
int light = GL_LIGHT0;
bool cel_shade = false;
//---------------------------------------------------------------------------//
// Variables used for ArcBall Rotation and zoom
GLfloat m[16];
Vec3f eye = Vec3f::makeVec(/*0.5, 0.5, 1.25*/1, 1, 2);
Vec3f center = Vec3f::makeVec(0, 0, 0);
GLfloat cur_trans[] = {1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1};
int startx = 0, starty = 0, curx = 0, cury = 0, zoom_y0 = 0, zoom_y1 = 0;
bool lDown = false, rDown = false;
Vec3f start = Vec3f::makeVec(0, 0, 1), end = Vec3f::makeVec(0, 0, 1), rotateV;
float rotateAngle, zoom = 1.0;
const double zoomScale = 0.9f, zoomMin = 0.01, zoomMax = 100.0;
//---------------------------------------------------------------------------//
// Lighting
Vec3f light1_pos = 5.0f * eye;
const GLfloat ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
const GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
//---------------------------------------------------------------------------//
//  Mesh, material, and texture details
Mesh* mesh;
vector<Mesh*> meshes;
GLuint* texture_ids;
//---------------------------------------------------------------------------//
//  Colors definitions
#define SKY_BLUE 0.196078f, 0.6f, 0.8f, 1
#define SCARLET 0.55f, 0.09f, 0.09f, 1
#define BLACK 0, 0, 0, 1
GLfloat normals_color[] = {SCARLET};
GLfloat outline_color[] = {BLACK};
//---------------------------------------------------------------------------//
//  Cell shading ?
//  Well, at least I'll get a basic outline
GLfloat outline_width = 5.0;

void Init() {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(SKY_BLUE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_POLYGON_SMOOTH);  //  Enable anti-aliasing

  glDisable(GL_TEXTURE_2D);

  // resize the window
  window_aspect = window_width/static_cast<float>(window_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, window_aspect, 1, 1500);

  glMatrixMode(GL_MODELVIEW);
  setLights();
  BoundingBox bb = mesh->bb();

  //  This may not always make the mesh fit "perfectly" in the
  //  starting view window, but it reliably gets pretty close for
  //  pretty cheap
  Vec3f diff = bb.max - bb.min;
  float scale = max(diff.x[0], diff.x[1]);
  eye *= scale;

  center = -1.0f * (bb.center());
  glTranslatef(center.x[0], center.x[1], center.x[2]);

  glGetFloatv(GL_MODELVIEW_MATRIX, cur_trans);
}

void Display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_RESCALE_NORMAL);

  SetEye();  //  Sets the camera
  glMultMatrixf(cur_trans);

  if (draw_axis)
    DrawAxis();

  glEnable(GL_LIGHTING);
  glShadeModel(model);

  //  Enables the desired light
  //  If we are using scene lighting, replace the light in the
  //  transformed scene
  glEnable(light);
  if (light == GL_LIGHT1) {
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos.x);
  }

  if (draw_box)
    DrawBounds();

  for (int i = 0; i < meshes.size(); i++) {
    RenderMesh(meshes[i]);

    if (cel_shade)
      Outline(meshes[i]);
  }

  glDisable(light);
  glFlush();
  glutSwapBuffers();
}

void RenderMesh(Mesh* me) {
  vector<Face*> faces = me->getFaces();
  vector<Vertex3f*> verts = me->getVertices();
  vector<Vec3f*> textVerts = me->getTextureVertices();
  Material* mtl;

  bool textured = me->num_materials() > 0;
  int mtlIndex = -1;

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glEnable(GL_LIGHTING);

  if (textured) glEnable(GL_TEXTURE_2D);

  int limitf = faces.size();
  for (int i = 0; i < limitf; i++) {
    //  Set the material
    //  Only need to do this once per material
    if (textured) {
      int temp = me->polygon2material(i);
      if (mtlIndex != temp) {
        mtlIndex = temp;
        mtl = &(me->material(mtlIndex));
        glBindTexture(GL_TEXTURE_2D, mtl->texture_id());
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mtl->ambient().x);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mtl->diffuse().x);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mtl->specular().x);
        glMateriali(GL_FRONT, GL_SHININESS, mtl->specular_coeff());
      }
    }

    //  Draw the polygon
    Face* face = faces[i];
    int limitv = face->vertices.size();
    glBegin(GL_POLYGON);
    for (int j = 0; j < limitv; j++) {
      Vertex3f* v = (verts[face->vertices[j]]);
      if (textured) {
        Vec3f* vt = textVerts[face->textureVertices[j]];
        glTexCoord2fv(vt->x);
      }
      glNormal3fv(v->normal.x);
      glVertex3fv(v->point.x);
    }
    glEnd();

    //  Draws normals of the vertices
    if (draw_normals) {
      glDisable(GL_LIGHTING);
      for (int j = 0; j < limitv; j++) {
        Vertex3f* v = (verts[face->vertices[j]]);
        glColor3fv(normals_color);
        glBegin(GL_LINES);
        glVertex3fv(v->point.x);
        glVertex3fv((v->point + v->normal).x);
        glEnd();
      }
      glEnable(GL_LIGHTING);
    }
  }
  glPopAttrib();
}

//  Draws a "toony" outline
void Outline(Mesh* me) {
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  //  Only draw the edges of the back facing polygons
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glPolygonMode(GL_BACK, GL_LINE);

  glDisable(GL_LIGHTING);  //  No shading on the outline

  //  These variables are defined up top,
  //  so they can all be changed easier
  glLineWidth(outline_width);
  glColor3fv(outline_color);

  glEnable(GL_LINE_SMOOTH);  //  Anti-alias the lines

  //  This is a proper subset of the render mesh function
  //  I decided not to use the whole thing to save
  //  the extra computation
  vector<Face*> faces = me->getFaces();
  vector<Vertex3f*> verts = me->getVertices();

  int limitf = faces.size();
  for (int i = 0; i < limitf; i++) {
    //  Draw the polygon
    Face* face = faces[i];
    int limitv = face->vertices.size();
    glBegin(GL_POLYGON);
    for (int j = 0; j < limitv; j++) {
      Vertex3f* v = (verts[face->vertices[j]]);
      glVertex3fv(v->point.x);
    }
    glEnd();
  }
  glPopAttrib();
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
  BoundingBox bb = mesh->bb();
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

void DrawAxis() {
  BoundingBox bb = mesh->bb();
  Vec3f diff = bb.max - bb.min;
  glDisable(GL_LIGHTING);
  glLineWidth(4);
  const Vec3f c = {0, 0, 0};
  const float L = 0.1 * max(diff.x[0], diff.x[1]);
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
    setRotation();
  }
  if (rDown) {
    zoom_y1 = y;
    setZoom();
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
  float discriminant = xf * xf + yf * yf;

  if (discriminant <= 1.0f)
    zf = sqrt(1.0 - discriminant);

  //  Handles the case where the mouse is outside the circle
  else
    zf = 0.0f;

  return Vec3f::makeVec(xf, -1.0 * yf, zf).unit();
}

//---------------------------------------------------------------------------//
//  Sets up the rotation vector and angle, then rotates and stores
//  the new transformation matrix
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

//---------------------------------------------------------------------------//
//  Simply changes the zoom factor whenever necessary
void setZoom() {
  if (zoom_y0 != zoom_y1) {
    zoom += zoomScale * (zoom_y0 - zoom_y1) / window_width;
    if (zoom < zoomMin) zoom = zoomMin;
    else if (zoom > zoomMax) zoom = zoomMax;

    zoom_y0 = zoom_y1;
  }
}

//---------------------------------------------------------------------------//
//  Sets the eye based on the zoom factor and calls gluLookAt
void SetEye() {
  Vec3f e = (eye * zoom);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(e.x[0], e.x[1], e.x[2],  0, 0, 0,  0, 1, 0);
}

//---------------------------------------------------------------------------//
//  Initializes both of the lights
//  light0 stays at the eye, light1 moves with the scene
//  Then in Display I simply enable the desired light before drawing
//  The two lights are otherwise the same
void setLights() {
  glLightfv(GL_LIGHT0, GL_POSITION, eye.x);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.001);

  glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.001);
}

//---------------------------------------------------------------------------//
//  Handles all the keyboard input and sets options appropriately
void Keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'a':
      draw_axis = !(draw_axis);
      if (draw_axis) {
        cout << "Drawing axises" << endl;
      } else {
        cout << "Hiding axises" << endl;
      }
      break;
    case 'b':
      draw_box = !(draw_box);
      if (draw_box) {
        cout << "Drawing bounding box" << endl;
      } else {
        cout << "Hiding bounding box" << endl;
      }
      break;
    case 'n':
      draw_normals = !(draw_normals);
      if (draw_normals) {
        cout << "Drawing normals" << endl;
      } else {
        cout << "Hiding normals" << endl;
      }
      break;
    case 'f':
      if (model == GL_FLAT) {
        model = GL_SMOOTH;
        cout << "Using smooth shading" << endl;
      } else {
        model = GL_FLAT;
        cout << "Using flat shading" << endl;
      }
      break;
    case 'l':
      if (light == GL_LIGHT0) {
        light = GL_LIGHT1;
        cout << "Fixed light." << endl;
      } else {
        light = GL_LIGHT0;
        cout << "Headlight" << endl;
      }
      break;
    case 'c':
      if (cel_shade) {
        cel_shade = false;
        cout << "Not cel shading" << endl;
      } else {
        cel_shade = true;
        cout << "Cel shading" << endl;
      }
      break;
    case 'q':
    case 27:  // esc
      exit(0);
      break;
  }
  glutPostRedisplay();
}

int main(int argc, char *argv[]) {
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

  if (argc < 2) {
    cout << endl;
    cout << "Usage: ./viewer (filename.obj | -s) [-l]" << endl;
    cout << endl;
    cout << "To load data/test.obj: ./viewer data/test.obj" << endl;
    cout << "To load a custom scene: ./viewer -s" << endl;
    cout << endl;
    return 0;
  }

  vector<string> filenames;
  string filename;
  if (string(argv[1]) == "-s") {
    cout << "Create scene" << endl;
    filenames.push_back("original.obj");
    filenames.push_back("data/elk.obj");
//    filenames.push_back("data/sponza.obj");
  } else {
    filenames.push_back(string(argv[1]));
  }

  // Parse arguments
  for (int i = 2; i < argc; i++) {
    if (string(argv[i]) == "-l") {
      light = GL_LIGHT1;
      cout << "Fixed light." << endl;
    } else if (string(argv[i]) == "-n") {
      draw_normals = true;
      cout << "Drawing normals" << endl;
    } else if (string(argv[i]) == "-a") {
      draw_axis = true;
      cout << "Drawing axises" << endl;
    } else if (string(argv[i]) == "-b") {
      draw_box = true;
      cout << "Drawing bounding box" << endl;
    } else if (string(argv[i]) == "-debug") {
      debug = true;
      cout << "Showing debug statements" << endl;
    } else if (string(argv[i]) == "-f") {
      model = GL_FLAT;
      cout << "Using flat shading" << endl;
    } else if (string(argv[i]) == "-c") {
      cel_shade = true;
      cout << "Using cel shading" << endl;
    } else {  // Assume it's a filename
      filenames.push_back(string(argv[i]));
    }
  }

  int total_materials = 0;
  for (int i = 0; i < filenames.size(); ++i) {
    // Parse the obj file, compute the normals, read the textures
    mesh = new Mesh();
    ParseObj(filenames[i], *mesh);
    mesh->compute_normals();

    total_materials += mesh->num_materials();
    meshes.push_back(mesh);
  }

  texture_ids = new GLuint[total_materials];
  glGenTextures(total_materials, texture_ids);

  int texture_count = 0;
  for (int i = 0; i < meshes.size(); ++i) {
    Mesh *m = meshes[i];
    for (int j = 0; j < m->num_materials(); ++j) {
      Material& material = m->material(j);
      material.LoadTexture(texture_ids[texture_count]);
      texture_count++;
    }
  }

  Init();
  glutMainLoop();

  return 0;
}
