/* Header Files */
#pragma once
#include <windows.h>
#include "OGLMain.h"
#include <stdio.h>
#include <stdlib.h>

/* OpenGL Header files */
#include "./includes/OpenAL.h"

// ======================================= SCENES/
#include "./scenes/MainScene/MainScene.h"
// ======================================= SCENES END

// ======================================= CAMERA
#include "./utils/camera/Camera.h"
#include "./utils/camera/BezierCamera.h"
// ======================================= CAMERA END

#include "./utils/common.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./includes/stb_image.h"
#include "./includes/helper_timer.h"

#define WINWIDTH 800
#define WINHEIGHT 600

GLuint giWindowWidth = 0;
GLuint giWindowHeight = 0;

vec3 eye = vec3(0.0f, 0.0f, 6.0f);
vec3 center = vec3(0.0f, 0.0f, 0.0f);
vec3 up = vec3(0.0f, 1.0f, 0.0f);

mat4 perspectiveProjectionMatrix;

mat4 modelMatrix = mat4::identity();
mat4 viewMatrix = mat4::identity();

/* Global Function Declartion */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ToggleFullScreen();
BOOL gbActiveWindow = FALSE;

// global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullScreen = FALSE;

// DOF
extern float focalDistance;
extern float focalDepth;

// Target frame rate
const double TARGET_FPS = 60.0;
const LONGLONG INTERVAL_100NS = static_cast<LONGLONG>(1e7 / TARGET_FPS);
bool isFrameLimitEnabled = false;

BOOL isDOFTrue = FALSE;
StopWatchInterface *timer = NULL;
// ================================= Camera
Camera camera;
BezierCamera *globalBezierCamera;

// ================================= Camera End

// ================================= HELPING VARIABLES
CommonShaders *commonShaders;
CommonModels *commonModels;

// Object
float objX = 0.0f;
float objY = 0.0f;
float objZ = 0.0f;
float objIncrement = 20.0f;

// Scale
float scaleX = 0.0;
float scaleY = 0.0;
float scaleZ = 0.0;
float scaleIncrement = 1.0f;

float globalTime = 0.0f;

float light_objX = 0.0f;
float light_objY = 0.0f;
float light_objZ = 0.0f;
float lightObjIncrement = 0.1f;

float objAngle = 0.0f;
float objAngleIncrement = 1.0f;

bool isMovementStarted = true;

// =============================== GLOBAL CONTROLS
BOOL USE_FPV_CAM = FALSE;
BOOL playMusic = FALSE;
BOOL enableBezierCameraControl = TRUE;
BOOL spaceBarIsPressed = FALSE;
float VOLUME_LEVEL = 0.8f;
// ==============================================//
// ============================================== GLOBAL DELTA TIME//
DOUBLE gDeltaTime = 0;
static uint64_t clockOffset = 0;
static LARGE_INTEGER frequency;
static LARGE_INTEGER startCount;

BOOL start_fade_out_opening = FALSE;

std::vector<std::vector<float>> bezierPoints = {

	{-28.600082f, 23.500046f, 15.500017f},
};

// YAW GLOBAL
std::vector<float> yawGlobal = {

	29.000000f,
};

// PITCH GLOBAL
std::vector<float> pitchGlobal = {

	-28.000000f,
};

// FOV GLOBAL
std::vector<float> fovGlobal = {

	-120.000000f,
};

int vectorIndex = bezierPoints.size() - 1;

// ================================= HELPING VARIABLES END
MainScene *mainScene;

// ======================================= OBJECTS
OpenAL myMusic;
float globalSpeedAdjust = 0.0f;

// =================================================== Global Delta Timer
uint64_t getTimerValue()
{
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);
	return (uint64_t)(currentCount.QuadPart - startCount.QuadPart);
}

DOUBLE getTime(void)
{
	return (DOUBLE)(getTimerValue() - clockOffset) / frequency.QuadPart;
}

