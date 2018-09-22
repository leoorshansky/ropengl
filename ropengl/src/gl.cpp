#include <Rcpp.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iterator>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "linmath.h"
using namespace Rcpp;
using namespace std;

static const struct
{
  float x, y;
  float r, g, b;
} vertices[3] =
  {
  { -0.6f, -0.4f, 1.f, 0.f, 0.f },
  {  0.6f, -0.4f, 0.f, 1.f, 0.f },
  {   0.f,  0.6f, 0.f, 0.f, 1.f }
  };
static const char* vertex_shader_text =
  "uniform mat4 MVP;\n"
  "attribute vec3 vCol;\n"
  "attribute vec2 vPos;\n"
  "varying vec3 color;\n"
  "void main()\n"
  "{\n"
  "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
  "    color = vCol;\n"
  "}\n";
static const char* fragment_shader_text =
  "varying vec3 color;\n"
  "void main()\n"
  "{\n"
  "    gl_FragColor = vec4(color, 1.0);\n"
  "}\n";
static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//' @export
// [[Rcpp::export]]
int example()
{
  GLFWwindow* window;
  GLuint vertex_buffer, vertex_shader, fragment_shader, program;
  GLint mvp_location, vpos_location, vcol_location;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    exit(EXIT_FAILURE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);
  // NOTE: OpenGL error checks have been omitted for brevity
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  mvp_location = glGetUniformLocation(program, "MVP");
  vpos_location = glGetAttribLocation(program, "vPos");
  vcol_location = glGetAttribLocation(program, "vCol");
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(float) * 5, (void*) 0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(float) * 5, (void*) (sizeof(float) * 2));
  while (!glfwWindowShouldClose(window))
  {
    float ratio;
    int width, height;
    mat4x4 m, p, mvp;
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, (float) glfwGetTime());
    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);
    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

map<std::string, GLenum> GL_FLAGS;

