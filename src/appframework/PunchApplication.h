/* 
 *	PunchApplication
 *	|	Main application class.
 *	|	Used to create the Punch window and perform steps to get everything running.
 */

#include "GL/glew.h"
#include "GLFW/glfw3.h" //Include GLFW3 header

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include "GLFW/glfw3native.h"
#endif

#include "glm.hpp"

#include <iostream>
#include <algorithm>
#include <array>
#define _USE_MATH_DEFINES
#include <math.h>

#include "errordefs.h"
#include "util/util.h"
#include "musicmodule/BASSModule.h"
#include "LayerManager.h"

#define APP_WINDOW_TITLE "Punch 2.0"

//#define WINDOW_NO_BORDER
#define WINDOW_HALF_SIZE

enum MusicModuleType
{
	MM_UNDEFINED,
	MM_BASS
};

#define MUSIC_MODULE_TYPE MM_BASS

class CPunchApplication
{
public:
	CPunchApplication();
	~CPunchApplication();

	void Run();
	void Exit();
	void Exit(int iErrorCode);
	void Exit(int iErrorCode, const char* cErrorMessage);
protected:
	//
private:
	void Initialize();
	void InitializeGLEW();
	void InitializeGLFW();
	void InitializeMusicModule();
	void InitializeLayerManager();

	void PopulatePlaylist();

	void MainLoop();

	void HandleLogic();
	void HandleDraw();

	static void HandleKeyEvents(GLFWwindow* window, int key, int scancode, int action, int mods);

	double	GetDeltaTime();
	void		UpdateDeltaTime();

	double m_dTimeLastFrame;
	GLFWwindow* m_pAppWindow;

	static CMusicModule* m_pMusicModule;
	static CLayerManager* m_pLayerManager;
};