/* Entry Point Function */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	/* function declartions */
	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	/* variable declarations */
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRetVal = 0;
	int iHeightOfWindow, iWidthOfWindow;

	// Code
	CreateLogFile();

	// Initializaion of wndclassex structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	/* Register Above wndclass */
	RegisterClassEx(&wndclass);

	iHeightOfWindow = GetSystemMetrics(SM_CYSCREEN); // Height of Window Screen
	iWidthOfWindow = GetSystemMetrics(SM_CXSCREEN);	 // Width Of Window Screen

	/* Create Window */
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName,
						  TEXT("RTR5 - PERSPECTIVE GROUP | The Child's Play"),
						  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
						  (iWidthOfWindow - WINWIDTH) / 2,
						  (iHeightOfWindow - WINHEIGHT) / 2,
						  WINWIDTH,
						  WINHEIGHT,
						  NULL,
						  NULL,
						  hInstance,
						  NULL);
	ghwnd = hwnd;

	// initizalize
	iRetVal = initialize();

	if (iRetVal == -1)
	{
		PrintLog("Choose Pixel format Failed...\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		PrintLog("Set Pixel format Failed...\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		PrintLog("Crete OpenGl Context Failed...\n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		PrintLog("Makeing OpenGL as current Context Failed...\n");
		uninitialize();
	}
	else if (iRetVal == -5)
	{
		PrintLog("GLEW Initialization Failed...\n");
		uninitialize();
	}
	else
	{
		PrintLog("Initialize Successful...\n");
	}

	ShowWindow(hwnd, iCmdShow);

	/* fore grounding and focusing window */
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Create a waitable timer
	HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (!timer)
	{
		PrintLog("Failed to create timer.");
		return -1;
	}

	/* Game Loop */
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			// Start timing for the frame
			LARGE_INTEGER dueTime;
			if (isFrameLimitEnabled)
			{
				dueTime.QuadPart = -INTERVAL_100NS; // Negative for relative time
				if (!SetWaitableTimer(timer, &dueTime, 0, NULL, NULL, FALSE))
				{
					std::cerr << "Failed to set timer." << std::endl;
					break;
				}
			}

			if (gbActiveWindow == TRUE)
			{
				/* Render the seen */
				display();

				static DOUBLE prevTime = getTime();
				DOUBLE current = getTime();
				DOUBLE delta = current - prevTime;
				prevTime = current;
				gDeltaTime = delta;

				// updatetheseen
				update();
			}
			if (isFrameLimitEnabled)
			{
				// Wait for the timer
				DWORD result = WaitForSingleObject(timer, INFINITE);
				if (result != WAIT_OBJECT_0)
				{
					std::cerr << "Timer wait failed." << std::endl;
					break;
				}
			}
		}
	}
	// Clean up
	CloseHandle(timer);

	uninitialize();
	return ((int)msg.wParam);
}

