#pragma once

#include "./OpenGL/GLHeadersAndMacros.h"
#include "./OpenGL/GLLog.h"

#include "../includes/vmath.h"
#include "../includes/helper_timer.h"

#include <map>

#define lerp(t, a, b) (a + t * (b - a))

enum SCENE
{
    SCENE_INTRO = 0,
    SCENE_00,
    SCENE_01,
    SCENE_02,
    SCENE_03,
    SCENE_04,
    SCENE_OUTRO
};

enum PHASE
{
    PHASE_MAIN,
};

#include "../shaders/CommonShaders.h"
#include "../shaders/CommonModels.h"
#include "../includes/3DModels.h"

#include "../includes/stb_image.h"

// #include "../includes/ft2build.h"

using namespace vmath;

#define MODEL_VIEW_MATRIX_STACK 100

extern DOUBLE gDeltaTime;
extern mat4 perspectiveProjectionMatrix;

extern vec3 eye;
extern vec3 center;
extern vec3 up;

extern mat4 modelMatrix;
extern mat4 viewMatrix;

extern mat4 matrixStack[MODEL_VIEW_MATRIX_STACK];
extern int matrixStackTop;

// extern Camera camera;
extern std::vector<std::vector<float>> bezierPoints;
extern std::vector<float> yawGlobal;
extern std::vector<float> pitchGlobal;
extern std::vector<float> fovGlobal;
extern BOOL USE_FPV_CAM;
extern float objX;
extern float objY;
extern float objZ;
extern float objIncrement;

extern float scaleX;
extern float scaleY;
extern float scaleZ;
extern float scaleIncrement;

extern float objAngle;
extern float objAngleIncrement;

extern float globalTime;
extern float ELAPSED_TIME;

extern GLuint giWindowWidth;
extern GLuint giWindowHeight;

extern CommonShaders *commonShaders;
extern CommonModels *commonModels;

extern GLuint FBO_DOF;
extern BOOL isDOFTrue;
// extern GLuint fbo_UserMap;

extern float light_objX;
extern float light_objY;
extern float light_objZ;

extern bool isMovementStarted;

// Timer
extern StopWatchInterface *timer;

//  tint effect flag
extern BOOL gEnableTint;
extern int gTintMode;
extern float gTintVignettePower;
extern float gTintStrength;

struct DeltaTimer
{
    double loadTime;
    double elapsedSeconds;
    double lastTime;
    double currentTime;
    double delta;
};

struct ControlVariables
{
    double globalTime;
    unsigned int gCurrentScene;
    bool isAnimating;
};

extern DeltaTimer deltaTimer;
extern ControlVariables controlVariables;

extern GLfloat Cubemap_Alpha;

#define FBO_WIDTH_LIGHT 2048
#define FBO_HEIGHT_LIGHT 2048

BOOL LoadPNGImage(GLuint *texture, const char *imageFilePath);
BOOL LoadGLTexture(GLuint *texture, TCHAR ImageResourceID[]);
BOOL Load1DPNGImage(GLuint *texture, float *data);
BOOL loadTextureForFire(GLuint *texture, const char *filename, bool wrap);
unsigned int loadCubemap(const char *faces[]);

void initializeMatrixStack(void);
void pushMatrix(mat4 matrix);
mat4 popMatrix(void);

float degToRad(float degrees);

float osc(float time, float minRange, float maxRange, float frequency);
void setSelectedScene(int selScene);
void setCurrentPhase(int phase);
void updateGlobalViewMatrix();
extern BOOL start_fade_out_opening;

void initializePlaneVAO(const int res, const int width, GLuint *planeVAO, GLuint *planeVBO, GLuint *planeEBO);
vmath::vec3 genRandomVec3();
void resetElapsedTime();
std::vector<float> flatten(const std::vector<std::vector<float>> &nestedVector);
