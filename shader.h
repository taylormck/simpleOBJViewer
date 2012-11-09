/*
 * shader.h
 *
 *  Created on: Nov 9, 2012
 *      Author: taylormckinney
 */

#ifndef SHADER_H_
#define SHADER_H_

#include <string>
#include <stdio.h>

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include "./common.h"

using namespace std;

//---------------------------------------------------------------------------//
//  Initialize glew so we can use shaders
//  Added to this utility class to make things easier
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLUSEPROGRAMPROC glUseProgram;

void* glXGetProcAddress(string s) {
  const unsigned char* c = (const unsigned char*) s.c_str();
  return (void*)glXGetProcAddressARB(c);
}

// Get glew running and link functions
// Really, I should probably check to make sure that glewInit doesn't
// crash, but...
void InitGlew() {
  glewInit();
  glCreateShader = (PFNGLCREATESHADERPROC)glXGetProcAddress("glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC)glXGetProcAddress("glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC)glXGetProcAddress("glCompileShader");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)glXGetProcAddress("glCreateProgram");
  glAttachShader = (PFNGLATTACHSHADERPROC)glXGetProcAddress("glAttachShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)glXGetProcAddress("glLinkProgram");
  glDetachShader = (PFNGLDETACHSHADERPROC)glXGetProcAddress("glDetachShader");
  glDeleteProgram = (PFNGLDELETESHADERPROC)glXGetProcAddress("glDeleteProgram");
  glUseProgram = (PFNGLUSEPROGRAMPROC)glXGetProcAddress("glUseProgram");
}
//---------------------------------------------------------------------------//

class Shader {
  private:
    GLuint shader_id, shader_vp, shader_fp;

    //  Reads the file into memory
    static GLchar* readFile(const GLchar* file_name) {
      GLchar* text;

      if (file_name != NULL) {
        FILE* file = fopen(file_name, "rt");

        if (file != NULL) {
          fseek(file, 0, SEEK_END);
          GLint count = ftell(file);
          rewind(file);

          if (count > 0) {
            text = (GLchar*) malloc(sizeof(GLchar) * (count + 1));
            count = fread(text, sizeof(GLchar), count, file);
            text[count] = '\0';
          }
          fclose(file);
        }
      }
      return text;
    }

  public:
    Shader() {}
    Shader(const GLchar* vs_file, const GLchar* fs_file) {
      init(vs_file, fs_file);
    }

    ~Shader() {
      glDetachShader(shader_id, shader_fp);
      glDetachShader(shader_id, shader_vp);
      glDeleteShader(shader_fp);
      glDeleteShader(shader_vp);
      glDeleteProgram(shader_id);
    }

    void init(const GLchar* vs_file, const GLchar* fs_file) {
      shader_vp = glCreateShader(GL_VERTEX_SHADER);
      shader_fp = glCreateShader(GL_FRAGMENT_SHADER);

      const GLchar* vs_text = readFile(vs_file);
      const GLchar* fs_text = readFile(fs_file);

      if (vs_text == NULL || fs_text == NULL) {
        cerr << "Shader source not found." << endl;
        return;
      }

      glShaderSource(shader_vp, 1, &vs_text, 0);
      glShaderSource(shader_fp, 1, &fs_text, 0);

      glCompileShader(shader_vp);
      glCompileShader(shader_fp);

      shader_id = glCreateProgram();
      glAttachShader(shader_id, shader_fp);
      glAttachShader(shader_id, shader_vp);
      glLinkProgram(shader_id);
    }

    void bind() {
      glUseProgram(shader_id);
    }

    void unbind() {
      glUseProgram(0);
    }

    GLuint id() {
      return shader_id;
    }
};


#endif /* SHADER_H_ */