/* CALLBACK Function */
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	/* fucntion declarations */

	// void ToggleFullScreen();
	void resize(int, int);

	// code
	switch (iMsg)
	{
	case WM_CREATE:
		if (enableBezierCameraControl)
		{
			objX = bezierPoints[bezierPoints.size() - 1][0];
			objY = bezierPoints[bezierPoints.size() - 1][1];
			objZ = bezierPoints[bezierPoints.size() - 1][2];
			scaleX = yawGlobal[yawGlobal.size() - 1];
			scaleY = pitchGlobal[pitchGlobal.size() - 1];
			scaleZ = fovGlobal[fovGlobal.size() - 1];
		}
		break;
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_ERASEBKGND:
		return (0);

	case WM_CHAR:
		camera.keyboardInputs(wParam);
		switch (wParam)
		{
		case 'f':
		case 'F':
			ToggleFullScreen();
			break;
		case '+':
			if (enableBezierCameraControl)
				scaleZ += (scaleIncrement * 20.0);
			else
				scaleZ += scaleIncrement;
			break;
		case '-':
			if (enableBezierCameraControl)
				scaleZ -= (scaleIncrement * 20.0);
			else
				scaleZ -= scaleIncrement;
			break;
		case 'C':
		case 'c':
			USE_FPV_CAM = !USE_FPV_CAM;
			break;
		case 'p':
		case 'P':
			if (enableBezierCameraControl)
			{
				bezierPoints.push_back({objX, objY, objZ});
				yawGlobal.push_back(scaleX);
				pitchGlobal.push_back(scaleY);
				fovGlobal.push_back(scaleZ);
				vectorIndex = bezierPoints.size() - 1;
			}
			break;
		case 'L':
		case 'l':
			if (enableBezierCameraControl)
			{
				bezierPoints.erase(--bezierPoints.end());
				yawGlobal.erase(--yawGlobal.end());
				pitchGlobal.erase(pitchGlobal.end());
				fovGlobal.erase(fovGlobal.end());
				vectorIndex = bezierPoints.size() - 1;
			}
			break;
		case 'G':
		case 'g':
			if (enableBezierCameraControl)
			{
				if (bezierPoints.size() > vectorIndex + 1)
				{
					vectorIndex++;
					objX = bezierPoints[vectorIndex][0];
					objY = bezierPoints[vectorIndex][1];
					objZ = bezierPoints[vectorIndex][2];
					scaleX = yawGlobal[vectorIndex];
					scaleY = pitchGlobal[vectorIndex];
					scaleZ = fovGlobal[vectorIndex];
				}
			}
			break;

		case 'B':
		case 'b':
			if (enableBezierCameraControl)
			{
				if (vectorIndex > 0)
				{
					vectorIndex--;
					objX = bezierPoints[vectorIndex][0];
					objY = bezierPoints[vectorIndex][1];
					objZ = bezierPoints[vectorIndex][2];
					scaleX = yawGlobal[vectorIndex];
					scaleY = pitchGlobal[vectorIndex];
					scaleZ = fovGlobal[vectorIndex];
				}
			}
			break;
		case 'X':
			objX += objIncrement;
			break;
		case 'x':
			objX -= objIncrement;
			isMovementStarted = true;
			break;
		case 'Y':
			objY += objIncrement;
			break;
		case 'y':
			objY -= objIncrement;
			break;
		case 'Z':
			objZ += objIncrement;
			break;
		case 'z':
			objZ -= objIncrement;
			break;
		case 'r':
		case 'R':
			globalTime = 0.0f;
			mainScene->scene->sceneEvents->resetT();
			break;
		case 'q':
		case 'Q':
			if (objIncrement == 20.0f)
				objIncrement = 5.0f;
			else
				objIncrement = 20.0f;

			// if (scaleIncrement == 1.0f)
			// 	scaleIncrement = 0.01f;
			// else
			// 	scaleIncrement = 1.0f;
			break;

		case 'i':
		case 'I':
			globalSpeedAdjust -= 0.0001f;
			break;
		case 'k':
		case 'K':
			globalSpeedAdjust += 0.0001f;
			break;

		case '/':
			PrintLog("\n\nVariables  | OBJ %ff, %ff, %ff | OBJ Angle %ff | SCALE %ff, %ff, %ff\n\n\n\n", objX, objY, objZ, objAngle, scaleX, scaleY, scaleZ);
			PrintLog("\nlight_objX : %f , light_objY : %f , light_objZ : %f\n ", light_objX, light_objY, light_objZ);

			if (enableBezierCameraControl)
			{
				// Required  for camera
				PrintLog("\n\nstd::vector<std::vector<float>> bezierPoints = {\n");
				for (int i = 0; i < bezierPoints.size(); i++)
				{
					PrintLog("{%ff, %ff, %ff},\n", bezierPoints[i][0], bezierPoints[i][1], bezierPoints[i][2]);
				}
				PrintLog("};\n");
				PrintLog("\n\n// YAW GLOBAL\n");
				PrintLog("std::vector<float> yawGlobal = {\n");
				for (int i = 0; i < yawGlobal.size(); i++)
				{
					PrintLog("%ff,\n", yawGlobal[i]);
				}
				PrintLog("};\n");
				PrintLog("\n\n// PITCH GLOBAL\n");
				PrintLog("std::vector<float> pitchGlobal = {\n");
				for (int i = 0; i < pitchGlobal.size(); i++)
				{
					PrintLog("%ff,\n\t", pitchGlobal[i]);
				}
				PrintLog("};\n");
				PrintLog("\n\n// FOV GLOBAL\n");
				PrintLog("std::vector<float> fovGlobal = {\n");
				for (int i = 0; i < fovGlobal.size(); i++)
				{
					PrintLog("%ff,\n\t", fovGlobal[i]);
				}
				PrintLog("};\n");
			}
			break;

		case 'e':
			objAngle += objAngleIncrement;
			break;

		case 'E':
			objAngle -= objAngleIncrement;
			break;

		case 27:
			PrintLog("\n\nVariables  | OBJ %ff, %ff, %ff | OBJ Angle %ff | SCALE %ff, %ff, %ff\n\n\n\n", objX, objY, objZ, objAngle, scaleX, scaleY, scaleZ);
			PrintLog("\nlight_objX : %f , light_objY : %f , light_objZ : %f\n ", light_objX, light_objY, light_objZ);

			if (enableBezierCameraControl)
			{
				// Required  for camera
				PrintLog("\n\nstd::vector<std::vector<float>> bezierPoints = {\n");
				for (int i = 0; i < bezierPoints.size(); i++)
				{
					PrintLog("{%ff, %ff, %ff},\n", bezierPoints[i][0], bezierPoints[i][1], bezierPoints[i][2]);
				}
				PrintLog("};\n");
				PrintLog("\n\n// YAW GLOBAL\n");
				PrintLog("std::vector<float> yawGlobal = {\n");
				for (int i = 0; i < yawGlobal.size(); i++)
				{
					PrintLog("%ff,\n", yawGlobal[i]);
				}
				PrintLog("};\n");
				PrintLog("\n\n// PITCH GLOBAL\n");
				PrintLog("std::vector<float> pitchGlobal = {\n");
				for (int i = 0; i < pitchGlobal.size(); i++)
				{
					PrintLog("%ff,\n\t", pitchGlobal[i]);
				}
				PrintLog("};\n");
				PrintLog("\n\n// FOV GLOBAL\n");
				PrintLog("std::vector<float> fovGlobal = {\n");
				for (int i = 0; i < fovGlobal.size(); i++)
				{
					PrintLog("%ff,\n\t", fovGlobal[i]);
				}
				PrintLog("};\n");
			}
			PostQuitMessage(0);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			scaleX -= scaleIncrement;
			break;
		case VK_RIGHT:
			scaleX += scaleIncrement;
			break;
		case VK_UP:
			scaleY += scaleIncrement;

			break;
		case VK_DOWN:
			scaleY -= scaleIncrement;

			break;
		case VK_NUMPAD8:
			objY += objIncrement;
			break;
		case VK_NUMPAD5:
			objY -= objIncrement;
			break;
		case VK_NUMPAD4:
			objX -= objIncrement;
			break;
		case VK_NUMPAD6:
			objX += objIncrement;
			break;
		case VK_NUMPAD7:
			objZ -= objIncrement;
			break;
		case VK_NUMPAD1:
			objZ += objIncrement;
			break;

		case VK_NUMPAD9:
			objAngle += objAngleIncrement;
			break;

		case VK_NUMPAD3:
			objAngle -= objAngleIncrement;
			break;
		default:
			break;
		}
		break;
	case WM_SIZE:
		resize(WORD(lParam), HIWORD(lParam));
		break;

	case WM_CLOSE:
		PrintLog("\n\nVariables  | OBJ %ff, %ff, %ff | SCALE %ff, %ff, %ff\n\n\n\n", objX, objY, objZ, scaleX, scaleY, scaleZ);
		PrintLog("light_objX : %f , light_objY : %f , light_objZ : %f\n ", light_objX, light_objY, light_objZ);

		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MOUSEMOVE:
	{
		float mouseX = LOWORD(lParam);
		float mouseY = HIWORD(lParam);

		// Send this to camera class
		camera.mouseInputs(mouseX, mouseY);
	}
	break;

	case WM_MOUSEWHEEL:
	{
		short scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		// Use scrollValue and scrollDelta here
		camera.mouseScroll(scrollDelta);
	}
	break;

	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen()
{
	// variable declartions
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	//	code
	wp.length = sizeof(WINDOWPLACEMENT);
	if (gbFullScreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED); // nccalksize
			}

			ShowCursor(FALSE);
			gbFullScreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}