void initFlagMap() {
  GL_FLAGS["GL_DEPTH_BUFFER_BIT"] = 0x00000100;
  GL_FLAGS["GL_STENCIL_BUFFER_BIT"] = 0x00000400;
  GL_FLAGS["GL_COLOR_BUFFER_BIT"] = 0x00004000;
  GL_FLAGS["GL_FALSE"] = 0;
  GL_FLAGS["GL_TRUE"] = 1;
  GL_FLAGS["GL_POINTS"] = 0x0000;
  GL_FLAGS["GL_LINES"] = 0x0001;
  GL_FLAGS["GL_LINE_LOOP"] = 0x0002;
  GL_FLAGS["GL_LINE_STRIP"] = 0x0003;
  GL_FLAGS["GL_TRIANGLES"] = 0x0004;
  GL_FLAGS["GL_TRIANGLE_STRIP"] = 0x0005;
  GL_FLAGS["GL_TRIANGLE_FAN"] = 0x0006;
  GL_FLAGS["GL_ZERO"] = 0;
  GL_FLAGS["GL_ONE"] = 1;
  GL_FLAGS["GL_SRC_COLOR"] = 0x0300;
  GL_FLAGS["GL_ONE_MINUS_SRC_COLOR"] = 0x0301;
  GL_FLAGS["GL_SRC_ALPHA"] = 0x0302;
  GL_FLAGS["GL_ONE_MINUS_SRC_ALPHA"] = 0x0303;
  GL_FLAGS["GL_DST_ALPHA"] = 0x0304;
  GL_FLAGS["GL_ONE_MINUS_DST_ALPHA"] = 0x0305;
  GL_FLAGS["GL_DST_COLOR"] = 0x0306;
  GL_FLAGS["GL_ONE_MINUS_DST_COLOR"] = 0x0307;
  GL_FLAGS["GL_SRC_ALPHA_SATURATE"] = 0x0308;
  GL_FLAGS["GL_FUNC_ADD"] = 0x8006;
  GL_FLAGS["GL_BLEND_EQUATION"] = 0x8009;
  GL_FLAGS["GL_BLEND_EQUATION_RGB"] = 0x8009;
  GL_FLAGS["GL_BLEND_EQUATION_ALPHA"] = 0x883D;
  GL_FLAGS["GL_FUNC_SUBTRACT"] = 0x800A;
  GL_FLAGS["GL_FUNC_REVERSE_SUBTRACT"] = 0x800B;
  GL_FLAGS["GL_BLEND_DST_RGB"] = 0x80C8;
  GL_FLAGS["GL_BLEND_SRC_RGB"] = 0x80C9;
  GL_FLAGS["GL_BLEND_DST_ALPHA"] = 0x80CA;
  GL_FLAGS["GL_BLEND_SRC_ALPHA"] = 0x80CB;
  GL_FLAGS["GL_CONSTANT_COLOR"] = 0x8001;
  GL_FLAGS["GL_ONE_MINUS_CONSTANT_COLOR"] = 0x8002;
  GL_FLAGS["GL_CONSTANT_ALPHA"] = 0x8003;
  GL_FLAGS["GL_ONE_MINUS_CONSTANT_ALPHA"] = 0x8004;
  GL_FLAGS["GL_BLEND_COLOR"] = 0x8005;
  GL_FLAGS["GL_ARRAY_BUFFER"] = 0x8892;
  GL_FLAGS["GL_ELEMENT_ARRAY_BUFFER"] = 0x8893;
  GL_FLAGS["GL_ARRAY_BUFFER_BINDING"] = 0x8894;
  GL_FLAGS["GL_ELEMENT_ARRAY_BUFFER_BINDING"] = 0x8895;
  GL_FLAGS["GL_STREAM_DRAW"] = 0x88E0;
  GL_FLAGS["GL_STATIC_DRAW"] = 0x88E4;
  GL_FLAGS["GL_DYNAMIC_DRAW"] = 0x88E8;
  GL_FLAGS["GL_BUFFER_SIZE"] = 0x8764;
  GL_FLAGS["GL_BUFFER_USAGE"] = 0x8765;
  GL_FLAGS["GL_CURRENT_VERTEX_ATTRIB"] = 0x8626;
  GL_FLAGS["GL_FRONT"] = 0x0404;
  GL_FLAGS["GL_BACK"] = 0x0405;
  GL_FLAGS["GL_FRONT_AND_BACK"] = 0x0408;
  GL_FLAGS["GL_TEXTURE_2D"] = 0x0DE1;
  GL_FLAGS["GL_CULL_FACE"] = 0x0B44;
  GL_FLAGS["GL_BLEND"] = 0x0BE2;
  GL_FLAGS["GL_DITHER"] = 0x0BD0;
  GL_FLAGS["GL_STENCIL_TEST"] = 0x0B90;
  GL_FLAGS["GL_DEPTH_TEST"] = 0x0B71;
  GL_FLAGS["GL_SCISSOR_TEST"] = 0x0C11;
  GL_FLAGS["GL_POLYGON_OFFSET_FILL"] = 0x8037;
  GL_FLAGS["GL_SAMPLE_ALPHA_TO_COVERAGE"] = 0x809E;
  GL_FLAGS["GL_SAMPLE_COVERAGE"] = 0x80A0;
  GL_FLAGS["GL_NO_ERROR"] = 0;
  GL_FLAGS["GL_INVALID_ENUM"] = 0x0500;
  GL_FLAGS["GL_INVALID_VALUE"] = 0x0501;
  GL_FLAGS["GL_INVALID_OPERATION"] = 0x0502;
  GL_FLAGS["GL_OUT_OF_MEMORY"] = 0x0505;
  GL_FLAGS["GL_CW"] = 0x0900;
  GL_FLAGS["GL_CCW"] = 0x0901;
  GL_FLAGS["GL_LINE_WIDTH"] = 0x0B21;
  GL_FLAGS["GL_ALIASED_POINT_SIZE_RANGE"] = 0x846D;
  GL_FLAGS["GL_ALIASED_LINE_WIDTH_RANGE"] = 0x846E;
  GL_FLAGS["GL_CULL_FACE_MODE"] = 0x0B45;
  GL_FLAGS["GL_FRONT_FACE"] = 0x0B46;
  GL_FLAGS["GL_DEPTH_RANGE"] = 0x0B70;
  GL_FLAGS["GL_DEPTH_WRITEMASK"] = 0x0B72;
  GL_FLAGS["GL_DEPTH_CLEAR_VALUE"] = 0x0B73;
  GL_FLAGS["GL_DEPTH_FUNC"] = 0x0B74;
  GL_FLAGS["GL_STENCIL_CLEAR_VALUE"] = 0x0B91;
  GL_FLAGS["GL_STENCIL_FUNC"] = 0x0B92;
  GL_FLAGS["GL_STENCIL_FAIL"] = 0x0B94;
  GL_FLAGS["GL_STENCIL_PASS_DEPTH_FAIL"] = 0x0B95;
  GL_FLAGS["GL_STENCIL_PASS_DEPTH_PASS"] = 0x0B96;
  GL_FLAGS["GL_STENCIL_REF"] = 0x0B97;
  GL_FLAGS["GL_STENCIL_VALUE_MASK"] = 0x0B93;
  GL_FLAGS["GL_STENCIL_WRITEMASK"] = 0x0B98;
  GL_FLAGS["GL_STENCIL_BACK_FUNC"] = 0x8800;
  GL_FLAGS["GL_STENCIL_BACK_FAIL"] = 0x8801;
  GL_FLAGS["GL_STENCIL_BACK_PASS_DEPTH_FAIL"] = 0x8802;
  GL_FLAGS["GL_STENCIL_BACK_PASS_DEPTH_PASS"] = 0x8803;
  GL_FLAGS["GL_STENCIL_BACK_REF"] = 0x8CA3;
  GL_FLAGS["GL_STENCIL_BACK_VALUE_MASK"] = 0x8CA4;
  GL_FLAGS["GL_STENCIL_BACK_WRITEMASK"] = 0x8CA5;
  GL_FLAGS["GL_VIEWPORT"] = 0x0BA2;
  GL_FLAGS["GL_SCISSOR_BOX"] = 0x0C10;
  GL_FLAGS["GL_COLOR_CLEAR_VALUE"] = 0x0C22;
  GL_FLAGS["GL_COLOR_WRITEMASK"] = 0x0C23;
  GL_FLAGS["GL_UNPACK_ALIGNMENT"] = 0x0CF5;
  GL_FLAGS["GL_PACK_ALIGNMENT"] = 0x0D05;
  GL_FLAGS["GL_MAX_TEXTURE_SIZE"] = 0x0D33;
  GL_FLAGS["GL_MAX_VIEWPORT_DIMS"] = 0x0D3A;
  GL_FLAGS["GL_SUBPIXEL_BITS"] = 0x0D50;
  GL_FLAGS["GL_RED_BITS"] = 0x0D52;
  GL_FLAGS["GL_GREEN_BITS"] = 0x0D53;
  GL_FLAGS["GL_BLUE_BITS"] = 0x0D54;
  GL_FLAGS["GL_ALPHA_BITS"] = 0x0D55;
  GL_FLAGS["GL_DEPTH_BITS"] = 0x0D56;
  GL_FLAGS["GL_STENCIL_BITS"] = 0x0D57;
  GL_FLAGS["GL_POLYGON_OFFSET_UNITS"] = 0x2A00;
  GL_FLAGS["GL_POLYGON_OFFSET_FACTOR"] = 0x8038;
  GL_FLAGS["GL_TEXTURE_BINDING_2D"] = 0x8069;
  GL_FLAGS["GL_SAMPLE_BUFFERS"] = 0x80A8;
  GL_FLAGS["GL_SAMPLES"] = 0x80A9;
  GL_FLAGS["GL_SAMPLE_COVERAGE_VALUE"] = 0x80AA;
  GL_FLAGS["GL_SAMPLE_COVERAGE_INVERT"] = 0x80AB;
  GL_FLAGS["GL_NUM_COMPRESSED_TEXTURE_FORMATS"] = 0x86A2;
  GL_FLAGS["GL_COMPRESSED_TEXTURE_FORMATS"] = 0x86A3;
  GL_FLAGS["GL_DONT_CARE"] = 0x1100;
  GL_FLAGS["GL_FASTEST"] = 0x1101;
  GL_FLAGS["GL_NICEST"] = 0x1102;
  GL_FLAGS["GL_GENERATE_MIPMAP_HINT"] = 0x8192;
  GL_FLAGS["GL_BYTE"] = 0x1400;
  GL_FLAGS["GL_UNSIGNED_BYTE"] = 0x1401;
  GL_FLAGS["GL_SHORT"] = 0x1402;
  GL_FLAGS["GL_UNSIGNED_SHORT"] = 0x1403;
  GL_FLAGS["GL_INT"] = 0x1404;
  GL_FLAGS["GL_UNSIGNED_INT"] = 0x1405;
  GL_FLAGS["GL_FLOAT"] = 0x1406;
  GL_FLAGS["GL_FIXED"] = 0x140C;
  GL_FLAGS["GL_DEPTH_COMPONENT"] = 0x1902;
  GL_FLAGS["GL_ALPHA"] = 0x1906;
  GL_FLAGS["GL_RGB"] = 0x1907;
  GL_FLAGS["GL_RGBA"] = 0x1908;
  GL_FLAGS["GL_LUMINANCE"] = 0x1909;
  GL_FLAGS["GL_LUMINANCE_ALPHA"] = 0x190A;
  GL_FLAGS["GL_UNSIGNED_SHORT_4_4_4_4"] = 0x8033;
  GL_FLAGS["GL_UNSIGNED_SHORT_5_5_5_1"] = 0x8034;
  GL_FLAGS["GL_UNSIGNED_SHORT_5_6_5"] = 0x8363;
  GL_FLAGS["GL_FRAGMENT_SHADER"] = 0x8B30;
  GL_FLAGS["GL_VERTEX_SHADER"] = 0x8B31;
  GL_FLAGS["GL_MAX_VERTEX_ATTRIBS"] = 0x8869;
  GL_FLAGS["GL_MAX_VERTEX_UNIFORM_VECTORS"] = 0x8DFB;
  GL_FLAGS["GL_MAX_VARYING_VECTORS"] = 0x8DFC;
  GL_FLAGS["GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS"] = 0x8B4D;
  GL_FLAGS["GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS"] = 0x8B4C;
  GL_FLAGS["GL_MAX_TEXTURE_IMAGE_UNITS"] = 0x8872;
  GL_FLAGS["GL_MAX_FRAGMENT_UNIFORM_VECTORS"] = 0x8DFD;
  GL_FLAGS["GL_SHADER_TYPE"] = 0x8B4F;
  GL_FLAGS["GL_DELETE_STATUS"] = 0x8B80;
  GL_FLAGS["GL_LINK_STATUS"] = 0x8B82;
  GL_FLAGS["GL_VALIDATE_STATUS"] = 0x8B83;
  GL_FLAGS["GL_ATTACHED_SHADERS"] = 0x8B85;
  GL_FLAGS["GL_ACTIVE_UNIFORMS"] = 0x8B86;
  GL_FLAGS["GL_ACTIVE_UNIFORM_MAX_LENGTH"] = 0x8B87;
  GL_FLAGS["GL_ACTIVE_ATTRIBUTES"] = 0x8B89;
  GL_FLAGS["GL_ACTIVE_ATTRIBUTE_MAX_LENGTH"] = 0x8B8A;
  GL_FLAGS["GL_SHADING_LANGUAGE_VERSION"] = 0x8B8C;
  GL_FLAGS["GL_CURRENT_PROGRAM"] = 0x8B8D;
  GL_FLAGS["GL_NEVER"] = 0x0200;
  GL_FLAGS["GL_LESS"] = 0x0201;
  GL_FLAGS["GL_EQUAL"] = 0x0202;
  GL_FLAGS["GL_LEQUAL"] = 0x0203;
  GL_FLAGS["GL_GREATER"] = 0x0204;
  GL_FLAGS["GL_NOTEQUAL"] = 0x0205;
  GL_FLAGS["GL_GEQUAL"] = 0x0206;
  GL_FLAGS["GL_ALWAYS"] = 0x0207;
  GL_FLAGS["GL_KEEP"] = 0x1E00;
  GL_FLAGS["GL_REPLACE"] = 0x1E01;
  GL_FLAGS["GL_INCR"] = 0x1E02;
  GL_FLAGS["GL_DECR"] = 0x1E03;
  GL_FLAGS["GL_INVERT"] = 0x150A;
  GL_FLAGS["GL_INCR_WRAP"] = 0x8507;
  GL_FLAGS["GL_DECR_WRAP"] = 0x8508;
  GL_FLAGS["GL_VENDOR"] = 0x1F00;
  GL_FLAGS["GL_RENDERER"] = 0x1F01;
  GL_FLAGS["GL_VERSION"] = 0x1F02;
  GL_FLAGS["GL_EXTENSIONS"] = 0x1F03;
  GL_FLAGS["GL_NEAREST"] = 0x2600;
  GL_FLAGS["GL_LINEAR"] = 0x2601;
  GL_FLAGS["GL_NEAREST_MIPMAP_NEAREST"] = 0x2700;
  GL_FLAGS["GL_LINEAR_MIPMAP_NEAREST"] = 0x2701;
  GL_FLAGS["GL_NEAREST_MIPMAP_LINEAR"] = 0x2702;
  GL_FLAGS["GL_LINEAR_MIPMAP_LINEAR"] = 0x2703;
  GL_FLAGS["GL_TEXTURE_MAG_FILTER"] = 0x2800;
  GL_FLAGS["GL_TEXTURE_MIN_FILTER"] = 0x2801;
  GL_FLAGS["GL_TEXTURE_WRAP_S"] = 0x2802;
  GL_FLAGS["GL_TEXTURE_WRAP_T"] = 0x2803;
  GL_FLAGS["GL_TEXTURE"] = 0x1702;
  GL_FLAGS["GL_TEXTURE_CUBE_MAP"] = 0x8513;
  GL_FLAGS["GL_TEXTURE_BINDING_CUBE_MAP"] = 0x8514;
  GL_FLAGS["GL_TEXTURE_CUBE_MAP_POSITIVE_X"] = 0x8515;
  GL_FLAGS["GL_TEXTURE_CUBE_MAP_NEGATIVE_X"] = 0x8516;
  GL_FLAGS["GL_TEXTURE_CUBE_MAP_POSITIVE_Y"] = 0x8517;
  GL_FLAGS["GL_TEXTURE_CUBE_MAP_NEGATIVE_Y"] = 0x8518;
  GL_FLAGS["GL_TEXTURE_CUBE_MAP_POSITIVE_Z"] = 0x8519;
  GL_FLAGS["GL_TEXTURE_CUBE_MAP_NEGATIVE_Z"] = 0x851A;
  GL_FLAGS["GL_MAX_CUBE_MAP_TEXTURE_SIZE"] = 0x851C;
  GL_FLAGS["GL_TEXTURE0"] = 0x84C0;
  GL_FLAGS["GL_TEXTURE1"] = 0x84C1;
  GL_FLAGS["GL_TEXTURE2"] = 0x84C2;
  GL_FLAGS["GL_TEXTURE3"] = 0x84C3;
  GL_FLAGS["GL_TEXTURE4"] = 0x84C4;
  GL_FLAGS["GL_TEXTURE5"] = 0x84C5;
  GL_FLAGS["GL_TEXTURE6"] = 0x84C6;
  GL_FLAGS["GL_TEXTURE7"] = 0x84C7;
  GL_FLAGS["GL_TEXTURE8"] = 0x84C8;
  GL_FLAGS["GL_TEXTURE9"] = 0x84C9;
  GL_FLAGS["GL_TEXTURE10"] = 0x84CA;
  GL_FLAGS["GL_TEXTURE11"] = 0x84CB;
  GL_FLAGS["GL_TEXTURE12"] = 0x84CC;
  GL_FLAGS["GL_TEXTURE13"] = 0x84CD;
  GL_FLAGS["GL_TEXTURE14"] = 0x84CE;
  GL_FLAGS["GL_TEXTURE15"] = 0x84CF;
  GL_FLAGS["GL_TEXTURE16"] = 0x84D0;
  GL_FLAGS["GL_TEXTURE17"] = 0x84D1;
  GL_FLAGS["GL_TEXTURE18"] = 0x84D2;
  GL_FLAGS["GL_TEXTURE19"] = 0x84D3;
  GL_FLAGS["GL_TEXTURE20"] = 0x84D4;
  GL_FLAGS["GL_TEXTURE21"] = 0x84D5;
  GL_FLAGS["GL_TEXTURE22"] = 0x84D6;
  GL_FLAGS["GL_TEXTURE23"] = 0x84D7;
  GL_FLAGS["GL_TEXTURE24"] = 0x84D8;
  GL_FLAGS["GL_TEXTURE25"] = 0x84D9;
  GL_FLAGS["GL_TEXTURE26"] = 0x84DA;
  GL_FLAGS["GL_TEXTURE27"] = 0x84DB;
  GL_FLAGS["GL_TEXTURE28"] = 0x84DC;
  GL_FLAGS["GL_TEXTURE29"] = 0x84DD;
  GL_FLAGS["GL_TEXTURE30"] = 0x84DE;
  GL_FLAGS["GL_TEXTURE31"] = 0x84DF;
  GL_FLAGS["GL_ACTIVE_TEXTURE"] = 0x84E0;
  GL_FLAGS["GL_REPEAT"] = 0x2901;
  GL_FLAGS["GL_CLAMP_TO_EDGE"] = 0x812F;
  GL_FLAGS["GL_MIRRORED_REPEAT"] = 0x8370;
  GL_FLAGS["GL_FLOAT_VEC2"] = 0x8B50;
  GL_FLAGS["GL_FLOAT_VEC3"] = 0x8B51;
  GL_FLAGS["GL_FLOAT_VEC4"] = 0x8B52;
  GL_FLAGS["GL_INT_VEC2"] = 0x8B53;
  GL_FLAGS["GL_INT_VEC3"] = 0x8B54;
  GL_FLAGS["GL_INT_VEC4"] = 0x8B55;
  GL_FLAGS["GL_BOOL"] = 0x8B56;
  GL_FLAGS["GL_BOOL_VEC2"] = 0x8B57;
  GL_FLAGS["GL_BOOL_VEC3"] = 0x8B58;
  GL_FLAGS["GL_BOOL_VEC4"] = 0x8B59;
  GL_FLAGS["GL_FLOAT_MAT2"] = 0x8B5A;
  GL_FLAGS["GL_FLOAT_MAT3"] = 0x8B5B;
  GL_FLAGS["GL_FLOAT_MAT4"] = 0x8B5C;
  GL_FLAGS["GL_SAMPLER_2D"] = 0x8B5E;
  GL_FLAGS["GL_SAMPLER_CUBE"] = 0x8B60;
  GL_FLAGS["GL_VERTEX_ATTRIB_ARRAY_ENABLED"] = 0x8622;
  GL_FLAGS["GL_VERTEX_ATTRIB_ARRAY_SIZE"] = 0x8623;
  GL_FLAGS["GL_VERTEX_ATTRIB_ARRAY_STRIDE"] = 0x8624;
  GL_FLAGS["GL_VERTEX_ATTRIB_ARRAY_TYPE"] = 0x8625;
  GL_FLAGS["GL_VERTEX_ATTRIB_ARRAY_NORMALIZED"] = 0x886A;
  GL_FLAGS["GL_VERTEX_ATTRIB_ARRAY_POINTER"] = 0x8645;
  GL_FLAGS["GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING"] = 0x889F;
  GL_FLAGS["GL_IMPLEMENTATION_COLOR_READ_TYPE"] = 0x8B9A;
  GL_FLAGS["GL_IMPLEMENTATION_COLOR_READ_FORMAT"] = 0x8B9B;
  GL_FLAGS["GL_COMPILE_STATUS"] = 0x8B81;
  GL_FLAGS["GL_INFO_LOG_LENGTH"] = 0x8B84;
  GL_FLAGS["GL_SHADER_SOURCE_LENGTH"] = 0x8B88;
  GL_FLAGS["GL_SHADER_COMPILER"] = 0x8DFA;
  GL_FLAGS["GL_SHADER_BINARY_FORMATS"] = 0x8DF8;
  GL_FLAGS["GL_NUM_SHADER_BINARY_FORMATS"] = 0x8DF9;
  GL_FLAGS["GL_LOW_FLOAT"] = 0x8DF0;
  GL_FLAGS["GL_MEDIUM_FLOAT"] = 0x8DF1;
  GL_FLAGS["GL_HIGH_FLOAT"] = 0x8DF2;
  GL_FLAGS["GL_LOW_INT"] = 0x8DF3;
  GL_FLAGS["GL_MEDIUM_INT"] = 0x8DF4;
  GL_FLAGS["GL_HIGH_INT"] = 0x8DF5;
  GL_FLAGS["GL_FRAMEBUFFER"] = 0x8D40;
  GL_FLAGS["GL_RENDERBUFFER"] = 0x8D41;
  GL_FLAGS["GL_RGBA4"] = 0x8056;
  GL_FLAGS["GL_RGB5_A1"] = 0x8057;
  GL_FLAGS["GL_RGB565"] = 0x8D62;
  GL_FLAGS["GL_DEPTH_COMPONENT16"] = 0x81A5;
  GL_FLAGS["GL_STENCIL_INDEX8"] = 0x8D48;
  GL_FLAGS["GL_RENDERBUFFER_WIDTH"] = 0x8D42;
  GL_FLAGS["GL_RENDERBUFFER_HEIGHT"] = 0x8D43;
  GL_FLAGS["GL_RENDERBUFFER_INTERNAL_FORMAT"] = 0x8D44;
  GL_FLAGS["GL_RENDERBUFFER_RED_SIZE"] = 0x8D50;
  GL_FLAGS["GL_RENDERBUFFER_GREEN_SIZE"] = 0x8D51;
  GL_FLAGS["GL_RENDERBUFFER_BLUE_SIZE"] = 0x8D52;
  GL_FLAGS["GL_RENDERBUFFER_ALPHA_SIZE"] = 0x8D53;
  GL_FLAGS["GL_RENDERBUFFER_DEPTH_SIZE"] = 0x8D54;
  GL_FLAGS["GL_RENDERBUFFER_STENCIL_SIZE"] = 0x8D55;
  GL_FLAGS["GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE"] = 0x8CD0;
  GL_FLAGS["GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME"] = 0x8CD1;
  GL_FLAGS["GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL"] = 0x8CD2;
  GL_FLAGS["GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE"] = 0x8CD3;
  GL_FLAGS["GL_COLOR_ATTACHMENT0"] = 0x8CE0;
  GL_FLAGS["GL_DEPTH_ATTACHMENT"] = 0x8D00;
  GL_FLAGS["GL_STENCIL_ATTACHMENT"] = 0x8D20;
  GL_FLAGS["GL_NONE"] = 0;
  GL_FLAGS["BUFFER_COMPLETE"] = 0x8CD5;
  GL_FLAGS["GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"] = 0x8CD6;
  GL_FLAGS["GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"] = 0x8CD7;
  GL_FLAGS["GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS"] = 0x8CD9;
  GL_FLAGS["GL_FRAMEBUFFER_UNSUPPORTED"] = 0x8CDD;
  GL_FLAGS["GL_FRAMEBUFFER_BINDING"] = 0x8CA6;
  GL_FLAGS["GL_RENDERBUFFER_BINDING"] = 0x8CA7;
  GL_FLAGS["GL_MAX_RENDERBUFFER_SIZE"] = 0x84E8;
  GL_FLAGS["GL_INVALID_FRAMEBUFFER_OPERATION"] = 0x0506;
}

