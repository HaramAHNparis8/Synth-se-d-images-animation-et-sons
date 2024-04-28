/*!\file window.c
 *
 * \brief using GL4Dummies and Assimp Library to load 3D models or scenes.
 *
 * \author Farès Belhadj amsi@up8.edu
 * \date February 14 2017, modified on March 24, 2024
 */
#include <stdio.h>
#include <GL4D/gl4duw_SDL2.h>
#include <assert.h>
#include <SDL_image.h>
#include "assimp.h"
#include <SDL2/SDL.h>
#include <SDL_mixer.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_ttf.h>

# define DEG2RAD (M_PI / 180.0)
#define GL4DH_INIT 0
#define GL4DH_DRAW 1
#define GL4DH_UPDATE_WITH_AUDIO 2
/*!\brief opened window width */
static int _windowWidth = 1024;
/*!\brief opened window height */
static int _windowHeight = 768;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
static Uint32 startTime = 0;
static GLboolean displayCredits = GL_FALSE;
Mix_Music *music = NULL;
GLuint _texId[2] = {0};
int horizon = 0;
int vertical = 0;
int espace = 0;

/*!\brief enum that index keyboard mapping for direction commands */
enum kyes_t {
  KLEFT = 0,
  KRIGHT,
  KUP,
  KDOWN,
  KPAGEUP,
  KPAGEDOWN
};

/*!\brief virtual keyboard for direction commands */
static GLuint _keys[] = {0, 0, 0, 0, 0, 0};

typedef struct cam_t cam_t;
/*!\brief a data structure for storing camera position and
 * orientation */
struct cam_t {
  GLfloat x, y, z;
  GLfloat theta;
};

/*!\brief the used camera */
static cam_t _cam = {0.0f, 0.0f, 1.0f, 0.0f};

/*!\brief toggle y-axis rotation pause */
static GLboolean _pause = GL_TRUE;

/*!\brief toggle view focused on the scene center */
static GLboolean _center_view = GL_FALSE;

/*!\brief identifiant de la scene générée par assimpGenScene */
static GLuint _id_sun = 0;
static GLuint _id_mer = 0;
static GLuint _id_scene3 = 0;
static GLuint _id_venus = 0;
static GLuint _id_mars = 0;
static GLuint _id_jupiter = 0;
static GLuint _id_saturn = 0;
static GLuint _id_uranus_ = 0;
static GLuint _id_neptune = 0;
/*!\brief rotation angles according to axis (0 = x, 1 = y, 2 = z)
 * \todo améliorer l'interface et ajouter rotations/zoom à la souris */
static GLfloat rot[3] = {0, 0, 0};

static void initGL(void);
static void quit(void);
static void resize(int w, int h);
static void idle(void);
static void draw(void);
static void keydown(int keycode);
static void keyup(int keycode);
static void initAudio(const char * filename);
static void inital();
static void exemple_de_transition_00(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
void animation1(int state);
void animation2(int state);
static void idle(void);
static void initText(GLuint * ptId, const char * text);
void scene_credit_creditant(GLuint _pId);

/*!\brief the main function.
 */

static void inital(){
    
    _cam.x = 0.0f;
    _cam.y = 0.0f;
    _cam.z = 10.0f;
    _cam.theta = 0.0f;
    if(TTF_Init() == -1) {
          fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
          exit(2);
      }
    
    
}
static void mixCallback(void *udata, Uint8 *stream, int len);
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Exemple de loader de modèles 3D", GL4DW_POS_UNDEFINED, GL4DW_POS_UNDEFINED,
                         _windowWidth, _windowHeight, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  _id_sun  = assimpGenScene("models/star_of_sun/scene_sun.gltf");
  _id_mer = assimpGenScene("models/mercury_planet/scene.gltf");
  _id_scene3 = assimpGenScene("models/earth/scene.gltf");
  _id_venus = assimpGenScene("models/venus_v1/scene.gltf");
  _id_mars = assimpGenScene("models/mars/scene.gltf");
  _id_jupiter = assimpGenScene("models/jupiter/scene.gltf");
  _id_saturn = assimpGenScene("models/saturn/scene.gltf");
  _id_uranus_ = assimpGenScene("models/realistic_uranus_4k/scene.gltf");
  _id_neptune = assimpGenScene("models/neptune/scene.gltf");
    
  if(argc < 2) {
      fprintf(stderr, "Usage: %s <audio_file>\n", argv[0]);
      return 1;
    }
  initAudio(argv[1]);

  initGL();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwIdleFunc(idle);
  gl4duwMainLoop();
  return 0;
}

/*!\brief function that initialize OpenGL / GL4D params and objects.
 */
static void initGL(void) {
  glEnable(GL_DEPTH_TEST);
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  resize(_windowWidth, _windowHeight);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/*!\brief function called by GL4Dummies' loop at resize. Sets the
 *  projection matrix and the viewport according to the given width
 *  and height.
 * \param w window width
 * \param h window height
 */
static void resize(int w, int h) {
  _windowWidth = w; 
  _windowHeight = h;
  glViewport(0.0f, 0.0f, _windowWidth, _windowHeight);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.005f, 0.005f, -0.005f * _windowHeight / _windowWidth, 0.005f * _windowHeight / _windowWidth, 0.01f, 1000.0f);
  /* même résultat en utilisant la fonction perspective */
  /* gl4duPerspectivef(60.0f, (GLfloat)_windowWidth/(GLfloat)_windowHeight, 0.01f, 1000.0f); */
  gl4duBindMatrix("modelViewMatrix");
}