int initialize(void)
{
	/* fucntion delcations */
	void resize(int, int);
	void uninitialize(void);
	void ToggleFullScreen();
	/* variable declartions */
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	/* code */
	/* initialization of pixelformatdesciptor structure */
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR)); // memset((void*)&pfd , NULL, sizeof(OIXELFORAMTEDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	/* GetDC */
	ghdc = GetDC(ghwnd);

	/* Choose Pixel Format */
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
		return -1;

	/* Set The choosen Puxel Format */
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
		return -2;

	/* binding API */
	/* Create OpenGL Rendering Context */
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
		return -3;

	/* make the rendering as current cintext */
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return -4;

	/* Here start OpeGL Code */
	// GLEW INITIALIZATION
	if (glewInit() != GLEW_OK)
		return -5;

	// ======================================================= START INITIALIZATION HERE
	commonShaders = new CommonShaders();
	commonShaders->initialize();

	mainScene = new MainScene();

	commonModels = new CommonModels();
	commonModels->initialize(mainScene->selected_scene, mainScene->START_E2E_DEMO);

	if (!mainScene->initialize())
	{
		PrintLog("Failed to initialize mainScene");
	}

	// Depth Related Changes
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);

	// Enabaling the texture
	glEnable(GL_TEXTURE_2D);

	/* Clear the  screen using blue color */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	// Setting up camera
	float _position[] = {0.0, 0.0, 10.0};
	camera = Camera(WINWIDTH, WINHEIGHT, _position);

	// warmup resize call
	resize(WINWIDTH, WINHEIGHT);
	ToggleFullScreen();

	if (playMusic)
	{
		myMusic.InitializeAudio(MAKEINTRESOURCE(MYMUSIC));

		if (mainScene->START_E2E_DEMO == true)
		{
			myMusic.Play();
		}

		// myMusic.setAudio(VOLUME_LEVEL);
	}

	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	// Initialize Timers
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startCount);

	return (0);
}