//' @export
// [[Rcpp::export]]
std::string readFile (std::string name) {
  ifstream file;
  std::string output = "";
  file.open(name.c_str());
  if(!file) {
    return "FILE COULD NOT BE OPENED";
  }
  while(file.good()) {
    std::string line;
    getline(file, line);
    output += line;
  }
  return output;
}

std::map<std::string, int> keys;

//' @export
// [[Rcpp::export(name = "glfwInit")]]
int myGlfwInit() {
  return glfwInit();
}

GLFWwindow * window;

//' @export
// [[Rcpp::export]]
void flip(){
  glfwSwapBuffers(window);
  glfwPollEvents();
}

NumericVector asNumericVector(glm::vec3 vec, int len = 3) {
  NumericVector outVec (len);
  for (int i = 0; i < len; i++) {
    outVec(i) = vec[i];
  }
  return outVec;
}

NumericMatrix asNumericMatrix(glm::mat4 mat, int col = 4, int row = 4) {
  NumericMatrix outMat (col, row);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      outMat(i, j) = mat[i][j];
    }
  }
  return outMat;
}

glm::vec3 asVec3(NumericVector vec) {
  glm::vec3 outVec = glm::vec3();
  for (int i = 0; i < 3; i++) {
    outVec[i] = vec(i);
  }
  return outVec;
}

