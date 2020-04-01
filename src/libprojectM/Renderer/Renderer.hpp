#ifndef Renderer_HPP
#define Renderer_HPP

#include "BeatDetect.hpp"
#include "Common.hpp"
#include <string>
#include <set>
#include "projectM-opengl.h"
#include "Pipeline.hpp"
#include "PerPixelMesh.hpp"
#include "Transformation.hpp"
#include "ShaderEngine.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace std::chrono;

#ifdef USE_TEXT_MENU

#define GLT_IMPLEMENTATION
#define GLT_DEBUG_PRINT
#include "gltext.h"

#endif /** USE_TEXT_MENU */

// for final composite grid:
#define FCGSX 32 // final composite gridsize - # verts - should be EVEN.
#define FCGSY 24 // final composite gridsize - # verts - should be EVEN.
                 // # of grid *cells* is two less,
                 // since we have redundant verts along the center line in X and Y (...for clean 'ang' interp)
typedef struct
{
    float x, y;     // screen position + Z-buffer depth
    float Diffuse[4];     // diffuse color
    float tu, tv;           // DYNAMIC
    float rad, ang;         // STATIC
} composite_shader_vertex;

struct Character {
    GLuint     TextureID;  // ID handle of the glyph texture
    glm::ivec2 Size;       // Size of glyph
    glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
};

class Texture;
class BeatDetect;
class TextureManager;

class Renderer
{

public:

  bool showfps;
  bool showtitle;
  bool showpreset;
  bool showhelp;
  bool showstats;

  bool studio;
  bool correction;

  bool noSwitch;

  milliseconds lastTime;
  milliseconds currentTime;

  int totalframes;
  float realfps;

  std::string title;
  int drawtitle;
  int texsizeX;
  int texsizeY;
  float m_fAspectX;
  float m_fAspectY;
  float m_fInvAspectX;
  float m_fInvAspectY;

  Renderer(int width, int height, int gx, int gy, BeatDetect *_beatDetect, std::string presetURL, std::string title_fontURL, std::string menu_fontURL, const std::string& datadir = "");
  ~Renderer();

  void RenderFrame(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void RenderFrameOnlyPass1(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void RenderFrameOnlyPass2(const Pipeline &pipeline, const PipelineContext &pipelineContext,int xoffset,int yoffset,int eye);
  void ResetTextures();
  void reset(int w, int h);
  GLuint initRenderToTexture();

  std::string SetPipeline(Pipeline &pipeline);

  void setPresetName(const std::string& theValue)
  {
    m_presetName = theValue;
  }

  std::string presetName() const
  {
    return m_presetName;
  }

  void setFPS(const int &theValue) {
		m_fps = std::to_string(theValue); 
  }

  std::string fps() const {
		return m_fps;
  }
  
private:

  PerPixelMesh mesh;
  BeatDetect *beatDetect;
  TextureManager *textureManager;
  static Pipeline* currentPipe;

#ifdef USE_TEXT_MENU

  void drawText(GLTtext* text, const char* string, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment,
                   int verticalAlignment);
  void drawText(const char* string, GLfloat x, GLfloat y, GLfloat scale, int horizontalAlignment,
                   int verticalAlignment);
  
  FT_Face titleCharacters;
  FT_Face menuCharacters;
    
  FT_Face LoadFont(const char* font);
    
#endif /** USE_TEXT_MENU */
  RenderContext renderContext;
  //per pixel equation variables
  ShaderEngine shaderEngine;
  std::string m_presetName;
  std::string m_datadir;
  std::string m_fps;

  float* p;

  int vstartx; /* view start x position - normally 0, but could be different if doing a subset of the window - like
                  for virtual reality */
  int vstarty; /* view start y position - normally 0, but could be different if doing a subset of the window - like
                  for virtual reality */
	       /* these are currently set only for rendering to the screen, not to the textbuffer */
		  
  int vw;
  int vh;

  float aspect;

  std::string title_fontURL;
  std::string menu_fontURL;
  std::string presetURL;

  GLuint m_vbo_Interpolation;
  GLuint m_vao_Interpolation;

  GLuint m_vbo_CompositeOutput;
  GLuint m_vao_CompositeOutput;

  GLuint m_vbo_CompositeShaderOutput;
  GLuint m_vao_CompositeShaderOutput;

#ifdef USE_TEXT_MENU
  GLTtext *title_font;
  GLTtext *other_font;
  GLTtext *poly_font;
#endif /** USE_TEXT_MENU */

  void SetupPass1(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void Interpolation(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void RenderItems(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void FinishPass1();
  void Pass2 (const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void CompositeShaderOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext);
  void CompositeOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext);

  inline static PixelPoint PerPixel(PixelPoint p, PerPixelContext &context)
  {
	  return currentPipe->PerPixel(p,context);
  }

  void rescale_per_pixel_matrices();

  void draw_fps();
  void draw_stats();
  void draw_help();
  void draw_preset();
  void draw_title();
  void draw_title_to_screen(bool flip);
  void draw_title_to_texture();

  int nearestPower2( int value );

  GLuint textureRenderToTexture;

  void InitCompositeShaderVertex();
  float SquishToCenter(float x, float fExp);
  void UvToMathSpace(float u, float v, float* rad, float* ang);
  composite_shader_vertex    m_comp_verts[FCGSX*FCGSY];
  int         m_comp_indices[(FCGSX-2)*(FCGSY-2)*6];

};

#endif
