#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <bcm_host.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <linux/soundcard.h>

typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   GLuint verbose;
   GLuint vshader;
   GLuint fshader;
   GLuint program;
   GLuint buf;
   GLuint attr_vertex;
   GLuint unif_rcp_resolution;
   GLuint unif_aspect;
   GLuint unif_time;
} STATE_T;

static STATE_T _state, *state=&_state;

static void init_ogl(STATE_T *state)
{
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };
   
   static const EGLint context_attributes[] = 
   {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   EGLConfig config;

   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   eglInitialize(state->display, NULL, NULL);
   eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   eglBindAPI(EGL_OPENGL_ES_API);
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
   graphics_get_display_size(0, &state->screen_width, &state->screen_height);
   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state->screen_width;
   dst_rect.height = state->screen_height;
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = state->screen_width << 16;
   src_rect.height = state->screen_height << 16;        
   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );   
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE,0,0,0);
   nativewindow.element = dispman_element;
   nativewindow.width = state->screen_width;
   nativewindow.height = state->screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );
   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   glClear( GL_COLOR_BUFFER_BIT );

}

static void init_shaders(STATE_T *state)
{
  static const GLfloat vertex_data[] = {
    -1.0,-1.0,1.0,1.0,
    1.0,-1.0,1.0,1.0,
    1.0,1.0,1.0,1.0,
    -1.0,1.0,1.0,1.0
  };

  const GLchar *vshader_source =
    "attribute vec4 v;"
    "void main(){"
      "gl_Position=v;"
    "}";

  const GLchar *fshader_source =
    "uniform vec2 r;"
    "uniform float t;"
    "uniform float aa;"
    "float sinf(float x)"
    "{"
      "x*=.159155;"
      "x-=floor(x);"
      "float xx=x*x,"
        "y=-6.87897;"
      "y=y*xx+33.7755;"
      "y=y*xx-72.5257;"
      "y=y*xx+80.5874;"
      "y=y*xx-41.2408;"
      "y=y*xx+6.28077;"
      "return x*y;"
    "}"
    "float cosf(float x)"
    "{"
      "return sinf(x+1.5708);"
    "}"
    "vec2 sinf(vec2 x)"
    "{"
      "return vec2(sinf(x.x),sinf(x.y));"
    "}"
    "float toct(vec3 p){"
      "vec4 a=vec4(1,0,.577,-.577);"
      "return max(max(max(max(max(max(abs(dot(p,a.xyy)),abs(dot(p,a.yxy))),abs(dot(p,a.yyx))),abs(dot(p,a.zzz))),abs(dot(p,a.wzz))),abs(dot(p,a.zwz))),abs(dot(p,a.zzw)))-5.;"
    "}"
    "float dodec(vec3 p){"
      "vec4 a4=vec4(0,.526,.851,-.851);"
      "return max(max(max(max(max("
        "abs(dot(p,a4.xzy)),abs(dot(p,a4.xwy))),"
        "abs(dot(p,a4.yxz))),abs(dot(p,-a4.yxw))),"
        "abs(dot(p,a4.zyx))),abs(dot(p,a4.wyx)))-5.;"
    "}"
    "float dScene(vec3 p){"
      "return dodec(p)*1.3+.5*sinf(p.y+t);"
    "}"
    "void main(){"
      "vec2 v=-1.+2.*gl_FragCoord.xy*r;"
      "vec3 vuv=vec3(sinf(t*.3)*.3,1,0),"
        "oo=vec3(sinf(t*.21)*20.+20.,10,sinf(t*.18+.159155)*20.+14.),"
        "vpn=normalize(-oo),"
        "u=normalize(cross(vuv,vpn)),"
        "dd=normalize(oo+vpn+v.x*u*aa+v.y*cross(vpn,u)-oo);"
      "float s=dScene(oo),"
        "f=.0;"
      "for(int i=0;i<15;i++){"
        "if(abs(s)<.01||f>200.)break;"
        "f+=.7*s;"
        "s=dScene(oo+dd*f);"
      "}"
      "if(f>200.){"
        "gl_FragColor=vec4(.3,.5,.7,1);"
      "}"
      "else"
      "{"
        "const vec2 e=vec2(.01,0);"
        "vec3 p=oo+dd*f,"
          "n=normalize(vec3(s-dScene(p-e.xyy),s-dScene(p-e.yxy),s-dScene(p-e.yyx)));"
        "float z=dot(n,normalize(oo-p));"
        "gl_FragColor=vec4((vec3(.1,.1,.3)+z*vec3(.1,.5,.9)+pow(z,8.)),1);"
      "}"
    "}"
    ;
  state->vshader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(state->vshader, 1, &vshader_source, 0);
  glCompileShader(state->vshader);
  state->fshader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(state->fshader, 1, &fshader_source, 0);
  glCompileShader(state->fshader);
  state->program = glCreateProgram();
  glAttachShader(state->program, state->vshader);
  glAttachShader(state->program, state->fshader);
  glLinkProgram(state->program);
  state->attr_vertex = glGetAttribLocation(state->program, "v");
  state->unif_rcp_resolution = glGetUniformLocation(state->program, "r");
  state->unif_aspect = glGetUniformLocation(state->program, "aa");
  state->unif_time = glGetUniformLocation(state->program, "t");
  glGenBuffers(1, &state->buf);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glViewport(0,0,state->screen_width, state->screen_height );
  glBindBuffer(GL_ARRAY_BUFFER, state->buf);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
  glVertexAttribPointer(state->attr_vertex, 4, GL_FLOAT, 0, 16, 0);
  glEnableVertexAttribArray(state->attr_vertex);
}
        
static void draw_triangles(
  STATE_T *state,
  GLfloat rx,
  GLfloat ry,
  GLfloat t
)
{
  // Now render to the main frame buffer
  glBindFramebuffer(GL_FRAMEBUFFER,0);
  glBindBuffer(GL_ARRAY_BUFFER, state->buf);
  glUseProgram(state->program);
  glUniform2f(state->unif_rcp_resolution, 1./rx, 1./ry);
  glUniform1f(state->unif_aspect, rx/(float)ry);
  glUniform1f(state->unif_time, t);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glFlush();
  glFinish();
  eglSwapBuffers(state->display, state->surface);
}

int main ()
{
  int terminate = 0;
  bcm_host_init();
  init_ogl(state);
  init_shaders(state);
  while (!terminate)
  {
    struct timeval timeNow;
    gettimeofday(&timeNow,NULL);
    float t = ((timeNow.tv_sec % 1000) + timeNow.tv_usec/1000000.)*4.;
    draw_triangles(state, state->screen_width, state->screen_height, t);
  }
  return 0;
}