/*!\brief function called by GL4Dummies' loop at idle.
 * 
 * uses the virtual keyboard states to move the camera according to
 * direction, orientation and time (dt = delta-time)
 */
static void initAudio(const char * filename) {
    int mixFlags = MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_MOD, res;
    res = Mix_Init(mixFlags);
    if ((res & mixFlags) != mixFlags) {
        fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliotheque SDL_Mixer\n");
        fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
        //exit(3); // Commenté pour certaines architectures
    }
    if (Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024) < 0) {
        exit(4);
    }
    music = Mix_LoadMUS(filename);
    if (!music) {
        fprintf(stderr, "Erreur lors du Mix_LoadMUS: %s\n", Mix_GetError());
        exit(5);
    }
    Mix_SetPostMix(mixCallback, NULL);
    if (!Mix_PlayingMusic()) {
        Mix_PlayMusic(music, 1);
    }
}
static void idle(void) {
    static float lastTime = 0.0f;
    float currentTime = SDL_GetTicks() / 1000.0f; // 현재 시간을 초 단위로 계산
    float frameTime = currentTime - lastTime; // 프레임 간 시간 간격을 계산
    lastTime = currentTime;


    float dtheta = M_PI, step = 1.0f;
    if (startTime != 0) {
            Uint32 elapsedTime = SDL_GetTicks() - startTime;
            if (elapsedTime < 5000) {
                exemple_de_transition_00(animation1, animation2, 5000, elapsedTime, GL4DH_DRAW);
            } else {
                startTime = 0;  // 전환 완료 후 startTime을 리셋
            }
        }
        

    if(_keys[KLEFT])
        _cam.theta += frameTime * dtheta;
    if(_keys[KRIGHT])
        _cam.theta -= frameTime * dtheta;
    if(_keys[KPAGEUP]) {
        _cam.y += frameTime * 0.5f * step;
    }
    if(_keys[KPAGEDOWN]) {
        _cam.y -= frameTime * 0.5f * step;
    }
    if(_keys[KUP]) {
        _cam.x += -frameTime * step * sin(_cam.theta);
        _cam.z += -frameTime * step * cos(_cam.theta);
    }
    if(_keys[KDOWN]) {
        _cam.x += frameTime * step * sin(_cam.theta);
        _cam.z += frameTime * step * cos(_cam.theta);
    }
    if(!_pause){
        rot[1] += 90.0f * frameTime;
    }
}

/*!\brief function called by GL4Dummies' loop at key-down (key
 * pressed) event.
 * 
 * stores the virtual keyboard states (1 = pressed) and toggles the
 * boolean parameters of the application.
 */
static void keydown(int keycode) {
  GLint v[2];
  switch(keycode) {
  case GL4DK_LEFT:
    _keys[KLEFT] = 1;
    break;
  case GL4DK_RIGHT:
    _keys[KRIGHT] = 1;
    break;
  case GL4DK_UP:
    _keys[KUP] = 1;
    break;
  case GL4DK_DOWN:
    _keys[KDOWN] = 1;
    break;
  case GL4DK_d:
    _keys[KPAGEDOWN] = 1;
    break;
  case GL4DK_u:
    _keys[KPAGEUP] = 1;
    break;
  case GL4DK_ESCAPE:
  case 'q':
    exit(0);
    /* when 'w' pressed, toggle between line and filled mode */
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(3.0f);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(1.0f);
    }
    break;
  case GL4DK_SPACE:
    _pause = !_pause;
    break;
  case GL4DK_c:
    _center_view = !_center_view;
    break;
  case 'm':
          inital();
          _cam.x = -0.1f;
          _cam.y = -0.1f;
          _cam.z = 0.08f;
          _cam.theta = 0.197f;
    break;
  case 'l':
          inital();
          break;
      
  case GL4DK_t:
          printf("T key pressed.\n");
          startTime = SDL_GetTicks();
          exemple_de_transition_00(animation1, animation2, 5000, 0, GL4DH_INIT);
                     
          
          break;
      
  default:
          
          
          break;
  }
}

/*!\brief function called by GL4Dummies' loop at key-up (key
 * released) event.
 * 
 * stores the virtual keyboard states (0 = released).
 */
static void keyup(int keycode) {
  switch(keycode) {
  case GL4DK_LEFT:
    _keys[KLEFT] = 0;
    break;
  case GL4DK_RIGHT:
    _keys[KRIGHT] = 0;
    break;
  case GL4DK_UP:
    _keys[KUP] = 0;
    break;
  case GL4DK_DOWN:
    _keys[KDOWN] = 0;
    break;
  case GL4DK_d:
    _keys[KPAGEDOWN] = 0;
    break;
  case GL4DK_u:
    _keys[KPAGEUP] = 0;
    break;
  default:
    break;
  }
}