glm::mat4 asMat4(NumericMatrix mat) {
  glm::mat4 outMat = glm::mat4();
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      outMat[i][j] = mat(i, j);
    }
  }
  return outMat;
}

//' @export
// [[Rcpp::export]]
NumericMatrix ortho (float left, float right, float bottom, float top, float zNear, float zFar) {
  return asNumericMatrix(glm::ortho(left, right, bottom, top, zNear, zFar));
}

//' @export
// [[Rcpp::export]]
NumericMatrix perspective (float fov, float aspect, float zNear, float zFar) {
  return asNumericMatrix(glm::perspective(fov, aspect, zNear, zFar));
}

//' @export
// [[Rcpp::export]]
NumericMatrix frustum (float left, float right, float bottom, float top, float nearVal, float farVal) {
  return asNumericMatrix(glm::frustum(left, right, bottom, top, nearVal, farVal));
}

//' @export
// [[Rcpp::export]]
NumericMatrix identity () {
  return asNumericMatrix(glm::mat4(1.0f));
}

//' @export
// [[Rcpp::export]]
NumericMatrix lookAt (NumericVector eye, NumericVector center, NumericVector up) {
  return asNumericMatrix(glm::lookAt(asVec3(eye), asVec3(center), asVec3(up)));
}

//' @export
// [[Rcpp::export]]
NumericMatrix scale (NumericMatrix mat, NumericVector vec) {
  return asNumericMatrix(glm::scale(asMat4(mat), asVec3(vec)));
}

//' @export
// [[Rcpp::export]]
NumericMatrix translate (NumericMatrix mat, NumericVector vec) {
  return asNumericMatrix(glm::translate(asMat4(mat), asVec3(vec)));
}

//' @export
// [[Rcpp::export]]
NumericMatrix rotate (NumericMatrix mat, float angle, NumericVector vec) {
  return asNumericMatrix(glm::rotate(asMat4(mat), angle, asVec3(vec)));
}

//' @export
// [[Rcpp::export]]
NumericVector normalize (NumericVector vec) {
  return asNumericVector(glm::normalize(asVec3(vec)));
}