void resize(int width, int height)
{
	/* code */
	if (height == 0)
		height = 1;

	glViewport(0, 0, width, height);

	// updating camera's resolution
	camera.updateResolution(width, height);

	giWindowWidth = width;
	giWindowHeight = height;

	perspectiveProjectionMatrix = vmath::perspective(
		45.0f,
		(GLfloat)width / (GLfloat)height,
		0.1f,
		10000000.0f);
}

void display(void)
{
	/* Code */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (enableBezierCameraControl)
	{
		bezierPoints[vectorIndex][0] = objX;
		bezierPoints[vectorIndex][1] = objY;
		bezierPoints[vectorIndex][2] = objZ;
		yawGlobal[vectorIndex] = scaleX;
		pitchGlobal[vectorIndex] = scaleY;
		fovGlobal[vectorIndex] = scaleZ;
	}

	updateGlobalViewMatrix();

	// ==================================== SCENE
	mainScene->display();
	// ==================================== DISPLAY TEXT IN TITLE BAR
	char titleText[255];
	sprintf(titleText, "SceneTime = %f,CameTime = %f Index = %d Variables | OBJ Angle %f | OBJ %f :: %f :: %f | SCALE %f :: %f :: %f", mainScene->scene->sceneEvents->getT(), globalTime, vectorIndex, objAngle, objX, objY, objZ, scaleX, scaleY, scaleZ);
	SetWindowTextA(ghwnd, (LPCSTR)titleText);
	// =============================================================

	SwapBuffers(ghdc);
}

float camSpeed = 100.0f;
void update(void)
{
	mainScene->update();
	if (globalTime <= 1.0f)
		globalTime += (0.000015f + globalSpeedAdjust);
	// globalTime += (0.000015f + camSpeed);

	// if (globalTime > 0.17f)
	// {
	// 	if (camSpeed < 0.0008f)
	// 		camSpeed += 0.00003f;
	// }
	// globalTime = 0.8f + globalSpeedAdjust;
}

void uninitialize(void)
{
	/* function declarations */
	void ToggleFullScreen(void);

	/* code */
	mainScene->uninitialize();
	commonShaders->uninitialize();

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghwnd = NULL;
		ghdc = NULL;
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
	}

	CloseLogFile();
}

void setGlobalBezierCamera(BezierCamera *bezierCamera)
{
	globalBezierCamera = bezierCamera;
}

void setSelectedScene(int selScene)
{
	// if (mainScene->START_E2E_DEMO == true)
	mainScene->selected_scene = selScene;
}

void updateGlobalViewMatrix()
{
	if (USE_FPV_CAM)
	{
		viewMatrix = camera.getViewMatrix();
	}
	else
	{
		if (globalBezierCamera)
			viewMatrix = globalBezierCamera->getViewMatrix();
	}
}
void resetElapsedTime()
{
}

void toggleFrameLimit(bool enable)
{
	isFrameLimitEnabled = enable;
}
