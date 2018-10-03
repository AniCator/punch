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
#include <string>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "errordefs.h"
#include "util/util.h"

#include "ConfigurationManager.h"
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
	void Exit( int ErrorCode );
	void Exit( int ErrorCode, const char* ErrorMessage );

	double	GetDeltaTime();
	double	GetDrawDeltaTime();

	void HandleKeyEvents( GLFWwindow* window, int key, int scancode, int action, int mods );
	void HandlePathDrop( GLFWwindow* window, int count, const char** paths );
protected:
	//
private:
	void Initialize();
	void InitializeGLEW();
	void InitializeGLFW();

	void InitializeMusicModule();
	void InitializeLayerManager();
	void CreateApplicationWindow();

	void PopulatePlaylist();

	void MainLoop();

	void HandleLogic();
	void HandleDraw();

	void InitializeProgressBar();
	void UpdateProgressBar();
	void DrawProgressBar();

	void	UpdateDeltaTime();
	void	UpdateDrawDeltaTime();

	double TickTime;
	double FrameTime;
	double LastTickTime;
	double LastFrameTime;
	GLFWwindow* ApplicationWindow;

	CMusicModule* MusicModuleInstance;
	CLayerManager* LayerManagerInstance;
	CConfigurationManager* ConfigurationManagerInstance;

public:
	static CPunchApplication& GetInstance()
	{
		static CPunchApplication    instance; // Guaranteed to be destroyed.
												  // Instantiated on first use.
		return instance;
	}
private:
	CPunchApplication( CPunchApplication const& ) = delete;
	void operator=( CPunchApplication const& ) = delete;
};