//' @export
// [[Rcpp::export]]
NumericVector cross (NumericVector a, NumericVector b) {
  return asNumericVector(glm::cross(asVec3(a), asVec3(b)));
}

//' @export
// [[Rcpp::export]]
int getKeyId (std::string name) {
  return keys[name];
}

//' @export
// [[Rcpp::export]]
int getKeyByName(std::string name) {
  return glfwGetKey(window, getKeyId(name));
}

//' @export
// [[Rcpp::export(name = "getKeyName")]]
std::string myGlfwGetKeyName (int id) {
  std::string name = (std::string) glfwGetKeyName(id, 0);
  return name;
}

//' @export
// [[Rcpp::export]]
int getKeyById(int id) {
  return glfwGetKey(window, id);
}

//' @export
// [[Rcpp::export]]
void initKeyMap() {
  std::vector<int> ids {32, 39, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 59, 61, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 96, 161, 162, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 280, 281, 282, 283, 284, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335, 336, 340, 341, 342, 343, 344, 345, 346, 347, 348};
  for(unsigned int i = 0; i < ids.size(); i++) {
    if(glfwGetKeyName(ids[i], 0) == NULL) {
      std::string name = "NULL";
      keys[name] = ids[i];
    }
    else {
      std::string name = (std::string) glfwGetKeyName(ids[i], 0);
      keys[name] = ids[i];
    }
  }
}

//' @export
// [[Rcpp::export]]
void hideCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

//' @export
// [[Rcpp::export]]
void showCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

int leftMouseButton;
int rightMouseButton;
int middleMouseButton;
double scrollX;
double scrollY;
double mouseX;
double mouseY;

void mouseButtonCallback (GLFWwindow * window, int button, int action, int mods) {
  if(button == GLFW_MOUSE_BUTTON_LEFT) {
    leftMouseButton = action;
  }
  else if(button == GLFW_MOUSE_BUTTON_RIGHT) {
    rightMouseButton = action;
  }
  else if(button == GLFW_MOUSE_BUTTON_MIDDLE) {
    middleMouseButton = action;
  }
}

//' @export
// [[Rcpp::export]]
std::vector<double> getCursorPos() {
  return std::vector<double> {mouseX, mouseY};
}

//' @export
// [[Rcpp::export]]
void setCursorPos(double x, double y) {
  mouseX = x;
  mouseY = y;
  glfwSetCursorPos(window, x, y);
}

void scrollCallback(GLFWwindow * window, double x, double y) {
  scrollX = x;
  scrollY = y;
}

void cursorCallback(GLFWwindow * window, double x, double y) {
  mouseX = x;
  mouseY = y;
}

//' @export
// [[Rcpp::export]]
std::vector<int> getMouseButtons() {
  return std::vector<int> {leftMouseButton, middleMouseButton, rightMouseButton};
}

//' @export
// [[Rcpp::export]]
std::vector<double> getScrollWheel() {
  return std::vector<double> {scrollX, scrollY};
}

//' Create new graphics window
//' @param x desired window width
//' @param y desired window height
//' @param name desired window name
//' @param fullScreen whether or not window is full screen
//' @export
// [[Rcpp::export]]
void createWindow(int x, int y, const char* name, bool fullScreen = false) {
  /* Initialize Module */
  glfwInit();
  initFlagMap();
  initKeyMap();
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  /* Create a windowed mode window and its OpenGL context */
  if(fullScreen) {
    window = glfwCreateWindow(x, y, name, glfwGetPrimaryMonitor(), NULL);
  }
  else {
    window = glfwCreateWindow(x, y, name, NULL, NULL);
  }
  
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  
  glfwSwapInterval(1);
  
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetCursorPosCallback(window, cursorCallback);
}

//' Focus input on graphics window
//' 
//' @export
// [[Rcpp::export]]
void focusWindow() {
  glfwFocusWindow(window);
}

//' Close graphics window
//' 
//' @export
// [[Rcpp::export]]
void closeWindow() {
  glfwDestroyWindow(window);
}

//' Is (x) button clicked on graphics window
//' 
//' @export
// [[Rcpp::export]]
bool shouldWindowClose() {
  return glfwWindowShouldClose(window);
}

//' @export
// [[Rcpp::export]]
unsigned int getFlag (std::string a) {
  return GL_FLAGS[a];
}

//' @export
// [[Rcpp::export]]
unsigned int getFlagsOR (std::string a, std::string b) {
  return GL_FLAGS[a] | GL_FLAGS[b];
}

//' @export
// [[Rcpp::export(name = 'glShaderSource')]]
void myGlShaderSource(unsigned int shader, std::string a, int count = 1) {
  char * r = &a[0u];
  char * const s = r;
  char * const * e = &s;
  glShaderSource(shader, count, e, NULL);
}

//' @export
// [[Rcpp::export(name = "glGenVertexArrays")]]
std::vector<int> myGlGenVertexArrays (int n) {
  
  GLuint arrays[n];
  glGenVertexArrays(n, arrays);
  return std::vector<int>(arrays, arrays + sizeof arrays / sizeof arrays[0]);

}

//' @export
// [[Rcpp::export(name = "glDeleteVertexArrays")]]
void myGlDeleteVertexArrays (int n, std::vector<unsigned int> arrays) {
  
  glDeleteVertexArrays(n, &arrays[0]);
  
}

//' @export
// [[Rcpp::export(name = "glBindVertexArray")]]
void myGlBindVertexArray (unsigned int array) {
  glBindVertexArray(array);
}

//' @export
// [[Rcpp::export(name = 'glActiveTexture')]]
void myGlActiveTexture(unsigned int texture) {
  
  glActiveTexture(texture);
  
}

//' @export
// [[Rcpp::export(name = 'glAttachShader')]]
void myGlAttachShader(unsigned int program, unsigned int shader) {
  
  glAttachShader(program, shader);
  
}