/*!\brief function called on each GL4Dummies' display event. It draws
 * the scene with its given params.
 */
static void exemple_de_transition_00(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state) {
    static float alpha = 0.0f;
    switch(state) {
        case GL4DH_INIT:
            alpha = 0.0f; // 알파 초기화
            break;
        case GL4DH_DRAW:
            alpha = (float)et / t; // 시간 경과에 따라 알파 값 계산
            if (alpha < 0.5f) {
                if(a0) a0(state);
            }
            else {
                if(a1) a1(state);
            }
            break;
    }
}
static void draw(void) {
  GLfloat lum[4] = {0.0f, 0.0f, 5.0f, 1.0f};
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId);

  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();

  gl4duLookAtf(_cam.x, _cam.y, _cam.z,
	       _cam.x - sin(_cam.theta),  _center_view ? 0.0f : _cam.y,  _cam.z - cos(_cam.theta),
	       0.0f, 1.0f, 0.0f);
  
    
  gl4duRotatef(rot[1], 0.0f, 1.0f, 0.0f);

  gl4duPushMatrix();
  gl4duTranslatef(-1.5f, 0.0f, 0.0f);
  gl4duScalef(0.3f, 0.3f, 0.3f);
  assimpDrawScene(_id_scene3);
  gl4duPopMatrix();
    
  gl4duPushMatrix();
  assimpDrawScene(_id_sun);
  gl4duPushMatrix();



  gl4duTranslatef(10.0f, 0.1f, 50.0f); // 'mer'의 위치를 지구와 다르게 조정, Z축 방향으로 조금 더 떨어지게 함
  gl4duScalef(10.0f, 10.f, 10.0f); // 필요하다면 'mer'의 크기 조정
  assimpDrawScene(_id_mer); // 'mer'를 그림
  gl4duPopMatrix(); // 'mer'에 대한 변환을 여기서 종료
    
  gl4duPushMatrix();
  gl4duTranslatef(-30.0f, 0.2f, 50.0f);
  gl4duScalef(10.0f, 10.f, 10.0f);
  assimpDrawScene(_id_venus);
  gl4duPopMatrix();
    
  gl4duPushMatrix();
  gl4duTranslatef(40.0f, 0.2f, 50.0f);
  gl4duScalef(10.0f, 10.f, 10.0f);
  assimpDrawScene(_id_mars);
  gl4duPopMatrix();

  gl4duPushMatrix();
  gl4duTranslatef(-60.0f, 1.0f, 50.0f);
  gl4duScalef(10.0f, 10.f, 10.0f);
  assimpDrawScene(_id_jupiter);
  gl4duPopMatrix();

  gl4duPushMatrix();
  gl4duTranslatef(50.0f, 0.3f, -3.0f);
  gl4duScalef(30.0f, 30.0f, 30.0f);
  assimpDrawScene(_id_saturn);
  gl4duPopMatrix();

    
  gl4duPushMatrix();
  gl4duTranslatef(-50.0f, 0.3f, -40.0f);
  gl4duScalef(15.0f, 15.0f, 15.0f);
  assimpDrawScene(_id_uranus_);
  gl4duPopMatrix();
    
  gl4duPushMatrix();
  gl4duTranslatef(20.0f, 0.3f, -40.0f);
  gl4duScalef(10.0f, 10.0f, 10.0f);
  assimpDrawScene(_id_neptune);
  gl4duPopMatrix();
  if (displayCredits) {
      scene_credit_creditant(_pId);
      }
  
}

/*!\brief function called at exit, it cleans all created GL4D objects.
 */
static void mixCallback(void *udata, Uint8 *stream, int len) {

    Sint16 *samples = (Sint16 *)stream;
    int i;

    int numSamples = len / 2;


    for(i = 0; i < numSamples; i++) {

        samples[i] = samples[i] / 2;
    }
}
void animation1(int state) {
    if(state == GL4DH_DRAW) {
        glPushMatrix();
        glTranslatef(-0.5f, 0.0f, -5.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex3f(-0.5f, -0.5f, 0.0f);
        glVertex3f( 0.5f, -0.5f, 0.0f);
        glVertex3f( 0.5f,  0.5f, 0.0f);
        glVertex3f(-0.5f,  0.5f, 0.0f);
        glEnd();
        glPopMatrix();
    }
}

void animation2(int state) {
    if(state == GL4DH_DRAW) {
        glPushMatrix();
        glTranslatef(0.5f, 0.0f, -5.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for(int i = 0; i <= 360; i++) {
            float degInRad = i * DEG2RAD;
            glVertex2f(cos(degInRad) * 0.5f, sin(degInRad) * 0.5f);
        }
        glEnd();
        glPopMatrix();
    }
}



static void quit(void) {
  gl4duClean(GL4DU_ALL);
    if(music) {
          Mix_FreeMusic(music);
          music = NULL;
      }
    if(_texId[0]) {
            glDeleteTextures(2, _texId);
            _texId[0] = 0;
        }
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