//' @export
// [[Rcpp::export(name = 'glBindAttribLocation')]]
void myGlBindAttribLocation(unsigned int program, unsigned int index, std::string name) {
  
  glBindAttribLocation(program, index,  & name[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glBindBuffer')]]
void myGlBindBuffer(unsigned int target, unsigned int buffer) {
  
  glBindBuffer(target, buffer);
  
}

//' @export
// [[Rcpp::export(name = 'glBindFramebuffer')]]
void myGlBindFramebuffer(unsigned int target, unsigned int framebuffer) {
  
  glBindFramebuffer(target, framebuffer);
  
}

//' @export
// [[Rcpp::export(name = 'glBindRenderbuffer')]]
void myGlBindRenderbuffer(unsigned int target, unsigned int renderbuffer) {
  
  glBindRenderbuffer(target, renderbuffer);
  
}

//' @export
// [[Rcpp::export(name = 'glBindTexture')]]
void myGlBindTexture(unsigned int target, unsigned int texture) {
  
  glBindTexture(target, texture);
  
}

//' @export
// [[Rcpp::export(name = 'glBlendColor')]]
void myGlBlendColor(float red, float green, float blue, float alpha) {
  
  glBlendColor(red, green, blue, alpha);
  
}

//' @export
// [[Rcpp::export(name = 'glBlendEquation')]]
void myGlBlendEquation(unsigned int mode) {
  
  glBlendEquation(mode);
  
}

//' @export
// [[Rcpp::export(name = 'glBlendEquationSeparate')]]
void myGlBlendEquationSeparate(unsigned int modeRGB, unsigned int modeAlpha) {
  
  glBlendEquationSeparate(modeRGB, modeAlpha);
  
}

//' @export
// [[Rcpp::export(name = 'glBlendFunc')]]
void myGlBlendFunc(unsigned int sfactor, unsigned int dfactor) {
  
  glBlendFunc(sfactor, dfactor);
  
}

//' @export
// [[Rcpp::export(name = 'glBlendFuncSeparate')]]
void myGlBlendFuncSeparate(unsigned int sfactorRGB, unsigned int dfactorRGB, unsigned int sfactorAlpha, unsigned int dfactorAlpha) {
  
  glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
  
}

//' @export
// [[Rcpp::export(name = 'glBufferData')]]
void myGlBufferData(unsigned int target, std::vector<float> data, unsigned int usage, bool elementArray = false) {
  if(elementArray) {
    GLsizeiptr size = data.size() * sizeof(data[0]);
    std::vector<unsigned int> yeet (data.begin(), data.end());
    glBufferData(target, size, & yeet[0], usage);
  }
  else {
    GLsizeiptr size = data.size() * sizeof(data[0]);
    glBufferData(target, size,  & data[0], usage);
  }
}

//' @export
// [[Rcpp::export(name = 'glBufferSubData')]]
void myGlBufferSubData(unsigned int target, long long int offset, long long int size, std::vector<float> data) {
  
  glBufferSubData(target, offset, size,  & data[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glCheckFramebufferStatus')]]
unsigned int myGlCheckFramebufferStatus(unsigned int target) {
  
  return glCheckFramebufferStatus(target);
  
}

//' @export
// [[Rcpp::export(name = 'glClear')]]
void myGlClear(unsigned int mask) {
  
  glClear(mask);
  
}

//' @export
// [[Rcpp::export(name = 'glClearColor')]]
void myGlClearColor(float red, float green, float blue, float alpha) {
  
  glClearColor(red, green, blue, alpha);
  
}

//' @export
// [[Rcpp::export(name = 'glClearDepthf')]]
void myGlClearDepthf(float d) {
  
  glClearDepthf(d);
  
}

//' @export
// [[Rcpp::export(name = 'glClearStencil')]]
void myGlClearStencil(int s) {
  
  glClearStencil(s);
  
}

//' @export
// [[Rcpp::export(name = 'glColorMask')]]
void myGlColorMask(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
  
  glColorMask(red, green, blue, alpha);
  
}

//' @export
// [[Rcpp::export(name = 'glCompileShader')]]
void myGlCompileShader(unsigned int shader) {
  
  glCompileShader(shader);
  
}

//' @export
// [[Rcpp::export(name = 'glCompressedTexImage2D')]]
void myGlCompressedTexImage2D(unsigned int target, int level, unsigned int internalformat, int width, int height, int border, int imageSize, std::vector<float> data) {
  
  glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize,  & data[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glCompressedTexSubImage2D')]]
void myGlCompressedTexSubImage2D(unsigned int target, int level, int xoffset, int yoffset, int width, int height, unsigned int format, int imageSize, std::vector<float> data) {
  
  glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize,  & data[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glCopyTexImage2D')]]
void myGlCopyTexImage2D(unsigned int target, int level, unsigned int internalformat, int x, int y, int width, int height, int border) {
  
  glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
  
}

//' @export
// [[Rcpp::export(name = 'glCopyTexSubImage2D')]]
void myGlCopyTexSubImage2D(unsigned int target, int level, int xoffset, int yoffset, int x, int y, int width, int height) {
  
  glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
  
}

//' @export
// [[Rcpp::export(name = 'glCreateProgram')]]
unsigned int myGlCreateProgram() {
  
  return glCreateProgram();
  
}

//' @export
// [[Rcpp::export(name = 'glCreateShader')]]
unsigned int myGlCreateShader(unsigned int type) {
  
  return glCreateShader(type);
  
}

//' @export
// [[Rcpp::export(name = 'glCullFace')]]
void myGlCullFace(unsigned int mode) {
  
  glCullFace(mode);
  
}

//' @export
// [[Rcpp::export(name = 'glDeleteBuffers')]]
void myGlDeleteBuffers(int n, std::vector<unsigned int> buffers) {
  
  glDeleteBuffers(n,  & buffers[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glDeleteFramebuffers')]]
void myGlDeleteFramebuffers(int n, std::vector<unsigned int> framebuffers) {
  
  glDeleteFramebuffers(n,  & framebuffers[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glDeleteProgram')]]
void myGlDeleteProgram(unsigned int program) {
  
  glDeleteProgram(program);
  
}

//' @export
// [[Rcpp::export(name = 'glDeleteRenderbuffers')]]
void myGlDeleteRenderbuffers(int n, std::vector<unsigned int> renderbuffers) {
  
  glDeleteRenderbuffers(n,  & renderbuffers[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glDeleteShader')]]
void myGlDeleteShader(unsigned int shader) {
  
  glDeleteShader(shader);
  
}

//' @export
// [[Rcpp::export(name = 'glDeleteTextures')]]
void myGlDeleteTextures(int n, std::vector<unsigned int> textures) {
  
  glDeleteTextures(n,  & textures[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glDepthFunc')]]
void myGlDepthFunc(unsigned int func) {
  
  glDepthFunc(func);
  
}

//' @export
// [[Rcpp::export(name = 'glDepthMask')]]
void myGlDepthMask(unsigned char flag) {
  
  glDepthMask(flag);
  
}

//' @export
// [[Rcpp::export(name = 'glDepthRangef')]]
void myGlDepthRangef(float n, float f) {
  
  glDepthRangef(n, f);
  
}

//' @export
// [[Rcpp::export(name = 'glDetachShader')]]
void myGlDetachShader(unsigned int program, unsigned int shader) {
  
  glDetachShader(program, shader);
  
}

//' @export
// [[Rcpp::export(name = 'glDisable')]]
void myGlDisable(unsigned int cap) {
  
  glDisable(cap);
  
}

//' @export
// [[Rcpp::export(name = 'glDisableVertexAttribArray')]]
void myGlDisableVertexAttribArray(unsigned int index) {
  
  glDisableVertexAttribArray(index);
  
}

//' @export
// [[Rcpp::export(name = 'glDrawArrays')]]
void myGlDrawArrays(unsigned int mode, int first, int count) {
  
  glDrawArrays(mode, first, count);
  
}

//' @export
// [[Rcpp::export(name = 'glDrawElements')]]
void myGlDrawElements(unsigned int mode, int count, unsigned int type) {
  
  glDrawElements(mode, count, GL_UNSIGNED_INT, (void*) 0);
  
}

//' @export
// [[Rcpp::export(name = 'glEnable')]]
void myGlEnable(unsigned int cap) {
  
  glEnable(cap);
  
}

//' @export
// [[Rcpp::export(name = 'glEnableVertexAttribArray')]]
void myGlEnableVertexAttribArray(unsigned int index) {
  
  glEnableVertexAttribArray(index);
  
}

//' @export
// [[Rcpp::export(name = 'glFinish')]]
void myGlFinish() {
  
  glFinish();
  
}

//' @export
// [[Rcpp::export(name = 'glFlush')]]
void myGlFlush() {
  
  glFlush();
  
}

//' @export
// [[Rcpp::export(name = 'glFramebufferRenderbuffer')]]
void myGlFramebufferRenderbuffer(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer) {
  
  glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
  
}

//' @export
// [[Rcpp::export(name = 'glFramebufferTexture2D')]]
void myGlFramebufferTexture2D(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level) {
  
  glFramebufferTexture2D(target, attachment, textarget, texture, level);
  
}

//' @export
// [[Rcpp::export(name = 'glFrontFace')]]
void myGlFrontFace(unsigned int mode) {
  
  glFrontFace(mode);
  
}

//' @export
// [[Rcpp::export(name = 'glGenBuffers')]]
std::vector<int> myGlGenBuffers(int n) {
  GLuint buffers[n];
  glGenBuffers(n, buffers);
  return std::vector<int>(buffers, buffers + sizeof buffers / sizeof buffers[0]);
}

//' @export
// [[Rcpp::export(name = 'glGenerateMipmap')]]
void myGlGenerateMipmap(unsigned int target) {
  
  glGenerateMipmap(target);
  
}

//' @export
// [[Rcpp::export(name = 'glGenFramebuffers')]]
std::vector<unsigned int> myGlGenFramebuffers(int n) {
  unsigned int framebuffers [n];
  glGenFramebuffers(n,  framebuffers);
  return std::vector<unsigned int>(framebuffers, framebuffers + sizeof framebuffers / sizeof framebuffers[0]);
}

//' @export
// [[Rcpp::export(name = 'glGenRenderbuffers')]]
std::vector<unsigned int> myGlGenRenderbuffers(int n) {
  unsigned int renderbuffers [n];
  glGenRenderbuffers(n,  renderbuffers);
  return std::vector<unsigned int>(renderbuffers, renderbuffers + sizeof renderbuffers / sizeof renderbuffers[0]);
}

//' @export
// [[Rcpp::export(name = 'glGenTextures')]]
std::vector<unsigned int> myGlGenTextures(int n) {
  unsigned int textures [n];
  glGenTextures(n,  textures);
  return std::vector<unsigned int>(textures, textures + sizeof textures / sizeof textures[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetAttribLocation')]]
int myGlGetAttribLocation(unsigned int program, std::string name) {
  
  return glGetAttribLocation(program,  & name[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glGetBufferParameteriv')]]
std::vector<int> myGlGetBufferParameteriv(unsigned int target, unsigned int pname, int n = 1) {
  int params [n];
  glGetBufferParameteriv(target, pname,  params);
  return std::vector<int>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetError')]]
unsigned int myGlGetError() {
  
  return glGetError();
  
}

//' @export
// [[Rcpp::export(name = 'glGetFloatv')]]
std::vector<float> myGlGetFloatv(unsigned int pname, int n = 1) {
  float data [n];
  glGetFloatv(pname, data);
  return std::vector<float>(data, data + sizeof data / sizeof data[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetFramebufferAttachmentParameteriv')]]
std::vector<int> myGlGetFramebufferAttachmentParameteriv(unsigned int target, unsigned int attachment, unsigned int pname, int n = 1) {
  int params [n];
  glGetFramebufferAttachmentParameteriv(target, attachment, pname,  params);
  return std::vector<int>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetIntegerv')]]
std::vector<int> myGlGetIntegerv(unsigned int pname, int n = 1) {
  int data[n];
  glGetIntegerv(pname, data);
  return std::vector<int>(data, data + sizeof data / sizeof data[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetProgramiv')]]
std::vector<int> myGlGetProgramiv(unsigned int program, unsigned int pname, int n = 1) {
  int params [n];
  glGetProgramiv(program, pname,  params);
  return std::vector<int>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetRenderbufferParameteriv')]]
std::vector<int> myGlGetRenderbufferParameteriv(unsigned int target, unsigned int pname, int n = 1) {
  int params [n];
  glGetRenderbufferParameteriv(target, pname, params);
  return std::vector<int>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetShaderiv')]]
std::vector<int> myGlGetShaderiv(unsigned int shader, unsigned int pname, int n = 1) {
  int params [n];
  glGetShaderiv(shader, pname,  params);
  return std::vector<int>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetTexParameterfv')]]
std::vector<float> myGlGetTexParameterfv(unsigned int target, unsigned int pname, int n = 1) {
  float params [n];
  glGetTexParameterfv(target, pname,  params);
  return std::vector<float>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetTexParameteriv')]]
std::vector<int> myGlGetTexParameteriv(unsigned int target, unsigned int pname, int n = 1) {
  int params [n];
  glGetTexParameteriv(target, pname,  params);
  return std::vector<int>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetUniformfv')]]
std::vector<float> myGlGetUniformfv(unsigned int program, int location, int n = 1) {
  float params [n];
  glGetUniformfv(program, location,  params);
  return std::vector<float>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetUniformiv')]]
std::vector<int> myGlGetUniformiv(unsigned int program, int location, int n = 1) {
  int params [n];
  glGetUniformiv(program, location,  params);
  return std::vector<int>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetUniformLocation')]]
int myGlGetUniformLocation(unsigned int program, std::string name) {
  
  return glGetUniformLocation(program,  & name[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glGetVertexAttribfv')]]
std::vector<float> myGlGetVertexAttribfv(unsigned int index, unsigned int pname, int n = 1) {
  float params [n];
  glGetVertexAttribfv(index, pname,  params);
  return std::vector<float>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glGetVertexAttribiv')]]
std::vector<int> myGlGetVertexAttribiv(unsigned int index, unsigned int pname, int n = 1) {
  int params [n];
  glGetVertexAttribiv(index, pname, params);
  return std::vector<int>(params, params + sizeof params / sizeof params[0]);
}

//' @export
// [[Rcpp::export(name = 'glHint')]]
void myGlHint(unsigned int target, unsigned int mode) {
  
  glHint(target, mode);
  
}

//' @export
// [[Rcpp::export(name = 'glIsBuffer')]]
unsigned char myGlIsBuffer(unsigned int buffer) {
  
  return glIsBuffer(buffer);
  
}

//' @export
// [[Rcpp::export(name = 'glIsEnabled')]]
unsigned char myGlIsEnabled(unsigned int cap) {
  
  return glIsEnabled(cap);
  
}

//' @export
// [[Rcpp::export(name = 'glIsFramebuffer')]]
unsigned char myGlIsFramebuffer(unsigned int framebuffer) {
  
  return glIsFramebuffer(framebuffer);
  
}

//' @export
// [[Rcpp::export(name = 'glIsProgram')]]
unsigned char myGlIsProgram(unsigned int program) {
  
  return glIsProgram(program);
  
}

//' @export
// [[Rcpp::export(name = 'glIsRenderbuffer')]]
unsigned char myGlIsRenderbuffer(unsigned int renderbuffer) {
  
  return glIsRenderbuffer(renderbuffer);
  
}

//' @export
// [[Rcpp::export(name = 'glIsShader')]]
unsigned char myGlIsShader(unsigned int shader) {
  
  return glIsShader(shader);
  
}

//' @export
// [[Rcpp::export(name = 'glIsTexture')]]
unsigned char myGlIsTexture(unsigned int texture) {
  
  return glIsTexture(texture);
  
}

//' @export
// [[Rcpp::export(name = 'glLineWidth')]]
void myGlLineWidth(float width) {
  
  glLineWidth(width);
  
}

//' @export
// [[Rcpp::export(name = 'glLinkProgram')]]
void myGlLinkProgram(unsigned int program) {
  
  glLinkProgram(program);
  
}

//' @export
// [[Rcpp::export(name = 'glPixelStorei')]]
void myGlPixelStorei(unsigned int pname, int param) {
  
  glPixelStorei(pname, param);
  
}

//' @export
// [[Rcpp::export(name = 'glPolygonOffset')]]
void myGlPolygonOffset(float factor, float units) {
  
  glPolygonOffset(factor, units);
  
}

//' @export
// [[Rcpp::export(name = 'glReadPixels')]]
std::vector<float> myGlReadPixels(int x, int y, int width, int height, unsigned int format, unsigned int type, int n = 1) {
  float pixels [n];
  glReadPixels(x, y, width, height, format, type,  pixels);
  return std::vector<float>(pixels, pixels + sizeof pixels / sizeof pixels[0]);
}

//' @export
// [[Rcpp::export(name = 'glReleaseShaderCompiler')]]
void myGlReleaseShaderCompiler() {
  
  glReleaseShaderCompiler();
  
}

//' @export
// [[Rcpp::export(name = 'glRenderbufferStorage')]]
void myGlRenderbufferStorage(unsigned int target, unsigned int internalformat, int width, int height) {
  
  glRenderbufferStorage(target, internalformat, width, height);
  
}

//' @export
// [[Rcpp::export(name = 'glSampleCoverage')]]
void myGlSampleCoverage(float value, unsigned char invert) {
  
  glSampleCoverage(value, invert);
  
}

//' @export
// [[Rcpp::export(name = 'glScissor')]]
void myGlScissor(int x, int y, int width, int height) {
  
  glScissor(x, y, width, height);
  
}

//' @export
// [[Rcpp::export(name = 'glStencilFunc')]]
void myGlStencilFunc(unsigned int func, int ref, unsigned int mask) {
  
  glStencilFunc(func, ref, mask);
  
}

//' @export
// [[Rcpp::export(name = 'glStencilFuncSeparate')]]
void myGlStencilFuncSeparate(unsigned int face, unsigned int func, int ref, unsigned int mask) {
  
  glStencilFuncSeparate(face, func, ref, mask);
  
}

//' @export
// [[Rcpp::export(name = 'glStencilMask')]]
void myGlStencilMask(unsigned int mask) {
  
  glStencilMask(mask);
  
}

//' @export
// [[Rcpp::export(name = 'glStencilMaskSeparate')]]
void myGlStencilMaskSeparate(unsigned int face, unsigned int mask) {
  
  glStencilMaskSeparate(face, mask);
  
}

//' @export
// [[Rcpp::export(name = 'glStencilOp')]]
void myGlStencilOp(unsigned int fail, unsigned int zfail, unsigned int zpass) {
  
  glStencilOp(fail, zfail, zpass);
  
}

//' @export
// [[Rcpp::export(name = 'glStencilOpSeparate')]]
void myGlStencilOpSeparate(unsigned int face, unsigned int sfail, unsigned int dpfail, unsigned int dppass) {
  
  glStencilOpSeparate(face, sfail, dpfail, dppass);
  
}

//' @export
// [[Rcpp::export(name = 'glTexImage2D')]]
void myGlTexImage2D(const char * fname) {
  int width, height, chan;
  unsigned char * data = stbi_load(fname, &width, &height, &chan, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

//' @export
// [[Rcpp::export(name = 'glTexParameterf')]]
void myGlTexParameterf(unsigned int target, unsigned int pname, float param) {
  
  glTexParameterf(target, pname, param);
  
}

//' @export
// [[Rcpp::export(name = 'glTexParameterfv')]]
void myGlTexParameterfv(unsigned int target, unsigned int pname, std::vector<float> params) {
  
  glTexParameterfv(target, pname,  & params[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glTexParameteri')]]
void myGlTexParameteri(unsigned int target, unsigned int pname, int param) {
  
  glTexParameteri(target, pname, param);
  
}

//' @export
// [[Rcpp::export(name = 'glTexParameteriv')]]
void myGlTexParameteriv(unsigned int target, unsigned int pname, std::vector<int> params) {
  
  glTexParameteriv(target, pname,  & params[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glTexSubImage2D')]]
void myGlTexSubImage2D(unsigned int target, int level, int xoffset, int yoffset, int width, int height, unsigned int format, unsigned int type, std::vector<float> pixels) {
  
  glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type,  & pixels[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform1f')]]
void myGlUniform1f(int location, float v0) {
  
  glUniform1f(location, v0);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform1fv')]]
void myGlUniform1fv(int location, int count, std::vector<float> value) {
  
  glUniform1fv(location, count,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform1i')]]
void myGlUniform1i(int location, int v0) {
  
  glUniform1i(location, v0);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform1iv')]]
void myGlUniform1iv(int location, int count, std::vector<int> value) {
  
  glUniform1iv(location, count,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform2f')]]
void myGlUniform2f(int location, float v0, float v1) {
  
  glUniform2f(location, v0, v1);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform2fv')]]
void myGlUniform2fv(int location, int count, std::vector<float> value) {
  
  glUniform2fv(location, count,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform2i')]]
void myGlUniform2i(int location, int v0, int v1) {
  
  glUniform2i(location, v0, v1);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform2iv')]]
void myGlUniform2iv(int location, int count, std::vector<int> value) {
  
  glUniform2iv(location, count,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform3f')]]
void myGlUniform3f(int location, float v0, float v1, float v2) {
  
  glUniform3f(location, v0, v1, v2);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform3fv')]]
void myGlUniform3fv(int location, int count, std::vector<float> value) {
  
  glUniform3fv(location, count,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform3i')]]
void myGlUniform3i(int location, int v0, int v1, int v2) {
  
  glUniform3i(location, v0, v1, v2);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform3iv')]]
void myGlUniform3iv(int location, int count, std::vector<int> value) {
  
  glUniform3iv(location, count,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform4f')]]
void myGlUniform4f(int location, float v0, float v1, float v2, float v3) {
  
  glUniform4f(location, v0, v1, v2, v3);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform4fv')]]
void myGlUniform4fv(int location, int count, std::vector<float> value) {
  
  glUniform4fv(location, count,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform4i')]]
void myGlUniform4i(int location, int v0, int v1, int v2, int v3) {
  
  glUniform4i(location, v0, v1, v2, v3);
  
}

//' @export
// [[Rcpp::export(name = 'glUniform4iv')]]
void myGlUniform4iv(int location, int count, std::vector<int> value) {
  
  glUniform4iv(location, count,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniformMatrix2fv')]]
void myGlUniformMatrix2fv(int location, int count, unsigned char transpose, std::vector<float> value) {
  
  glUniformMatrix2fv(location, count, transpose,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniformMatrix3fv')]]
void myGlUniformMatrix3fv(int location, int count, unsigned char transpose, std::vector<float> value) {
  
  glUniformMatrix3fv(location, count, transpose,  & value[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUniformMatrix4fv')]]
void myGlUniformMatrix4fv(int location, int count, unsigned char transpose, NumericMatrix mat) {
  glm::mat4 value = asMat4(mat);
  glUniformMatrix4fv(location, count, transpose,  & value[0][0]);
  
}

//' @export
// [[Rcpp::export(name = 'glUseProgram')]]
void myGlUseProgram(unsigned int program) {
  
  glUseProgram(program);
  
}

//' @export
// [[Rcpp::export(name = 'glValidateProgram')]]
void myGlValidateProgram(unsigned int program) {
  
  glValidateProgram(program);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttrib1f')]]
void myGlVertexAttrib1f(unsigned int index, float x) {
  
  glVertexAttrib1f(index, x);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttrib1fv')]]
void myGlVertexAttrib1fv(unsigned int index, std::vector<float> v) {
  
  glVertexAttrib1fv(index,  & v[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttrib2f')]]
void myGlVertexAttrib2f(unsigned int index, float x, float y) {
  
  glVertexAttrib2f(index, x, y);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttrib2fv')]]
void myGlVertexAttrib2fv(unsigned int index, std::vector<float> v) {
  
  glVertexAttrib2fv(index,  & v[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttrib3f')]]
void myGlVertexAttrib3f(unsigned int index, float x, float y, float z) {
  
  glVertexAttrib3f(index, x, y, z);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttrib3fv')]]
void myGlVertexAttrib3fv(unsigned int index, std::vector<float> v) {
  
  glVertexAttrib3fv(index,  & v[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttrib4f')]]
void myGlVertexAttrib4f(unsigned int index, float x, float y, float z, float w) {
  
  glVertexAttrib4f(index, x, y, z, w);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttrib4fv')]]
void myGlVertexAttrib4fv(unsigned int index, std::vector<float> v) {
  
  glVertexAttrib4fv(index,  & v[0]);
  
}

//' @export
// [[Rcpp::export(name = 'glVertexAttribPointer')]]
void myGlVertexAttribPointer(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, int pointer) {
  
  glVertexAttribPointer(index, size, type, normalized, stride * sizeof(float),  (void *) (pointer * sizeof(float)));
  
}

//' @export
// [[Rcpp::export(name = 'glViewport')]]
void myGlViewport(int x, int y, int width, int height) {
  
  glViewport(x, y, width, height);
  
}

